#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <curses.h>

#define KEY_SPACE ' ' // not defined in ncurses.h
#define KEY_Enter 10
#define START_ROW 5
#define START_COL 5

int HEIGHT, WIDTH;

void saveGame(int **board, int players, int row, int col, int turn, char *file){// function to save current status of game.

	FILE* fp = NULL;															// file pointer to write save file.
	fp = fopen(file, "w");														// create or rewrite file.
	fprintf(fp, "%d %d %d %d %d %d\n", players, row, col, turn, HEIGHT, WIDTH); // write this game's current playernum, row, col, turn, HEIGHT, WIDTH for first row of file.
	for(int i = 0; i < HEIGHT; i++){											// for loop to read board's 1st dimension.
		for(int j = 0; j < WIDTH; j++){											// for loop to read board's 2nd dimension.
			fputc(board[i][j], fp);												// write board's value sequentially for 2nd row of file.
		}
	}
	fclose(fp);																	// close the file.

}

int** readSavedGame(int **board, int *row, int *col, int *turn, int *players, char *file){ 	// function to set stone(O,X,Y) from saved gameboard.

	FILE* fp = NULL;																		// file pointer to read saved file.
	fp = fopen(file, "r");																	// open saved file with read mode.
	fscanf(fp, "%d %d %d %d %d %d\n", players, row, col, turn, &HEIGHT, &WIDTH);			// read saved game's current playernum, row, col, turn, HEIGHT, WIDTH for 1st row of file.
	
	char savedboard[10000];																	// char array to save saved game board of 2nd row of file.
	fscanf(fp, "%s", savedboard);															// save saved gameboard to savedboard.
	int num = 0;																			// int to indexing savedboard.
	for(int i = 0; i < HEIGHT; i++){														// for loop to read board's 1st dimension.
		for(int j = 0; j < WIDTH; j++){														// for loop to read board's 2nd dimension.
			if(savedboard[num] == 'O'){														// while reading savedboard[num], if there's stone(O,X,Y) set same stone to the board.
				board[i][j] = 'O';
			}
			else if(savedboard[num] == 'X'){
				board[i][j] = 'X';
			}
			else if(savedboard[num] == 'Y'){
				board[i][j] = 'Y';
			}
			num++;																			// +1 to num to read next savedboard.
		}
	}
	fclose(fp);																				// close the file.
}

void readSavedGameStatus(int *row, int *col, int *turn, int *players, char *file){			// function to read gamefile's current row, col, turn, players, file.
	FILE* fp = NULL;																		// file pointer to read saved file.
	fp = fopen(file, "r");																	// open saved file with read mode.
	fscanf(fp, "%d %d %d %d %d %d\n", players, row, col, turn, &HEIGHT, &WIDTH);			// read saved game's current playernum, row, col, turn, HEIGHT, WIDTH for 1st row of file.
	fclose(fp);																				// close the file.
}


// modify two dimentional array int board[][]
int** initBoard(int **board, int *row, int *col, int *turn){								// initialize the board.

	board = (int**)malloc(sizeof(int*)*HEIGHT);												// allocate memory to board to save board's shape.
	for(int i=0; i<HEIGHT; i++){
		board[i] = (int*)malloc(sizeof(int)*WIDTH);
	}

	printf("%d %d\n", HEIGHT, WIDTH);
	board[0][0] = ACS_ULCORNER;//'┌'														// set the board by given HEIGHT, WIDTH.
	for (int i=1; i < WIDTH-1; i++)
		board[0][i] = ACS_TTEE;//'┬'
	board[0][WIDTH-1] = ACS_URCORNER; //'┐'

	for (int i=1; i<HEIGHT-1; i++){
		board[i][0] = ACS_LTEE; // '├'
		for (int j=1; j < WIDTH-1; j++)
			board[i][j] = ACS_PLUS; //'┼'
		board[i][WIDTH-1] = ACS_RTEE; //'┤'
	}

	board[HEIGHT-1][0] = ACS_LLCORNER; //'└'
	for (int i=1; i < WIDTH-1; i++)
		board[HEIGHT-1][i] = ACS_BTEE; //'┴'
	board[HEIGHT-1][WIDTH-1] = ACS_LRCORNER; // '┘'

	return board;																			// return the completed board.
}

