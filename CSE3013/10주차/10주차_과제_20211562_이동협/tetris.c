#include "tetris.h"

static struct sigaction act, oact;
int ranknum;

int main(){
	int exit=0;
	createRankList();
	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	rRoot = (RecNode*)malloc(sizeof(RecNode));
	rRoot->accumulatedScore = 0;
	rRoot->level = 0;
	rRoot->f = field;

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_REC_PLAY: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void initTree(RecNode *root){
	if(root->level+1 > VISIBLE_BLOCKS){
		return;
	}
	else{
		for(int i = 0; i < CHILDREN_MAX; i++){
			root->child[i] = (RecNode*)malloc(sizeof(RecNode));
			root->child[i]->level = root->level+1;
			root->child[i]->accumulatedScore = 0;
			root->child[i]->f = (char(*)[WIDTH])malloc(sizeof(char)*HEIGHT*WIDTH);
			root->child[i]->parent = root;
			initTree(root->child[i]);
		}
		return;
	}
}
void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0] = rand()%7; nextBlock[1] = rand()%7; nextBlock[2] = rand()%7;

	tNode = NULL;
	initTree(rRoot);
	recommend(rRoot);
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
	
	DrawBlockWithFeatures (blockY, blockX, nextBlock[0], blockRotate);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);

	DrawBox(9,WIDTH+10,4,8); // 두번째 블럭칸
	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
		move(10+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}		
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	for(int i = 0; i < 4; i++){ 
	    for(int j = 0; j < 4; j++){
	        if(block[currentBlock][blockRotate][i][j] == 1){            
				if(blockY+i >= HEIGHT || blockX+j < 0 || blockX+j >= WIDTH){ // 블록을 나타내는 4×4 행렬의 각 요소의 실제 필드상의 y 좌표가 HEIGHT보다 크거나 같은지 여부 // 블록을 나타내는 4×4 행렬의 각 요소의 실제 필드상의 x 좌표가 0보다 작은지 혹은 WIDTH 크거나 같은지 여부 
					return 0;
				}
	        	else if(f[blockY+i][blockX+j] == 1){ // 블록을 놓으려고 하는 필드에 이미 블록이 쌓여져 있는지 여부
	            	return 0;
 	   			}	
 	   		}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code

	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다.
	switch(command){
	case KEY_UP:
    	for(int i = 0; i < 4; i++){
			int prevRotate;
			prevRotate = blockRotate-1;
			if (prevRotate == -1) prevRotate = 3;
    		for(int j = 0; j < 4; j++){
    		    if(block[nextBlock[0]][prevRotate][i][j] == 1 && field[blockY+i][blockX+j] == 1){
					field[blockY+i][blockX+j] = 0;
				}
    		}
		}

		break;		
	case KEY_DOWN:
    	for(int i = 0; i < 4; i++){
    		for(int j = 0; j < 4; j++){
    		    if(block[nextBlock[0]][blockRotate][i][j] == 1 && field[blockY+i-1][blockX+j] == 1){
					field[blockY+i][blockX+j] = 0;
				}
    		}
		}

		break;	
	case KEY_RIGHT:
    	for(int i = 0; i < 4; i++){
    		for(int j = 0; j < 4; j++){
    		    if(block[nextBlock[0]][blockRotate][i][j] == 1 && field[blockY+i][blockX+j-1] == 1){
					field[blockY+i][blockX+j] = 0;
				}
    		}
		}

		break;	
	case KEY_LEFT:
    	for(int i = 0; i < 4; i++){
    		for(int j = 0; j < 4; j++){
    		    if(block[nextBlock[0]][blockRotate][i][j] == 1 && field[blockY+i][blockX+j+1] == 1){
					field[blockY+i][blockX+j] = 0;
				}
    		}
		}

		break;		
	default:
		break;
	}
	DrawField();
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
}

void BlockDown(int sig){
	// user code

	//강의자료 p26-27의 플로우차트를 참고한다.
	timed_out = 0;
	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX) == 1){
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	} 
	else{
	    if (blockY == -1){
			gameOver = 1;
		}
	    else{
			score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
	    	score += DeleteLine(field);
			nextBlock[0] = nextBlock[1]; nextBlock[1] = nextBlock[2];
			nextBlock[2] = rand()%7;
 	    	blockRotate = 0; blockY = -1; blockX=WIDTH/2-2;
			
			
			tNode = NULL;	//추천 블럭을 그릴 정보를 담은 트리노드
			recommend(rRoot);
					
    		DrawNextBlock(nextBlock);
			PrintScore(score);
			DrawField();
			DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);		
		}
	}
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	//Block이 추가된 영역의 필드값을 바꾼다.
	int touched = 0;
	
	for(int i = 0; i < 4; i++){
    	for(int j = 0; j < 4; j++){
        	if(block[currentBlock][blockRotate][i][j] == 1){
				f[blockY+i][blockX+j] = 1;
				if(f[blockY+i+1][blockX+j] == 1){
					touched++;
				}
				else if(blockY+i+1 == 22) touched++;
			} 
    	}
	}
	return touched*10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
	int dline = 0;
	for(int i = HEIGHT-1; i >= 0; i--){
    	int flag = 0;
		for(int j = 0; j < WIDTH; j++){
			if(f[i][j] == 0){
				flag = 1;
				break;
			}
		}
    	if(flag == 0){
    	    dline++;
    	    for(int y = i; y >0; y--){
    	        for(int x = 0; x < WIDTH; x++){
    	            f[y][x] = f[y-1][x];
    	        }
    	    }
			DrawField();
			i++;
    	}
	}
	return dline*dline*100; 
}

