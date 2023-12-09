/* masterOnly.c
 * ... illustrates the master-only pattern in OpenMP
 *
 * Joel Adams, Calvin College, Fall 2023.
 * Usage: ./masterOnly
 * Exercise:
 * - Compile and run as is.
 * - Uncomment #pragma omp parallel directive, re-compile and re-run
 * - Compare and trace the different executions.
 * - Comment out the #pragma omp barrier directive; re-compile 
 *     and re-run multiple times.
 * - How does removing the barrier change the executions?
 * - What is the effect of the #pragma omp master directive?
 */

#include <stdio.h>   // printf()
#include <omp.h>     // OpenMP

int main(int argc, char** argv) {
    printf("\n");

//    #pragma omp parallel 
    {
        int id = omp_get_thread_num();
        int numThreads = omp_get_num_threads();

        printf("Greetings from thread %d of %d threads\n",
                            id, numThreads);

        #pragma omp barrier

        #pragma omp master
        printf("\nThread %d of %d threads is the master!\n",
                            id, numThreads);
    }

    printf("\n");
    return 0;
}

