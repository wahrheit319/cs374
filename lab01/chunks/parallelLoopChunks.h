/* parallelLoopChunks.h
 *
 *  ... defines getChunkStartStopValues() to set up an MPI parallel loop.
 *  An MPI process can call it to compute the start and stop values
 *  of a contiguous 'chunk' of the loop's iterations,
 *  and any two processes' chunk-sizes will differ by at most 1.
 *
 *  This header file makes the function easier to reuse.
 *
 * Joel Adams, Calvin University, Fall 2023.
 *
 * See: parallelLoopChunks.c (driver program)
 *
 */

#include <stdio.h>  // printf()
#include <stdlib.h> // exit()
#include <mpi.h>    // MPI
#include <math.h>   // ceil()

/* Calculate the start and stop values for this MPI process's 
 *  contiguous chunk of a set of loop-iterations, 0..REPS-1,
 *  so that each process chunk-sizes are equal (or nearly so).
 *
 * @param: id, an int containing this process's MPI rank
 * @param: numProcs, an int containing the number of processes
 * @param: REPS, a const unsigned containing the for loop's iteration total
 * Precondition: id == this process's MPI rank
 *            && numProcs == the number of MPI processes
 *            && REPS == the total number of 0-based loop iterations needed
 *            && numProcs <= REPS 
 *            && REPS < 2^32
 * @param: start, the address of the unsigned variable through which the 
 *          starting value of this process's chunk should be returned
 * @param: stop, the address of the unsigned variable through which the
 *          stopping value of this process's chunk should be returned
 * Postcondition: *start = this process's first iteration value 
 *             && *stop = this process's last iteration value + 1.
 */
void getChunkStartStopValues(int id, int numProcs, const unsigned REPS,
                              unsigned* start, unsigned* stop)
{
   // check precondition before proceeding
   if (numProcs > REPS) {
      if (id == 0) {
         printf("\n*** Number of MPI processes (%u) exceeds REPS (%u)\n",
                 numProcs, REPS);
         printf("*** Please run with -np less than or equal to %u\n\n", REPS);
      }
      MPI_Finalize();
      exit(1);
   }

   // compute the chunk size that works in many cases
   unsigned chunkSize1 = (long)ceil(((double)REPS) / numProcs);
   unsigned begin = id * chunkSize1;
   unsigned end = begin + chunkSize1;
   // see if there are any leftover iterations
   unsigned remainder = REPS % numProcs;
   // If remainder == 0, chunkSize1 = chunk-size for all processes;
   // If remainder != 0, chunkSize1 = chunk-size for p_0..p_remainder-1
   //   but for processes p_remainder..p_numProcs-1
   //   recompute begin and end using a smaller-by-1 chunk size, chunkSize2.
   if (remainder > 0 && id >= remainder) {
     unsigned chunkSize2 = chunkSize1 - 1;
     unsigned remainderBase = remainder * chunkSize1;
     unsigned processOffset = (id-remainder) * chunkSize2;
     begin = remainderBase + processOffset;
     end = begin + chunkSize2;
   } 
   // pass back this proc's begin and end values via start and stop
   *start = begin;
   *stop = end;
}

