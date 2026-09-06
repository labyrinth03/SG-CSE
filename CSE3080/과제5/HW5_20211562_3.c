#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX_NODE_SIZE 100

typedef struct node *treePointer;
typedef struct node{
    int key;
    treePointer leftChild, rightChild;
}node;

treePointer new_node(int key){
    treePointer newnode = (treePointer)malloc(sizeof(node));
    newnode->key = key;
    newnode->leftChild = NULL;
    newnode->rightChild = NULL;
    return newnode;
}

int max_node(treePointer node){
    treePointer current = node;
    if(node == NULL) return -1;

    while(current->rightChild != NULL){
        current = current->rightChild;
    }

    int max = current->key;

    return max;
}
treePointer min_node(treePointer node){
    treePointer current = node;

    while(current->leftChild != NULL){
        current = current->leftChild;
    }

    return current;
}

treePointer push(treePointer node, int key, FILE* ofp){
    if(node == NULL){
        fprintf(ofp, "Push %d\n", key);
        return new_node(key);
    }

    if(key == node->key){
        fprintf(ofp, "Exist number\n");
        return node;
    }
    else if(key < node->key){
        node->leftChild = push(node->leftChild, key, ofp);
    }
    else if(key > node->key){
        node->rightChild = push(node->rightChild, key, ofp);
    }
    
    return node;
}


int top(treePointer node, FILE* ofp){
    if (node == NULL){
        fprintf(ofp, "The queue is empty\n");
        return -1;
    }
    else{
        fprintf(ofp, "The top is %d\n", max_node(node));
        return 0;
    }
}
treePointer pop(treePointer node, int key, FILE* ofp){
    if (max_node(node) == -1){
        fprintf(ofp, "The queue is empty\n");
        return node;
    }

    if(key < node->key){
        node->leftChild = pop(node->leftChild, key, ofp);
    }
    else if(key > node->key){
        node->rightChild = pop(node->rightChild, key, ofp);
    }
    else{
        if(node->leftChild == NULL){
            treePointer temp = node->rightChild;
            fprintf(ofp, "Pop %d\n", node->key);
            free(node);
            return temp;
        }
        else if(node->rightChild == NULL){
            treePointer temp = node->leftChild;
            fprintf(ofp, "Pop %d\n", node->key);
            free(node);
            return temp;
        }
        else{
            treePointer temp = min_node(node->rightChild);
            node->key = temp->key;
            node->rightChild = pop(node->rightChild, temp->key, ofp);
        }
    }
    
    return node;
}



int main(){
    FILE* ifp, *ofp;
    ifp = fopen("input3.txt", "r");
    ofp = fopen("output3.txt", "w");
    char buffer[20];
    treePointer root = NULL;

    for(fgets(buffer, sizeof(buffer), ifp); buffer[0] != 'q'; fgets(buffer, 10, ifp)){
        char* ptr = strtok(buffer, " ");
        if(strcmp(ptr,"push") == 0){
            int num;
            ptr = strtok(NULL, " ");
            num = atoi(ptr);
            root = push(root, num, ofp);
        }
        else if(strcmp(ptr, "top\n") == 0){
            top(root, ofp);
        }
        else if(strcmp(ptr, "pop\n") == 0){
            root = pop(root, max_node(root), ofp);
        }
    }
    fclose(ifp); fclose(ofp);
}