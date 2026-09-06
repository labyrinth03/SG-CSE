#include<stdio.h>
#include<stdlib.h>
#define MAX_NODE_SIZE 100
int EXIT_ROW;
int EXIT_COL;
int top = 0; 
typedef struct{
    short int vert;
    short int horiz;
} offsets;
offsets move[8];


typedef struct{
    short int row;
    short int col;
    short int dir;
}element;

typedef struct mnode *node_pointer;
typedef struct mnode{
    node_pointer llink;
    element item;
    int num;
    node_pointer rlink;
}mnode;

node_pointer new_node(){
    node_pointer newnode;
    newnode = malloc(sizeof(mnode));
    newnode->rlink = newnode; newnode->llink = newnode;
    return newnode;
}

//node_pointer head;
void dinit(node_pointer node){
    node->rlink = node;
    node->llink = node;
    printf("dinit end\n");
}
void dinsert(node_pointer node, node_pointer newnode){
    newnode->llink = node;
    newnode->rlink = node->rlink;
    node->rlink->llink = newnode;
    node->rlink = newnode;
    //newnode->num = ++top;
}
element ddelete(node_pointer node, node_pointer dnode) {
    element temp;
    temp = dnode->item;
    if (node == dnode)
    printf("Deletion of head node not permitted.\n");
    else {
        //printf("1\n");
        dnode->llink->rlink = dnode->rlink;
        //printf("2\n");
        dnode->rlink->llink = dnode->llink;
        //printf("3\n");
        //free(dnode);
        top--;
    }
    return temp;
}

int maze[100][100]; int mark[100][100];
void path(void)
{
    move[0].vert = -1; move[0].horiz = 0;
    move[1].vert = -1; move[1].horiz = 1;
    move[2].vert = 0; move[2].horiz = 1;
    move[3].vert = 1; move[3].horiz = 1;
    move[4].vert = 1; move[4].horiz = 0;
    move[5].vert = 1; move[5].horiz = -1;
    move[6].vert = 0; move[6].horiz = -1;
    move[7].vert = -1; move[7].horiz = -1;
/* output a path through the maze if such a path exists */
    int i, row, col, nextRow, nextCol, dir, found = 0;//FOUND원래는 FALSE였음
    element position;
    node_pointer node, temp, push, head;
    head = new_node();

    mark[1][1] = 1;                                                //LINKEDLIST
    node = new_node();
    node->llink = node; node->rlink = node;
    node->item.row = 1; node->item.col = 1; node->item.dir = 1; node->num = 0;                   //LINKEDLIST
    dinsert(head, node);
    //printf("node init complete\n");

    int ang = 0;
    while (head->llink != head && !found) {//LINKEDLIST IS NOT EMPTY

       // printf("%dth loop, flag is %d\n", ang++, head->llink!= head);
        temp = new_node();
        // for(temp = head->rlink; temp->rlink != head; temp = temp->rlink){
        //     printf("node %d %d %d ", temp->item.row, temp->item.col, temp->item.dir);
        // }  
        //printf("predelete\n");                                         
        position = ddelete(head, head->rlink);                                                   //DELETE FROM LINKED LIST
        //printf("delete %d %d %d\n", position.row, position.col, position.dir);
        row = position.row; col = position.col, dir = position.dir;         ////LINKEDLIST
        while (dir < 8 && !found) {
        /* move in direction dir */
            nextRow = row + move[dir].vert;
            nextCol = col + move[dir].horiz;
            if (nextRow == EXIT_ROW && nextCol == EXIT_COL)
                found = 1;//FOUND 원래는 TRUE였음
            else if (!maze[nextRow][nextCol] && !mark [nextRow][nextCol]) {//LINKEDLIST
                push = new_node();
                mark [nextRow][nextCol] = 1;
                position.row = row; position.col = col;
                position.dir = ++dir;
                push->item = position;    
                //printf("insert %d %d %d\n", position.row, position.col, position.dir);           
                dinsert(head, push);                                             //LINKEDLIST
                row = nextRow; col = nextCol; dir = 0;
            }
            else ++dir;
        }
    }
    if (found) {
        FILE*fp = fopen("path.txt","w");

        temp = new_node();
        temp = head->rlink;
        
        for (temp = head->llink; temp->llink != head; temp = temp->llink){           
            fprintf(fp, "%d %d\n", temp->item.row, temp->item.col);
        }
        fprintf(fp, "%d %d\n", temp->item.row, temp->item.col);   
        fprintf(fp, "%d %d\n", row, col);
        fprintf(fp, "%d %d\n", EXIT_ROW, EXIT_COL);
    }
    else printf("The maze does not have a path \n");
}
int main(){
    FILE*fp = fopen("maze.txt", "r");
    char line[100];
    int rownum = 0;
    int colnum = 0;
    while(fgets(line, 30, fp) != NULL){
        if(line[0] != '0' && line[0] != '1') break;
        if(rownum == 0)
        for(int i = 0; line[i] !='\n'; i++){
            maze[rownum][i] = line[i] - '0';
            colnum++;
        }
        else{
        for(int i = 0; i < colnum; i++){
            maze[rownum][i] = line[i] - '0';
        }            
        }
        rownum++;
    }
    
    EXIT_ROW = rownum -2;
    EXIT_COL = colnum -2;

    path();

    return 0;
}