void DrawShadow(int y, int x, int blockID, int blockRotate){
	while(CheckToMove(field, blockID, blockRotate, y, x) == 1){
		y++;
	}
	y--;
	DrawBlock(y, x, blockID, blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	DrawField();//field에 add된 블록의 그림자를 없애기 위해서
	
	DrawBlock(y, x, blockID, blockRotate, ' ');
	DrawShadow(y, x, blockID, blockRotate);
	DrawRecommend(); //그림자보다는 추천위치를 우선으로 보았음
}

void createRankList(){
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기
	FILE *fp;
	int i, j;

	//1. 파일 열기
	fp = fopen("rank.txt", "r");

	// 2. 정보읽어오기
	/* int fscanf(FILE* stream, const char* format, ...);
	stream:데이터를 읽어올 스트림의 FILE 객체를 가리키는 파일포인터
	format: 형식지정자 등등
	변수의 주소: 포인터
	return: 성공할 경우, fscanf 함수는 읽어들인 데이터의 수를 리턴, 실패하면 EOF리턴 */
	// EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문
	if(fp != NULL){

		if (fscanf(fp, "%d", &ranknum) != EOF) { // 랭크 개수 스캔
			node* temp;
			temp = (node*)malloc(sizeof(node));
			temp->next = NULL;

			fscanf(fp, "%s %d", temp->name, &temp->score); 
			head = temp;
			head->next = NULL;

			for(int i = 0; i < ranknum - 1; i++){
				node* temp;
				temp = (node*)malloc(sizeof(node));
				temp->next = NULL;
				fscanf(fp, "%s %d", temp->name, &temp->score);
			
				node1 = head; 

				for(node2 = head; node2; node2 = node2->next){
					if(node2->score < temp->score && node2 == head){ // 가장 큰 값
						temp->next = node2;
						head = temp;
						break;
					}
					else if(node2->next == NULL && node2->score > temp->score){ // 가장 작은 값
						node2->next = temp;
						break;
					}
					else if(node1->score >= temp->score && node2->score <= temp->score){ // 중간 값(맨 처음과 현재 node2 사이)
						temp->next = node1->next;
						node1->next = temp;
						break;
					}
					node1 = node2;
				}
			}
		}
		else {
			ranknum = 0;
			fprintf(fp, "0\n");			
		}

	}
	else{
		fp = fopen("rank.txt", "w");
		fprintf(fp, "0\n");
	}
	// 4. 파일닫기
	fclose(fp);
}

void rank(){
	
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	//1. 문자열 초기화
	int X = 0; int Y = 0;
	int score_number, ch, i, j;
	clear();

	//2. printw()로 3개의 메뉴출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	
	//3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);

	//4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	if (ch == '1') {
		echo();
		printw("X: "); scanw("%d", &X);
		printw("Y: "); scanw("%d", &Y);
		noecho();
		printw("       name      |      score       \n");
		printw("------------------------------------\n");
		//X--; Y--;
		if(ranknum == 0){
			printw("search failure: no rank in the list\n");
		}
		else if(Y <= ranknum && X == 0 && Y != 0){
			node* temp;
			temp = (node*)malloc(sizeof(node));
			temp = head;
			int j = 0;
			for(i = 0; i < Y; i++, j++){
				mvprintw(7+j, 1, "%s", temp->name);
				mvprintw(7+j, 17, "| %d", temp->score);
				temp = temp->next;
			}
		}
		else if(X != 0 && Y == 0){
			node* temp;
			temp = (node*)malloc(sizeof(node));
			temp = head;
			int j = 0;
			for(int i = 0; i < X-1; i++){
				temp = temp->next;
			}			
			for(i = X; i <= ranknum; i++, j++){
				mvprintw(7+j, 1, "%s", temp->name);
				mvprintw(7+j, 17, "| %d", temp->score);
				temp = temp->next;
			}				
		}
		else if(X == 0 && Y == 0){
			node* temp;
			temp = (node*)malloc(sizeof(node));
			temp = head;
			int j = 0;
			for(i = 0; i < ranknum; i++, j++){
				mvprintw(7+j, 1, "%s", temp->name);
				mvprintw(7+j, 17, "| %d", temp->score);
				temp = temp->next;
			}			
		}
		else if(Y <= ranknum && X <= Y && ranknum != 0 && X <= ranknum){
			node* temp;
			temp = (node*)malloc(sizeof(node));
			temp = head;
			int j = 0;
			for(int i = 0; i < X-1; i++){
				temp = temp->next;
			}
			for(i = X; i <= Y; i++, j++){
				mvprintw(7+j, 1, "%s", temp->name);
				mvprintw(7+j, 17, "| %d", temp->score);
				temp = temp->next;
			}
			
		}
		else{
			printw("search failure: no rank in the list\n");
		}
	}

	//4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if ( ch == '2') {
		char str[NAMELEN+1] = "";
		int check = 0;						//검색한 결과가 없는 경우를 위한 변수
		echo();
		printw("input the name: "); scanw("%s", &str);
		noecho();		
		printw("       name      |      score       \n");
		printw("------------------------------------\n");
		if(strcmp(str, "") == 0){
			printw("search failure: no name in the list\n");
		}
		else{
			node* temp;
			temp = (node*)malloc(sizeof(node));
			temp = head;
			for(i = 0; i < ranknum; i++){
				if (strcmp(temp->name, str) == 0){
					mvprintw(6+check, 1, "%s", temp->name);
					mvprintw(6+check, 17, "| %d", temp->score);
					check++;			
				}
				temp = temp->next;
			}
			if (check == 0){
				printw("search failure: no information in the list\n");
			}
		}
	}

	//4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
	else if ( ch == '3') {
		int num; int count = 0;
		echo();
		printw("input the rank: "); scanw("%d", &num);
		noecho();
		if(num <= 0 || num > ranknum || ranknum == 0){
			printw("search failure: the rank not in the list\n");
		}
		else if (num == 1){	
			head = head->next;
			ranknum--;
			printw("result: the rank deleted\n");
		}
		else{
			node* temp;
			temp = (node*)malloc(sizeof(node));
			node* previousnode;
			previousnode = (node*)malloc(sizeof(node));			
			node* nextnode;
			nextnode = (node*)malloc(sizeof(node));

			temp = head;
			int j = 0;
			for(int i = 1; i < num-1; i++){
				temp = temp->next;
			}
			previousnode = temp;
			temp = temp->next; temp = temp->next;
			nextnode = temp;
			previousnode->next = nextnode;
			printw("result: the rank deleted\n");
			ranknum--;
		}
		writeRankFile();
	}

	getch();	//각 기능 수행 후 아무 키가 눌리면 메뉴로 이동
}

