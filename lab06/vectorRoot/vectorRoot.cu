/**
 * Vector addition: C = A + B.
 *
 * This sample is a very basic sample that implements element by element
 * vector addition. It is the same as the sample illustrating Chapter 2
 * of the programming guide with some additions like error checking.
 *
 * Copyright 1993-2013 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 * Extended for use in CS 374 at Calvin University by Joel C. Adams,
 *   adding error-handling, verification, and sequential comparison.
 */

#include <stdio.h>
#include <cuda_runtime.h> // CUDA runtime routines (prefixed with "cuda_")
#include <omp.h>

/**
 * CUDA Kernel Device code
 * Computes the vector addition of A and B into C.
 * The 3 vectors have the same number of elements numElements.
 */
__global__ void vectorRoot(const float *A, float *C, const unsigned long numElements)
{
    unsigned long i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < numElements)
    {
        C[i] = sqrtf(A[i]);
    }
}

/**
 * Utility to clean up error checking
 * It takes a CUDA error code and an error message to print
 *  if the error code is not cudaSuccess
 */
void checkErr(cudaError_t err, const char *msg)
{
    if (err != cudaSuccess)
    {
        fprintf(stderr, "\n*** %s (error code %d: '%s')!\n\n",
                msg, err, cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }
}

/**
 * Host main routine
 */
int main(int argc, char **argv)
{
    // CUDA error code to check return values of CUDA calls
    cudaError_t err = cudaSuccess;

    // Timing variables
    double start, end;
    double time_copy_to_device = 0.0, time_compute = 0.0, time_copy_to_host = 0.0, time_cuda_total = 0.0;
    double time_sequential = 0.0;

    // Get the desired vector length
    unsigned long numElements = 20000; // default value
    if (argc > 1)
    {
        numElements = strtoul(argv[1], 0, 10);
    }

    // Provide initial user feedback...
    printf("\nVector addition of %lu elements using CUDA\n", numElements);

    // Allocate the host input vectors A & B
    size_t size = numElements * sizeof(float);
    float *h_A = (float *)malloc(size);
    float *h_B = (float *)malloc(size);

    // Allocate the host output vector C
    float *h_C = (float *)malloc(size);

    // Verify that host allocations succeeded
    if (h_A == NULL || h_B == NULL || h_C == NULL)
    {
        fprintf(stderr, "Unable to allocate host vectors!\n");
        exit(EXIT_FAILURE);
    }

    // Initialize host input vectors to floats in range 0.0..1.0
    for (int i = 0; i < numElements; ++i)
    {
        h_A[i] = rand() / (float)RAND_MAX;
        h_B[i] = rand() / (float)RAND_MAX;
    }

    // 1a. Allocate the device input vectors A & B
    float *d_A = NULL;
    err = cudaMalloc((void **)&d_A, size);
    checkErr(err, "Unable to allocate device vector A");
    float *d_B = NULL;
    err = cudaMalloc((void **)&d_B, size);
    checkErr(err, "Unable to allocate device vector B");

    // 1.b. Allocate the device output vector C
    float *d_C = NULL;
    err = cudaMalloc((void **)&d_C, size);
    checkErr(err, "Unable to allocate device vector C");

    // 2. Copy the host input vectors A and B in host memory to the device input vectors in device memory
    start = omp_get_wtime();
    err = cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
    checkErr(err, "Unable to copy device vector A from host to device");
    err = cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);
    checkErr(err, "Unable to copy device vector B from host to device");
    end = omp_get_wtime();
    time_copy_to_device = end - start;

    // 3. Launch the CUDA Kernel
    int threadsPerBlock = 256;
    int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;
    start = omp_get_wtime();
    vectorRoot<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_C, numElements);

    err = cudaGetLastError();
    checkErr(err, "Unable to launch vectorAdd kernel");
    end = omp_get_wtime();
    time_compute = end - start;

    // 4. Copy the device result vector in device memory to the host result vector in host memory.
    start = omp_get_wtime();
    err = cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);
    checkErr(err, "Unable to copy vector C from device to host");
    end = omp_get_wtime();
    time_copy_to_host = end - start;

    // Calculate total CUDA time
    time_cuda_total = time_copy_to_device + time_compute + time_copy_to_host;

    // Inside main(), update the verification loop
    const float ACCEPT_THRESHOLD = 1.0e-6;
    for (int i = 0; i < numElements; ++i)
    {
        if (fabs(sqrtf(h_A[i]) - h_C[i]) > ACCEPT_THRESHOLD)
        {
            fprintf(stderr, "Result verification failed at element %d!\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Free device global memory
    err = cudaFree(d_A);
    checkErr(err, "Unable to free device vector A");
    err = cudaFree(d_B);
    checkErr(err, "Unable to free device vector B");
    err = cudaFree(d_C);
    checkErr(err, "Unable to free device vector C");

    // Repeat the computation sequentially with timing
    float *test_C = (float *)malloc(size);
    if (test_C == NULL)
    {
        fprintf(stderr, "Unable to allocate test-result vector!\n");
        exit(EXIT_FAILURE);
    }

    start = omp_get_wtime();
    // Inside main(), update the sequential computation loop
    for (int i = 0; i < numElements; ++i)
    {
        test_C[i] = sqrtf(h_A[i]);
    }

    end = omp_get_wtime();
    time_sequential = end - start;

    // Verify CUDA against sequential computation
    for (int i = 0; i < numElements; ++i)
    {
        if (fabs(test_C[i] - h_C[i]) > ACCEPT_THRESHOLD)
        {
            fprintf(stderr, "Result verification failed at element %d!\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Free host memory
    free(h_A);
    free(h_B);
    free(h_C);
    free(test_C);

    // Display the timing results
    printf("CUDA Computation Timings:\n");
    printf("\tTime copying to device:\t%lf seconds\n", time_copy_to_device);
    printf("\tTime for computation:\t%lf seconds\n", time_compute);
    printf("\tTime copying to host:\t%lf seconds\n", time_copy_to_host);
    printf("\tTotal CUDA time:\t%lf seconds\n\n", time_cuda_total);

    printf("Sequential Computation Time:\n");
    printf("\tTotal Sequential time:\t%lf seconds\n", time_sequential);

    // Reset the device and exit
    err = cudaDeviceReset();
    checkErr(err, "Unable to reset device");

    printf("\nProgram complete\n\n");
    return 0;
}
