#include<stdio.h>
#include<stdlib.h>

int setsize;
char alphaset[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u' ,'v', 'w', 'x', 'y', 'z'};
int tset[26] = {0};

void printSet(int* set){
    printf("{");
    for(int i = 0; i < setsize; i++){
        if(set[i] == 1){
            printf("%c ", alphaset[i]);
        }
    }
    printf("}\n");
}

void PowerSet(int size, int num){
    // int* set = (int*)malloc(sizeof(int)*setsize);
    // for(int i = 0; i < setsize; i++){
    //     set[i] = 0;
    // }
    // int currsize = setsize - size;
    // printf("size is %d, num is %d\n", size, num);
    // for(int i = 0; i < 26; i++){
    //     printf("%d ", tset[i]);
    // }
    // printf("\n");

    if(num == setsize){
        printSet(tset);
        return;
    }

    PowerSet(size, num + 1);
    if(tset[num] == 1){
        tset[num] = 0;
    }
    else{
        tset[num] = 1;
    }
    PowerSet(size + 1, num + 1);

}
int main(){
    printf("Insert the size of set: ");
    scanf("%d", &setsize);
    //char* set = (char*)malloc(sizeof(char)*num);

    // for(int i = 0; i < num; i++){
    //     set[i] = alphaset[i];
    //     printf("%d ", set[i]);
    // }
    PowerSet(0, 0);
    return 0;
}