void writeRankFile(){
	// user code
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	int sn, i;
	//1. "rank.txt" 연다
	FILE *fp = fopen("rank.txt", "r");
	fscanf(fp, "%d", &sn);
	//2. 랭킹 정보들의 수를 "rank.txt"에 기록
	
	//3. 탐색할 노드가 더 있는지 체크하고 있으면 다음 노드로 이동, 없으면 종료
	if (sn == ranknum){
		fclose(fp); return;
	}
	else {
		fclose(fp); fp = fopen("rank.txt", "w");
		fprintf(fp, "%d\n", ranknum);
		node* temp;
		temp = (node*)malloc(sizeof(node));
		temp = head;
		for(int i = 0; i < ranknum; i++){
			fprintf(fp, "%s %d\n", temp->name, temp->score);
			temp = temp->next;
		}
		fclose(fp);
		return;
	}
}

void newRank(int score){
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	node* temp;
	temp = (node*)malloc(sizeof(node));	
	int i, j;
	clear();
	//1. 사용자 이름을 입력받음
	echo();
	printw("yourname: "); scanw("%s", &temp->name);
	temp->score = score; temp->next = NULL;
	noecho();
	//2. 새로운 노드를 생성해 이름과 점수를 저장, ranknum이 0이면(저장된게 없으면) 헤드에 바로저장 
	if(ranknum == 0){
		head = temp;
	}
	else{
		node1 = head; 
		for(node2 = head; node2; node2 = node2->next){
			if(node2 == head && node2->score < temp->score ){
				temp->next = node2;
				head = temp;
				break;
			}
			else if(node2->next == NULL && node2->score > temp->score){
				node2->next = temp;
				break;
			}
			else if(node1->score >= temp->score && node2->score <= temp->score){
				temp->next = node1->next;
				node1->next = temp;
				break;
			}
			node1 = node2;
		}
	}
	ranknum++; // 새로운 랭크가 저장되었으니 랭크 개수 ++
	writeRankFile(); // 랭크 변동이 있으니 랭크파일 재작성
}
///////////////////////////////////////////////////////////////////////////
void DrawRecommend(){
	RecNode *pnode;
	if(tNode){
		pnode = tNode;
		for(int i = 0; i < VISIBLE_BLOCKS - 1; i++){
			pnode = pnode->parent;
		}
		DrawBlock(pnode->recBlockY, pnode->recBlockX, pnode->curBlockID, pnode->recBlockRotate, 'R');
	}
	
}

