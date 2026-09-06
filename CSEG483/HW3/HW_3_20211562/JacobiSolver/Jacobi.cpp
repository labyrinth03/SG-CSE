#include <stdio.h>
#include <math.h>
#include <random>
#include "Jacobi.h"
#include "Matrix_utility.h"

#define _USE_KAHAN_SUM 1

extern "C" void Jacobi_iteration_CC_cuda(float* B, float* D, float* R,
	const float* A, const float* C, const float* U, const float* V,
	size_t matrix_size, int max_iterations, float epsilon, float v_norm,
	int* completed_iterations, float* final_relative_residual_norm, int* status_code);

extern "C" void Jacobi_iteration_TC_cuda(float* B, float* D, float* R,
	const float* A, const float* C, const float* U, const float* V,
	size_t matrix_size, int max_iterations, float epsilon, float v_norm,
	int* completed_iterations, float* final_relative_residual_norm, int* status_code);

void Jacobi::prepare_Jacobi_iteration() {
	// prepare the matrices for Jacobi iteration by normalizing the rows of A and C.
	for (size_t i = 0; i < matrix_size; i++) {
		double inverse_diagonal = 1.0 / U[i * matrix_size + i]; // Use the diagonal element for normalization	
		for (size_t j = 0; j < matrix_size; j++) {
			A[i * matrix_size + j] = -U[i * matrix_size + j] * inverse_diagonal; // normalize row i of U to create A.	
			C[i * matrix_size + j] = V[i * matrix_size + j] * inverse_diagonal; // normalize row i of V to create C.	
			if (i == j)
				A[i * matrix_size + j] += 1.0f; // set diagonal of A to 0.	
		}
	}
	// initialize B to the identity matrix for the initial guess of the solution.
	for (size_t i = 0; i < matrix_size; i++) {
		for (size_t j = 0; j < matrix_size; j++) {
			if (i == j)
				B[i * matrix_size + j] = 1.0f; 
			else
				B[i * matrix_size + j] = 0.0f;
		}
	}
}

int Jacobi::finish_Jacobi_iteration(int next_iteration, Status& flag) {
#ifdef _USE_KAHAN_SUM
	matrix_residual_Kahan_sum(R.data(), U.data(), B.data(), V.data(), matrix_size);
#else
	matrix_residual(R.data(), U.data(), B.data(), V.data(), matrix_size);
#endif
	final_relative_residual_norm = matrix_infinity_norm(R.data(), matrix_size) / V_norm;
	if (next_iteration >= stopping_criteria.max_iterations) {
		flag = Status::FAIL_MAX_ITERATION;		
		return 1; // stop if the number of iterations exceeds the maximum allowed.
	}
	if (final_relative_residual_norm < stopping_criteria.epsilon) {
		flag = Status::SUCCESS;
		return 1; // stop if the relative residual norm is small enough.
	}
	return 0;
}

void Jacobi::Jacobi_iteration_HOST(Status& flag) {
	fprintf(stdout, ">>> Trying to solve for X in UX = V of size %d using %d maximum Jacobi iterations...\n",
		matrix_size, stopping_criteria.max_iterations);
	// return the approximate solution in B after performing n_iterations Jacobi iterations.
	fprintf(stdout, "    ");
	V_norm = matrix_infinity_norm(V.data(), matrix_size); // should take care of the case when V_norm is near zero.

	int iteration_count = 0;
	while (1) { 
		if (finish_Jacobi_iteration(iteration_count, flag)) break;
		// each Jacobi iteration updates the approximate solution two times. 	
		matrix_fma(D.data(), A.data(), B.data(), C.data(), matrix_size); // Compute D = A * B + C
		fprintf(stdout, ".");
		matrix_fma(B.data(), A.data(), D.data(), C.data(), matrix_size); // Update B = A * D + C for the next iteration	
		fprintf(stdout, ".");
		iteration_count += 2;
	}
	fprintf(stdout, "\n>>> The final relative residual error after %d iterations ||R||/||V|| = ||UX - V||/||V|| is %e.\n",
		iteration_count, final_relative_residual_norm);
}

