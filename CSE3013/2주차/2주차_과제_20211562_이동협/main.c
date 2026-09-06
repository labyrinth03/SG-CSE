#include "header.h"

int main(){
        int casenum; //int to save number of test cases
        scanf("%d", &casenum); // scan number of test cases
        int* book = (int*)malloc(sizeof(int) * casenum);      // array to save number of pages of books
        for(int i = 0; i < casenum; i++){ // for to scan pages
                int tempscan; // temp int to save number of pages
                scanf("%d", &tempscan); // scan number of pages
                book[i] = tempscan;  // save each number of pages to each array
        }
                
        for(int i = 0; i < casenum; i++){ // for loop to repeat for number of cases
                int* intarr = (int*)calloc(10, sizeof(int)); //array to save each number of 0~9
                for(int j = 1; j <= book[i]; j++){ // for loop to repeat for number of pages
                        int obj = j; // int to save page for temporarily
                        int num = 1; // int to save powernum of 10
                        while(obj != 0){ // for loop during pagenum != 0
                                SaveAtArr(intarr, (obj%power(10,num))); // save each digit to intarr
                                obj = obj / power(10, num); // delete saved digits
                        }
                }
                for(int j = 0; j < 10; j++){ // for loop to print intarr's arguments
                        printf("%d ", intarr[j]); // print each array's digits
                }
                printf("\n"); // print Enter to divide each cases
                free(intarr); // free memory of intarr
        }
        free(book); // free memory of book
        
}
