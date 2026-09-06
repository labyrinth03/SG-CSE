 /* 
 * 
 * HW1: 1D Gaussian Filtering   
 *
 */

//
// Written for Sogang University CSEG483/CSE5483
//

#define NUMBER_OF_TESTS_ON_GPU 10
#define NUMBER_OF_ELEMENTS (1 << 26)
#define THREADS_PER_BLOCK 1024
#define GRID_REDUCTION_FACTOR 1

#include <stdio.h>
#include <cmath>
#include <random>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include "measure_host_time_3.h"

const float GAUSSIAN_KERNEL_1D[36] = {
    1.0f,
    0.25f, 0.5f, 0.25f,
    0.0625f, 0.2500f, 0.3750f, 0.2500f, 0.0625f,
	0.015625f, 0.09375f, 0.234375f, 0.3125f, 0.234375f, 0.09375f, 0.015625f,
    0.0039f, 0.0312f, 0.1094f, 0.2188f, 0.2734f, 0.2188f, 0.1094f, 0.0312f, 0.0039f,
    0.000977f, 0.009766f, 0.043945f, 0.117188f, 0.205078f, 0.246094f, 0.205078f, 0.117188f, 0.043945f, 0.009766f, 0.000977f
};

void gen_uniform_distribution_int(int* array, int a, int b, int N) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(a, b);

    for (int i = 0; i < N; i++)  
        array[i] = dist(gen);
}

#define _MAX_KERNEL_SIZE_DEVICE 11
__constant__ float d_kernel_weights[_MAX_KERNEL_SIZE_DEVICE];

void apply_1D_Gaussian_host(const int* image_in, int* image_out, int N, int kernel_width) {
    int half_width = kernel_width / 2;
    const float* kernel = GAUSSIAN_KERNEL_1D + half_width * half_width;

    for (int i = 0; i < N; i++) {
        float sum = 0.0f;

        for (int j = -half_width; j <= half_width; j++) {
            int index = i + j;
            // boarder handling: mirror reflection
            if (index < 0) {
                index = -index;
            }
            else if (index >= N) {
                index = 2 * N - 2 - index;
            }
            sum += image_in[index] * kernel[j + half_width];
        }
		image_out[i] = (int)(sum + 0.5f); // looks faster than std::round(), and it is sufficient for this application  
        // image_out[i] = static_cast<int>(std::round(sum));
    }
}

__global__ void apply_1D_Gaussian_device(const int* image_in, int* image_out, int N, int kernel_width) {
    for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < N; i += blockDim.x * gridDim.x) {
        int half_width = kernel_width / 2;
        float sum = 0.0f;

        for (int j = -half_width; j <= half_width; j++) {
            int index = i + j;

            if (index < 0) {
                index = -index;
            }
            else if (index >= N) {
                index = 2 * N - 2 - index;
            }
            sum += image_in[index] * d_kernel_weights[j + half_width];
        }
        image_out[i] = (int)(sum + 0.5f);
    }
}


