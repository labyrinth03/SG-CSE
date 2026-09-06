#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <int1> <int2> <int3> <int4>\n", argv[0]);
        return 0;
    }

    int arg_nums[4];
    
    for (int i = 0; i < 4; i++) {
        arg_nums[i] = atoi(argv[i + 1]);
    }
    int result_fibo = fibonacci(arg_nums[0]);
    int result_max = max_of_four_int(arg_nums[0], arg_nums[1], arg_nums[2], arg_nums[3]);
    printf("%d %d\n", result_fibo, result_max);

    return 1;
}