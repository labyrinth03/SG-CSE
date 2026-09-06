#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define MAX_SIZE 50
int row, col;

void saveMaze(char** board, int row, int col){
    FILE *fp;
    fp = fopen("save.maz", "w");
    for(int i = 0; i < 2*row + 1; i++){
        for(int j = 0; j < 2*col + 1; j++){
            fprintf(fp, "%c", board[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);    
}

char** makeMaze(int row, int col){
    int ** maze = (int**)malloc(sizeof(int*)*row);
    for(int i = 0; i < row; i++){
        maze[i] = (int*)malloc(sizeof(int)*col);
    }                                               //maze 메모리 설정
    char** board = (char**)malloc(sizeof(char*)*(row*2 + 1));
    for(int i = 0; i < 2*row + 1; i++){
        board[i] = (char*)malloc(sizeof(char)*(2*col + 1));
    }          
    srand((unsigned int)time(NULL));

    for(int i = 0; i < 2*row + 1; i++){           //maze의 보드판 초기설정
        for(int j = 0; j < 2*col + 1; j++){
            if(i % 2 == 1 && j % 2 == 1){
                board[i][j] = ' ';
            }
            else if(i % 2 == 0 && j % 2 == 1){
                board[i][j] = '-';
            }
            else if(i % 2 == 1 && j % 2 == 0){
                board[i][j] = '|';
            }
            else if(i % 2 == 0 && j % 2 == 0){
                board[i][j] = '+';
            }
        }
    }

    int set = 0;
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            maze[i][j] = set++;             //각각의 방에 대해 초기 집합 설정
        }
    }

    int sameset, maintain;
    int i;
    for(i = 0; i < row - 1; i++){
                 
        for(int j = 0; j < col - 1; j++){           //열의 마지막방 제외한 처음부터 마지막 전 방까지의 알고리즘
            if(maze[i][j] != maze[i][j+1]){         //현재 방이랑 오른쪽 방의 집합 값이 다를 때               
                int rannum = rand();              //벽 제거 확률은 50%               
                if(rannum % 2 == 1 ){  

                            for(int y = 0; y < i; y++){
                                for(int x = 0; x < col; x++){
                                    if(maze[i][j+1] == maze[y][x]){
                                        maze[y][x] = maze[i][j];
                                    }
                                }
                            }
                            maze[i][j+1] = maze[i][j];
                            board[2*i + 1][2*(j+1)] = ' ';                            
            


                }
            }

        }
        //if(i < row - 1)                  
        for(int j = 0; j < col; ){
            sameset = 0; maintain = 0;
            while(maze[i][j] == maze[i][j+1]){      //현재 방이랑 오른쪽 방의 집합 값이 같을 때
                sameset++;
                    
                int rannum = rand();              //벽 제거 확률은 50%  
                if(rannum % 2 == 1){//

                    maze[i+1][j] = maze[i][j];
                    board[(i+1)*2][2*j + 1] = ' ';
                }
                else{
                    maintain++;
                }

                j++;
                if(j >= col - 1) break;             
            }
            if(sameset != 0 && (maze[i][j]!=maze[i][j-1])){                       //처음 두 집합이 같았다면 
                sameset++;
                
                int rannum = rand();              //벽 제거 확률은 50%                      
                if(rannum % 2 == 1){
                   
                    maze[i+1][j] = maze[i][j];     //아래 방 열기
                    board[(i+1)*2][2*j + 1] = ' ';
                }
                else{
                    maintain++;
                }
                if(j >= col - 1){
                    break;
                }                  
            }
           
            j++;
            if(sameset == 0 || sameset == maintain){    //
                //한번도 같은 set에서 안 열었거나 행이 모두 다르면

                if(j >= col){
                    maze[i+1][j-1] = maze[i][j-1];
                    board[(i+1)*2][2*(j-1)+1] = ' ';
                }
                else{
                    maze[i+1][j-1] = maze[i][j-1];     //아래 방 열기
                    board[(i+1)*2][2*(j-1) + 1] = ' ';                    
                }

               
            }
        }
     



        //위아래는 짝수 홀수 board[(i+1)*2][2*j + 1] = ' ';
        //양옆은 홀수 짝수 board[2*i + 1][2*(j+1)] = ' ';


        if(i == row -2){                             //마지막 열 방 열기 과정 // if(i == row-2)
            

            
            i++;
            for(int j = 0; j < col - 1; j++){
                if(j == 0){
                    if(maze[i][j] == maze[i-1][j] && maze[i-1][j+1] != maze[i][j+1] && maze[i][j] != maze[i][j+1]){
                        maze[i][j+1] =maze[i][j];
                        board[2*i + 1][2*(j+1)] = ' ';
                    }

                }
                else if(maze[i][j] != maze[i][j+1] && maze[i-1][j] != maze[i-1][j+1]){

                    maze[i][j+1] =maze[i][j];
                    board[2*i + 1][2*(j+1)] = ' ';
                }
                else if(maze[i][j]!= maze[i][j+1]){
                    maze[i][j+1] =maze[i][j];
                    board[2*i + 1][2*(j+1)] = ' ';                    
                }
                

                if(j == col - 2){
                    if(maze[i][j+1] == row*col-1){  //마지막 보강
                        maze[i][j+1] = maze[i][j];
                        board[2*i + 1][2*(j+1)] = ' '; 
                    }
                    if(maze[i][0] != maze[i][1]){   //처음보강
                        maze[i][0] = maze[i][1];
                        board[2*i + 1][2] = ' '; 
                    }
                }
            }


        }
    }

    return board;
}



int main(){

    printf("Number of ROW: ");
    scanf("%d", &row);
    printf("Number of COLUMN: ");
    scanf("%d", &col);
    saveMaze(makeMaze(row, col), row, col);

    return 0;
}