#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

#include "pds.h"
#include "bst.h"

struct PDS_RepoInfo repo_handle;

int unload(FILE* fp, struct BST_Node* root);

int pds_create(char *repo_name)
{
  char filename[30], indexfile[30];
  strcpy(filename,repo_name);
  strcpy(indexfile,repo_name);
  strcat(filename,".dat");
  strcat(indexfile,".ndx");
  FILE *fp = fopen(filename,"wb+");
  FILE *ifp = fopen(indexfile,"wb+");
  if(fp  == NULL || ifp == NULL) return PDS_FILE_ERROR;\
  fclose(fp);
  fclose(ifp);

  return PDS_SUCCESS;
}

int pds_open(char* repo_name, int rec_size) // Same as before
{
// Update the fields of PDS_RepoInfo appropriately
// Build BST and store in pds_bst by reading index entries from the index file
// Close only the index file

	// Open the data file and index file in rb+ mode
	FILE *fp1, *fp2;
  	char data_file_name[31];
	strcpy(data_file_name, repo_name);
	strcat(data_file_name, ".dat");
	char index_file_name[31];
	strcpy(index_file_name, repo_name);
	strcat(index_file_name, ".ndx");

  	fp1 = fopen(data_file_name, "rb+");
  	fp2 = fopen(index_file_name, "rb+");

	//handle file pointer error
	if(fp1 == NULL || fp2 == NULL) {
	  return PDS_FILE_ERROR;
	}

	//update the fields of PDS_RepoInfo appropriately
	//store pointer to repo_handle
	repo_handle.pds_data_fp = fp1;

	//store pointer to repo_handle
	repo_handle.pds_ndx_fp = fp2;

	//update pds_name
	strcpy(repo_handle.pds_name,repo_name);

	//update  repo_status
	repo_handle.repo_status = PDS_REPO_OPEN;

	//update rec_size
	repo_handle.rec_size = rec_size;

  	//build BST and store in pds_bst by reading index entries from the index file
  	pds_load_ndx();

  	//Close only the index file
  	fclose(repo_handle.pds_ndx_fp);
  	return PDS_SUCCESS;
}

int pds_load_ndx()
{
	/*
	pds_load_ndx has the following functionality:
	It is an internal function used by pds_open to read index entries into BST
	*/

	while(!feof(repo_handle.pds_ndx_fp)) {
	int key, status, offset, b_status;

		status = fread(&key, sizeof(int), 1, repo_handle.pds_ndx_fp);
		b_status = fread(&offset, sizeof(int), 1, repo_handle.pds_ndx_fp);

    if(status == 0 || b_status == 0) {
  		return PDS_LOAD_NDX_FAILED;
  	}

		struct PDS_NdxInfo* ndxInfo = malloc(sizeof(struct PDS_NdxInfo));
		ndxInfo->key = key;
    ndxInfo->offset = offset;

    if (status != 0 && b_status != 0) {
    		bst_add_node(&repo_handle.pds_bst, key, ndxInfo);
    }
	}
	return PDS_SUCCESS;
}

int put_rec_by_key(int key, void*rec)
{
  	int check;

	//Check file status
	if(repo_handle.repo_status != PDS_REPO_OPEN || repo_handle.repo_status != PDS_REPO_ALREADY_OPEN) {
		repo_handle.repo_status = PDS_REPO_CLOSED;
	}

	//Seek to the end of the data file
	fseek(repo_handle.pds_data_fp, 0, SEEK_END);

	//Create an index entry with the current data file location using ftell
	struct PDS_NdxInfo *ndxInfo = malloc(sizeof(struct PDS_NdxInfo));
	ndxInfo->key = key; //putting given key
	ndxInfo->offset = ftell(repo_handle.pds_data_fp);

	//(NEW) ENSURE is_deleted is set to 0 when creating index entry
  	ndxInfo->is_deleted = 0;

	//Add index entry to BST using offset returned by ftell
	check = bst_add_node(&repo_handle.pds_bst, key, ndxInfo);

	//Check for errors
	if (check == BST_DUP_KEY || check == !BST_SUCCESS) {
      	return PDS_ADD_FAILED;
  	}

	// Write the key at the current data file location
	fwrite(&key, sizeof(int), 1, repo_handle.pds_data_fp);

	// Write the record after writing the key
	fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
	return PDS_SUCCESS;
}

int get_rec_by_ndx_key(int key, void*rec)
{
	//Search for index entry in BST
	struct PDS_NdxInfo *ndxInfo = malloc(sizeof(struct PDS_NdxInfo));
	struct BST_Node *found = bst_search(repo_handle.pds_bst, key);

	if(found == NULL) {
		return PDS_REC_NOT_FOUND;
	}

	ndxInfo = found->data; //Storing key, offset and delete status

	//(NEW) Check if the entry is deleted and if it is deleted, return PDS_REC_NOT_FOUND
	if(ndxInfo->is_deleted == 1) {
		return PDS_REC_NOT_FOUND;
	}

  	//Seek to the file location based on offset in index entry
  	fseek(repo_handle.pds_data_fp, ndxInfo->offset, 0);

  	int stored;
	//Read the key at the current file location
	fread(&stored, sizeof(int), 1, repo_handle.pds_data_fp);

	//Read the record after reading the key
	fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);

  if(stored == key) {
    return PDS_SUCCESS;
  }
}