void paintBoard(int **board, WINDOW *win, int row, int col){								// show the board to window by given board array.
	/*
		Print the board to the given WINDOW 
		using functions of the ncurses library.
	*/
	// TODO
	wmove(win, 0, 0);																		// move to 0, 0 to show board from the beginning position.
	for(int i = 0; i < HEIGHT; i++){														// for loop to show the board to window by given board array. 
		for(int j = 0; j < WIDTH; j++){
				waddch(win, board[i][j]);													// show the value of board[i][j] on the win.
		}
	}
	refresh();																				// to show up-to-date board.
	wrefresh(win);																			// to show up-to-date board.
	wmove(win, row, col);																	// move cursor to latest position.
}

void paintMenu(int **board, WINDOW *win, int row, int col, int turn, int players){	// function to show the game menu.

	if(turn == 1){																// when turn is O's turn.
		mvprintw(HEIGHT + 3, 5, "Current Turn : O");							// show whose turn.
	}
	else if(turn == 2){															// when turn is X's turn.
		mvprintw(HEIGHT + 3, 5, "Current Turn : X");							// show whose turn.
	}
	else if(turn == 3){															// when turn is Y's turn.
		mvprintw(HEIGHT + 3, 5, "Current Turn : Y");							// show whose turn.
	}
	mvprintw(HEIGHT + 4, 5, "1. Press 1 to save");								// show 1 function.
	mvprintw(HEIGHT + 5, 5, "2. Exit without save");							// show 2 function.

	refresh();																	// to show up-to-dated board.
	wrefresh(win);																// to show up-to-dated board.
	wmove(win, row, col);														// move cursor to latest position.

}

int checkWin(int **board, int turn, int players){ // function to check if there's winner. It returns 1 when there's a winner, or 0.

	if (players == 2){					 // case of playernum is 2
		for(int i = 0; i < HEIGHT; i++){ // WIDTH win check
			for(int j = 2; j < WIDTH-2; j++){ 
				if(board[i][j-2] == 'O' && board[i][j-1] == 'O' && board[i][j] == 'O' && board[i][j+1] == 'O' && board[i][j+2] == 'O'){
					return 1;
				}
				else if(board[i][j-2] == 'X' && board[i][j-1] == 'X' && board[i][j] == 'X' && board[i][j+1] == 'X' && board[i][j+2] == 'X'){
					return 1;
				}
			}
		}
		for(int i = 0; i < WIDTH; i++){ // HEIGHT win check
			for(int j = 2; j < HEIGHT-2; j++){
				if(board[j-2][i] == 'O' && board[j-1][i] == 'O' && board[j][i] == 'O' && board[j+1][i] == 'O' && board[j+2][i] == 'O'){
					return 1;
				}
				else if(board[j-2][i] == 'X' && board[j-1][i] == 'X' && board[j][i] == 'X' && board[j+1][i] == 'X' && board[j+2][i] == 'X'){
					return 1;
				}				
			}
		}
		for(int i = 2; i < HEIGHT-2; i++){ // DIAGONAL win check
			for(int j = 2; j < WIDTH-2; j++){
				if(board[i-2][j-2] == 'O' && board[i-1][j-1] == 'O' && board[i][j] =='O' && board[i+1][j+1] == 'O' && board[i+2][j+2] == 'O'){			// case of diagonal win(shape of \)
					return 1;
				}
				else if(board[i-2][j-2] == 'X' && board[i-1][j-1] == 'X' && board[i][j] =='X' && board[i+1][j+1] == 'X' && board[i+2][j+2] == 'X'){		// case of diagonal win(shape of \)
					return 1;
				}
				else if(board[i-2][j+2] == 'O' && board[i-1][j+1] == 'O' && board[i][j] == 'O' && board[i+1][j-1] == 'O' && board[i+2][j-2] == 'O'){	// case of diagonal win(shape of /)
					return 1;
				}
				else if(board[i-2][j+2] == 'X' && board[i-1][j+1] == 'X' && board[i][j] == 'X' && board[i+1][j-1] == 'X' && board[i+2][j-2] == 'X'){	// case of diagonal win(shape of /)
					return 1;
				}
			}
		}
	}
	else if(players == 3){
		for(int i = 0; i < HEIGHT; i++){ // WIDTH win check
			for(int j = 3; j < WIDTH; j++){ 
				if(board[i][j-3] == 'O' && board[i][j-2] == 'O' && board[i][j-1] == 'O' && board[i][j] == 'O'){
					return 1;
				}
				else if(board[i][j-3] == 'X' && board[i][j-2] == 'X' && board[i][j-1] == 'X' && board[i][j] == 'X'){
					return 1;
				}
				else if(board[i][j-3] == 'Y' && board[i][j-2] == 'Y' && board[i][j-1] == 'Y' && board[i][j] == 'Y'){
					return 1;
				}
			}
		}
		for(int i = 0; i < WIDTH; i++){ // HEIGHT win check
			for(int j = 3; j < HEIGHT; j++){
				if(board[j-3][i] == 'O' && board[j-2][i] == 'O' && board[j-1][i] == 'O' && board[j][i] == 'O'){
					return 1;
				}
				else if(board[j-3][i] == 'X' && board[j-2][i] == 'X' && board[j-1][i] == 'X' && board[j][i] == 'X'){
					return 1;
				}		
				else if(board[j-3][i] == 'Y' && board[j-2][i] == 'Y' && board[j-1][i] == 'Y' && board[j][i] == 'Y'){
					return 1;
				}			
			}
		}
		for(int i = 3; i < HEIGHT; i++){ // DIAGONAL win check '\' SHAPE
			for(int j = 3; j < WIDTH; j++){
				if(board[i-3][j-3] == 'O' && board[i-2][j-2] == 'O' && board[i-1][j-1] =='O' && board[i][j] == 'O'){
					return 1;
				}
				else if(board[i-3][j-3] == 'X' && board[i-2][j-2] == 'X' && board[i-1][j-1] =='X' && board[i][j] == 'X'){
					return 1;
				}
				else if(board[i-3][j-3] == 'Y' && board[i-2][j-2] == 'Y' && board[i-1][j-1] =='Y' && board[i][j] == 'Y'){
					return 1;
				}
			}
		}
		for(int i = 3; i < HEIGHT; i++){ // DIAGONAL win check '/' SHAPE
			for(int j = 0; j < WIDTH - 3; j++){
				if(board[i-3][j+3] == 'O' && board[i-2][j+2] == 'O' && board[i-1][j+1] == 'O' && board[i][j] == 'O'){
					return 1;
				}
				else if(board[i-3][j+3] == 'X' && board[i-2][j+2] == 'X' && board[i-1][j+1] == 'X' && board[i][j] == 'X'){
					return 1;
				}
				else if(board[i-3][j+3] == 'Y' && board[i-2][j+2] == 'Y' && board[i-1][j+1] == 'Y' && board[i][j] == 'Y'){
					return 1;
				}
			}
		}
	}
	return 0;							// If there's no winner.
}

