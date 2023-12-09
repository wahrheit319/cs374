/* squareAndSumSeqBinary.c computes the sum of the squares
 *  of the values in an input file,
 *  whose name is given on the command-line.
 *
 * The program is written using typedef to declare
 *  Item as a generic type, currently double.
 *
 * Joel Adams, Fall 2023
 * for CS 374 (HPC) at Calvin University.
 * 
 * author Yuese Li
 * why    Project 4, CS 374
 * where  Calvin University
 * date   Fall 2023
 */

#include <stdio.h>      
#include <stdlib.h>     
#include <mpi.h>        

typedef double Item;

void readArray(char * fileName, Item ** a, int * n);
double arraySquareAndSum(Item* a, int numValues);

int main(int argc, char * argv[])
{
    int howMany, rank, size;
    Item sum, localSum;
    Item *a = NULL, *localA = NULL;
    int *sendCounts, *displs;
    double startTime, fileReadTime, scatterTime, computationTime, totalTime;

    MPI_Init(&argc, &argv); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) {
            fprintf(stderr, "\n*** Usage: squareAndSum <inputFile>\n\n");
        }
        MPI_Finalize();
        exit(1);
    }

    startTime = MPI_Wtime();

    if (rank == 0) {
        double fileReadStartTime = MPI_Wtime();
        readArray(argv[1], &a, &howMany);
        fileReadTime = MPI_Wtime() - fileReadStartTime;

        sendCounts = malloc(size * sizeof(int));
        displs = malloc(size * sizeof(int));

        int remainder = howMany % size;
        int sum = 0;
        for (int i = 0; i < size; i++) {
            sendCounts[i] = howMany / size + (i < remainder);
            displs[i] = sum;
            sum += sendCounts[i];
        }
    }

    double scatterStartTime = MPI_Wtime();
    int localSize;
    MPI_Scatter(sendCounts, 1, MPI_INT, &localSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    localA = malloc(localSize * sizeof(Item));
    MPI_Scatterv(a, sendCounts, displs, MPI_DOUBLE, localA, localSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    scatterTime = MPI_Wtime() - scatterStartTime;

    double computationStartTime = MPI_Wtime();
    localSum = arraySquareAndSum(localA, localSize);
    computationTime = MPI_Wtime() - computationStartTime;

    MPI_Reduce(&localSum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    totalTime = MPI_Wtime() - startTime;

    if (rank == 0) {
        printf("The sum of the squares of the values in the file '%s' is %g\n", argv[1], sum);
        printf("Time taken for file reading: %f seconds\n", fileReadTime);
        printf("Time taken for scattering: %f seconds\n", scatterTime);
        printf("Time taken for computation: %f seconds\n", computationTime);
        printf("Total time taken: %f seconds\n", totalTime);
    }

    free(localA);
    if (rank == 0) {
        free(a);
        free(sendCounts);
        free(displs);
    }

    MPI_Finalize();
    return 0;
}

/* readArray fills an array with Item values from a file.
 * Receive: fileName, a char*,
 *          a, the address of a pointer to an Item array,
 *          n, the address of an int.
 * PRE: fileName contains N, followed by N double values.
 * POST: a points to a dynamically allocated array
 *        containing the N values from fileName
 *        and n == N.
 */

void readArray(char * fileName, Item** a, int * n) {
    FILE * fin;
    long fileSize;
    int howMany;

    fin = fopen(fileName, "rb");  // Open the file in binary read mode
    if (fin == NULL) {
        fprintf(stderr, "\n*** Unable to open input file '%s'\n\n", fileName);
        exit(1);
    }

    // Get the file size
    fseek(fin, 0L, SEEK_END);
    fileSize = ftell(fin);
    rewind(fin);

    // Calculate the number of items
    howMany = fileSize / sizeof(Item);

    // Allocate memory for the array
    *a = (Item *)calloc(howMany, sizeof(Item));
    if (*a == NULL) {
        fprintf(stderr, "\n*** Unable to allocate %d-length array\n", howMany);
        fclose(fin);
        exit(1);
    }

    // Read the values into the array
    fread(*a, sizeof(Item), howMany, fin);
    fclose(fin);

    *n = howMany;
}


/* arraySquareAndSum sums the squares of the values
 *  in an array of numeric Items.
 * Receive: a, a pointer to the head of an array of Items;
 *          numValues, the number of values in the array.
 * Return: the sum of the values in the array.
 */

Item arraySquareAndSum(Item* a, int numValues) {
  Item result = 0.0;

  for (int i = 0; i < numValues; ++i) {
    result += (a[i] * a[i]);
  }

  return result;
}

