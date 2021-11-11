#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//Including header files
#include "pds.h"
#include "criminal_record.h"

//#define TREPORT(a1,a2) printf("Status %s: %s\n\n",a1,a2); fflush(stdout);

int main() {
  printf("\n\n\t\tCriminal Records Database\n\n\n");

  //The test_case is the choice the user makes
  int test_case;


  printf("Make your choice of what action you wish to perform and type that particular number.\n");
  printf("Any choice outside the given options will display an error message. You MUST create a database before trying to open one. If it is already created, it can be opened.\n");
  printf("DISCLAIMER! The most recently opened database is the one you interact with!\n");
  printf("Another note! Any non_numeric data (i.e. all except ID and age) which has a space has to be replaced with a hyphen or an underscore.\nEg: if the name you want to enter is 'Dwight Schrute', enter it as 'Dwight_Schrute'. Not doing so will lead to a fatal error!");

  int create_status, open_status, store_status, ndx_status, non_ndx_status, delete_status, overwrite_status;
  struct Criminal test_criminal;
  int rec_size = sizeof(struct Criminal);

  while(1) {
      //Displays the options for users to choose from
      printf("\nChoose the operation you wish to perform.\n");
      printf("=>1. CREATE (creates new database)\n");
      printf("=>2. OPEN (opens exisiting database and enables interaction)\n");
      printf("=>3. STORE (gives options to enter information and store a new record)\n");
      printf("=>4. NDX_SEARCH (search based on ID)\n");
      printf("=>5. NON_NDX_SEARCH (search based on Criminal Name)\n");
      printf("=>6. NDX_DELETE (delete record based on ID)\n");
      printf("=>7. OVERWRITE (overwrite record based on ID)\n");
      printf("=>8. EXIT (closes open database and exits the application)\n");

      scanf("%d",&test_case);

      switch(test_case) {
        //CREATE - takes repo name from user
        case 1: {
          char repo_name[100];
          printf("\nEnter name for your database: ");
          scanf("%s", repo_name);
          create_status = pds_create(repo_name);
          if(create_status == RECORD_SUCCESS) {
            printf("\nNew database created successfully\n");
          }
          else {
            printf("\nCreate failed. Restart application or try again\n");
          }
          break;
        }

        //OPEN - takes repo name from user
        case 2: {
          char repo_name[100];
          printf("\nEnter name of the database you want to access: ");
          scanf("%s", repo_name);

          open_status = pds_open(repo_name, rec_size);
          if(open_status == RECORD_SUCCESS) {
            printf("\nChosen database opened successfully\n");
          }
          else {
            printf("\nOpen failed. Restart application or try again or enter a database which has already been created.\n");
          }
          break;
        }

        //STORE - takes ID, name, crime, DNA profiling status, age, date of arrest
        case 3: {
          int id = 0;
          printf("\nEnter Criminal ID: ");
          scanf("%d", &id);
          test_criminal.ID = id;

          char name[50];
          printf("Enter Criminal Name: ");
          scanf("%s", name);
          strcpy(test_criminal.criminal_name, name);

          char crime[50];
          printf("Enter Crime Committed: ");
          scanf("%s", crime);
          strcpy(test_criminal.crime_committed, crime);

          char dna[50];
          printf("Enter status of DNA profiling: ");
          scanf("%s", dna);
          strcpy(test_criminal.DNA_profiling_done_or_not, dna);

          int age = 0;
          printf("Enter Criminal age: ");
          scanf("%d", &age);
          test_criminal.age = age;

          char date[50];
          printf("Enter Date of Arrest: ");
          scanf("%s", date);
          strcpy(test_criminal.date_of_arrest, date);

          store_status = add_criminal_record(&test_criminal);

          if(store_status == PDS_SUCCESS) {
            printf("\nNew record created and stored successfully\n");
            print_criminal_record(&test_criminal);
          }

          else {
            printf("Record store failed\n");
          }

          break;
        }

        //NDX_SEARCH - takes ID from user to search for it
        case 4: {
          int id_to_search = 0;
          printf("\nEnter Criminal ID to search for record: ");
          scanf("%d", &id_to_search);

          ndx_status = search_for_criminal_by_ID(id_to_search, &test_criminal);

          if(ndx_status == PDS_SUCCESS) {
            printf("\nRecord found!\n");
            print_criminal_record(&test_criminal);
          }

          else {
            printf("\nRecord not found\n");
          }
          break;
        }

        //NON_NDX_SEARCH - takes name of criminal from user to search for it
        case 5: {
          char name_to_search[50];
          printf("\nEnter Criminal Name to search for record: ");
          scanf("%s", name_to_search);

          int actual_io = 0;
          non_ndx_status = search_for_criminal_by_name(name_to_search, &test_criminal, &actual_io);

          if(non_ndx_status == PDS_SUCCESS) {
            printf("\nRecord found!\n");
            print_criminal_record(&test_criminal);
          }

          else {
            printf("\nRecord not found\n");
          }
          break;
        }

        //NDX_DELETE - takes ID from user to delete record
        case 6: {
          int id_to_delete = 0;
          printf("\nEnter Criminal ID to delete record: ");
          scanf("%d", &id_to_delete);

          test_criminal.ID = -1;
          delete_status = delete_criminal_record(id_to_delete);

          if(delete_status == PDS_SUCCESS) {
            printf("\nRecord deleted successfully!\n");
          }
          else {
            printf("\nRecord delete failed\n");
          }
          break;
        }

        //OVERWRITE - takes ID and the other record information from user to overwrite that particular record
        case 7: {
          int ov_id = 0;
          printf("\nEnter Criminal ID to overwrite: ");
          scanf("%d", &ov_id);
          test_criminal.ID = ov_id;

          char ov_name[50];
          printf("Enter new Criminal Name: ");
          scanf("%s", ov_name);
          strcpy(test_criminal.criminal_name, ov_name);

          char ov_crime[50];
          printf("Enter new Crime Committed: ");
          scanf("%s", ov_crime);
          strcpy(test_criminal.crime_committed, ov_crime);

          char ov_dna[50];
          printf("Enter new status of DNA profiling: ");
          scanf("%s", ov_dna);
          strcpy(test_criminal.DNA_profiling_done_or_not, ov_dna);

          int ov_age = 0;
          printf("Enter new Criminal age: ");
          scanf("%d", &ov_age);
          test_criminal.age = ov_age;

          char ov_date[50];
          printf("Enter new Date of Arrest: ");
          scanf("%s",ov_date);
          strcpy(test_criminal.date_of_arrest, ov_date);

          overwrite_status = overwrite_criminal_data(&test_criminal);

          if(overwrite_status == PDS_SUCCESS) {
            printf("\nRecord updated successfully!\n");
          }

          else {
            printf("\nRecord update failed\n");
          }
          break;
        }

        //EXIT - if a repo is open, it closes and exits. Otherwise, it just exits the application
        case 8: {
          if(open_status == RECORD_SUCCESS) {
            if(pds_close() == PDS_SUCCESS) {
              printf("\nTHANK YOU!\n");
              exit(0);
            }
          }
          printf("\nTHANK YOU!\n");
          exit(0);
          break;
        }

        //INVALID CHOICE - if user enters a number which is not in the options
        default:
          printf("\n Invalid choice. Try again.\n");
        }
    }
}