int Action(WINDOW *win, int **board, int keyin, int *row, int *col, int *turn, int players, int *save){	// function to manipulate actions by user.
	mvprintw(HEIGHT + 6, 5,"                                  ");			// hide there's already stone message.
	if(keyin == KEY_UP){													// when up_arrow typed.	
		if(*row-1 < 0){														// when cursor heading to -row.
			return 0;														// avoid cursor bound error.
		}
		wmove(win, *row-1, *col);											// move cursor to upper.
		*row -= 1;															// apply changes to row					
		return 0;						
	}
	else if(keyin == KEY_DOWN){												// when down_arrow typed.
		if(*row+1 > HEIGHT - 1){											// when cursor heading to HEIGHT limit.
			return 0;														// avoid cursor bound error.
		}
		wmove(win, *row+1, *col);											// move cursor to lower.
		*row += 1;															// apply changes to row
		return 0;
	}
	else if(keyin == KEY_LEFT){												// when left_arrow typed.
		if(*col-1 < 0){														// when cursor heading to -col.
			return 0;														// avoid cursor bound error.
		}
		wmove(win, *row, *col-1);											// move cursor to leftside.
		*col -= 1;															// apply changes to col.
		return 0;
	}
	else if(keyin == KEY_RIGHT){											// when right_arrow typed.
		if(*col+1 > WIDTH - 1){												// when cursor heading to WIDTH limit.
			return 0;														// avoid cursor bound error.
		}
		wmove(win, *row, *col+1);											// move cursor to rightside.
		*col += 1;															// apply changes to col.
		return 0;
	}
	else if(keyin == KEY_SPACE || keyin == KEY_Enter){														// when space or enter key typed.
		if(board[*row][*col] != 'O' && board[*row][*col] != 'X' && board[*row][*col] != 'Y'){				// when cursor is not on the set baduk stone.
			if(*turn == 1){																					// when player1's turn.
				board[*row][*col] = 'O';																	// set 'O' stone on the board.
			}	
			else if(*turn == 2){																			// when player2's turn.
				board[*row][*col] = 'X';																	// set 'X' stone on the board.
			}
			else if(*turn == 3){																			// when player3's turn.
				board[*row][*col] = 'Y';																	// set 'Y' stone on the board.
			}					
			paintBoard(board, win, *col, *row); 															// To show the stone normally when the current set stone is key of win.

			if(checkWin(board, *turn, players) == 1){														// check if there's winner. and the case there's a winner.
				if(*turn == 1){																				// when player1's turn.
					mvprintw(HEIGHT + 2, 5, "PLAYER1 WIN !! Press any button to termiante the program");	// print player1 won.
					if(getch() != ERR){																		// when any button pressed.
						return 1;																			// return 1 to show winner is decided.
					}
				}
				else if(*turn == 2){																		// when player2's turn.
					mvprintw(HEIGHT + 2, 5, "PLAYER2 WIN !! Press any button to termiante the program");	// print player2 won.
					if(getch() != ERR){																		// when any button pressed.
						return 1;																			// return 1 to show winner is decided.
					}
				}
				else if(*turn == 3){																		// when player2's turn.
					mvprintw(HEIGHT + 2, 5, "PLAYER3 WIN !! Press any button to termiante the program");	// print player2 won.
					if(getch() != ERR){																		// when any button pressed.
						return 1;																			// return 1 to show winner is decided.
					}
				}
			}
			if(players == 2){
				if(*turn == 1){																					// when player1's turn ended.
					*turn = 2;																					// change turn to player2.
				}
				else if(*turn == 2){																			// when player2's turn ended.
					*turn = 1;																					// change turn to player1.
				}					
			}
			else if(players == 3){
				if(*turn == 1){																					// when player1's turn ended.
					*turn = 2;																					// change turn to player2.
				}
				else if(*turn == 2){																			// when player2's turn ended.
					*turn = 3;																					// change turn to player3.
				}
				else if(*turn == 3){																			// when player3's turn ended.
					*turn = 1;																					// change turn to player1.
				}
			}
		}
		else{																								// when cursor is on the set baduk stone.
			mvprintw(HEIGHT + 6, 5,"There's another stone already.");										// print error message.
			return 0;																						// return 0 because there's no winner.
		}
	}
	else if(keyin == '1'){																						// case of player wants to save file.
		char file[100];																							// declare file array to scan file name.
		char* filename;																							// declare pointer to save filename and carry to SaveGame()
		filename = file;																						// make space to txt
		mvprintw(6, WIDTH + 4, "ENTER FILE NAME : ");															// print
		echo();																									// show the string that player is writing.
		scanw("%s", filename);																					// scan the file name.
		saveGame(board, players, *row, *col, *turn, filename);													// call SaveGame to save Game status.
		noecho();																								// do not show the string that player is writing.
		*save = 1;																								// set save's element 1 to end game normally.
		return 0;																								// return 0 because there's no winner.
	}
	else if(keyin == '2'){																						// case of player doesn't save game and end the game.
		*save = -1; 																							// set save's element -1 to end game normally.
		return 0;
	}
	
	return 0;																									// return 0 because there's no winner.
}


