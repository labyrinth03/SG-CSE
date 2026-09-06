#include<stdio.h>
#include<stdlib.h>

#define MAX_SIZE 50 /* size of largest matrix */
typedef enum {head, entry} tagfield;
typedef struct matrix_node *matrix_pointer;

typedef struct entry_node{
    int row;
    int col;
    int value;
}entry_node;

typedef struct matrix_node{
    matrix_pointer down;
    matrix_pointer right;
    tagfield tag;
    union u{
        matrix_pointer next;
        entry_node entry;
    } u;
}matrix_node;

matrix_pointer hdnode[MAX_SIZE];
matrix_pointer hdnode_t[MAX_SIZE];

matrix_pointer new_node(){
    matrix_pointer newnode;
    newnode = malloc(sizeof(matrix_node));
    return newnode;
}

matrix_pointer mread(){ /* read in a matrix and set up its linked representation. An auxiliary global array hdnode is used */
    FILE* fp;
    fp = fopen("input.txt", "r");
    int num_rows, num_cols,num_terms, num_heads, i;
    int row, col, value, current_row;
    matrix_pointer temp, last, node, temp2;
    fscanf(fp, "%d %d %d", &num_rows, &num_cols, &num_terms);  //scan num of rows, num of cols, num of nonzero elements
    num_heads = (num_cols > num_rows) ? num_cols : num_rows;//decide head is row or col

    node = new_node();
    node->tag = entry;                   /* set up header node for the list of header nodes */
    node->u.entry.row = num_rows;
    node->u.entry.col = num_cols;
    node->u.entry.value = num_terms; // 추가한거
    

    if (!num_heads) node->right = node;                     //if there's no node
    else { 
        for(i = 0; i < num_heads; i++) {                   //initialize the header nodes
            temp = new_node();
            hdnode[i] = temp; hdnode[i]->tag = head;
            hdnode[i]->right = temp; hdnode[i]->u.next=temp;
        }
        current_row = 0;
        last = hdnode[0]; /* last node in current row */
        for(i = 0; i < num_terms; i++) {
            fscanf(fp, "%d %d %d", &row, &col, &value);
            if (row > current_row) { /* close current row */
                last->right = hdnode[current_row];
                current_row = row; last = hdnode[row];
            }
            
            temp = new_node();
            
            //temp = NULL; 
            temp->tag = entry;
            temp->u.entry.row = row; temp->u.entry.col = col;
            temp->u.entry.value = value;
            last->right = temp; /* link into row list */
            last = temp;
            hdnode[col]->u.next->down = temp; /* link into column list */ // 여기
            hdnode[col]->u.next = temp;
            hdnode[col]->u.entry.value = 0; //추가한거임 에러뜨면 삭제
        }
/* close last row */
        last->right = hdnode[current_row];
/* close all column lists */
        for (i=0; i<num_cols; i++)
            hdnode[i]->u.next->down = hdnode[i]; 
/* link all header nodes together */
        for (i=0; i<num_heads-1; i++)
            hdnode[i]->u.next = hdnode[i+1];
        hdnode[num_heads-1]->u.next = node;
        node->right = hdnode[0];
    }
    fclose(fp);

    return node;
}


matrix_pointer mtranspose(matrix_pointer matrix){
    int num_rows, num_cols,num_terms, num_heads, i;
    int row, col, value, current_row;
    num_rows = matrix->u.entry.row; 
    num_cols = matrix->u.entry.col;
    num_terms = matrix->u.entry.value;
    //printf("num_rows is %d num_cols is %d num_terms is %d\n", num_rows, num_cols, num_terms);
    matrix_pointer temp, last,hlast,  node, temp2;

    num_heads = (num_cols > num_rows) ? num_cols : num_rows;//decide head is row or col
    
    node = new_node();
    node->tag = entry;                   /* set up header node for the list of header nodes */
    node->u.entry.row = num_cols;       //뒤바꿔저장
    node->u.entry.col = num_rows;
    node->u.entry.value = num_terms;
    int k = 1;
    if (!num_heads) node->right = node;                     //if there's no node
    else { 
        for(i = 0; i < num_heads; i++) {                   //initialize the header nodes
            temp = new_node();
            hdnode_t[i] = temp; hdnode_t[i]->tag = head;
            hdnode_t[i]->right = temp; hdnode_t[i]->u.next=temp;
        }
        current_row = 0;
        last = hdnode_t[0];

        hlast = hdnode[0]; /* last node in current row *///읽을거
        hlast = hlast->right->down; //행순서로 읽기
        

        for(i = 0; i < num_terms; i++) {
            row = hlast->u.entry.col;
            col = hlast->u.entry.row;
            value = hlast->u.entry.value;
            //printf("row is %d col is %d value is %d\n", row, col, value); //scanf부분

            if (row > current_row) { /* close current row */
                last->right = hdnode_t[current_row];
                current_row = row; last = hdnode_t[row];
            }
            
            temp = new_node();
            

            temp->tag = entry;
            temp->u.entry.row = row; temp->u.entry.col = col;
            temp->u.entry.value = value;
            last->right = temp; /* link into row list */
            last = temp;
            hdnode_t[col]->u.next->down = temp; /* link into column list */ // 여기
            hdnode_t[col]->u.next = temp;

            if(hlast->down->u.entry.value == 0){        //아래가 노드면
                hlast = hlast->down->u.next->down;            //노드의 넥스트(다음노드)의 다운이 된다.
            }
            else{
                hlast = hlast->down;
            }
        }
/* close last row */
        last->right = hdnode_t[current_row];
/* close all column lists */
        for (i=0; i<num_cols; i++)
            hdnode_t[i]->u.next->down = hdnode_t[i]; 
/* link all header nodes together */
        for (i=0; i<num_heads-1; i++)
            hdnode_t[i]->u.next = hdnode_t[i+1];
        hdnode_t[num_heads-1]->u.next = node;
        node->right = hdnode_t[0];
    }
    
    return node;
}

void mwrite(matrix_pointer node){ /* print out the matrix in row major form */
    int i;
    matrix_pointer temp, head = node->right;
/* matrix dimensions */
//     printf("\n num_rows=%d, num_cols=%d, num_terms=%d\n", node->u.entry.row, node->u.entry.col, node->u.entry.value);
//     printf(" The matrix by row, column, and value: \n\n");
//     for(i = 0; i < node->u.entry.row + 1; i++) {
// /* print out the entries in each row */
//         for(temp=head->right; temp!=head; temp=temp->right)
//             printf("%5d%5d%5d\n", temp->u.entry.row, temp->u.entry.col, temp->u.entry.value);
//         head = head->u.next; /* next row */
//     }

    FILE*fp = fopen("output.txt", "w");
    fprintf(fp, "%d %d %d\n", node->u.entry.row, node->u.entry.col, node->u.entry.value);
    for(i = 0; i < node->u.entry.row ; i++) {
        for(temp=head->right; temp!=head; temp=temp->right)
            fprintf(fp, "%d %d %d\n", temp->u.entry.row, temp->u.entry.col, temp->u.entry.value);
        head = head->u.next; 
    }
    fclose(fp);

}


int main(){
    *hdnode = mread();
    *hdnode_t = mtranspose(*hdnode);
    mwrite(*hdnode_t);
    return 0;
}