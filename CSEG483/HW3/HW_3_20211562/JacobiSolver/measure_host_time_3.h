#pragma once
/******************************************************************************************************/
#include <Windows.h>
static __int64 _start, _freq, _end;
static float _compute_time;
#define CHECK_TIME_START(start,freq) QueryPerformanceFrequency((LARGE_INTEGER*)&freq); QueryPerformanceCounter((LARGE_INTEGER*)&start)
#define CHECK_TIME_END(start,end,freq,time) QueryPerformanceCounter((LARGE_INTEGER*)&end); time = (float)((float)(end - start) / (freq * 1.0e-3f))
/******************************************************************************************************/

// CHECK_TIME_START(_start, _freq);
// CHECK_TIME_END(_start, _end, _freq, _compute_time);

static void report_potential_occupancy(void* kernel, int block_size, size_t shared_mem) {
    int device;
    cudaDeviceProp prop;
    int num_blocks, num_active_warps, max_num_warps;

    cudaGetDevice(&device);
    cudaGetDeviceProperties(&prop, device);
    cudaOccupancyMaxActiveBlocksPerMultiprocessor(&num_blocks, kernel, block_size, shared_mem);

    num_active_warps = num_blocks * block_size / prop.warpSize;
    max_num_warps = prop.maxThreadsPerMultiProcessor / prop.warpSize;

    fprintf(stdout, "\n$$$ Maximum possible warps per SM = %d warps (%d threads)\n",
        prop.maxThreadsPerMultiProcessor / prop.warpSize, prop.maxThreadsPerMultiProcessor);
    fprintf(stdout, "$$$ Active blocks per SM = %d ", num_blocks);
    fprintf(stdout, "--> Active warps per SM = %d (%d threads)\n", num_active_warps, num_blocks * block_size);
    fprintf(stdout, "$$$ Occupancy = %.3f\n\n", (float)num_active_warps / max_num_warps);
}

#define CHECK_CUDA_ERROR(call)                                                         \
    do {                                                                                            \
         cudaError_t error = call;                                                              \
         if (error != cudaSuccess) {                                                            \
           fprintf(stderr, "### CUDA function call error: %s (%s) at %s:%d\n",     \
                    cudaGetErrorName(error), cudaGetErrorString(error),               \
                    __FILE__, __LINE__);                                                           \
           exit(EXIT_FAILURE);                                                                   \
         }                                                                                             \
    } while (0)

#define CHECK_CUDA_LAST_ERROR()                                                         \
    do {                                                                                               \
        cudaError_t error = cudaGetLastError();                                              \
        if (error != cudaSuccess) {                                                               \
            fprintf(stderr, "### CUDA last error: %s (%s) at %s:%d\n",                 \
                    cudaGetErrorName(error), cudaGetErrorString(error),                  \
                    __FILE__, __LINE__);                                                             \
            exit(EXIT_FAILURE);                                                                     \
        }                                                                                                \
    } while (0)