#include<stdio.h>
#include<stdlib.h>

int check_array(int* array, int num){                      // function to check array's element is continuous or not
    int* narray = (int*)malloc(sizeof(int)*100);           // number array to save each digit's number
    for(int i = 0; i < 100; i++){                          // for loop to initialize narray
        narray[i] = 0;                                     // intialize narray
    }
    for(int i = 0; i < num; i++){                           // for loop to find number of each digit
        narray[array[i] - 1]++;                             // plus one to each digit's number
    }

    for(int i = 0; i < 100; i++){                           // forloop to check continous or not
        if(narray[i] == 1){                                 // if there's inum only one
            for(int j = 1; j < num && i + j < 100; j++){    // forloop to check while size of array
                if(narray[i+j] != 1){                       // if next digit doesn't exist or number isn't 1
                    return 0;                               // return 0(false)
                }
            }
            return 1;                                       // if array is continuous, return 1
        }
    }

    return 0;
}
int main(){

    int num;                                    // int var to save number of array

    scanf("%d\n", &num);                        // save number of array

    int* array = (int*)malloc(sizeof(int)*num); // allocate memory of array

    for(int i = 0; i < num; i++){               // for loop to repeat for num times
        int temp;                               // int var to save each element of array
        scanf("%d", &temp);                     // save each element of array
        array[i] = temp;                        // save temp's value to array[i]
    }

    printf("%d\n", check_array(array, num));    // print return value of check_array()

    free(array);                                // free memory of array
    return 0;
}