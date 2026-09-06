#include <cuda_runtime.h>
#include <mma.h>
#include <stdio.h>
#include <stdlib.h>

#include "Jacobi.h"

namespace {

constexpr int TILE = 16;
constexpr int REDUCE_THREADS = 256;
constexpr int OP_FMA = 0;
constexpr int OP_RESIDUAL = 1;

// CUDA 호출 결과를 확인하는 매크로
#define CUDA_CHECK(call)                                                                  \
    do {                                                                                  \
        cudaError_t error = (call);                                                       \
        if (error != cudaSuccess) {                                                       \
            fprintf(stderr, "CUDA error: %s (%s) at %s:%d\n", cudaGetErrorName(error),   \
                cudaGetErrorString(error), __FILE__, __LINE__);                           \
            exit(EXIT_FAILURE);                                                           \
        }                                                                                 \
    } while (0)



// CUDA Core 공통 matrix multiply kernel
// OP_FMA이면 Out = A * B + C, OP_RESIDUAL이면 Out = A * B - C를 계산
__global__ void matmul_cuda_kernel(float* Out, const float* A, const float* B,
    const float* C, int n, int op) {
    __shared__ float tile_a[TILE][TILE];
    __shared__ float tile_b[TILE][TILE];

    // 현재 thread가 담당하는 output matrix의 원소 위치
    const int row = blockIdx.y * TILE + threadIdx.y;
    const int col = blockIdx.x * TILE + threadIdx.x;

    // CPU matrix_residual()도 double sum을 쓰므로 residual 경로만 double로 계산
    if (op == OP_RESIDUAL) {
        double sum = 0.0;
        for (int t = 0; t < n; t += TILE) {
            tile_a[threadIdx.y][threadIdx.x] = A[row * n + t + threadIdx.x];
            tile_b[threadIdx.y][threadIdx.x] = B[(t + threadIdx.y) * n + col];

            __syncthreads();

#pragma unroll
            for (int k = 0; k < TILE; k++)
                sum += static_cast<double>(tile_a[threadIdx.y][k]) * static_cast<double>(tile_b[k][threadIdx.x]);

            __syncthreads();
        }

        Out[row * n + col] = static_cast<float>(sum - static_cast<double>(C[row * n + col]));
        return;
    }

    float sum = 0.0f;

    
    // A와 B를 TILE x TILE 단위로 Shared Memory에 올리고 inner product를 누적
    for (int t = 0; t < n; t += TILE) {
        tile_a[threadIdx.y][threadIdx.x] = A[row * n + t + threadIdx.x];
        tile_b[threadIdx.y][threadIdx.x] = B[(t + threadIdx.y) * n + col];

        __syncthreads();

        // Shared memoryM에 올라온 두 Tile로 한 출력 원소의 부분합을 계산
#pragma unroll
        for (int k = 0; k < TILE; k++)
            sum = fmaf(tile_a[threadIdx.y][k], tile_b[k][threadIdx.x], sum);

        __syncthreads();
    }

    Out[row * n + col] = (op == OP_FMA) ? (sum + C[row * n + col]) : (sum - C[row * n + col]);
}





// row sum은 항상 0 이상이므로 float bit pattern에 atomicCAS를 적용해 max 값을 갱신
__device__ void atomicMaxFloatNonnegative(float* address, float value) {
    int* address_as_int = reinterpret_cast<int*>(address);
    int old = *address_as_int;


    while (value > __int_as_float(old)) {
        const int assumed = old;
        old = atomicCAS(address_as_int, assumed, __float_as_int(value));

        if (old == assumed)
            break;
    }
}




// Matrix infinity norm 계산 kernel
// block 하나가 row 하나의 abs 합을 구하고 global max를 atomic으로 갱신
__global__ void row_abs_sum_and_max_kernel(const float* matrix, float* norm, int n) {
    __shared__ float scratch[REDUCE_THREADS];
    const int row = blockIdx.x;
    float sum = 0.0f;

    // block당 thread 수는 256개이므로 n이 512 이상이면 한 thread가 여러 column을 더함
    for (int col = threadIdx.x; col < n; col += blockDim.x)
        sum += fabsf(matrix[row * n + col]);

    // block 내부 reduction으로 한 row의 abs 합을 구함
    scratch[threadIdx.x] = sum;

    __syncthreads();

    for (int stride = blockDim.x / 2; stride > 0; stride >>= 1) {
        if (threadIdx.x < stride)
            scratch[threadIdx.x] += scratch[threadIdx.x + stride];

        __syncthreads();

    }

    if (threadIdx.x == 0)
        atomicMaxFloatNonnegative(norm, scratch[0]);
}





// Tensor Core WMMA TF32 kernel.
// A와 B는 fragment 내부에서 TF32 precision으로 변환, accumulator와 출력 D는 FP32
__global__ void wmma_gemm_tf32_kernel(float* D, const float* A, const float* B,
    const float* C, int n) {
    using namespace nvcuda;

    const int row = blockIdx.y * TILE;
    const int col = blockIdx.x * TILE;

    wmma::fragment<wmma::matrix_a, 16, 16, 8, wmma::precision::tf32, wmma::row_major> a_frag;
    wmma::fragment<wmma::matrix_b, 16, 16, 8, wmma::precision::tf32, wmma::row_major> b_frag;
    wmma::fragment<wmma::accumulator, 16, 16, 8, float> acc_frag;



    wmma::load_matrix_sync(acc_frag, C + row * n + col, n, wmma::mem_row_major);

    // TF32 WMMA는 k dimension이 8이므로 k를 8씩 이동하며 누적
    for (int k = 0; k < n; k += 8) {
        wmma::load_matrix_sync(a_frag, A + row * n + k, n);
        wmma::load_matrix_sync(b_frag, B + k * n + col, n);



        for (int i = 0; i < a_frag.num_elements; i++)
            a_frag.x[i] = wmma::__float_to_tf32(a_frag.x[i]);
        for (int i = 0; i < b_frag.num_elements; i++)
            b_frag.x[i] = wmma::__float_to_tf32(b_frag.x[i]);

        wmma::mma_sync(acc_frag, a_frag, b_frag, acc_frag);



    }

    wmma::store_matrix_sync(D + row * n + col, acc_frag, n, wmma::mem_row_major);
}






// device에 있는 matrix의 infinity norm 계산
// norm 값은 kernel 실행 전 0으로 초기화하고 각 row block이 atomic으로 최대 row sum을 갱신
float infinity_norm_on_device(float* d_norm, const float* d_matrix, int n) {
    float norm = 0.0f;
    CUDA_CHECK(cudaMemset(d_norm, 0, sizeof(float)));


    row_abs_sum_and_max_kernel<<<n, REDUCE_THREADS>>>(d_matrix, d_norm, n);


    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaMemcpy(&norm, d_norm, sizeof(float), cudaMemcpyDeviceToHost));
    return norm;
}






