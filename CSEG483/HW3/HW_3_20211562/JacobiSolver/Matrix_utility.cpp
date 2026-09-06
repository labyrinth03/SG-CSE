#include <stdio.h>
#include <math.h>
#include <vector>

void print_square_matrix(const float* __restrict matrix, size_t matrix_size) {
	fprintf(stdout, "+++++++++++++++++++++++++++++++++++++++++++++++\n");
	for (size_t i = 0; i < matrix_size; i++) {
		fprintf(stdout, "Row %zu: ", i);
		for (size_t j = 0; j < matrix_size; j++) {
			fprintf(stdout, "%f ", matrix[i * matrix_size + j]);
		}
		fprintf(stdout, "\n");
	}
	fprintf(stdout, "+++++++++++++++++++++++++++++++++++++++++++++++\n\n");
}

// This fuction should be accelerated using CUDA Cores or Tensor Cores.
void matrix_fma(float* __restrict D, const float* __restrict A, const float* __restrict B, const float* __restrict C, size_t matrix_size) {
	// perform the operation D = A * B + C using a fused multiply-add approach	
	for (size_t i = 0; i < matrix_size; i++) {
		for (size_t j = 0; j < matrix_size; j++) {
			double sum = 0.0;
			for (size_t k = 0; k < matrix_size; k++) {
				sum += (double) A[i * matrix_size + k] * B[k * matrix_size + j];
			}
			D[i * matrix_size + j] = sum + C[i * matrix_size + j]; // Store the result in D
		}
	}
}

float matrix_infinity_norm(const float* __restrict matrix, size_t matrix_size) {
	double max_row_sum = 0.0;
	for (size_t i = 0; i < matrix_size; i++) {
		double row_sum = 0.0;
		for (size_t j = 0; j < matrix_size; j++) {
			row_sum += fabs(matrix[i * matrix_size + j]);
		}
		if (row_sum > max_row_sum) {
			max_row_sum = row_sum;
		}
	}
	return max_row_sum;
}

double vector_inner_product_Kahan_sum(const float* __restrict a, const float* __restrict b, size_t size) {
	double sum = 0.0;
	volatile double c = 0.0;
	for (size_t i = 0; i < size; i++) {
		double product = static_cast<double>(a[i]) * static_cast<double>(b[i]);
		volatile double y = product - c;
		volatile double t = sum + y;
		c = (t - sum) - y;
		sum = t;
	}
	return sum;
}

// This fuction should be accelerated using CUDA Cores or Tensor Cores.
void matrix_residual(float* __restrict residual, const float* __restrict A, const float* __restrict B, 
	const float* __restrict C, size_t matrix_size) {
	// compute the residual R = A * B - C.	
	for (size_t i = 0; i < matrix_size; i++) {
		for (size_t j = 0; j < matrix_size; j++) {
			double sum = 0.0f;
			for (size_t k = 0; k < matrix_size; k++) {
				sum += A[i * matrix_size + k] * B[k * matrix_size + j];
			}
			residual[i * matrix_size + j] = sum - C[i * matrix_size + j];
		}
	}
}

void matrix_residual_Kahan_sum(float* __restrict residual, float* __restrict A,
	float* __restrict B, float* __restrict C, size_t matrix_size) {
	// compute the residual R = A * B - C.	
	std::vector<float> B_T(matrix_size * matrix_size);
	for (size_t i = 0; i < matrix_size; i++) {
		for (size_t j = 0; j < matrix_size; j++) {
			B_T[j * matrix_size + i] = B[i * matrix_size + j];
		}
	}
	for (size_t i = 0; i < matrix_size; i++) {
		for (size_t j = 0; j < matrix_size; j++) {
			double dot_product = vector_inner_product_Kahan_sum(&A[i * matrix_size],
				&B_T[j * matrix_size], matrix_size);
			double res_val = dot_product - static_cast<double>(C[i * matrix_size + j]);
			residual[i * matrix_size + j] = static_cast<float>(res_val);
		}
	}
}
