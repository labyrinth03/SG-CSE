#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Definitions.h"

Window wd;
Status st;
My_Polygon pg;
float zoom_scale;  // zoom 기능에 사용하는 변수

// GLUT callbacks
void timer(int value) {
	rotate_points_around_center_of_grivity(&pg);
	glutPostRedisplay();
	if (st.rotation_mode)
		glutTimerFunc(ROTATION_STEP, timer, 0);
}


void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	if (pg.n_points > 0)
		draw_lines_by_points(&pg, zoom_scale, st.leftbuttonpressed);	//현재 확대 비율과 이동 모드인지에 따른 polygon 그리기
	if (st.polygon_mode) {
		update_center_of_gravity(&pg);			// 무게중심 업데이트
	}
	
	 
	if (st.polygon_mode) {						// 무게중심 그리기
		glColor3f(CENTER_POINT_COLOR);
		glBegin(GL_POINTS);
		glVertex2f(pg.center_x, pg.center_y);
		glEnd();
	}

	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'c':
		if (!st.rotation_mode) {
			pg.n_points = 0, st.polygon_mode = 0;
		}
		zoom_scale = 1.0f;						// 확대비율 초기화
		st.pkeypressed = 0;						// p 키 눌림 확인 초기화
		glutPostRedisplay();
		break;
	case 'p':
		st.pkeypressed = 1;						// p 키 눌림 확인
		if (!st.polygon_mode) {
			if (pg.n_points >= 3) {				// polygon이 만들어졌을 시
				st.polygon_mode = 1;
				draw_lines_by_points(&pg, zoom_scale, st.leftbuttonpressed);	//현재 확대 비율과 이동 모드인지에 따른 polygon 그리기
				fprintf(stderr, "*** Polygon selection is finished!\n");
				glutPostRedisplay();
			}
			else {
				fprintf(stderr, "*** Choose at least three points!\n");
			}
		}
		break;
	case 'r':
		if (st.polygon_mode) {
			if (!st.rotation_mode) {
				update_center_of_gravity(&pg);
			}
			st.rotation_mode = 1 - st.rotation_mode;
			if (st.rotation_mode)
				glutTimerFunc(ROTATION_STEP, timer, 0);
		}
		break;
	case 'f':
		glutLeaveMainLoop(); 
		break;
	}
}

void special(int key, int x, int y) {
	if (st.rotation_mode || !st.polygon_mode) return;
	switch (key) {
	case GLUT_KEY_LEFT:
		move_points(&pg, -TRANSLATION_OFFSET, 0.0f);
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		move_points(&pg, TRANSLATION_OFFSET, 0.0f);
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		move_points(&pg, 0.0f, -TRANSLATION_OFFSET);
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		move_points(&pg, 0.0f, TRANSLATION_OFFSET);
		glutPostRedisplay();
		break;
	}
}

static int prev_x, prev_y;
void mousepress(int button, int state, int x, int y) {
	if (st.rotation_mode) {															//회전 모드일 때
		if ((button == GLUT_LEFT_BUTTON) &&
			(abs(x - (int)((pg.center_x + 1.0f) * 0.5f * wd.width)) <= 3) &&
			(abs(y - (int)((1.0f - pg.center_y) * 0.5f * wd.height)) <= 3)) {		// 좌클릭으로 무게중심을 눌렀다면
			if (state == GLUT_DOWN) {												// 눌렀을 때 좌클릭 flag를 1로 바꾸고 커서 좌표 전달
				st.leftbuttonpressed = 1;
				prev_x = x, prev_y = y;
			}
			else if (state == GLUT_UP)												// 뗐을 때 flag를 0으로 설정
				st.leftbuttonpressed = 0;
		}
	}
	if (!st.polygon_mode && st.pkeypressed) {							// 도형을 그리기 전
		if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {	//우클릭을 눌렀다면
			printf("### The right mouse button is pressed at (%d, %d).\n", x, y);
			int key_state = glutGetModifiers();
			if (key_state & GLUT_ACTIVE_CTRL) {							// ctrl을 누른 상태에서 우클릭을 눌렀다면
				add_point(&pg, &wd, x, y);								// 점을 추가
				glutPostRedisplay();
			}
		}
	}
	else {										//polygon mode
		if ((button == GLUT_LEFT_BUTTON) &&
			(abs(x - (int)((pg.center_x + 1.0f) * 0.5f * wd.width)) <= 3) &&
			(abs(y - (int)((1.0f - pg.center_y) * 0.5f * wd.height)) <= 3)) {	// polygon 모드에서 무게중심을 좌클릭
			if (state == GLUT_DOWN) {											// 좌클릭 했다면 flag를 1로 설정하고 커서 좌표 전달
				st.leftbuttonpressed = 1;										// 이동 모드를 위한 좌클릭 flag
				prev_x = x, prev_y = y;
			}
			else if (state == GLUT_UP)											// 뗐을 때 flag를 0으로 설정
				st.leftbuttonpressed = 0;
		}
	}
}

