#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct poly_node *poly_pointer; 
typedef struct poly_node{
    int num;
    int coef;
    int expon;
    poly_pointer link; 
}poly_node;

int COMPARE(int a, int b){
    if(a < b){
        return -1;
    }
    else if(a = b){
        return 0;
    }
    else if(a > b){
        return 1;
    }
}

void attach(int coefficient, int exponent, poly_pointer *ptr)
{
/* create a new node with coef = coefficient and expon = exponent,
attach it to the node pointed to by ptr. ptr is updated to point to this new node */ 
    poly_pointer  temp;
    temp = (poly_pointer)malloc(sizeof(poly_node)); 
    
    // if (IS_FULL(temp))  {
    //     fprintf(stderr, "The memory is full\n"); 
    //     exit(1);
    // }
    temp->coef = coefficient; 
    temp->expon = exponent; 
    temp->link = NULL;
   
    (*ptr)->link = temp;
    *ptr = temp; 
}


poly_pointer pmult(poly_pointer a, poly_pointer b){
    int arr[100] = {0};
    poly_pointer arear, brear;
    arear = (poly_pointer)malloc(sizeof(poly_node));
    brear = (poly_pointer)malloc(sizeof(poly_node));
    poly_pointer c, rear, temp;
    int mult;
    rear = (poly_pointer)malloc(sizeof(poly_node));
    c = rear; arear = a; brear = b;
    for(a = a->link; a; a = a->link){
        b = brear;
        for(b = b->link; b; b = b->link){
            arr[a->expon + b->expon] += a->coef * b->coef;
        }
    }
    int j = 0;
    for(int i = 99; i >=0; i--){
        if(arr[i] != 0){
            attach(arr[i], i, &rear); j++;
        }
    }
    //rear->link = NULL;
    temp = c; c = c->link; free(temp);
    c->num = j;
    return c;
}

void pwrite(poly_pointer r){

    FILE *fp;
    fp = fopen("d.txt", "w");
    
    fprintf(fp, "%d\n", r->num);
    
    int num = r->num;

    for(int i = 0; i < num; i++){
        fprintf(fp, "%d %d\n", r->coef, r->expon);
        r = r->link;
    }
    fclose(fp);
}


int main(){
    poly_pointer a; poly_pointer arear; 
    poly_pointer b; poly_pointer brear;
    poly_pointer d;

    int anum, bnum;
    int cof, exp;
    FILE* afp; FILE* bfp; FILE* dfp;

    arear = (poly_pointer)malloc(sizeof(poly_node));
    brear = (poly_pointer)malloc(sizeof(poly_node));

    a = arear;
    afp = fopen("a.txt", "r");
    fscanf(afp, "%d", &anum);
    arear->num = anum;
    for(int i = 0 ; i < anum; i++){
        fscanf(afp, "%d %d", &cof, &exp);
        attach(cof, exp, &arear);
    }
    fclose(afp);


    b = brear;
    bfp = fopen("b.txt", "r");
    fscanf(bfp, "%d", &bnum);
    brear->num = bnum;
    for(int i = 0 ; i < bnum; i++){
        fscanf(bfp, "%d %d", &cof, &exp);
        attach(cof, exp, &brear);
    }
    fclose(bfp);


    d = pmult(a,b);

    pwrite(d);
    return 0;
}