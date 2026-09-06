//
// Put your kernels in this header file.
//

__global__ void apply_1D_Gaussian_wrap_device_ORIGINAL(const int* d_image_in,
    int* d_image_out, int N, int kernel_width) {
    // N does not need to be a multiple of blockDim.x!
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= N) return;

    int half_width = kernel_width / 2;
    float sum = 0.0f;

    for (int j = -half_width; j <= half_width; j++) {
        int index = i + j;
        if (index < 0){
            index += N;
        }
        else if (index >= N){
            index -= N;
        }
        sum += d_image_in[index] * d_kernel_weights[j + half_width];
    }
    d_image_out[i] = static_cast<int>(sum + 0.5f);
}

__global__ void apply_1D_Gaussian_wrap_device_SHARED(const int* d_image_in, int* d_image_out, int N,
    int kernel_width) {
    // N does not need to be a multiple of blockDim.x!
    extern __shared__ int s_image_in[];

    int half_width = kernel_width / 2;
    int block_start = blockIdx.x * blockDim.x;
    int i = block_start + threadIdx.x;

    s_image_in[threadIdx.x + half_width] = d_image_in[i < N ? i : i - N];

    if (threadIdx.x < half_width) {
        int left_global = block_start - half_width + threadIdx.x;
        if (left_global < 0) left_global += N;
        s_image_in[threadIdx.x] = d_image_in[left_global];
    }

    if (threadIdx.x < half_width) {
        int right_global = block_start + blockDim.x + threadIdx.x;
        if (right_global >= N) right_global -= N;
        s_image_in[half_width + blockDim.x + threadIdx.x] = d_image_in[right_global];
    }
     
    __syncthreads();

    if (i >= N) return;

    float sum = 0.0f;
    for (int j = 0; j < kernel_width; j++) {
        sum += s_image_in[threadIdx.x + j] * d_kernel_weights[j];
    }
    d_image_out[i] = static_cast<int>(sum + 0.5f);
}

__global__ void apply_1D_Gaussian_wrap_device_STRIDE(const int* d_image_in, int* d_image_out, int N,
    int kernel_width) {
    // N does not need to be a multiple of blockDim.x!
    int half_width = kernel_width / 2;
    int stride = blockDim.x * gridDim.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;

    while (i < N) {
        float sum = 0.0f;
        for (int j = -half_width; j <= half_width; j++) {
            int index = i + j;
            if (index < 0){
                index += N;
            }
            else if (index >= N){
                index -= N;
            }
            sum += d_image_in[index] * d_kernel_weights[j + half_width];
        }
        d_image_out[i] = static_cast<int>(sum + 0.5f);
        i += stride;
    }
}
