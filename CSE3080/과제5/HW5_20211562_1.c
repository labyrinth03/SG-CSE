#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct node *treePointer;
typedef struct node{
    int key;
    treePointer parent;
    treePointer leftChild, rightChild;
}node;
typedef struct queue *qpointer;
typedef struct queue{
    qpointer next;
    treePointer data;
}queue;

//treePointer root;
FILE* ifp, *ofp;
int exist;
treePointer new_node(int key, treePointer parent){
    treePointer newnode = (treePointer)malloc(sizeof(node));
    newnode->key = key;
    newnode->leftChild = NULL;
    newnode->rightChild = NULL;
    newnode->parent = parent;
    return newnode;
}
qpointer new_queue(treePointer data){
    qpointer q = (qpointer)malloc(sizeof(queue));
    q->next = NULL;
    q->data = data;
    return q;
}
void freeq(qpointer* q){
    qpointer curq = *q;
    while(curq != NULL){
        *q = curq->next;
        free(curq);
        curq = *q;
    }
}

void push(treePointer node, qpointer* last){
    
    if(node->leftChild != NULL){
        (*last)->next = new_queue(node->leftChild);
        (*last) = (*last)->next;
    }
    if(node->rightChild != NULL){
        (*last)->next = new_queue(node->rightChild);
        (*last) = (*last)->next;        
    }
}
void inorder (treePointer ptr, int key)
{ /* inorder tree traversal */
    if (ptr) {
        inorder (ptr -> leftChild, key);
        if(ptr->key == key) exist = 1;
        inorder (ptr -> rightChild, key);
    }
}
void insert(treePointer *node, int num){
    exist = 0;
    if(*node == NULL){
        *node = new_node(num, NULL);
        fprintf(ofp, "Insert %d\n", num);
    }
    else{
        qpointer q = new_queue(*node);
        qpointer curq = q;
        qpointer lastq = q;
        treePointer curnode;

        inorder(*node, num);
        if(exist == 1){
            fprintf(ofp, "Exist number\n");    
            return;        
        }

        
        while(curq != NULL){
            curnode = curq->data;
            
            if(curnode->leftChild == NULL){
                fprintf(ofp, "Insert %d\n", num);
                curnode->leftChild = new_node(num, curnode);
                curnode = curnode->leftChild;
            }
            else if(curnode->rightChild == NULL){
                fprintf(ofp, "Insert %d\n", num);
                curnode->rightChild = new_node(num, curnode);
                curnode = curnode->rightChild;
            }
            else{
                push(curnode, &lastq);
                curq = curq->next;
                continue;
            }
            while(curnode->parent != NULL && curnode->parent->key < curnode->key){
                int temp = curnode->parent->key;
                curnode->parent->key = curnode->key;
                curnode->key = temp;
                curnode = curnode->parent;
            }
            break;
        }
        freeq(&q);
    }
    return;
}

treePointer delete(treePointer* node){
    if(*node != NULL){
        qpointer q = new_queue(*node);
        qpointer curq = q;
        qpointer lastq = q;
        qpointer prevq;
        treePointer curnode;

        while(curq != NULL){
            curnode = curq->data;
            push(curnode, &lastq);
            prevq = curq;
            curq = curq->next;
        }
        curnode = prevq->data;
        fprintf(ofp, "Delete %d\n", q->data->key);       
        freeq(&q);

        if(curnode->parent == NULL){
            free(curnode);
            *node = NULL;
            return *node;
        }
        else{

            (*node)->key = curnode->key;
            curnode = curnode->parent;
            if(curnode->rightChild != NULL){

                free(curnode->rightChild);
                curnode->rightChild = NULL;
            }
            else{
                free(curnode->leftChild);
                curnode->leftChild = NULL;
            }

            int x, y, z;
            curnode = *node;
            while(1){
                if(curnode->leftChild == NULL){
                    break;
                }
                else if(curnode->rightChild == NULL){
                    x = curnode->key;
                    y = curnode->leftChild->key;
                    if(x<y){
                        curnode->key = y;
                        curnode->leftChild->key = x;
                        curnode = curnode->leftChild;
                    }
                    else{
                        break;
                    }
                }
                else{
                    x = curnode->key;
                    y = curnode->leftChild->key;
                    z = curnode->rightChild->key;
                    if(x >= y && x >= z){
                        break;
                    }
                    else if(x < y && z <= y){
                        curnode->leftChild->key = x;
                        curnode->key = y;
                        curnode = curnode->leftChild;
                    }
                    else{
                        curnode->rightChild->key = x;
                        curnode->key = z;
                        curnode = curnode->rightChild;
                    }
                }
            }
        }
    }
    else{
        fprintf(ofp, "The heap is empty\n");
    }
    return *node;
}
int main(){

    ifp = fopen("input1.txt", "r");
    ofp = fopen("output1.txt", "w");
    char buffer[10];

    treePointer root = NULL;


    for(fgets(buffer, 10, ifp); buffer[0] != 'q'; fgets(buffer, 10, ifp)){
        if(buffer[0] == 'i'){
            char* ptr = strtok(buffer, " ");
            int num;
            ptr = strtok(NULL, " ");
            num = atoi(ptr);
            insert(&root, num);
        }
        else if(buffer[0] == 'd'){
            root = delete(&root);
        }
    }
    fclose(ifp); fclose(ofp);
}