void mousemove(int x, int y) {
	if (st.leftbuttonpressed && st.polygon_mode) {	// 좌클릭이 눌려있고 polygon 모드일 때
		float delx, dely;
		delx = 2.0f * ((float) x - prev_x) / wd.width;
		dely = 2.0f * ((float) prev_y - y) / wd.height;
		prev_x = x, prev_y = y;
		move_points(&pg, delx, dely);
		glutPostRedisplay();
	}
}



void mousewheel(int wheel, int direction, int x, int y) {	// 확대축소 기능을 위한 wheel event 처리 함수
	if (direction > 0) {									// wheel이 위로 올라갔을 때		
		zoom_scale += 0.02f;								// 확대 비율 설정 (처음 비율 대비 2% 확대)
	}
	else {
		if (zoom_scale > 0.02f) {
			zoom_scale -= 0.02f;							// 축소 비율 설정 (처음 비율 대비 2% 축소)
		}
		else {												// 축소 비율이 0 이하로 가는 것을 방지
			printf("Can't zoom more\n");
		}
	}

	glutPostRedisplay();
}


void reshape(int width, int height) {
	fprintf(stdout, "### The new window size is %dx%d.\n", width, height);
	wd.width = width, wd.height = height;
	glViewport(0, 0, wd.width, wd.height);
}

void close(void) {
	fprintf(stdout, "\n^^^ The control is at the close callback function now.\n\n");
}
// End of GLUT callbacks

void initialize_polygon_editor(void) {
	wd.width = 800, wd.height = 600, wd.initial_anchor_x = 500, wd.initial_anchor_y = 200;
	st.rightbuttonpressed = 0, st.leftbuttonpressed = 0, st.pkeypressed = 0, st.rotation_mode = 0, st.polygon_mode = 0;
	pg.n_points = 0; pg.center_x = 0.0f; pg.center_y = 0.0f; zoom_scale = 1.0f;
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mousepress);
	glutMotionFunc(mousemove);
	glutMouseWheelFunc(mousewheel);  // wheel 이벤트 함수 등록
	glutReshapeFunc(reshape);
	glutCloseFunc(close);
}

void initialize_renderer(void) {
	register_callbacks();

	glPointSize(5.0);
	glClearColor(BACKGROUND_COLOR, 1.0f);
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = TRUE;
	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 4
int main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 SimplefreeGLUTcode_Polygon_Editor";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'p', 'c', 'r', 'f'",
		"    - Special keys used: LEFT, RIGHT, UP, DOWN",
		"    - Mouse used: L-click, R-click and move",
		"    - Other operations: window reshape"
	};

	glutInit(&argc, argv);
	initialize_polygon_editor();

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE); // <-- Be sure to use this profile for this example code!
 //	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_RGBA);

	glutInitWindowSize(wd.width, wd.height);
	glutInitWindowPosition(wd.initial_anchor_x, wd.initial_anchor_y);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

   // glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_EXIT); // default
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	
	glutMainLoop();
	fprintf(stdout, "^^^ The control is at the end of main function now.\n\n");
	return 0;
}
