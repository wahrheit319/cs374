/* parallelLoopChunks.c
 * ... illustrates the use of OpenMP's default parallel for loop
 *  	in which threads iterate through blocks of the index range
 *	(cache-beneficial when accessing adjacent memory locations).
 *
 * Joel Adams, Calvin College, November 2009.
 * Usage: ./parallelLoopChunks [numThreads]
 * Exercise
 * - Compile, run 
 * - Uncomment #pragma, recompile
 * - Run using different numbers of threads
 * - Trace and compare executions
 */

#include <stdio.h>    // printf()
#include <stdlib.h>   // atoi()
#include <omp.h>      // OpenMP

int main(int argc, char** argv) {
    const int REPS = 8;

    printf("\n");
    if (argc > 1) {
        omp_set_num_threads( atoi(argv[1]) );
    }

//    #pragma omp parallel for 
    for (int i = 0; i < REPS; ++i) {
        printf("Thread %d performed iteration %d\n", 
                 omp_get_thread_num(), i);
    }

    printf("\n");
    return 0;
}