void gameStart(WINDOW *win, char* filename, int load, int players){												// function to start and end game.
	int **board;																								// two-dimension pointer to save gameboard.
	int row = 0;																								// integer to save current row value.
	int col = 0;																								// integer to save current col value.
	int keyin;																									// integer to save lastly typed key.
	int turn = 1;																								// integer to save whose turn.
	int save = 0;																								// integer to save 'game save or not'

	if(load == 1){																								// when start game with savedfile.
		delwin(win);																							// delete win because it's temp win.
		readSavedGameStatus(&row, &col, &turn, &players, filename);												// call readsavedgamestatus to get ready to initboard.
		WINDOW * win = newwin(HEIGHT, WIDTH, 0, 0);																// declare new window by saved HEIGHT, WIDTH value.
		board = initBoard(board, &row, &col, &turn); 															// Initiating the board	
		readSavedGame(board, &row, &col, &turn, &players, filename);											// call readsavedgame to load set stone.
		load = 0;																								// CHANGE load's value to 'this if statement' runs only one time.
	}
	else{																										// when start game without savedfile.
		board = initBoard(board, &row, &col, &turn); 															// Initiating the board
	}
	wmove(win, row, col); 																						// move cursor to lastly saved row and col position.
	keypad(stdscr, TRUE);																						// function to allow to use keypad key.
	
	while(1){																									// function to player's action can be tried until the winner's emergence.

		paintBoard(board, win, row, col);																		// call paintboard to show game board on win.
		paintMenu(board, win, row, col, turn, players);															// call gamemenu to show gamemenu(below the gameboard) on win.
		keyin = getch();																						// wait for key input and save it for keyin.
		if(Action(win, board, keyin, &row, &col, &turn, players, &save) == 1){									// when win or lose determined.
			for(int i = 0; i < HEIGHT; i++){																	// for loop to free memory of 2nd dimension.
				free(board[i]);																					// free each memory.
			}
			free(board);																						// free memory of board.
			break;																								// stop the while loop.
		}
		else{																									// when this turn ended with no winner.
			if(save == 1){																						// when player saved this game.
				for(int i = 0; i < HEIGHT; i++){																// for loop to free memory of 2nd dimension.
						free(board[i]);																			// free each memory.
					}
				free(board);																					// free memory of board.
				break;																							// stop the while loop.
			}																									
			else if(save == -1){																				// when player exit this game.
				for(int i = 0; i < HEIGHT; i++){																// for loop to free memory of 2nd dimension.
					free(board[i]);																				// free each memory.
				}
				free(board);																					// free memory of board.
				break;																							// stop the while loop.
			}
		}
		refresh();																								// refresh to show up-to-date status.
		wrefresh(win);																							// refresh of win to avoid error.
	}

	return;
}

