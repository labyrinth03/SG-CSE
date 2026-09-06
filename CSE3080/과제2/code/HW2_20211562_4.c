#include<stdio.h>
#include<stdlib.h>

struct name{        // struct to save each name
    char* lname;    // lastname
    int llen;       // lastname length
    char* fname;    // firstname
    int flen;       // firstname length
};

void swapList(struct name* aname, struct name* bname){  //function to swap list's element.
    struct name temp;                                   //to save aname temporarily
    temp = *aname;                                      //save aname at temp
    *aname = *bname;                                    //save bname at aname
    *bname = temp;                                      //save temp at bname
}

void sortList(int num, struct name *list){                                          //sort namelist
    for(int i = 0; i < num; i++){                                                   
        for(int j = 0; j < num-1; j++){                                             //To sort each list element one by one.(bubble sort) 
            int loop = 0;                                                           //int variable to prevent case like (choi, cho)
            int goto_fname = 0;                                                     //int variable to judge "check fname" or not
            for(int m = 0; list[j].lname[m] != 0 && list[j+1].lname[m] != 0; m++){  //for loop while current and next lastname's mth char is exist.
                if(list[j].lname[m] > list[j+1].lname[m]){                          //if current lastname's mth char is bigger than next's.
                    swapList(&list[j], &list[j+1]);                                 //swap current and next name
                    goto_fname = 0;                                                 //so we must not see the first name
                    break;                                                          //as lastname swapped, quit the loop.
                }
                else if(list[j].lname[m] == list[j+1].lname[m]){                    // if current lastname's mth char is same with next's.
                    loop++;                                                         // plus one to lnamesame.
                    goto_fname = 1;                                                 // if current lastname and next lastname is same, we have to see firstname.
                }   
                else if(list[j].lname[m] < list[j+1].lname[m]){                    // if current lastname's mth char is smaller than next's.
                    goto_fname = 0;                                                // we dont' have to see firstname.
                    break;                                                         // as order of current and next is valid, quit loop.
                }
            }
            if(loop == list[j].llen ^ loop == list[j+1].llen){                     // case j is choi, j+1 is cho
                if(list[j].llen > list[j+1].llen){                                 // if current is choi, next is cho
                    swapList(&list[j], &list[j+1]);                                // swap current and next
                    goto_fname = 0;                                                // as swap processed, we don't have to see firstname.
                }
            }
            if(goto_fname == 1){                                                            // case lname is same, we have to see first name.
                int loop = 0;                                                               // variable to find case of ex. jin, jini
                for(int m = 0; list[j].fname[m] != 0 && list[j+1].fname[m+1] != 0; m++){    // for loop while current and next firstname's mth char is exist. 
                    if(list[j].fname[m] > list[j+1].fname[m]){                              // if current firstname's mth char is bigger than next's. 
                        swapList(&list[j], &list[j+1]);                                     // swap current and next name
                        break;                                                              // as firstname swapped, quit the loop.
                    }
                    else if(list[j].fname[m] == list[j+1].fname[m]){                        // if current firstname's mth char is same with next's.
                        loop++;                                                             // plus one to lnamesame.
                    }
                    else if(list[j].fname[m] < list[j+1].fname[m]){                         // if current firstname's mth char is smaller than next's.
                        break;                                                              // as order of current and next is valid, quit loop.
                    }
                }
                if(loop == list[j].flen ^ loop == list[j+1].flen){           // case j is jini, j+1 is jin
                    if(list[j].flen > list[j+1].flen){                       // if current is jini, next is jin
                        swapList(&list[j], &list[j+1]);                      // swap current and next
                    }
                }                  
            }                
        } 
    }


}

int main(){

    int num;                // integer var to save number of name
    scanf("%d\n", &num);    // scan student number.

    struct name* list = (struct name*)malloc(sizeof(struct name)*num);    // allocate memory of namelist
    for(int i = 0; i < num; i++){                                         // for loop to allocate memory of namelist's elements.
        list[i].fname = (char*)calloc(100, sizeof(char));                 // for loop to allocate memory of firstname.
        list[i].lname = (char*)calloc(100, sizeof(char));                 // for loop to allocate memory of lastname.
    }

    for(int i = 0; i < num; i++){                           // forloop to save lastname to struct name
        scanf("%s", list[i].lname);                         // save lastname to namelist's ith array.
        list[i].llen = 0;                                   // initialize lastname length
        for(int j = 0; j < list[i].lname[j] != 0; j++){     // for loop to count lastname's length.
            list[i].llen++;                                 // plus one to llen.
        }

        scanf("%s", list[i].fname);                         // save firstname to namelist's ith array.
        list[i].flen = 0;                                   // initialize firstname length.
        for(int j = 0; j < list[i].fname[j] != 0; j++){     // for loop to count firstname's length.
            list[i].flen++;                                 // plus one to flen.
        }     
    }
    
    sortList(num, list);                                    // call sortList

    for(int i = 0; i < num; i++){                           // for loop to print each sorted name 
        printf("%s %s\n", list[i].lname, list[i].fname);    // print each name by line.
    } 

    for(int i = 0; i < num; i++){                           // for loop to free each memory.
        free(list[i].fname);                                // free firstname's memory.
        free(list[i].lname);                                // free lastname's memory.
    }
    free(list);                                             // free namelist.

    return 0;
}