int main(void) {
	int n_elements = NUMBER_OF_ELEMENTS;
    size_t size = n_elements * sizeof(int);
   
    // Allocate the host memory
    int* h_image_in = (int*)malloc(size);
    int* h_image_out_host = (int*)malloc(size);
    int* h_image_out_device = (int*)malloc(size);
    if (h_image_in == NULL || h_image_out_host == NULL || h_image_out_device == NULL) {
        fprintf(stderr, "^^^ Failed to allocate host vectors!\n");
        exit(EXIT_FAILURE);
    }

    gen_uniform_distribution_int(h_image_in, 0, 255, n_elements);
    // Allocate the device memory
    int* d_image_in = NULL;
    cudaMalloc((void**)&d_image_in, size);
    int* d_image_out = NULL;
    cudaMalloc((void**)&d_image_out, size);



 //   int kernel_width = GF_KERNEL_WIDTH; // 1, 3, 5, 7, 9, or 11  
    for (int kernel_width = 1; kernel_width <= _MAX_KERNEL_SIZE_DEVICE; kernel_width += 2) {
        fprintf(stdout, "\n[1D Gaussian filtering of image of %d pixels(KERNEL WIDTH = %d)]\n",
            n_elements, kernel_width);
        // Add vectors on the host
        CHECK_TIME_START(_start, _freq);
        apply_1D_Gaussian_host(h_image_in, h_image_out_host, n_elements, kernel_width);
        CHECK_TIME_END(_start, _end, _freq, _compute_time);
        fprintf(stdout, "\n^^^ Time to filter an image of %d pixels on the host = %.3f(ms)\n",
            n_elements, _compute_time);
		float host_time = _compute_time;

		cudaFree(0);  
        // Call to cudaFree(0) is not necessary, but it is a good practice to initialize the CUDA runtime 
        // before starting the timer, to avoid including the initialization time in the measurement.   
        int half_width = kernel_width / 2;
        // Copy the kernel weights to the constant memory on the device
        cudaMemcpyToSymbol(d_kernel_weights,
            (const float*)GAUSSIAN_KERNEL_1D + half_width * half_width,
            kernel_width * sizeof(float));
        
        cudaDeviceSynchronize(); // actually not necessary here, but it is a good practice to ensure that all preceding CUDA calls have completed before proceeding, especially when measuring time.
        // Launch the 1D GF kernel
        int threads_per_block = THREADS_PER_BLOCK;
        int blocks_per_grid = (n_elements + threads_per_block - 1) / threads_per_block;
        int blocks_per_grid_reduced = blocks_per_grid / GRID_REDUCTION_FACTOR;
        fprintf(stdout, "\n^^^ CUDA kernel launch with %d(%d) blocks of %d threads\n", blocks_per_grid_reduced, blocks_per_grid,
            threads_per_block);

		// Do something here!!!

        CHECK_CUDA_ERROR(cudaFree(0));

		cudaMemcpy(d_image_in, h_image_in, size, cudaMemcpyHostToDevice);

        apply_1D_Gaussian_device << < blocks_per_grid_reduced, threads_per_block >> > (d_image_in, d_image_out, n_elements, kernel_width);

		CHECK_TIME_START(_start, _freq);
		apply_1D_Gaussian_device << < blocks_per_grid_reduced, threads_per_block >> > (d_image_in, d_image_out, n_elements, kernel_width);
		CHECK_CUDA_ERROR(cudaGetLastError());
		CHECK_CUDA_ERROR(cudaDeviceSynchronize());
		CHECK_TIME_END(_start, _end, _freq, _compute_time);
		fprintf(stdout, "\n^^^ Time to filter an image of %d pixels on the device = %.3f(ms)\n",
			n_elements, _compute_time);


        CHECK_TIME_START(_start, _freq);
        for (int i = 0; i < NUMBER_OF_TESTS_ON_GPU; i++) {
			apply_1D_Gaussian_device << < blocks_per_grid_reduced, threads_per_block >> > (d_image_in, d_image_out, n_elements, kernel_width);
        }
		cudaDeviceSynchronize();
		CHECK_TIME_END(_start, _end, _freq, _compute_time);
        fprintf(stdout, "\n^^^ Average time to filter an image of %d pixels on the device = %.3f(ms)\n",
			n_elements, _compute_time / NUMBER_OF_TESTS_ON_GPU);
        CHECK_CUDA_ERROR(cudaGetLastError());


        
        // Verify that the result from the device is correct
        // How?

		fprintf(stdout, "\n^^^ Verifying the result from the device... ");
		CHECK_CUDA_ERROR(cudaMemcpy(h_image_out_device, d_image_out, size, cudaMemcpyDeviceToHost));

        for (int i = 0; i < n_elements; i++) {
            if (abs(h_image_out_host[i] - h_image_out_device[i])> 1) {
				fprintf(stdout, "Host : %d, Device : %d\n", h_image_out_host[i], h_image_out_device[i]);
                fprintf(stderr, "^^^ Result verification failed at element %d!\n", i);
                exit(EXIT_FAILURE);
            }
        }
        fprintf(stdout, "\n\n^^^ Test PASSED\n");

        //// method 2
        if (kernel_width == 11) {

            fprintf(stdout, "\n^^^ --------------------------- [Method 2] Started ---------------------------\n");
            int half_width = kernel_width / 2;
            cudaMemcpyToSymbol(d_kernel_weights,
                (const float*)GAUSSIAN_KERNEL_1D + half_width * half_width,
                kernel_width * sizeof(float));

            int threads_per_block = THREADS_PER_BLOCK;
            int blocks_per_grid = (n_elements + threads_per_block - 1) / threads_per_block;

            for (int i = 1; i <= 8; i *= 2) {
                int blocks_per_grid_reduced = blocks_per_grid / i;

                fprintf(stdout, "\n--- GRID_REDUCTION_FACTOR = %d ---\n", i);
                fprintf(stdout, "\n^^^ CUDA kernel launch with %d(%d) blocks of %d threads\n", blocks_per_grid_reduced, blocks_per_grid, threads_per_block);

                CHECK_CUDA_ERROR(cudaFree(0));

                cudaMemcpy(d_image_in, h_image_in, size, cudaMemcpyHostToDevice);

                apply_1D_Gaussian_device << < blocks_per_grid_reduced, threads_per_block >> > (d_image_in, d_image_out, n_elements, kernel_width);

                CHECK_TIME_START(_start, _freq);
                apply_1D_Gaussian_device << < blocks_per_grid_reduced, threads_per_block >> > (d_image_in, d_image_out, n_elements, kernel_width);
                CHECK_CUDA_ERROR(cudaGetLastError());
                CHECK_CUDA_ERROR(cudaDeviceSynchronize());
                CHECK_TIME_END(_start, _end, _freq, _compute_time);
                fprintf(stdout, "\n^^^ Time to filter an image of %d pixels on the device = %.3f(ms)\n",
                    n_elements, _compute_time);


                CHECK_TIME_START(_start, _freq);
                for (int i = 0; i < NUMBER_OF_TESTS_ON_GPU; i++) {
                    apply_1D_Gaussian_device << < blocks_per_grid_reduced, threads_per_block >> > (d_image_in, d_image_out, n_elements, kernel_width);
                }
                cudaDeviceSynchronize();
                CHECK_TIME_END(_start, _end, _freq, _compute_time);
                fprintf(stdout, "\n^^^ Average time to filter an image of %d pixels on the device = %.3f(ms)\n",
                    n_elements, _compute_time / NUMBER_OF_TESTS_ON_GPU);
                CHECK_CUDA_ERROR(cudaGetLastError());



                // Verify that the result from the device is correct
                // How?

                fprintf(stdout, "\n^^^ Verifying the result from the device... ");
                CHECK_CUDA_ERROR(cudaMemcpy(h_image_out_device, d_image_out, size, cudaMemcpyDeviceToHost));

                for (int i = 0; i < n_elements; i++) {
                    if (abs(h_image_out_host[i] - h_image_out_device[i]) > 1) {
                        fprintf(stdout, "Host : %d, Device : %d\n", h_image_out_host[i], h_image_out_device[i]);
                        fprintf(stderr, "^^^ Result verification failed at element %d!\n", i);
                        exit(EXIT_FAILURE);
                    }
                }
                fprintf(stdout, "\n\n^^^ Test PASSED\n");
            }


        }
    }

	// Free device memory   
	cudaFree(d_image_in);
	cudaFree(d_image_out);  

    // Free host memory
    free(h_image_in);
    free(h_image_out_host);
    free(h_image_out_device);

    fprintf(stdout, "\n^^^ Done\n");
    return 0;
}