int main(){
	int load = 0;															// integer to save 'load the game or not'(It is used in StartGame too).
	char savedfile[100];													// char array to save string of savedgamefile's name.
	int players;															// integer to save playernum.
	char start;																// integer to save 'load the game or not'(it is used only in main() ).
	int escape = 0;															// integer to exit the program when there's error case.
 												
	printf("Want to load the game?[y/n] : ");											
	scanf("%c", &start);													// scan load the saved gamefile or not.
	if(start == 'n'){														// when doesn't use saved gamefile.
		printf("Enter the HEIGHT of the board : ");							
		scanf("%d", &HEIGHT);												// scan HEIGHT of the board.
		printf("Enter the WIDTH of the board : ");
		scanf("%d", &WIDTH);												// scan WIDTH of the board.
		printf("Enter the number of players[2/3] : ");
		scanf("%d", &players);												// scan playernum of the board.
		if(HEIGHT == 0 || WIDTH == 0){										// if there's zero in size.
			printf("\nInvalid Inputs!(Invalid height or width.)\n");
			return 0;														// end the program.
		}
		if(players == 2 || players == 3){									// when playernum is 2 or 3.
			initscr();														// to start curses mode.
			noecho();														// to do not show typed key.
			keypad(stdscr, TRUE);											// allow to use keypad key.
			nodelay(stdscr, FALSE);											// to wait program at getch()
			WINDOW * win = newwin(HEIGHT, WIDTH, 0, 0);						// declare window size of HEIGHT*WIDTH
			gameStart(win, savedfile, load, players);						// start the game.
			delwin(win);													// delete the win.
			endwin();														// end the curses mode.
			printf("Game Ended.\n");										// print the game end message.
			return 0;														// stop the program.
		}
		else{
			printf("\nInvalid Inputs!(Unsupported players amount.)\n");		// when another playernum is typed.
			return 0;														// stop the program.
		}
	}
	else if(start == 'y'){													// when using saved gamefile.
		printf("Enter the name of the file : ");
		scanf("%s", savedfile);												// scan the saved gamefile's name.
		FILE* fp;																// file pointer to handle fileopen error.
		if ((fp = fopen(savedfile, "r")) == NULL){								// if there's no file named savedfile.
			printf("\nInvalid Inputs! There's no file named %s.\n", savedfile); // print error message.
			return 0;															// terminate the program.
		}
		load = 1;															// set load to 1 to load game status at StartGame
		initscr();															// to start curses mode.
		noecho();															// to do not show typed key.
		keypad(stdscr, TRUE);												// allow to use keypad key.
		nodelay(stdscr, FALSE);												// to wait program at getch()
		WINDOW * win = newwin(HEIGHT, WIDTH, 0, 0); 						// temp window creation(because here's no HEIGHT, WIDTH yet.)
		gameStart(win, savedfile, load, players);							// start the game.
		delwin(win);														// delete the win.
		endwin();															// end the curses mode.
		printf("Game Ended.\n");											// print the game end message.
		return 0;															// stop the program.
	}
	else{																	// when another input typed.
		printf("Invalid Input. Type [y/n].\n");								// print error message.
		return 0;															// end the program.
	}
	return 0;
}
