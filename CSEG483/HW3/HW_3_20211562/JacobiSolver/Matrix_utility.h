#pragma once

void print_square_matrix(const float* __restrict matrix, size_t matrix_size);
void matrix_fma(float* __restrict D, const float* __restrict A, const float* __restrict B,
	const float* __restrict C, size_t matrix_size);
float matrix_infinity_norm(const float* __restrict matrix, size_t matrix_size);
double vector_inner_product_Kahan_sum(const float* __restrict a, const float* __restrict b, size_t size);
void matrix_residual(float* __restrict residual, const float* __restrict A, const float* __restrict B,
	const float* __restrict C, size_t matrix_size);
void matrix_residual_Kahan_sum(float* __restrict residual, float* __restrict A,
	float* __restrict B, float* __restrict C, size_t matrix_size);