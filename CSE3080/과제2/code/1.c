#include<stdio.h>
#include<stdlib.h>

int inputsize;      // int var to save inputsize
char alpha[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u' ,'v', 'w', 'x', 'y', 'z'}; // alphabet set to alphabet is used only in print section.
int tset[26] = {0};      // tset to save temp set;

struct rset{                // rest structure to save each element.
    int setsize;            // int var to save set's size.
    int set[26];            // int array to save each subset.
    char alphaset[26];      // char array to save set into alphabet.
};
struct rset result[10000];  // rset array to save result of Powerset().
int resultnum = 0;          // int var to save number of result's subset.

void swapSet(struct rset* a, struct rset* b){   // function to swap rset a and b's order.
    struct rset temp;   // temp rset to swap a and b
    temp = *a;          // save a at temp
    *a = *b;            // save b at a
    *b = temp;          // save temp(a) at b
}

void printSet(){                                        // function to print result set
    for(int i = 0; i < resultnum; i++){                 // for loop to repeat for size of result
        printf("{");                                    // to satisfy output form.
        for(int j = 0; j < result[i].setsize; j++){     // for loop to repeat for size of result[i]'s subset size.
            if(j < result[i].setsize - 1){              // if j isn't not last element.
                printf("%c ", result[i].alphaset[j]);   // print each element of subset.
            }
            else{                                       // if j is last element.
                printf("%c", result[i].alphaset[j]);    // print last element of subset.
            }
            
        }
        printf("}\n");                                  // to satisfy output form.
    }

}

void sortSet(){                                                             // function to sort set
    for(int i = 0; i < resultnum; i++){                                     // for loop to repeat for size of result 
        for(int j = 0; j < resultnum - 1; j++){                             // for loop to repeat for size of result - 1
            if(result[j].setsize > result[j+1].setsize){                    // if current subset's size is greater than next's
                swapSet(&result[j], &result[j+1]);                          // swap current & next subset.
            }       
            else if(result[j].setsize == result[j+1].setsize){              // if current & next subset's size is the same.
                for(int m = 0; m < result[j].setsize; m++){                 // for loop to repeat for size of current subset's size
                    if(result[j].alphaset[m] > result[j+1].alphaset[m]){    // if current subset's mth element is greater than next subset's.
                        swapSet(&result[j], &result[j+1]);                  // swap current & next subset.
                        break;                                              // as swap processed, quit the loop.
                    }
                    else if(result[j].alphaset[m] < result[j+1].alphaset[m]){   // if current subset's mth element is smaller than next subset's.
                        break;                                                  // as order of current & next is valid, quit the loop.
                    }
                }
            }
        }
    }
}

void saveSet(){                                                     // function to save subset got by PowerSet to resultset 
    int setsize = 0;                                                // int var to save setsize
    for(int i = 0; i < 26; i++){                                    // for loop to repeat for size of alphabet(26) abcde... (because each subset's maxsize is AtoZ)
        if(tset[i] == 1){                                           // if ith element exist
            result[resultnum].set[i] = 1;                           // set set ith boolean to 1
            result[resultnum].alphaset[setsize] = alpha[i];         // set result's alphaset to ith alphabet 
            setsize++;                                              // as one element saved, plus 1 to setsize
        }
        else{                                                       // if ith element doesn't exist
            result[resultnum].set[i] = 0;                           // set set ith boolean to 0
        }
    }
    result[resultnum].setsize = setsize;                            // save result to its setsize
    resultnum++;                                                    // as one result set saved, plus 1 to resultnum
}



void PowerSet(int size, int num){                                   // function to make powerset
    if(size == inputsize && num == 0){                              // if it's last PowerSet process
        saveSet();                                                  // save current set
        sortSet();                                                  // sort element of result set
        printSet();                                                 // print element of result set 
        return;                                                     // quit the function
    }
    if(num == 0){                                                   // if num is 0
        saveSet();                                                  // save current subset;
        return;                                                     // as there's no num -1, quit the funciton
    }
    PowerSet(size, num - 1);                                        // make subset same size, next element is selected.
    if(tset[num-1] == 1){                                           // if the element was set exist
        tset[num-1] = 0;                                            // set doesn't exist
    }   
    else{                                                           // if the element was set doesn't exist
        tset[num-1] = 1;                                            // set exist
    }
    PowerSet(size + 1, num - 1);                                    // make subset next size, next element is selected.
}

int main(){
    printf("Insert the size of set: ");                             // instruction
    scanf("%d", &inputsize);                                        // save size of set
    PowerSet(0, inputsize);                                         // call PowerSet
    return 0;
}