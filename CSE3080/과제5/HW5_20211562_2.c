#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX_STACK_SIZE 50
typedef struct node *treePointer;
typedef struct node{
    int key;
    treePointer leftChild, rightChild;
}node;

treePointer root;
int num; int arr[50];

treePointer new_node(int key){
    node* newnode = (node*)malloc(sizeof(node));
    newnode->key = key;
    newnode->leftChild = NULL;
    newnode->rightChild = NULL;
    return newnode;
}

treePointer makeTree(treePointer node, int num){

    if(node == NULL){
        //node = new_node(num);
        return new_node(num);
    }
    if(num < node->key){
        node->leftChild = makeTree(node->leftChild, num);
    }
    else if(num > node->key){
        node->rightChild = makeTree(node->rightChild, num);
    }
    return node;
}

void postorder (treePointer ptr, FILE* ofp)
{ /* postorder tree traversal */
    if (ptr) {
        postorder (ptr -> leftChild, ofp);
        postorder (ptr -> rightChild, ofp);
        if(ptr != root) fprintf (ofp, "%d ", ptr -> key);
    }
}

void inorder (treePointer ptr, FILE* ofp)
{ /* inorder tree traversal */
    if (ptr) {
        inorder (ptr -> leftChild, ofp);
        if(ptr != root) fprintf (ofp, "%d ", ptr -> key);
        inorder (ptr -> rightChild, ofp);
    }
}


int main(){
    FILE* ifp;
    ifp = fopen("input2.txt", "r");
    
    fscanf(ifp, "%d\n", &num);

    char buffer[100];
    fgets(buffer, sizeof(buffer), ifp);
    char *ptr = strtok(buffer, " ");
    int k = 0;
    while(ptr != NULL){
        arr[k] = atoi(ptr);
        k++;
        ptr = strtok(NULL, " ");
    }
    fclose(ifp);

    FILE* ofp;
    ofp = fopen("output2.txt", "w");

    int flag = 0;
    for(int i = 0; i < num; i++){
        for(int j = i+1; j < num; j++){
            if(arr[i] == arr[j]) flag = 1;
        }
    }
    
    if(flag == 1){
        fprintf(ofp, "cannot construct BST\n");
        fclose(ofp);
        return 0;
    }

    root = (node*)malloc(sizeof(node));
    root->leftChild = NULL;
    root->rightChild = NULL;

    for(int i = 0; i < num; i++){
        makeTree(root, arr[i]);
    }

    fprintf(ofp, "Inorder: ");
    inorder(root, ofp);
    fprintf(ofp, "\n");
    fprintf(ofp, "Postorder: ");
    postorder(root, ofp);
    fprintf(ofp, "\n");
    fclose(ofp);

    return 0;
}