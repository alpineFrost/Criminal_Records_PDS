#ifndef CRIMINAL_RECORD_H
#define CRIMINAL_RECORD_H

#define RECORD_SUCCESS 0
#define RECORD_FAILURE 1

//struct which stores the data
struct Criminal {
  int ID;
  char criminal_name[50];
  char crime_committed[50];
  char DNA_profiling_done_or_not[50];
  int age;
  char date_of_arrest[50];
};

extern struct PDS_RepoInfo *repoHandle;

//Function used to store records from a csv file (not implemented in this menu driven application)
int store_records(char *criminal_records_file);

//To print the chosen record when the function is called
void print_criminal_record(struct Criminal *criminal);

//To call get_rec_by_ndx_key (search by criminal ID)
int search_for_criminal_by_ID(int ID, struct Criminal* criminal);

//To store a record. Called for each individual record to be stored
int add_criminal_record(struct Criminal *criminal);

//To call get_rec_by_non_ndx_key (search by criminal name)
int search_for_criminal_by_name(char *criminal_name, struct Criminal *criminal, int *io_count);

//Utility function
int match_data(void *rec, void *key);

//To delete a criminal record based on Criminal ID entered by user
int delete_criminal_record(int ID);

//To overwrite or update a criminal record in the database
int overwrite_criminal_data(struct Criminal *criminal);

#endif
