#include <stdio.h>  // I/O
#include <stdlib.h> // calloc(), exit(), etc.
#include <vector>   // vector
#include "00_MPI_IO.h" // ParallelReader

typedef double Item;

int main(int argc, char *argv[]) {
    const int MASTER = 0;
    int id = -1, numProcs = -1;
    double startTime = 0.0, totalTime = 0.0;
    double chunkSum = 0.0, totalSum = 0.0;

    if (argc != 2) {
        fprintf(stderr, "\n*** Usage: squareAndSum <inputFile> \n\n");
        exit(1);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    startTime = MPI_Wtime();

    ParallelReader<double> reader(argv[1], MPI_DOUBLE, id, numProcs);
    std::vector<double> vec;
    reader.readChunk(vec);
    reader.close();

    for (double i : vec) {
        chunkSum += i * i;
    }

    MPI_Reduce(&chunkSum, &totalSum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

    totalTime = MPI_Wtime() - startTime;

    if (id == MASTER) {
        printf("The sum of the squares of the values in the file '%s' is %g\n", argv[1], totalSum);
        printf("Total time: %f secs\n", totalTime);
    }

    MPI_Finalize();
    return 0;
}

// Function to sum the squares of the values in an array of numeric Items
Item arraySquareAndSum(Item *a, int numValues) {
    Item result = 0.0;
    for (int i = 0; i < numValues; ++i) {
        result += (a[i] * a[i]);
    }
    return result;
}
