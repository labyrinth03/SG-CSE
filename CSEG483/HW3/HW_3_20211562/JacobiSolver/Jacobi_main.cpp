#include <stdio.h>
#include <cuda_runtime.h>
#include "Jacobi_debug_mode.h"
#include "Jacobi.h"
#include "Matrix_utility.h"
#include "measure_host_time_3.h"

constexpr int MATRIX_SIZE = 2048;
// MAX_ITERATIONSmust be an even number since we perform two updates in each Jacobi iteration
//constexpr int MAX_ITERATIONS = 6; 	
constexpr int MAX_ITERATIONS = 6;

static float recompute_relative_residual_on_host(Jacobi& jacobi) {
	matrix_residual_Kahan_sum(jacobi.R.data(), jacobi.U.data(), jacobi.B.data(), jacobi.V.data(), jacobi.matrix_size);
	return matrix_infinity_norm(jacobi.R.data(), jacobi.matrix_size) / jacobi.V_norm;
}

int main() {
	size_t matrix_size = MATRIX_SIZE;	
	int max_iterations = MAX_ITERATIONS;

	Jacobi jacobi{ matrix_size };

	jacobi.set_stopping_criteria(1.0e-7, 1.0e-7, max_iterations);
	jacobi.generate_linear_systems_strictly_row_diagonal_dominant(-1.0f, 1.0f);
	//print_square_matrix(jacobi.U.data(), matrix_size);
    //print_square_matrix(jacobi.V.data(), matrix_size);
	jacobi.prepare_Jacobi_iteration();
	//print_square_matrix(jacobi.A.data(), matrix_size);
	//print_square_matrix(jacobi.C.data(), matrix_size);
#if defined(_JACOBI_ITERATION_DEBUG_MODE)	
	jacobi.Jacobi_iteration_HOST_debug_mode();
#else
	Jacobi jacobi_host = jacobi;
	Jacobi jacobi_cc = jacobi;
	Jacobi jacobi_tc = jacobi;

	Jacobi::Status flag_host = Jacobi::Status::SUCCESS;
	Jacobi::Status flag_cc = Jacobi::Status::SUCCESS;
	Jacobi::Status flag_tc = Jacobi::Status::SUCCESS;

	float host_time_ms = 0.0f;
	float cc_time_ms = 0.0f;
	float tc_time_ms = 0.0f;

	fprintf(stdout, "\n================ HOST ================\n");
	CHECK_TIME_START(_start, _freq);
	jacobi_host.Jacobi_iteration_HOST(flag_host);
	CHECK_TIME_END(_start, _end, _freq, host_time_ms);
	fprintf(stdout, ">>> HOST elapsed time: %.3f ms\n", host_time_ms);
	fprintf(stdout, ">>> HOST residual recomputed on CPU: %e\n",
		recompute_relative_residual_on_host(jacobi_host));

	fprintf(stdout, "\n============ CUDA CORE ===============\n");
	CHECK_TIME_START(_start, _freq);
	jacobi_cc.Jacobi_iteration_CC(flag_cc);
	CHECK_TIME_END(_start, _end, _freq, cc_time_ms);
	fprintf(stdout, ">>> CUDA Core elapsed time: %.3f ms\n", cc_time_ms);
	fprintf(stdout, ">>> CUDA Core residual recomputed on CPU: %e\n",
		recompute_relative_residual_on_host(jacobi_cc));

	fprintf(stdout, "\n=========== TENSOR CORE ==============\n");
	CHECK_TIME_START(_start, _freq);
	jacobi_tc.Jacobi_iteration_TC(flag_tc);
	CHECK_TIME_END(_start, _end, _freq, tc_time_ms);
	fprintf(stdout, ">>> Tensor Core elapsed time: %.3f ms\n", tc_time_ms);
	fprintf(stdout, ">>> Tensor Core residual recomputed on CPU: %e\n",
		recompute_relative_residual_on_host(jacobi_tc));

	fprintf(stdout, "\n=============== SUMMARY ==============\n");
	fprintf(stdout, "HOST        : %.3f ms\n", host_time_ms);
	fprintf(stdout, "CUDA Core   : %.3f ms\n", cc_time_ms);
	fprintf(stdout, "Tensor Core : %.3f ms\n", tc_time_ms);

	switch (flag_host) {
	case Jacobi::Status::SUCCESS:
		fprintf(stdout, ">>> HOST Jacobi iteration completed successfully.\n");
		break;
	case Jacobi::Status::FAIL_MAX_ITERATION:
		fprintf(stdout, ">>> HOST Jacobi iteration failed to converge (Error Code: %d).\n", static_cast<int>(flag_host));
		break;
	default:
		break;
	}
	switch (flag_cc) {
	case Jacobi::Status::SUCCESS:
		fprintf(stdout, ">>> CUDA Core Jacobi iteration completed successfully.\n");
		break;
	case Jacobi::Status::FAIL_MAX_ITERATION:
		fprintf(stdout, ">>> CUDA Core Jacobi iteration failed to converge (Error Code: %d).\n", static_cast<int>(flag_cc));
		break;
	default:
		break;
	}
	switch (flag_tc) {
	case Jacobi::Status::SUCCESS:
		fprintf(stdout, ">>> Tensor Core Jacobi iteration completed successfully.\n");
		break;
	case Jacobi::Status::FAIL_MAX_ITERATION:
		fprintf(stdout, ">>> Tensor Core Jacobi iteration failed to converge (Error Code: %d).\n", static_cast<int>(flag_tc));
		break;
	default:
		break;
	}
#endif
}