int recommend(RecNode *root){
	int max = 0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int rotate[7] = {2, 4, 4, 4, 1, 2, 2};	//각 blockID마다 회전체의 개수가 다르므로(일부는 겹치는 경우가 있으니) 이를 배열로 쉽게 꺼내쓸 수 있게
	int num = 0;
	int bottom; int touched = 0; int deletion = 0; int delete = 0;
	RecNode *croot;	
	int currentID = nextBlock[root->level];
	

	if(root->level + 1 > VISIBLE_BLOCKS){	//depth가 최대에 달하면 tNode에 최대점수를 갖는 트리노드를 저장
		if(tNode == NULL){
			tNode = root;
		}
		else if(root->accumulatedScore >= tNode->accumulatedScore){
			tNode = root;
		}
		return 0;
	}	
												
	for(int br = 0; br < rotate[nextBlock[root->level]]; br++){				//br: blockrotate
		for(int rx = -4; rx < WIDTH; rx++){										//rx: recommend 상에서의 x좌표
			if(CheckToMove(root->f, currentID, br, 0, rx) == 1){ 			// (rx, 0)좌표에 대해 이동이 가능하다면
				croot = root->child[num];									//childnode 설정
				croot->level = root->level + 1;								//childnode의 level 설정(현재 노드의 level + 1)
				croot->curBlockID = currentID;								//childnode의 currentID 설정
				croot->recBlockRotate = br;									//childnode의 blockrotate값 설정
				croot->recBlockX = rx;										//childnode의 x값 설정	

				for(int rfx = 0; rfx < HEIGHT; rfx++){													//childnode의 recField initialize
					for(int rfy = 0; rfy < WIDTH; rfy++){
						croot->f[rfx][rfy] = root->f[rfx][rfy];//c
					}
				}						

					
				for(bottom = 0; bottom < HEIGHT; bottom++){												//childnode가 어디까지 내려갈 수 있는지
					if(CheckToMove(croot->f, currentID, br, bottom+1, rx) == 0){
						break;
					}
				}
				croot->recBlockY = bottom;

				deletion = 0;
				for(int i = 0; i < HEIGHT; i++){												//블록 설치 후 deleteline으로 얻는 점수 고려
					delete = 0;
					for(int j = 0; j < WIDTH; j++){
						if(croot->f[i][j] == 1){
							delete++;
						}
					}
					if(delete == WIDTH){
						deletion++;
					}
				}
					
				touched = 0;
				for(int i = 0; i < BLOCK_HEIGHT; i++){											//블록 설치 후 필드에 닿아서 얻는 점수 고려
					for(int j = 0; j < BLOCK_WIDTH; j++){
						if(block[currentID][br][i][j] == 1){
							if(bottom + i == HEIGHT - 1){
								touched++;
							}
							else if(croot->f[bottom+i+1][rx+j] == 1){
								touched++;
							}
							croot->f[bottom+i][rx+j] = 1;
						}
					}
				}




				croot->accumulatedScore = root->accumulatedScore + (10*touched) + (deletion*deletion*100);	//해당 블록배치로 얻을 수 있는 점수를 저장
				recommend(croot);
				num++;

			}
		}
	}

	return max;

}

