/* parBinIn.cpp 
 *  ... uses OO-MPI-IO to read N doubles 
 *   from a binary-format file.
 *
 * @author: Joel Adams, for CS 374 at Calvin University, Fall 2023.
 *
 * Usage: ./parBinIn <fileName>
 */

#include <stdio.h>              // fprint(), etc.
#include <stdlib.h>             // exit(), ...
#include <vector>               // vector
#include "OO_MPI_IO.h"          // ParallelReader


char* processCmdLineArgs(int argc, char** argv);

int main(int argc, char** argv) {
   const int MASTER = 0;
   int id = -1, numProcs = -1;

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
   MPI_Comm_rank(MPI_COMM_WORLD, &id);

   char* fileName = processCmdLineArgs(argc, argv);

   double startTime = MPI_Wtime();

   ParallelReader<double> 
     reader(fileName, MPI_DOUBLE, id, numProcs);

   std::vector<double> vec;
   reader.readChunk(vec);

   reader.close();

   double totalTime = MPI_Wtime() - startTime;

   if (id == MASTER) {
      printf("\n%ld doubles read from file '%s' in %lf secs\n\n",
              reader.getNumItemsInFile(), fileName, totalTime);
   }

   MPI_Finalize();
   return 0;
}

/* utility to check and process the command line argument
 * @param: argc, an int
 * @param: argv, a char**
 * Precondition: argc == 2
 *            && argv[1] is the string representation of
 *                a potentially long integer value.
 *
 * @return: the long equivalent of argv[1].
 */
char* processCmdLineArgs(int argc, char** argv) {
   if (argc != 2) {
      fprintf(stderr, "\n\n*** Usage: [mpirun ...] ./parBinIn fileName\n\n");
      exit(1);
   }

   return argv[1];
}

