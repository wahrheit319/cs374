/* parallelLoopStripes.c
 * ... illustrates how to make OpenMP map threads to 
 *	parallel for-loop iterations in 'stripes' instead of chunks
 *	(use only when not accessing arrays/memory).
 *
 * Joel Adams, Calvin College, November 2009.
 *
 * Usage: ./parallelLoopStripes [numThreads]
 * Exercise:
 * - Compile and run
 * - Uncomment schedule clause
 * - Save, compile, run with 1, 2, 3, 4, ... threads 
 * - Compare output to 'Chunks' version
 */

#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    const int REPS = 8;

    printf("\n");
    if (argc > 1) {
        omp_set_num_threads( atoi(argv[1]) );
    }

    #pragma omp parallel for // schedule(static,1)
    for (int i = 0; i < REPS; ++i) {
        printf("Thread %d performed iteration %d\n", 
                     omp_get_thread_num(), i);
    }

    printf("\n");
    return 0;
}

