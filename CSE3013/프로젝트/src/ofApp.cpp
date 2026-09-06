/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.
	
	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

	http://www.spout.zeal.co

    =========================================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    =========================================================================

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
using namespace std;
//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("Random Maze!!"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth()-windowWidth)/2, (ofGetScreenHeight()-windowHeight)/2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked
	
	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true;  // screen info display on
	menu->AddPopupItem(hPopup, "Show DFS",false,false); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Random Start", false, false); // Not checked (default)
	menu->AddPopupItem(hPopup, "Random Exit", false, false); // Not checked (default)
	menu->AddPopupItem(hPopup, "All Random!!", false, false); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if(title == "Open") {
		readFile();
	}
	if(title == "Exit") {
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//
	if(title == "Show DFS") {								//DFS 경로 화면상에 표시
		if (isOpen)											//파일이 열린 경우
		{
			START_ROW = 0; START_COL = 0;					//시작지점 설정
			EXIT_ROW = WIDTH - 1; EXIT_COL = HEIGHT - 1;	//탈출지점 설정
			DFS();											//DFS 호출
			isdfs = true;									//DFS FLAG
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;
	}

	if(title == "Random Exit") {
		if (isOpen)
		{
			while (1) {									//시작/탈출지점이 다를 때까지
				exitmani();								//랜덤 탈출지점
				if (!(EXIT_ROW == 0 && EXIT_COL == 0)) { //시작/탈출지점이 다른 경우
					break;
				}
			}
			if (isdfs) {							//화면상에 DFS 경로가 존재하는 경우
				free(track); tracknum = 0;			//모든자취 array free, 모든자취 경로개수 초기화
				free(mtrack); mtracknum = 0;		//최단경로 array free, 최단경로 경로개수 초기화
			}
			START_COL = 0; START_ROW = 0;			//시작지점 설정
			DFS();									//DFS 호출
			isdfs = true;							//DFS FLAG
		}
		else
			cout << "you must open file first" << endl;
	}
	if (title == "Random Start") {
		if (isOpen)
		{
			while (1) {									//시작/탈출지점이 다를 때까지
				startmani();
				if (!(START_ROW == WIDTH - 1 && START_COL == HEIGHT - 1)) { //시작/탈출지점이 다른 경우
					break;
				}
			}
			if (isdfs) {							//화면상에 DFS 경로가 존재하는 경우
				free(track); tracknum = 0;			//모든자취 array free, 모든자취 경로개수 초기화
				free(mtrack); mtracknum = 0;		//최단경로 array free, 최단경로 경로개수 초기화
			}
			EXIT_COL = HEIGHT - 1; EXIT_ROW = WIDTH - 1;	//탈출지점 설정
			DFS();											//DFS 호출
			isdfs = true;									//DFS FLAG
		}
		else
			cout << "you must open file first" << endl;
	}
	if (title == "All Random!!") {
		if (isOpen)
		{
			while (1) {													//시작/탈출지점이 다를 때까지
				startmani();												//랜덤 시작지점
				exitmani();													//랜덤 탈출지점
				if (!(START_ROW == EXIT_ROW && START_COL == EXIT_COL)) { //시작/탈출지점이 다른 경우
					break;
				}
			}
			if (isdfs) {							//화면상에 DFS 경로가 존재하는 경우
				free(track); tracknum = 0;			//모든자취 array free, 모든자취 경로개수 초기화
				free(mtrack); mtracknum = 0;		//최단경로 array free, 최단경로 경로개수 초기화
			}
			DFS();											//DFS 호출
			isdfs = true;									//DFS FLAG
		}
		else
			cout << "you must open file first" << endl;
	}
	if(title == "Full screen") {					//전체화면
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if(title == "About") {
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {

	char str[256];
	//ofBackground(0, 0, 0, 0);
	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;
	
	if (isOpen) {
		for (i = 0; i < cHEIGHT; i++) {
			for (j = 0; j < cWIDTH; j++) {
				if (input[i][j] == '-') {	//가로선 그리기
					ofDrawLine(15 * (j - 0.65), 15 * (i + 0.5), 15 * (j + 1.65), 15 * (i + 0.5));
				}
				else if (input[i][j] == '|') {	//세로선 그리기
					ofDrawLine(15 * (j + 0.5), 15 * (i - 0.65), 15 * (j + 0.5), 15 * (i + 1.65));
				}
			}
		}
	}

	if (isdfs)			//DFS가 실행된 경우
	{
		if (isOpen)		//파일이 열려있으면
			dfsdraw();	//DFS 경로 그리기
		else
			cout << "You must open file first" << endl;
	}

	ofSetColor(200);
	sprintf(str, "Final Project");
	myFont.drawString(str, 15, ofGetHeight()-20);

} 


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if(bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else { 
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU)); 
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth()-ofGetWidth())/2, (ofGetScreenHeight()-ofGetHeight())/2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if(bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if(bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if(GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

} 

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
bool ofApp::readFile()
{
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	string filePath;
	size_t pos;
	// Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		//string fileName = "maze0.maz";
		printf("file name is %s\n", fileName);
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

			ofFile file(fileName);

			if (!file.exists()) {
				cout << "Target file does not exists." << endl;
				return false;
			}
			else {
				cout << "We found the target file." << endl;
				isOpen = 1;
			}

			ofBuffer buffer(file);

			//[0]파일을 불러왔을 때 이전 파일로 인한 메모리가 잔재하는 경우 freeMemory 호출
			if (memory_flag == 1) freeMemory(); 

			HEIGHT = 0; WIDTH = 0; cHEIGHT = 0; cWIDTH = 0;
			
			//[1]미로 크기를 모르므로 파일을 한번 읽어서 크기 먼저 구하기
			int i = 0;
			for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
				string line = *it;
				cWIDTH = 0;
				for (i = 0; line[i] == '+' || line[i] == '-' || line[i] == ' ' || line[i] == '|'; i++) {
					cWIDTH++;
				}
				cHEIGHT++;
			}
			printf("size is HEIGHT : %d WIDTH: %d\n", cHEIGHT, cWIDTH);


			//[2]입력받은 미로 크기에 따라 input, map 메모리 동적할당
			HEIGHT = cHEIGHT / 2; WIDTH = cWIDTH / 2;			//cHEIGHT, cWDITH는 char**인 input 크기, HEIGHT, WIDTH는 int**인 map 크기

			input = (char**)malloc(sizeof(char*)*cHEIGHT);
			for (i = 0; i < cHEIGHT; i++) {
				input[i] = (char*)malloc(sizeof(char) * cWIDTH);
			}
			printf("INPUT malloc ended\n");

			map = (graph**)malloc(sizeof(graph*) * HEIGHT);
			for (int i = 0; i < HEIGHT; i++) {
				map[i] = (graph*)malloc(sizeof(graph) * WIDTH);
			}
			printf("MAP malloc ended\n");
			memory_flag = 1;									//memory 할당이 되었으므로 flag set 1


			//[3]다시 읽어서 input에 char 저장
			i = 0;
			for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
				string line = *it;
				for (int j = 0; j < cWIDTH; j++) {
					input[i][j] = line[j];
					printf("%c", input[i][j]);
				}
				printf("\n");
				i++;
			}//input 정보넣기 작업 + 콘솔에 .maz 형태 출력


			//[4]map 정보넣기 작업
			i = 0; roomnum = 0;
			int linenum = 0;
			for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
				string line = *it;
				if (linenum == 0) {
					i++; linenum++;  continue;
				}
				else if (linenum % 2 == 1) {		//홀수열(방이 있는 열)
					for (int j = 0; j < WIDTH; j++) {
						if (line[2*j + 2] == ' ')	map[i-1][j].right = 1;	//1은 연결
						else						map[i-1][j].right = 0;	//0은 폐쇄
						map[i-1][j].id = roomnum++;
					}
				}
				else if (linenum % 2 == 0) {		//짝수열(방이 없는 열)
					for (int j = 0; j < WIDTH; j++) {
						if (line[2*j + 1] == ' ')	map[i-1][j].down = 1;	//1은 연결
						else						map[i-1][j].down = 0;	//0은 폐쇄
					}
					i++;
				}
				linenum++;	
			}
		}
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
}
void ofApp::freeMemory() {
	for (int i = 0; i < cHEIGHT; i++) {
		free(input[i]);
	}
	free(input);							//미로형태 char** free
	for (int i = 0; i < HEIGHT; i++) {
		free(map[i]);
	}
	free(map);								//미로구조 graph free
	if (isdfs) {
		free(track); tracknum = 0;			//모든경로 array free, 모든경로 경로개수 초기화
		free(mtrack); mtracknum = 0;		//최단경로 array free, 최단경로 경로개수 초기화
		isdfs = false;
	}
	memory_flag = 0;						//memory free되었으므로 flag set 0
	printf("Memory freed\n");
}

bool ofApp::DFS()										//DFS탐색을 하는 함수
{	
	printf("DFS started\n");
	int** mark;											//visited 체킹할 int** mark;
	mark = (int**)malloc(sizeof(int*) * HEIGHT + 1);
	for (int i = 0; i < HEIGHT + 1; i++) {
		mark[i] = (int*)calloc(WIDTH + 1, sizeof(int));
	}

	int i = START_COL; int j = START_ROW;				//시작지점 좌표지정
	graph init;											//시작지점 graph
	init = map[i][j];
	init.xpos = j;
	init.ypos = i; 
	s.push(init);										//최단거리 stack에 처음위치 push
	all.push(init);										//모든자취 stack에 처음위치 push
	mark[i][j] = 1;										//처음위치에서 시작하므로 처음위치 방문한 적 있는 것으로 설정

	while (!s.empty()) {								//스택 s가 비어있지 않은 동안만
		graph top = s.top();							//최단거리 stack의 top 값
		
		if (i == EXIT_COL && j == EXIT_ROW) {		//EXIT ROW COL 도착했으므로 더 이상 경로찾기(dfs) 실행 안 함
			break;
		}

		if (top.right == 1 && mark[i][j+1] == 0) {		//오른쪽이 열려있고 오른쪽에 간 적이 없다면
			graph right;								//push할 graph right
			right = map[i][j + 1];						//right의 set값
			strcpy(right.tag, "right");					//pop하는 과정에서 이전 위치로 돌아갈 때를 위해 tag를 사용
			right.xpos = j+1;							//push하는 graph의 좌표값을 오른쪽의 좌표로
			right.ypos = i;

			s.push(right);								//최단거리 stack에 push
			all.push(right);							//모든자취 stack에 push
			mark[i][j + 1] = 1;							//이동한 적 있는 상태로설정
			j++;										//현재 좌표 이동(오른쪽으로)
			continue;									//이동했으므로 다른 push나 pop생략
		}

		if (top.down == 1 && mark[i+1][j] == 0) {		//아래가 열려있고 아래에 간 적이 없다면
			graph down;									//graph down에 아래방 좌표 저장
			down = map[i+1][j];			
			strcpy(down.tag, "down");
			down.xpos = j; down.ypos = i+1;

			s.push(down);								//s, all에 down push
			all.push(down);
			mark[i + 1][j] = 1;							//아래 방 이동한 것으로 표기
			i++;										//아래로 이동
			continue;
		}

		if (j > 0) { //왼쪽이 열려있고 왼쪽에 간 적이 없다면			(segmentation fault 방지를 위해 j>0)
			if (map[i][j - 1].right == 1 && mark[i][j - 1] == 0) {
				graph left;											//graph left에 왼쪽방 좌표 저장
				left = map[i][j - 1];
				strcpy(left.tag, "left");
				left.xpos = j-1; left.ypos = i;

				s.push(left);										//s, all에 left push
				all.push(left);
				mark[i][j - 1] = 1;									//왼쪽 방 이동한 것으로 표기
				j--;												//왼쪽으로 이동
				continue;	
			}
		}

		if (i > 0) { //위가 열려있고 위에 간 적이 없다면				(segmentation fault 방지를 위해 i>0)
			if (map[i - 1][j].down == 1 && mark[i - 1][j] == 0) {
				graph up;											//graph up에 왼쪽방 좌표 저장
				up = map[i - 1][j];
				strcpy(up.tag, "up");
				up.xpos = j;
				up.ypos = i - 1;

				s.push(up);											//s, all에 up push
				all.push(up);
				mark[i - 1][j] = 1;									//위쪽 방 이동한 것으로 표기
				i--;												//위쪽으로 이동
				continue;
			}
		}

		graph pop = s.top();							//graph pop에 s의 top 값 저장
		all.push(pop);									//모든경로 stack은 모든 경로를 가져야하므로 push
		if (strcmp(pop.tag, "right") == 0) {			//pop의 tag가 right이면
			j--;										//왼쪽으로
		}
		else if (strcmp(pop.tag, "down") == 0) {		//pop의 tag가 down이면
			i--;										//위쪽으로
		}
		else if (strcmp(pop.tag, "up") == 0) {			//pop의 tag가 up이면
			i++;										//아래쪽으로
		}
		else if (strcmp(pop.tag, "left") == 0) {		//pop의 tag가 left면
			j++;										//오른쪽으로
		}
		s.pop();										//최단거리 stack은 돌아간 자취는 필요 없으므로 pop
		
	}

	track = (graph*)malloc(sizeof(graph) * 10000);								//stack에 있던 graph들을 array에 저장(draw()함수가 프로그램이 동작되는 동안 무한히 작동하기 때문에 drawdfs에서 pop할 수 없음.
	while (!all.empty()) {														//all stack에서 하나씩 pop하여 track에 저장
		track[tracknum] = all.top();
		all.pop();
		tracknum++;
	}
	mtrack = (graph*)malloc(sizeof(graph) * 10000);
	while (!s.empty()) {														//s stack에서 하나씩 pop하여 mtrack에 저장
		mtrack[mtracknum] = s.top();
		s.pop();
		mtracknum++;
	}
	
	return 0;
}
void ofApp::dfsdraw()
{
		ofSetLineWidth(6);

		ofSetColor(51, 255, 51);
		for (int j = 0; j < tracknum - 1; j++) {																									//모든경로 그리기
			if (!strcmp(track[j].tag, "right")) {
				ofDrawLine(30 * (track[j].xpos - 1.09) + 21, 30 * track[j].ypos + 21, 30 * track[j].xpos + 21, 30 * track[j].ypos + 21);
			}
			else if (!strcmp(track[j].tag, "down")) {
				ofDrawLine(30 * track[j].xpos + 21, 30 * (track[j].ypos - 1.09) + 21, 30 * track[j].xpos + 21, 30 * track[j].ypos + 21);
			}
			else if (!strcmp(track[j].tag, "up")) {
				ofDrawLine(30 * track[j].xpos + 21, 30 * (track[j].ypos + 1.09) + 21, 30 * track[j].xpos + 21, 30 * track[j].ypos + 21);
			}
			else if (!strcmp(track[j].tag, "left")) {
				ofDrawLine(30 * (track[j].xpos + 1.09) + 21, 30 * track[j].ypos + 21, 30 * track[j].xpos + 21, 30 * track[j].ypos + 21);
			}
		}

		ofSetColor(255, 153, 51);																													//최단경로 그리기
		for (int j = 0; j < mtracknum - 1; j++) {
			if (!strcmp(mtrack[j].tag, "right")) {
				ofDrawLine(30 * (mtrack[j].xpos - 1.09) + 21, 30 * mtrack[j].ypos + 21, 30 * mtrack[j].xpos + 21, 30 * mtrack[j].ypos + 21);
			}
			else if (!strcmp(mtrack[j].tag, "down")) {
				ofDrawLine(30 * mtrack[j].xpos + 21, 30 * (mtrack[j].ypos - 1.09) + 21, 30 * mtrack[j].xpos + 21, 30 * mtrack[j].ypos + 21);
			}
			else if (!strcmp(mtrack[j].tag, "up")) {
				ofDrawLine(30 * mtrack[j].xpos + 21, 30 * (mtrack[j].ypos + 1.09) + 21, 30 * mtrack[j].xpos + 21, 30 * mtrack[j].ypos + 21);
			}
			else if (!strcmp(mtrack[j].tag, "left")) {
				ofDrawLine(30 * (mtrack[j].xpos + 1.09) + 21, 30 * mtrack[j].ypos + 21, 30 * mtrack[j].xpos + 21, 30 * mtrack[j].ypos + 21);
			}
		}

		ofSetColor(0, 51, 204);
		ofDrawRectangle(30 * (START_ROW)+15, 30 * (START_COL)+15, 12, 12);	//시작지점
		ofSetColor(255, 0, 51);
		ofDrawRectangle(30 * (EXIT_ROW)+15, 30 * (EXIT_COL)+15, 12, 12);	//탈출지점
	
	return;
}

void ofApp::exitmani() {
	random_device rd;									//랜덤엔진 설정
	mt19937 mt(rd());
	uniform_int_distribution<int> dist(0, WIDTH - 1);	//범위는 WIDTH-1
	auto randNum = dist(mt);
	EXIT_ROW = randNum;									//랜덤 EXIT_ROW

	uniform_int_distribution<int> d(0, HEIGHT - 1);		//범위는 HEIGHT - 1
	randNum = d(mt);	
	EXIT_COL = randNum;									//랜덤 EXIT_COL
}
void ofApp:: startmani() {
	random_device rd;									//랜덤엔진 설정
	mt19937 mt(rd());
	uniform_int_distribution<int> dist(0, WIDTH - 1);	//범위는 WIDTH-1
	auto randNum = dist(mt);
	START_ROW = randNum;								//랜덤 START_ROW

	uniform_int_distribution<int> d(0, HEIGHT - 1);		//범위는 HEIGHT - 1
	randNum = d(mt);
	START_COL = randNum;								//랜덤 START_COL
}