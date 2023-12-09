/* ringMessagePassing.c
 *
 * A program that demonstrates the use of MPI to pass a message around a ring of processes.
 * Each process in the ring receives a message from the previous process, appends its rank to the message,
 * and passes it to the next process. The master process starts the timer, initiates the message passing, 
 * and calculates the time taken for the message to traverse the entire ring once it's received back from the last process.
 *
 * Author: Yuese Li
 * Institution: Calvin University
 * Course: CS374 (High Performance Computing)
 * Date: November 6, 2023
 * Purpose: To demonstrate the master-worker pattern in parallel computing using MPI, 
 *          and to measure the time taken for a message to be passed around a ring of processes.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    MPI_Status status;
    double startTime, endTime;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // The buffer size is large enough to hold a message with all ranks
    // Each rank needs up to 3 characters, plus a space, plus 1 for the null terminator
    int bufferSize = (3 + 1) * size + 1;
    char *message = (char*)malloc(bufferSize);
    memset(message, 0, bufferSize);

    // Master process
    if (rank == 0) {
        // Record the starting time
        startTime = MPI_Wtime();

        // Create a message containing its rank
        sprintf(message, "%d", rank);

        // Send that message to the next process (rank 1)
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);

        // Receive a message from the last worker (rank n-1)
        MPI_Recv(message, bufferSize, MPI_CHAR, size - 1, 0, MPI_COMM_WORLD, &status);

        // Calculate the end time and print the results
        endTime = MPI_Wtime();
        printf("%s\n", message);
        printf("time: %f secs\n", endTime - startTime);
    }
    // Worker process
    else {
        // Receive a message from the process before it in the ring
        MPI_Recv(message, bufferSize, MPI_CHAR, rank - 1, 0, MPI_COMM_WORLD, &status);

        // Append its own rank to the message
        // Buffer size adjusted to hold up to a 3-digit number, a space, and a null terminator
        char newMessage[5];
        sprintf(newMessage, " %d", rank);
        strcat(message, newMessage);

        // Send the new message to the next process in the ring
        int nextRank = (rank + 1) % size;
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, nextRank, 0, MPI_COMM_WORLD);
    }

    // Clean up and finalize MPI
    free(message);
    MPI_Finalize();

    return 0;
}