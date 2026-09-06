#include<stdio.h>
#include<stdlib.h>

int EXIT_ROW;           //global var to save EXIT_ROW value
int EXIT_COL;           //global var to save EXIT_COL value

typedef struct{
    short int vert;
    short int horiz;
} offsets;              //offset struct to save value of each movements
offsets move[8];

typedef struct{
    short int row;
    short int col;
    short int dir;
}element;               //element struct to save row, col, dir

typedef struct mnode *node_pointer;
typedef struct mnode{
    node_pointer llink;
    element item;
    node_pointer rlink;
}mnode;                 //maze node to implement circular doubly linked list

node_pointer new_node(){    //func to make new mnode pointer and return it
    node_pointer newnode;
    newnode = malloc(sizeof(mnode));
    newnode->rlink = newnode; newnode->llink = newnode; //init llink, rlink
    return newnode;
}

void dinsert(node_pointer node, node_pointer newnode){      //func to insert node to doubly linked list 
    newnode->llink = node;                                  //change newnode's llink to node
    newnode->rlink = node->rlink;                           //change newnode's rlink to where node rlinked
    node->rlink->llink = newnode;                           //change node right node(node->rlink) llink to newnode
    node->rlink = newnode;                                  //change node's rlink to newnode
}

element ddelete(node_pointer node, node_pointer dnode){     //func to delete node of doubly linked list
    element temp;                                           //element to save deleting node's element value
    temp = dnode->item;                                     //save deleting element value
    if (node == dnode)                                      //if trying to delete head node
    printf("Deletion of head node not permitted.\n");       //error message
    else {                                                  //if trying to delete normal node
        dnode->llink->rlink = dnode->rlink;                 //change dnode's left node's rlink to dnode's right node
        dnode->rlink->llink = dnode->llink;                 //change dnode's right node's llink to dnod's left node
    }
    return temp;                                            //return deleted element value
}

int maze[100][100]; int mark[100][100];
void path(void)
{
    move[0].vert = -1; move[0].horiz = 0;                   //init each maze values
    move[1].vert = -1; move[1].horiz = 1;
    move[2].vert = 0; move[2].horiz = 1;
    move[3].vert = 1; move[3].horiz = 1;
    move[4].vert = 1; move[4].horiz = 0;
    move[5].vert = 1; move[5].horiz = -1;
    move[6].vert = 0; move[6].horiz = -1;
    move[7].vert = -1; move[7].horiz = -1;
    /* output a path through the maze if such a path exists */
    int i, row, col, nextRow, nextCol, dir, found = 0;
    element position;                                       //element var to save element value
    node_pointer node, temp, push, head;                    //node_pointer to save each nodes
    head = new_node();                                      //init head node
    
    mark[1][1] = 1;                                                             //init 2D array mark
    node = new_node();                                                          //init node
    node->llink = node; node->rlink = node;                                     //init llink, rlink
    node->item.row = 1; node->item.col = 1; node->item.dir = 1;                 //first node
    dinsert(head, node);                                                        //insert first node

    while (head->llink != head && !found) {         //if LINKEDLIST IS NOT EMPTY & path is not found                                
        position = ddelete(head, head->rlink);                        //deleted the last inserted node
        row = position.row; col = position.col, dir = position.dir;   //save deleted node's element
        while (dir < 8 && !found) {                                   //while trying 8dir, no found path yet
        /* move in direction dir */
            nextRow = row + move[dir].vert;                                 //set next row by adding direction
            nextCol = col + move[dir].horiz;                                //set node col by adding direction
            if (nextRow == EXIT_ROW && nextCol == EXIT_COL)                 //if this direction is correct
                found = 1;                                                  //set found to 1
            else if (!maze[nextRow][nextCol] && !mark [nextRow][nextCol]) { //if next coordinate's maze is open and mark is also open
                push = new_node();                                          //init newnode to insert this direction node
                mark [nextRow][nextCol] = 1;                                //mark next coordinate
                position.row = row; position.col = col;                     //save coordinate
                position.dir = ++dir;                                       //save direction
                push->item = position;                                      //save element to push node
                dinsert(head, push);                                        //insert newnode right to head node
                row = nextRow; col = nextCol; dir = 0;                      //change current row, col to next row, col & init dir
            }
            else ++dir;                                                     //try next direction
        }
    }
    if (found) {                                                            //if path to exit coordinate founds
        FILE*fp = fopen("path.txt","w");                                    //open path.txt

        temp = new_node();                                                  //init temp node to traverse node one by one
        
        for (temp = head->llink; temp->llink != head; temp = temp->llink){  //from left node of head, go left one by one(cause head's left node is the very firstly inserted node)        
            fprintf(fp, "%d %d\n", temp->item.row, temp->item.col);         //write each path one by onde
        }       
        fprintf(fp, "%d %d\n", temp->item.row, temp->item.col);             //write second to last node
        fprintf(fp, "%d %d\n", row, col);                                   //write current coordinate
        fprintf(fp, "%d %d\n", EXIT_ROW, EXIT_COL);                         //write destination of maze
    }
    else printf("The maze does not have a path \n");                        //if there's no path to escape maze
}
int main(){
    FILE*fp = fopen("maze.txt", "r");               //open maze.txt
    char line[100];                                 //buffer to get line
    int rownum = 0;                                 //int var to save rownum
    int colnum = 0;                                 //int var to save colnum
    while(fgets(line, 30, fp) != NULL){             //get string line 

        if(rownum == 0)                             //at firstline, it get colnum to prevent scanning garbage value at last line
        for(int i = 0; line[i] !='\n'; i++){        //until it's \n
            maze[rownum][i] = line[i] - '0';        //save maze value one by one
            colnum++;                               //count colnum
        }
        else{                                       //after firstline
        for(int i = 0; i < colnum; i++){            //get maze value number of colnum
            maze[rownum][i] = line[i] - '0';        //save maze value one by one
        }            
        }
        rownum++;                                   //count rownum
    }
    
    EXIT_ROW = rownum -2;                           //init EXITROW(as example's exit is bottom right, it followed)
    EXIT_COL = colnum -2;                           //init EXITCOL(as example's exit is bottom right, it followed)

    path();                                         //call path

    return 0;
}