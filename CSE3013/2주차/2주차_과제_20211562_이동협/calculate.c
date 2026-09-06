#include "header.h"

int power(int x, int n){ // function to caculate power
        int result = 1; // int to save result
        for(int i = 0; i < n; i++){ // for loop to calculate x^n
                 result *= x; // multiply x to result
        }
        return result; // return calculated result
}
