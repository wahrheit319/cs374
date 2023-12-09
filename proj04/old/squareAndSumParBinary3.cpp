/* squareAndSumParBinary.cpp computes the sum of the squares
 * of the values in a binary input file in parallel,
 * with the file name provided on the command-line.
 *
 * This version of the program leverages MPI for parallel processing,
 * significantly improving performance for large datasets.
 * It uses C++ features such as vectors and OO_MPI_IO for parallel I/O.
 * Item is typedef-ed as a generic type, currently double.
 *
 * Joel Adams, Fall 2023
 * for CS 374 (HPC) at Calvin University.
 *
 * author Yuese Li
 * why    Project 4, CS 374
 * where  Calvin University
 * date   Fall 2023
 */

#include <stdio.h>     // Standard Input/Output functions
#include <stdlib.h>    // Standard library functions, including exit
#include <vector>      // Use of vector container
#include "OO_MPI_IO.h" // MPI-based Input/Output operations
#include <mpi.h>       // MPI library

typedef double Item; // Defining 'Item' as an alias for double

// Function to sum the squares of the values in a vector of numeric Items
double arraySquareAndSum(const std::vector<Item> &data)
{
    double result = 0.0;
    for (Item val : data)
    {
        result += val * val; // Squaring each value and adding to the result
    }
    return result;
}

int main(int argc, char *argv[])
{
    const int MASTER = 0;                                              // Defining MASTER process for MPI
    int id = -1, numProcs = -1;                                        // Process id and number of processes
    double fileReadTime = 0.0, computationTime = 0.0, totalTime = 0.0; // For timing
    double chunkSum = 0.0, totalSum = 0.0;                             // Sum of chunks and total sum

    // Check command-line arguments
    if (argc != 2)
    {
        fprintf(stderr, "\n*** Usage: squareAndSum <inputFile> \n\n");
        exit(1);
    }

    // Initialize MPI environment
    MPI_Init(&argc, &argv);
    // Get the total number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    // Get the current process ID
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    // Start timing for file reading
    double fileReadStartTime = MPI_Wtime();

    // Reading a chunk of data using ParallelReader
    ParallelReader<Item> reader(argv[1], MPI_DOUBLE, id, numProcs);
    std::vector<Item> vec; // Vector to store the chunk of data
    reader.readChunk(vec); // Read the chunk into the vector
    reader.close();        // Close the reader

    // Stop timing for file reading
    fileReadTime = MPI_Wtime() - fileReadStartTime;

    // Start timing for computation
    double computationStartTime = MPI_Wtime();

    // Compute sum of squares for the chunk
    chunkSum = arraySquareAndSum(vec);

    // Stop timing for computation
    computationTime = MPI_Wtime() - computationStartTime;

    // Reduce operation to sum up the chunks from all processes
    MPI_Reduce(&chunkSum, &totalSum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

    // Calculate total elapsed time
    totalTime = MPI_Wtime() - fileReadStartTime;

    // MASTER process prints the result
    if (id == MASTER)
    {
        printf("The sum of the squares of the values in the file '%s' is %g\n", argv[1], totalSum);
        printf("Time taken for file reading: %f seconds\n", fileReadTime);
        printf("Time taken for computation: %f seconds\n", computationTime);
        printf("Total time: %f secs\n", totalTime);
    }

    // Finalize the MPI environment
    MPI_Finalize();
    return 0;
}