void recommendedPlay(){
	int command;

	RecNode* node;

	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	nodelay(stdscr,TRUE);

	recommend(rRoot);
	do{
		node = tNode;
		for (int i = 0; i < VISIBLE_BLOCKS-1; i++)
			node = node->parent;

		blockX=node->recBlockX;
		blockRotate=node->recBlockRotate;
		blockY=node->recBlockY;
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		// if(blockY < -1){
		// 	gameOver = 1;
		// }


		if(CheckToMove(field, node->curBlockID, blockRotate, blockY, blockX)){
			AddBlockToField(field, node->curBlockID, blockRotate, blockY, blockX);
		}
		

		command = GetCommand();

		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1, WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

int modified_recommend(RecNode* root){
	int max = 0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int rotate[7] = {2, 4, 4, 4, 1, 2, 2};	//각 blockID마다 회전체의 개수가 다르므로(일부는 겹치는 경우가 있으니) 이를 배열로 쉽게 꺼내쓸 수 있게
	int num = 0;
	int bottom; int touched = 0; int deletion = 0; int delete = 0;
	RecNode *croot;	
	int currentID = nextBlock[root->level];
	
	if(root->level + 1 > VISIBLE_BLOCKS){	//depth가 최대에 달하면 tNode에 최대점수를 갖는 트리노드를 저장
		if(tNode == NULL){
			tNode = root;
		}
		else if(root->accumulatedScore >= tNode->accumulatedScore && root->recBlockY < tNode->recBlockY){
			tNode = root;
		}
		return 0;
	}	
												
	for(int br = 0; br < rotate[nextBlock[root->level]]; br++){				//br: blockrotate
		for(int rx = -3; rx < WIDTH; rx++){									//rx: recommend 상에서의 x좌표
			if(CheckToMove(root->f, currentID, br, 0, rx) == 1){ 			// (rx, 0)좌표에 대해 이동이 가능하다면
				croot = root->child[num];									//childnode 설정
				croot->level = root->level + 1;								//childnode의 level 설정(현재 노드의 level + 1)
				croot->curBlockID = currentID;								//childnode의 currentID 설정
				croot->recBlockRotate = br;									//childnode의 blockrotate값 설정
				croot->recBlockX = rx;										//childnode의 x값 설정	
				for(int rfx = 0; rfx < HEIGHT; rfx++){													//childnode의 recField initialize
					for(int rfy = 0; rfy < WIDTH; rfy++){
						croot->f[rfx][rfy] = root->f[rfx][rfy];//c
					}
				}							
				for(bottom = 0; bottom < HEIGHT; bottom++){												//childnode가 어디까지 내려갈 수 있는지
					if(CheckToMove(croot->f, currentID, br, bottom+1, rx) == 0){
						break;
					}
				}
				croot->recBlockY = bottom;
				deletion = 0;
				for(int i = 0; i < HEIGHT; i++){												//블록 설치 후 deleteline으로 얻는 점수 고려
					delete = 0;
					for(int j = 0; j < WIDTH; j++){
						if(croot->f[i][j] == 1){
							delete++;
						}
					}
					if(delete == WIDTH){
						deletion++;
					}
				}
				touched = 0;
				for(int i = 0; i < BLOCK_HEIGHT; i++){											//블록 설치 후 필드에 닿아서 얻는 점수 고려
					for(int j = 0; j < BLOCK_WIDTH; j++){
						if(block[currentID][br][i][j] == 1){
							if(bottom + i == HEIGHT - 1){
								touched++;
							}
							else if(croot->f[bottom+i+1][rx+j] == 1){
								touched++;
							}
							croot->f[bottom+i][rx+j] = 1;
						}
					}
				}
				croot->accumulatedScore = root->accumulatedScore + (10*touched) + (deletion*deletion*100);	//해당 블록배치로 얻을 수 있는 점수를 저장
				recommend(croot);
				num++;
			}
		}
	}
	return max;	
}