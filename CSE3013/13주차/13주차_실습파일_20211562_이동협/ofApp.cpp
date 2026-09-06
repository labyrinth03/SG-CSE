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

	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
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
	menu->AddPopupItem(hPopup, "Show BFS"); // Not checked (default)
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
	if(title == "Show DFS") {
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		if (isOpen)
		{
			DFS();
			isdfs = true;
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;
		
	}

	if(title == "Show BFS") {
		doTopmost(bChecked); // Use the checked value directly

	}

	if(title == "Full screen") {
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
	
	// TO DO : DRAW MAZE; 
	// 저장된 자료구조를 이용해 미로를 그린다.
	// add code here
	if (isOpen) {
		for (i = 0; i < cHEIGHT; i++) {
			for (j = 0; j < cWIDTH; j++) {
				if (input[i][j] == '-') {
					ofDrawLine(15 * (j - 0.65), 15 * (i + 0.5), 15 * (j + 1.65), 15 * (i + 0.5));
				}
				else if (input[i][j] == '|') {
					ofDrawLine(15 * (j + 0.5), 15 * (i - 0.65), 15 * (j + 0.5), 15 * (i + 1.65));
				}
			}
		}
	}

	

	if (isdfs)
	{
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen)
			dfsdraw();
		else
			cout << "You must open file first" << endl;
	}
	if(bShowInfo) {
		// Show keyboard duplicates of menu functions
		sprintf(str, " comsil project");
		myFont.drawString(str, 15, ofGetHeight()-20);
	}

} // end Draw


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
	
	// Escape key exit has been disabled but it can be checked here
	if(key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if(bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if(key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if(key == 'f') {
		bFullscreen = !bFullscreen;	
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}

} // end keyPressed

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

			// Input_flag is a variable for indication the type of input.
			// If input_flag is zero, then work of line input is progress.
			// If input_flag is one, then work of dot input is progress.
			int input_flag = 0;

			// Idx is a variable for index of array.
			int idx = 0;

			// Read file line by line
			int cnt = 0;
			


			// TO DO
			// .maz 파일을 input으로 받아서 적절히 자료구조에 넣는다

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

			printf("MAP\n");
			for (i = 0; i < HEIGHT; i++) {
				for (int j = 0; j < WIDTH; j++) {
					printf("%2d", map[i][j].id);
				}
				printf("\n");
			}
			//map value 출력

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
	free(input);						//미로형태 char** free
	for (int i = 0; i < HEIGHT; i++) {
		free(map[i]);
	}
	free(map);							//미로 형태 graph free
	free(track); tracknum = 0;			//모든자취 array free, 모든자취 경로개수 초기화
	free(mtrack); mtracknum = 0;		//최단경로 array free, 최단경로 경로개수 초기화
	memory_flag = 0;					//memory free되었으므로 flag set 0
	printf("Memory freed\n");
	//TO DO
	// malloc한 memory를 free해주는 함수
}

bool ofApp::DFS()										//DFS탐색을 하는 함수
{	
	printf("DFS started\n");
	int** mark;											//visited 체킹할 int** mark;
	mark = (int**)malloc(sizeof(int*) * HEIGHT + 1);
	for (int i = 0; i < HEIGHT + 1; i++) {
		mark[i] = (int*)calloc(WIDTH + 1, sizeof(int));
	}
	printf("calloc ended\n");


	int i = 0; int j = 0;
	graph init;
	init = map[i][j];
	init.xpos = j;
	init.ypos = i;
	s.push(init);										//최단거리 stack에 처음위치 push
	all.push(init);										//모든자취 stack에 처음위치 push
	mark[i][j] = 1;										//처음위치에서 시작하므로 처음위치 방문한 적 있는 것으로 설정

	while (!s.empty()) {
		
		graph top = s.top();							//최단거리 stack의 top 값
		printf("here is %d\n", top.id);
		if (i == HEIGHT - 1 && j == WIDTH - 1) {		//EXIT ROW COL 도착했으므로 더 이상 경로찾기(dfs) 실행 안 함
			printf("EXIT!!!!!!!\n");
			break;
		}

		if (top.right == 1 && mark[i][j+1] == 0) {		//오른쪽이 열려있고 오른쪽에 간 적이 없다면
			printf("we go to right\n");
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
			printf("we go to down\n");
			graph down;
			down = map[i+1][j];			
			strcpy(down.tag, "down");
			down.xpos = j; down.ypos = i+1;

			s.push(down);
			all.push(down);
			mark[i + 1][j] = 1;
			i++;
			continue;
		}

		if (j > 0) { //왼쪽이 열려있고 왼쪽에 간 적이 없다면			(좌하 이동의 우선순위를 우상보다 낮게 잡음(어차피 시작지점은 좌상, 도착지점은 우하이므로) (segmentation fault 방지를 위해 j>0)
			if (map[i][j - 1].right == 1 && mark[i][j - 1] == 0) {
				printf("we go to left\n");
				graph left;
				left = map[i][j - 1];
				strcpy(left.tag, "left");
				left.xpos = j-1; left.ypos = i;

				s.push(left);
				all.push(left);
				mark[i][j - 1] = 1;
				j--;
				continue;
			}
		}

		if (i > 0) { //위가 열려있고 위에 간 적이 없다면				(segmentation fault 방지를 위해 i>0)
			if (map[i - 1][j].down == 1 && mark[i - 1][j] == 0) {
				printf("we go to up\n");
				graph up;
				up = map[i - 1][j];
				strcpy(up.tag, "up");
				up.xpos = j;
				up.ypos = i - 1;

				s.push(up);
				all.push(up);
				mark[i - 1][j] = 1;
				i--;
				continue;
			}
		}
			printf("we go back\n");
			graph pop = s.top();
			all.push(pop);									//모든자취 stack은 모든 자취를 가져야하므로 push
			if (strcmp(pop.tag, "right") == 0) {
				j--;
			}
			else if (strcmp(pop.tag, "down") == 0) {
				i--;
			}
			else if (strcmp(pop.tag, "up") == 0) {
				i++;
			}
			else if (strcmp(pop.tag, "left") == 0) {
				j++;
			}
			s.pop();										//최단거리 stack은 돌아간 자취는 필요 없으므로 pop
		
	}
	printf("stack completed and printing\n");
	/*while (!s.empty()) {
		graph top;
		top = s.top();
		printf("%d ", top.id);
		s.pop();
	}
	printf("\n all track printing\n");
	while (!all.empty()) {
		graph top;
		top = all.top();
		printf("%d ", top.id);
		all.pop();
	}
	printf("\n");*/
	track = (graph*)malloc(sizeof(graph) * 10000);								//stack에 있던 graph들을 array에 저장(draw()함수가 프로그램이 동작되는 동안 무한히 작동하기 때문에 drawdfs에서 pop할 수 없음.
	track[0].xpos = HEIGHT - 1; track[0].ypos = WIDTH - 1; tracknum++;
	while (!all.empty()) {
		track[tracknum] = all.top();
		printf("track %d %d\n", track[tracknum].ypos, track[tracknum].xpos);
		all.pop();
		tracknum++;
	}
	mtrack = (graph*)malloc(sizeof(graph) * 10000);
	mtrack[0].xpos = HEIGHT - 1; mtrack[0].ypos = WIDTH - 1; mtracknum++;
	while (!s.empty()) {
		mtrack[mtracknum] = s.top();
		s.pop();
		mtracknum++;
	}

	//dfsdraw();
	//isdfs = true;
	return 0;
}
void ofApp::dfsdraw()
{
		ofSetColor(0, 255, 0);
		//ofDrawRectangle(15, 15, 10, 10);											//시작지점
		ofSetColor(0, 0, 225);		
		//ofDrawRectangle(30 * (HEIGHT - 1) + 15, 30 * (WIDTH - 1) + 15, 10, 10);	//도착지점
		ofSetLineWidth(5);

		ofSetColor(225, 0, 0);
		
		for (int j = 0; j < tracknum - 1; j++) {																							//모든자취 그리기
			if (abs(track[j].xpos - track[j+1].xpos) + abs(track[j].ypos - track[j + 1].ypos) <= 1) {										//예외방지(그리는 선의 길이가 1 초과인 경우)
				
				ofDrawLine(30 * track[j].xpos + 20, 30 * track[j].ypos + 20, 30 * track[j + 1].xpos + 20, 30 * track[j + 1].ypos + 20);
			}
			else {
				//printf("problem %d %d, tag is %s", track[j].xpos, track[j].ypos, track[j].tag);
				if (!strcmp(track[j].tag ,"right")) {
					//printf(" right\n");
					ofDrawLine(30 * (track[j].xpos-1) + 20, 30 * track[j].ypos + 20, 30 * track[j].xpos + 20, 30 * track[j].ypos + 20);
				}
				else if (!strcmp(track[j].tag, "down")) {
					//printf(" down\n");
					ofDrawLine(30 * track[j].xpos + 20, 30 * (track[j].ypos-1) + 20, 30 * track[j].xpos + 20, 30 * track[j].ypos + 20);
				}
				else if (!strcmp(track[j].tag, "up")) {
					//printf(" up\n");
					ofDrawLine(30 * track[j].xpos + 20, 30 * (track[j].ypos+1) + 20, 30 * track[j].xpos + 20, 30 * track[j].ypos + 20);
				}
				else if (!strcmp(track[j].tag, "left")) {
					//printf(" left\n");
					ofDrawLine(30 * (track[j].xpos+1) + 20, 30 * track[j].ypos + 20, 30 * track[j].xpos + 20, 30 * track[j].ypos + 20);
				}
				/*for (int k = j; k >= 0; k++) {
					if (abs(track[j].xpos - track[k].xpos) + abs(track[j].ypos - track[k].ypos) <= 1) {
						ofDrawLine(30 * track[j].xpos + 20, 30 * track[j].ypos + 20, 30 * track[k].xpos + 20, 30 * track[k].ypos + 20);
						break;
					}
				}*/
				
			}
		}

		ofSetColor(0, 225, 0);																													//최단거리 그리기
		if(mtracknum) ofDrawLine(30 * mtrack[0].xpos + 20, 30 * mtrack[0].ypos + 20, 30 * (HEIGHT - 1) + 20, 30 * (WIDTH - 1) + 20);
		for (int j = 0; j < mtracknum - 1; j++) {
			if (abs(mtrack[j].xpos - mtrack[j + 1].xpos) + abs(mtrack[j].ypos - mtrack[j + 1].ypos) <= 1) {

				ofDrawLine(30 * mtrack[j].xpos + 20, 30 * mtrack[j].ypos + 20, 30 * mtrack[j + 1].xpos + 20, 30 * mtrack[j + 1].ypos + 20);
			}
		}
		/*graph temp1;
		temp1 = all.top();
		
		all.pop();
		while (!all.empty() && all.size() > 0) {

			graph top;
			top = all.top();
			printf("draw from %d %d to %d %d\n", top.xpos, top.ypos, temp1.xpos, temp1.ypos);
			if (abs(temp1.xpos - top.xpos) + abs(temp1.ypos - top.ypos) <= 1) {
				ofDrawLine(30 * temp1.xpos + 20, 30 * temp1.ypos + 20, 30 * top.xpos + 20, 30 * top.ypos + 20);
			}
			temp1 = all.top();
			all.pop();
		}*/
		
		//printf("drawing ended\n");
	


	//TO DO 
	//DFS를 수행한 결과를 그린다. (3주차 내용)
	
	return;
}

