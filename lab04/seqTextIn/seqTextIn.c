/* seqTextIn.c 
 *  ... reads N double values from a text file
 *      whose name is given on the commandline.
 *
 *  Precondition: the first line of filename == N.
 *
 * @author: Joel Adams, for CS 374 at Calvin University, Fall 2023.
 *
 * Usage: ./seqTextIn <fileName>
 */

#include <stdio.h>              // fprintf(), fscanf(), etc.
#include <stdlib.h>             // exit(), malloc(), ...
#include <ctype.h>              // isdigit(), ...
#include <mpi.h>                // MPI functions for timing

#define MAX_FILE_NAME_SIZE 64

typedef double ItemType;

char* processCmdLineArgs(int argc, char** argv);
void checkOpen(FILE* fPtr, char* fileName);
long getFileSize(FILE* fPtr); // Function to get file size

int main(int argc, char** argv) {
   MPI_Init(&argc, &argv); // Initialize MPI environment

   double startTime = MPI_Wtime(); // Start timing

   char* fileName = processCmdLineArgs(argc, argv);

   FILE * filePtr = fopen(fileName, "r");
   checkOpen(filePtr, fileName);

   long numValues = 0;
   fscanf(filePtr, " %ld", &numValues);
   ItemType* arrPtr = (ItemType*) malloc(numValues * sizeof(ItemType));

   for (long i = 0; i < numValues; ++i) {
      fscanf(filePtr, " %lf", &(arrPtr[i]));
   }

   fclose(filePtr);

   double endTime = MPI_Wtime(); // End timing

   // Report number of doubles read and time taken
   printf("\n%ld doubles read from file '%s'\n", numValues, fileName);
   printf("Time taken: %f seconds\n\n", endTime - startTime);

   MPI_Finalize(); // Finalize MPI environment
   return 0;
}

/* utility to check and process the command line argument
 * @param: argc, an int
 * @param: argv, a char**
 * Precondition: argc == 2
 *            && argv[1] is the string representation of
 *                a potentially long integer value.
 * @return: the long equivalent of argv[1].
 */
char* processCmdLineArgs(int argc, char** argv) {
   if (argc != 2) {
      fprintf(stderr, "\n\n*** Usage: ./seqTextIn fileName\n\n");
      exit(1);
   }

   return argv[1];
}

/* utility to check that opening the file succeeded.
 * @param: fPtr, a FILE*
 * @param: fileName, a char*
 * Precondition: fPtr holds the result of a call to fopen()
 *            && fileName is the name of the arg to fopen().
 * Postcondition: if fPtr == NULL, an error message has been displayed.
 */
void checkOpen(FILE* fPtr, char* fileName) {
   if (fPtr == NULL) {
      fprintf(stderr, "\n*** Unable to open file '%s'\n\n",
                                fileName);
      exit(1);
   }
}

/* utility to calculate the size of an open file in bytes
 * @param: fPtr, a FILE*
 * Precondition: fPtr is to a file that has just been opened.
 * @return: the number of bytes in the file.
 */
long getFileSize(FILE* fPtr) {
    fseek(fPtr, 0L, SEEK_END); // seek to the end of the file
    long res = ftell(fPtr); // calculate the size of the file
    fseek(fPtr, 0L, SEEK_SET); // return the file ptr to the beginning
    return res;
}