#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "pds.h"
#include "criminal_record.h"

//Function used to store records from a csv file (not implemented in this menu driven application)
int store_records(char *criminal_records_file) {
  FILE *fp;
  char record_line[500], token;
  struct Criminal criminal, dummy;

  fp = (FILE* )fopen(criminal_records_file, "r");

  while(fgets(record_line, sizeof(record_line) - 1, fp)) {
      sscanf(record_line, "%d%s%s%s%d%s", &(criminal.ID), criminal.criminal_name, criminal.crime_committed, criminal.DNA_profiling_done_or_not, &(criminal.age), criminal.date_of_arrest);
      print_criminal_record(&criminal);
      add_criminal_record(&criminal);
  }
}

//To print the chosen record when the function is called
void print_criminal_record(struct Criminal *criminal) {
  printf("Record of Criminal with ID %d", criminal->ID);
  printf("\nCriminal Name: %s", criminal->criminal_name);
  printf("\nCrime Committed: %s", criminal->crime_committed);
  printf("\nDNA profiling Status: %s", criminal->DNA_profiling_done_or_not);
  printf("\nCriminal Age: %d", criminal->age);
  printf("\nDate of Arrest: %s\n", criminal->date_of_arrest);
}

//To call get_rec_by_ndx_key (search by criminal ID)
int search_for_criminal_by_ID(int ID, struct Criminal* criminal) {
  return get_rec_by_ndx_key(ID, criminal);
}

//To store a record. Called for each individual record to be stored
int add_criminal_record(struct Criminal *criminal) {
  int status;
  status = put_rec_by_key(criminal->ID, criminal);

  if(status != PDS_SUCCESS) {
    fprintf(stderr, "Unable to add criminal record with key %d. Error %d", criminal->ID, status);
		return RECORD_FAILURE;
  }
  return PDS_SUCCESS;
}

//To call get_rec_by_non_ndx_key (search by criminal name)
int search_for_criminal_by_name(char *criminal_name, struct Criminal *criminal, int *io_count) {
  	int status = get_rec_by_non_ndx_key(criminal_name, criminal, &match_data, io_count);
    if(status != PDS_SUCCESS) {
      return RECORD_FAILURE;
    }
    return status;

}

//Utility function
int match_data(void *rec, void *key) {
	// Store the rec in a struct Criminal pointer
	struct Criminal *criminal = (struct Criminal *)rec;

	// Store the key in a char pointer
	char *key_in_pointer = (char *)key;

	// Compare the crime_committed values in key and record
	// Return 0,1,>1 based on above condition

  // Return > 1 in case of any other error
	if(criminal == NULL || key_in_pointer == NULL){
		return 10;
	}

	// Return 0 if criminal_name of the criminal matches with criminal_name parameter
	if(strcmp(key_in_pointer, criminal->criminal_name) == 0) {
		return 0;
	}

  // Return 1 if criminal_name of the criminal does NOT match
  return 1;
}

//To delete a criminal record based on Criminal ID entered by user
int delete_criminal_record(int ID) {
  int status = delete_rec_by_ndx_key(ID);

  if(status != PDS_SUCCESS) {
    return RECORD_FAILURE;
  }
  return status;
}

//To overwrite or update a criminal record in the database
int overwrite_criminal_data(struct Criminal *criminal) {
  int result = pds_overwrite(criminal->ID, criminal);
  if (result != PDS_SUCCESS) {
		return RECORD_FAILURE;
	}
	return PDS_SUCCESS;
}
