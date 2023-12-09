/* firestarter.c 
 * David Joiner
 * Usage: Fire [forestSize(20)] [numTrials(5000)] * [numProbabilities(101)] [showGraph(1)]
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "X-graph.h"

#define UNBURNT 0
#define SMOLDERING 1
#define BURNING 2
#define BURNT 3

typedef int boolean;

extern void seed_by_time(int);
extern int **allocate_forest(int);
extern void initialize_forest(int, int **);
extern double get_percent_burned(int, int **);
extern void delete_forest(int, int **);
extern void light_tree(int, int **, int, int);
extern boolean forest_is_burning(int, int **);
extern void forest_burns(int, int **, double);
extern int burn_until_out(int, int **, double, int, int);
extern void print_forest(int, int **);

int main(int argc, char **argv) {
    // Initialize MPI
    MPI_Init(&argc, &argv);
    int id, numProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

    // Start timing
    double start_time = MPI_Wtime();

    // Initial conditions and variable definitions
    int forest_size = 20; // This can be modified based on your project's requirements
    double prob_min = 0.0, prob_max = 1.0, prob_step;
    int n_trials = 5000, n_probs = 101;
    //int do_display = 1; // Set to 0 to skip displaying the graph

    // Allocate memory for arrays
    double *prob_spread = (double *)malloc(n_probs * sizeof(double));
    double *local_percent_burned = (double *)calloc(n_probs, sizeof(double));
    double *global_percent_burned = (double *)calloc(n_probs, sizeof(double));
    double *local_iterations = (double *)calloc(n_probs, sizeof(double));
    double *global_iterations = (double *)calloc(n_probs, sizeof(double));

    // Setup problem
    seed_by_time(0);
    int **forest = allocate_forest(forest_size);
    prob_step = (prob_max - prob_min) / (double)(n_probs - 1);

    // Parallel computation
    for (int i_prob = 0; i_prob < n_probs; i_prob++) {
        prob_spread[i_prob] = prob_min + (double)i_prob * prob_step;
        for (int i_trial = id; i_trial < n_trials; i_trial += numProcesses) {
            int iterations = burn_until_out(forest_size, forest, prob_spread[i_prob], forest_size / 2, forest_size / 2);
            local_iterations[i_prob] += iterations;
            local_percent_burned[i_prob] += get_percent_burned(forest_size, forest);
        }
    }

    // MPI reduction to aggregate results
    MPI_Reduce(local_percent_burned, global_percent_burned, n_probs, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(local_iterations, global_iterations, n_probs, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Normalize and print results
    if (id == 0) {
        printf("Probability, Average Percent Burned, Average Iterations\n");
        for (int i_prob = 0; i_prob < n_probs; i_prob++) {
            global_percent_burned[i_prob] /= n_trials;
            global_iterations[i_prob] /= n_trials;
            printf("%lf, %lf, %lf\n", prob_spread[i_prob], global_percent_burned[i_prob], global_iterations[i_prob]);
        }
    }

    // End timing and print execution time
    double end_time = MPI_Wtime();
    if (id == 0) {
        printf("Execution Time: %f seconds\n", end_time - start_time);
    }

    // Cleanup
    delete_forest(forest_size, forest);
    free(prob_spread);
    free(local_percent_burned);
    free(global_percent_burned);
    free(local_iterations);
    free(global_iterations);

    // Finalize MPI
    MPI_Finalize();
    return 0;
}



#include <time.h>

void seed_by_time(int offset) {
    time_t the_time;
    time(&the_time);
    srand((int)the_time+offset);
}

int burn_until_out(int forest_size, int ** forest, double prob_spread, int start_i, int start_j) {
    int count = 0;
    initialize_forest(forest_size, forest);
    light_tree(forest_size, forest, start_i, start_j);

    while (forest_is_burning(forest_size, forest)) {
        forest_burns(forest_size, forest, prob_spread);
        count++;
    }
    return count; // Return the iteration count
}

double get_percent_burned(int forest_size,int ** forest) {
    int i,j;
    int total = forest_size*forest_size-1;
    int sum=0;

    // calculate pecrent burned
    for (i=0;i<forest_size;i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNT) {
                sum++;
            }
        }
    }

    // return percent burned;
    return ((double)(sum-1)/(double)total);
}


int ** allocate_forest(int forest_size) {
    int i;
    int ** forest;

    forest = (int **) malloc (sizeof(int*)*forest_size);
    for (i=0;i<forest_size;i++) {
        forest[i] = (int *) malloc (sizeof(int)*forest_size);
    }

    return forest;
}

void initialize_forest(int forest_size, int ** forest) {
    int i,j;

    for (i=0;i<forest_size;i++) {
        for (j=0;j<forest_size;j++) {
            forest[i][j]=UNBURNT;
        }
    }
}

void delete_forest(int forest_size, int ** forest) {
    int i;

    for (i=0;i<forest_size;i++) {
        free(forest[i]);
    }
    free(forest);
}

void light_tree(int forest_size, int ** forest, int i, int j) {
    forest[i][j]=SMOLDERING;
}

boolean fire_spreads(double prob_spread) {
    return (double)rand() / (double)RAND_MAX < prob_spread;
}

void forest_burns(int forest_size, int **forest,double prob_spread) {
    int i,j;
    extern boolean fire_spreads(double);

    //burning trees burn down, smoldering trees ignite
    for (i=0; i<forest_size; i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNING) forest[i][j]=BURNT;
            if (forest[i][j]==SMOLDERING) forest[i][j]=BURNING;
        }
    }

    //unburnt trees catch fire
    for (i=0; i<forest_size; i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNING) {
                if (i!=0) { // North
                    if (fire_spreads(prob_spread)&&forest[i-1][j]==UNBURNT) {
                        forest[i-1][j]=SMOLDERING;
                    }
                }
                if (i!=forest_size-1) { //South
                    if (fire_spreads(prob_spread)&&forest[i+1][j]==UNBURNT) {
                        forest[i+1][j]=SMOLDERING;
                    }
                }
                if (j!=0) { // West
                    if (fire_spreads(prob_spread)&&forest[i][j-1]==UNBURNT) {
                        forest[i][j-1]=SMOLDERING;
                    }
                }
                if (j!=forest_size-1) { // East
                    if (fire_spreads(prob_spread)&&forest[i][j+1]==UNBURNT) {
                        forest[i][j+1]=SMOLDERING;
                    }
                }
            }
        }
    }
}

boolean forest_is_burning(int forest_size, int **forest) {
    int i, j;
    for (i = 0; i < forest_size; i++) {
        for (j = 0; j < forest_size; j++) {
            if (forest[i][j] == SMOLDERING || forest[i][j] == BURNING) {
                return True;
            }
        }
    }
    return False;
}

void print_forest(int forest_size,int ** forest) {
    int i,j;

    for (i=0;i<forest_size;i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNT) {
                printf(".");
            } else {
                printf("X");
            }
        }
        printf("\n");
    }
}