void Jacobi::Jacobi_iteration_HOST_debug_mode() {
	fprintf(stdout, ">>> Trying to solve for X in UX = V of size %d using %d Jacobi iterations(DEBUG mode)...\n", 
		matrix_size, stopping_criteria.max_iterations);
	V_norm = matrix_infinity_norm(V.data(), matrix_size);	// should take care of the case when V_norm is near zero.

	float residual_norm = 0.0f;
	for (int iteration_count = 0; iteration_count < stopping_criteria.max_iterations; iteration_count += 2) {
		// each Jacobi iteration updates the approximate solution two times 	
		matrix_fma(D.data(), A.data(), B.data(), C.data(), matrix_size); // compute D = A * B + C.
#ifdef _USE_KAHAN_SUM
		matrix_residual_Kahan_sum(R.data(), U.data(), D.data(), V.data(), matrix_size);
#else
		matrix_residual(R.data(), U.data(), D.data(), V.data(), matrix_size);	
#endif
		residual_norm = matrix_infinity_norm(R.data(), matrix_size);
		fprintf(stdout, "    After the iteration %2d, the relative error is ||R||/||V|| = ||UX - V||/||V|| = %e.\n", 
			iteration_count + 1, residual_norm / V_norm);

		matrix_fma(B.data(), A.data(), D.data(), C.data(), matrix_size); // update B = A * D + C for the next iteration.
#ifdef _USE_KAHAN_SUM
		matrix_residual_Kahan_sum(R.data(), U.data(), B.data(), V.data(), matrix_size);
#else
		matrix_residual(R.data(), U.data(), B.data(), V.data(), matrix_size);
#endif
		residual_norm = matrix_infinity_norm(R.data(), matrix_size);
		fprintf(stdout, "    After the iteration %2d, the relative error is ||R||/||V|| = ||UX - V||/||V|| = %e.\n",
			iteration_count + 2, residual_norm / V_norm);
	}
	fprintf(stdout, ">>> Done!\n");
}

#define JACOBI_DIAGONAL_DOMINANCE_FACTOR 1.1f	// Do not change this factor!
void Jacobi::generate_linear_systems_strictly_row_diagonal_dominant(const float lower_bound, const float upper_bound) {
	// UX= V can be solved using Jacobi iteration, where U is a positive definite matrix and V is a random matrix.
	// generate a random positive definite matrix U and a random matrix V.	
#if defined(_JACOBI_MATRIX_GENERATION_DEBUG_MODE)	
	unsigned int fixed_seed = 20260601;
	std::mt19937 gen(fixed_seed);
	std::uniform_real_distribution<> dist(lower_bound, upper_bound);
#else
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(lower_bound, upper_bound);
#endif

	for (size_t i = 0; i < matrix_size * matrix_size; i++)
		U[i] = dist(gen);
	// make U positive definite by having the diagonal elements larger than the sum of 
	// the absolute values of the non-diagonal elements in the same row.	
	for (size_t i = 0; i < matrix_size; i++) {
		double row_sum = 0.0;
		for (size_t j = 0; j < matrix_size; j++) {
			if (i != j) {
				row_sum += fabs(U[i * matrix_size + j]);
			}
		}
		U[i * matrix_size + i] = row_sum * JACOBI_DIAGONAL_DOMINANCE_FACTOR;
	}
	for (size_t i = 0; i < matrix_size * matrix_size; i++)
		V[i] = dist(gen);
}

void Jacobi::Jacobi_iteration_CC(Status& flag) {
	fprintf(stdout, ">>> Trying to solve for X in UX = V of size %d using %d maximum Jacobi iterations on CUDA Cores...\n",
		static_cast<int>(matrix_size), stopping_criteria.max_iterations);
	fprintf(stdout, "    ");

	V_norm = matrix_infinity_norm(V.data(), matrix_size);
	int iteration_count = 0;
	int status_code = JACOBI_ITERATION_SUCCESS;

	Jacobi_iteration_CC_cuda(B.data(), D.data(), R.data(), A.data(), C.data(), U.data(), V.data(),
		matrix_size, stopping_criteria.max_iterations, stopping_criteria.epsilon, V_norm,
		&iteration_count, &final_relative_residual_norm, &status_code);

	flag = (status_code == JACOBI_ITERATION_SUCCESS) ? Status::SUCCESS : Status::FAIL_MAX_ITERATION;
	for (int i = 0; i < iteration_count; i++)
		fprintf(stdout, ".");
	fprintf(stdout, "\n>>> residual error after %d iterations ||R||/||V|| = ||UX - V||/||V|| is %e.\n",
		iteration_count, final_relative_residual_norm);
}

void Jacobi::Jacobi_iteration_TC(Status& flag) {
	fprintf(stdout, ">>> Trying to solve for X in UX = V of size %d using %d maximum Jacobi iterations on Tensor Cores...\n",
		static_cast<int>(matrix_size), stopping_criteria.max_iterations);
	fprintf(stdout, "    ");

	V_norm = matrix_infinity_norm(V.data(), matrix_size);
	int iteration_count = 0;
	int status_code = JACOBI_ITERATION_SUCCESS;

	Jacobi_iteration_TC_cuda(B.data(), D.data(), R.data(), A.data(), C.data(), U.data(), V.data(),
		matrix_size, stopping_criteria.max_iterations, stopping_criteria.epsilon, V_norm,
		&iteration_count, &final_relative_residual_norm, &status_code);

	flag = (status_code == JACOBI_ITERATION_SUCCESS) ? Status::SUCCESS : Status::FAIL_MAX_ITERATION;
	for (int i = 0; i < iteration_count; i++)
		fprintf(stdout, ".");
	fprintf(stdout, "\n>>> residual error after %d iterations ||R||/||V|| = ||UX - V||/||V|| is %e.\n",
		iteration_count, final_relative_residual_norm);
}