int pds_close()
{
	char index_file_name[31];
	strcpy(index_file_name, repo_handle.pds_name);
	strcat(index_file_name, ".ndx");

	//Open the index file in wb mode (write mode, not append mode)
	FILE *fp_in;
	fp_in = fopen(index_file_name, "wb");

	//Unload the BST into the index file by traversing it in PRE-ORDER (overwrite the entire index file)
	unload(fp_in, repo_handle.pds_bst);
	repo_handle.pds_bst = NULL;

	//Free the BST by calling bst_destroy()
	bst_destroy(repo_handle.pds_bst);

	//Close the index file and data file
	fclose(repo_handle.pds_data_fp);
	fclose(fp_in);

  return PDS_SUCCESS;
}

int unload(FILE* fp, struct BST_Node* root) {
  	int key, w_key, offset, w_offset;

  	if (root == NULL) {
    	return PDS_SUCCESS;
  	}

  	struct PDS_NdxInfo* ndx_info = (struct PDS_NdxInfo*)root->data;

    if(ndx_info->is_deleted == 0) {

      key = ndx_info->key;
    	w_key = fwrite(&key, sizeof(int), 1, fp);
    	offset = ndx_info->offset;
    	w_offset = fwrite(&offset, sizeof(int), 1, fp);

    	if(w_key == 0 || w_offset == 0) {
    		return PDS_FILE_ERROR;
    	}
    }

  	int left = unload(fp, root->left_child);
  	int right = unload(fp,root->right_child);

  	if (right != PDS_SUCCESS) {
  		return right;
  	}
  	return left;
}


int get_rec_by_non_ndx_key(void *key_to_search, void *rec, int (*matcher)(void *rec, void *key), int *io_count)
{
  int key_to_check;

  // Seek to beginning of file
  fseek(repo_handle.pds_data_fp, 0, SEEK_SET);

  // Perform a table scan - iterate over all the records
  // Read the key and the record
  while(fread(&key_to_check, sizeof(int), 1, repo_handle.pds_data_fp) == 1 && fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp) == 1) {

    // Increment io_count by 1 to reflect count no. of records read
    (*io_count)++;
    // char* key = (char*) key_to_search;

    // Use the function in function pointer to compare the record with required key
    if(matcher(rec, key_to_search) == 0) {

      struct BST_Node* found= bst_search(repo_handle.pds_bst,key_to_check);
      if(!found) {
        return PDS_REC_NOT_FOUND;
      }

      struct PDS_NdxInfo* ndxInfo = (struct PDS_NdxInfo*)found->data;

      if(!ndxInfo || ndxInfo->is_deleted == 1) {
        return PDS_REC_NOT_FOUND;
      }
      return PDS_SUCCESS;
    }
  }
  return PDS_REC_NOT_FOUND;
}

int delete_rec_by_ndx_key(int key) // New Function
{
	//Search for index entry in BST
	struct BST_Node *root = bst_search(repo_handle.pds_bst, key);

	if(root == NULL) {
		return PDS_DELETE_FAILED;
	}

	struct PDS_NdxInfo* ndxInfo = malloc(sizeof(struct PDS_NdxInfo));
	ndxInfo = root->data;

	if(ndxInfo->is_deleted == 1) {
    	return PDS_DELETE_FAILED;
  	}

	ndxInfo->is_deleted = 1;
	return PDS_SUCCESS;
}

// Overwrite an existing record with a new record
// Return relevant error code if record does not exist or any other error
// This implementation overwrites existing record with the given object
int pds_overwrite(int key, void *rec )
{
	// Search for index entry in BST
	struct PDS_NdxInfo *ndxInfo = malloc(sizeof(struct PDS_NdxInfo));
	struct BST_Node *found = bst_search(repo_handle.pds_bst, key);

	if(found == NULL) {
		return PDS_REC_NOT_FOUND;
	}

	ndxInfo = found->data; //Storing key and offset

	//(NEW) Check if the entry is deleted and if it is deleted, return PDS_REC_NOT_FOUND
	if(ndxInfo->is_deleted == 1) {
		return PDS_REC_NOT_FOUND;
	}

  	//Seek to the file location based on offset in index entry
  	fseek(repo_handle.pds_data_fp, ndxInfo->offset, SEEK_SET);

	int stored;
	//Read the key at the current file location
	fread(&stored, sizeof(int), 1, repo_handle.pds_data_fp);

	if(stored != key) {
		return PDS_REC_NOT_FOUND;
	}

	int status = fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);

	//Checking if write was successful or not
	if(status != 0) {
    	return PDS_SUCCESS;
    }

	return PDS_UPDATE_FAILED;
}