// CUDA Core와 Tensor Core Jacobi 반복에서 공통으로 사용하는 실행 함수
// use_tensor_cores가 false면 CUDA Core tiled GEMM을 사용하고 true면 Tensor Core TF32 WMMA GEMM을 사용
void run_jacobi(bool use_tensor_cores, float* B, float* D, float* R, const float* A,
    const float* C, const float* U, const float* V, size_t matrix_size, int max_iterations,
    float epsilon, float v_norm, int* completed_iterations, float* final_relative_residual_norm,
    int* status_code) {
    const int n = static_cast<int>(matrix_size);
    const size_t bytes = static_cast<size_t>(n) * n * sizeof(float);
    const dim3 block(TILE, TILE);
    const dim3 grid(n / TILE, n / TILE);

    float* d_A = nullptr;
    float* d_B = nullptr;
    float* d_C = nullptr;
    float* d_D = nullptr;
    float* d_U = nullptr;
    float* d_V = nullptr;
    float* d_R = nullptr;
    float* d_norm = nullptr;

    // Jacobi 반복과 residual 계산에 필요한 matrix를 한 번만 device로 복사
    CUDA_CHECK(cudaMalloc(&d_A, bytes));
    CUDA_CHECK(cudaMalloc(&d_B, bytes));
    CUDA_CHECK(cudaMalloc(&d_C, bytes));
    CUDA_CHECK(cudaMalloc(&d_D, bytes));
    CUDA_CHECK(cudaMalloc(&d_U, bytes));
    CUDA_CHECK(cudaMalloc(&d_V, bytes));
    CUDA_CHECK(cudaMalloc(&d_R, bytes));
    CUDA_CHECK(cudaMalloc(&d_norm, sizeof(float)));

    CUDA_CHECK(cudaMemcpy(d_A, A, bytes, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_B, B, bytes, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_C, C, bytes, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_U, U, bytes, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_V, V, bytes, cudaMemcpyHostToDevice));




    int iteration_count = 0;
    float relative_residual = 0.0f;
    int status = JACOBI_ITERATION_SUCCESS;

    while (true) {
        // stopping criteria: ||U * B - V|| / ||V|| < epsilon
        matmul_cuda_kernel<<<grid, block>>>(d_R, d_U, d_B, d_V, n, OP_RESIDUAL);
        relative_residual = infinity_norm_on_device(d_norm, d_R, n) / v_norm;

        if (iteration_count >= max_iterations) {
            status = JACOBI_ITERATION_FAIL_MAX_ITERATION;
            break;
        }
        if (relative_residual < epsilon)
            break;

        // Jacobi iteration은 매 loop에서 두 번 진행 1) D = A * B + C, 2) B = A * D + C
        if (use_tensor_cores) {
            wmma_gemm_tf32_kernel<<<grid, 32>>>(d_D, d_A, d_B, d_C, n);
            wmma_gemm_tf32_kernel<<<grid, 32>>>(d_B, d_A, d_D, d_C, n);
        }
        else {
            matmul_cuda_kernel<<<grid, block>>>(d_D, d_A, d_B, d_C, n, OP_FMA);
            matmul_cuda_kernel<<<grid, block>>>(d_B, d_A, d_D, d_C, n, OP_FMA);
        }
        CUDA_CHECK(cudaGetLastError());
        iteration_count += 2;
    }



    CUDA_CHECK(cudaMemcpy(B, d_B, bytes, cudaMemcpyDeviceToHost));
    CUDA_CHECK(cudaMemcpy(D, d_D, bytes, cudaMemcpyDeviceToHost));
    CUDA_CHECK(cudaMemcpy(R, d_R, bytes, cudaMemcpyDeviceToHost));

    *completed_iterations = iteration_count;
    *final_relative_residual_norm = relative_residual;
    *status_code = status;

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
    cudaFree(d_D);
    cudaFree(d_U);
    cudaFree(d_V);
    cudaFree(d_R);
    cudaFree(d_norm);
}








} // namespace

// Jacobi_iteration_CC()에서 호출되는 CUDA Core wrapper.
extern "C" void Jacobi_iteration_CC_cuda(float* B, float* D, float* R, const float* A,
    const float* C, const float* U, const float* V, size_t matrix_size, int max_iterations,
    float epsilon, float v_norm, int* completed_iterations, float* final_relative_residual_norm,
    int* status_code) {
    run_jacobi(false, B, D, R, A, C, U, V, matrix_size, max_iterations, epsilon,
        v_norm, completed_iterations, final_relative_residual_norm, status_code);
}

// Jacobi_iteration_TC()에서 호출되는 Tensor Core TF32 wrapper.
extern "C" void Jacobi_iteration_TC_cuda(float* B, float* D, float* R, const float* A,
    const float* C, const float* U, const float* V, size_t matrix_size, int max_iterations,
    float epsilon, float v_norm, int* completed_iterations, float* final_relative_residual_norm,
    int* status_code) {
    run_jacobi(true, B, D, R, A, C, U, V, matrix_size, max_iterations, epsilon,
        v_norm, completed_iterations, final_relative_residual_norm, status_code);
}
