/* circuitSatisfiabilityChunks.c
 *
 * An adaptation of circuitSatisfiability.c, this program employs MPI for 
 * parallel processing, distributing tasks using the 'Chunks' Parallel Loop pattern.
 *
 * Author: Yuese Li
 * Institution: Calvin University
 * Course: CS374 (High Performance Computing)
 * Date: October 28, 2023
 * Purpose: Project 1 - To learn and implement parallel computing concepts
 */


#include <stdio.h>     // printf()
#include <limits.h>    // UINT_MAX
#include <mpi.h>       // MPI functions

int checkCircuit (int, long);

// Function to calculate start and stop values for each chunk
void getChunkStartStopValues(int id, int numProcesses, long numIterations, long* start, long* stop) {
    long chunkSize = numIterations / numProcesses;
    long remainder = numIterations % numProcesses;
    
    if (id < remainder) {
        *start = id * (chunkSize + 1);
        *stop = *start + chunkSize + 1;
    } else {
        *start = id * chunkSize + remainder;
        *stop = *start + chunkSize;
    }
}

int main (int argc, char *argv[]) {
    int id;               // process id 
    int numProcesses;     // number of processes
    int count = 0;        // number of solutions 
    long start, stop;     // chunk start and stop values
    int globalCount = 0;  // total number of solutions across all processes
    
    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

    // Calculate start and stop values for this process's chunk
    getChunkStartStopValues(id, numProcesses, UINT_MAX+1L, &start, &stop);

    // Start timer for process 0
    double startTime = 0.0, totalTime = 0.0;
    if (id == 0) {
        printf ("\nProcess %d is checking the circuit...\n", id);
        startTime = MPI_Wtime();
    }

    // Main loop: check the circuit for all values in this process's chunk
    for (long i = start; i < stop; i++) {
        count += checkCircuit(id, i);
    }

    // Collect results from all processes
    MPI_Reduce(&count, &globalCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Print results for process 0
    if (id == 0) {
        totalTime = MPI_Wtime() - startTime;
        printf ("Process %d finished in time %f secs.\n", id, totalTime);
        printf("A total of %d solutions were found.\n", globalCount);
    }

    // Finalize MPI
    MPI_Finalize();
    return 0;
}

/* EXTRACT_BIT is a macro that extracts the ith bit of number n.
 *
 * parameters: n, a number;
 *             i, the position of the bit we want to know.
 *
 * return: 1 if 'i'th bit of 'n' is 1; 0 otherwise 
 */

#define EXTRACT_BIT(n,i) ((n & (1<<i)) ? 1 : 0)

/* checkCircuit() checks the circuit for a given input.
 * parameters: id, the id of the process checking;
 *             bits, the (long) rep. of the input being checked.
 *
 * output: the binary rep. of bits if the circuit outputs 1
 * return: 1 if the circuit outputs 1; 0 otherwise.
 */

#define SIZE 32

int checkCircuit (int id, long bits) {
   int v[SIZE];        /* Each element is one of the 32 bits */
   int i;

   for (i = 0; i < SIZE; i++) {
     v[i] = EXTRACT_BIT(bits,i);
   }

   if ( ( (v[0] || v[1]) && (!v[1] || !v[3]) && (v[2] || v[3])
       && (!v[3] || !v[4]) && (v[4] || !v[5])
       && (v[5] || !v[6]) && (v[5] || v[6])
       && (v[6] || !v[15]) && (v[7] || !v[8])
       && (!v[7] || !v[13]) && (v[8] || v[9])
       && (v[8] || !v[9]) && (!v[9] || !v[10])
       && (v[9] || v[11]) && (v[10] || v[11])
       && (v[12] || v[13]) && (v[13] || !v[14])
       && (v[14] || v[15]) )
       &&
          ( (v[16] || v[17]) && (!v[17] || !v[19]) && (v[18] || v[19])
       && (!v[19] || !v[20]) && (v[20] || !v[21])
       && (v[21] || !v[22]) && (v[21] || v[22])
       && (v[22] || !v[31]) && (v[23] || !v[24])
       && (!v[23] || !v[29]) && (v[24] || v[25])
       && (v[24] || !v[25]) && (!v[25] || !v[26])
       && (v[25] || v[27]) && (v[26] || v[27])
       && (v[28] || v[29]) && (v[29] || !v[30])
       && (v[30] || v[31]) ) )
   {
      printf ("%d) %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n", id,
         v[31],v[30],v[29],v[28],v[27],v[26],v[25],v[24],v[23],v[22],
         v[21],v[20],v[19],v[18],v[17],v[16],v[15],v[14],v[13],v[12],
         v[11],v[10],v[9],v[8],v[7],v[6],v[5],v[4],v[3],v[2],v[1],v[0]);
      fflush (stdout);
      return 1;
   } else {
      return 0;
   }
}

