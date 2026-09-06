#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Shaders/LoadShaders.h"
#include "Scene_Definitions.h"

Scene scene;
bool show_camera_frame = false;
bool mouse_left_button_down = false; 

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (camera_ID_mapper[CAMERA_MAIN] < scene.camera_list.size()) {
			Camera& main_camera = scene.camera_list[camera_ID_mapper[CAMERA_MAIN]].get();
			if (main_camera.flag_valid) {
				main_camera.view_port.x = 0;
				main_camera.view_port.y = 0;
				main_camera.view_port.w = scene.window.width * 0.75;
				main_camera.view_port.h = scene.window.height * 0.75;


				glViewport(main_camera.view_port.x, main_camera.view_port.y,
					main_camera.view_port.w, main_camera.view_port.h);
				scene.ViewMatrix = main_camera.ViewMatrix;
				scene.ProjectionMatrix = main_camera.ProjectionMatrix;

				scene.draw_world();

				scene.axis_object.draw_axis(static_cast<Shader_Simple*>(&scene.shader_list[shader_ID_mapper[SHADER_SIMPLE]].get()),
					scene.ViewMatrix, scene.ProjectionMatrix); 

				if(show_camera_frame)
					scene.draw_camera_axes_in_viewport(main_camera);
			}
		}

		int cctv_width = scene.window.width / 4;
		int cctv_height = scene.window.height / 4;

		if (scene.show_cctv_viewport) {
			Camera* current_cctv_camera_ptr = nullptr;
			switch (scene.current_cctv_camera_idx) {
			case 0:
				if (camera_ID_mapper[CAMERA_CC0] < scene.camera_list.size())
					current_cctv_camera_ptr = &scene.camera_list[camera_ID_mapper[CAMERA_CC0]].get();
				break;
			case 1:
				if (camera_ID_mapper[CAMERA_CC1] < scene.camera_list.size())
					current_cctv_camera_ptr = &scene.camera_list[camera_ID_mapper[CAMERA_CC1]].get();
				break;
			case 2:
				if (camera_ID_mapper[CAMERA_CC2] < scene.camera_list.size())
					current_cctv_camera_ptr = &scene.camera_list[camera_ID_mapper[CAMERA_CC2]].get();
				break;
			}

			if (current_cctv_camera_ptr && current_cctv_camera_ptr->flag_valid) {
				Camera& current_cctv_camera = *current_cctv_camera_ptr;

				current_cctv_camera.view_port.x = 0;
				current_cctv_camera.view_port.y = scene.window.height - (scene.window.height / 4); // 좌측 상단
				current_cctv_camera.view_port.w = scene.window.width / 4;
				current_cctv_camera.view_port.h = scene.window.height / 4;

				glViewport(current_cctv_camera.view_port.x, current_cctv_camera.view_port.y,
					current_cctv_camera.view_port.w, current_cctv_camera.view_port.h);

				scene.ViewMatrix = current_cctv_camera.ViewMatrix;
				scene.ProjectionMatrix = current_cctv_camera.ProjectionMatrix;

				scene.draw_static_world();
				scene.draw_dynamic_world();

				scene.axis_object.draw_axis(
					static_cast<Shader_Simple*>(&scene.shader_data.shader_simple),
					scene.ViewMatrix,
					scene.ProjectionMatrix
				);

				if(show_camera_frame)
					scene.draw_camera_axes_in_viewport(current_cctv_camera);
			}
		}


		int dynamic_cam_width = scene.window.width / 4;
		int dynamic_cam_height = scene.window.height / 4;
		int dynamic_cam_margin = 10;

		if (camera_ID_mapper[CAMERA_DYNAMIC] < scene.camera_list.size()) {
			Camera& dynamic_camera = scene.camera_list[camera_ID_mapper[CAMERA_DYNAMIC]].get();
			if (dynamic_camera.flag_valid) {
				dynamic_camera.view_port.x = dynamic_cam_width;
				dynamic_camera.view_port.y = scene.window.height - (scene.window.height / 4);
				dynamic_camera.view_port.w = dynamic_cam_width;
				dynamic_camera.view_port.h = dynamic_cam_height;

				glViewport(dynamic_camera.view_port.x, dynamic_camera.view_port.y, dynamic_camera.view_port.w, dynamic_camera.view_port.h);
				
				
				scene.ViewMatrix = dynamic_camera.ViewMatrix;
				scene.ProjectionMatrix = dynamic_camera.ProjectionMatrix;
				scene.draw_world(); 
				scene.axis_object.draw_axis(static_cast<Shader_Simple*>(&scene.shader_list[shader_ID_mapper[SHADER_SIMPLE]].get()),
					scene.ViewMatrix, scene.ProjectionMatrix);

				if(show_camera_frame)
					scene.draw_camera_axes_in_viewport(dynamic_camera);
			}
		}


		int ortho_view_width = scene.window.width / 5; 
		int ortho_view_height = scene.window.height / 5;
		int ortho_margin = 10;
		int ortho_start_x = scene.window.width - ortho_view_width - ortho_margin;


		if (camera_ID_mapper[CAMERA_VIEW_FRONT] < scene.camera_list.size()) {
			Camera& front_view = scene.camera_list[camera_ID_mapper[CAMERA_VIEW_FRONT]].get();
			if (front_view.flag_valid) {
				front_view.view_port.x = ortho_start_x * 0.88;
				front_view.view_port.y = (scene.window.height - ortho_view_height - ortho_margin)*0.8;
				front_view.view_port.w = ortho_view_width * 1.5;
				front_view.view_port.h = ortho_view_height * 1.5;
				glViewport(front_view.view_port.x, front_view.view_port.y, front_view.view_port.w, front_view.view_port.h);
				scene.ViewMatrix = front_view.ViewMatrix;
				scene.ProjectionMatrix = front_view.ProjectionMatrix;
				scene.draw_world();
				scene.axis_object.draw_axis(static_cast<Shader_Simple*>(&scene.shader_list[shader_ID_mapper[SHADER_SIMPLE]].get()),
					scene.ViewMatrix, scene.ProjectionMatrix);

				if(show_camera_frame)
					scene.draw_camera_axes_in_viewport(front_view);
			}
		}


		if (camera_ID_mapper[CAMERA_VIEW_SIDE_RIGHT] < scene.camera_list.size()) {
			Camera& side_view = scene.camera_list[camera_ID_mapper[CAMERA_VIEW_SIDE_RIGHT]].get();
			if (side_view.flag_valid) {
				side_view.view_port.x = ortho_start_x;
				side_view.view_port.y = scene.window.height / 2 - ortho_view_height / 2;
				side_view.view_port.w = ortho_view_width;
				side_view.view_port.h = ortho_view_height;
				glViewport(side_view.view_port.x, side_view.view_port.y, side_view.view_port.w, side_view.view_port.h);
				scene.ViewMatrix = side_view.ViewMatrix;
				scene.ProjectionMatrix = side_view.ProjectionMatrix;
				scene.draw_world();
				scene.axis_object.draw_axis(static_cast<Shader_Simple*>(&scene.shader_list[shader_ID_mapper[SHADER_SIMPLE]].get()),
					scene.ViewMatrix, scene.ProjectionMatrix);

				if(show_camera_frame)
					scene.draw_camera_axes_in_viewport(side_view);
			}
		}


		if (camera_ID_mapper[CAMERA_VIEW_TOP] < scene.camera_list.size()) {
			Camera& top_view = scene.camera_list[camera_ID_mapper[CAMERA_VIEW_TOP]].get();
			if (top_view.flag_valid) {
				top_view.view_port.x = ortho_start_x;;
				top_view.view_port.y = ortho_margin;
				top_view.view_port.w = ortho_view_width;
				top_view.view_port.h = ortho_view_height;
				glViewport(top_view.view_port.x, top_view.view_port.y, top_view.view_port.w, top_view.view_port.h);
				scene.ViewMatrix = top_view.ViewMatrix;
				scene.ProjectionMatrix = top_view.ProjectionMatrix;
				scene.draw_world();
				scene.axis_object.draw_axis(static_cast<Shader_Simple*>(&scene.shader_list[shader_ID_mapper[SHADER_SIMPLE]].get()),
					scene.ViewMatrix, scene.ProjectionMatrix);

				if(show_camera_frame)
					scene.draw_camera_axes_in_viewport(top_view);
			}
		}
	glutSwapBuffers();
}

bool rotate_mode = false;

void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0, polygon_fill_on = 0, depth_test_on = 0;

	float move_speed = 5.0f; 
	float rotate_angle = 1.0f;

	Camera& dynamic_camera = scene.camera_list[camera_ID_mapper[CAMERA_DYNAMIC]].get();

	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 'i':
	case 'I':
		flag_cull_face = (flag_cull_face + 1) % 3;
		switch (flag_cull_face) {
		case 0:
			glDisable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ No faces are culled.\n");
			break;
		case 1: // cull back faces;
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Back faces are culled.\n");
			break;
		case 2: // cull front faces;
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Front faces are culled.\n");
			break;
		}
		break;
	case 'o':
	case 'O':
		polygon_fill_on = 1 - polygon_fill_on;
		if (polygon_fill_on) {
		 	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			fprintf(stdout, "^^^ Polygon filling enabled.\n");
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			fprintf(stdout, "^^^ Line drawing enabled.\n");
		}
		glutPostRedisplay();
		break;
	case 'p':
	case 'P':
		depth_test_on = 1 - depth_test_on;
		if (depth_test_on) {
			glEnable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test enabled.\n");
		}
		else {
			glDisable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test disabled.\n");
		}
		glutPostRedisplay();
		break;
	case 'e': 
	case 'E':
		printf("e pressed!\n");
		if (rotate_mode) {
			scene.camera_list[CAMERA_MAIN].get().rotate_n_axis(glm::radians(-rotate_angle)); 
		}
		else {
			scene.camera_list[CAMERA_MAIN].get().move_forward(move_speed);
		}
		glutPostRedisplay();
		break;
	case 'q': 
	case 'Q':
		printf("q pressed!\n");
		if (rotate_mode) {
			scene.camera_list[CAMERA_MAIN].get().rotate_n_axis(glm::radians(rotate_angle)); 
		}
		else {
			scene.camera_list[CAMERA_MAIN].get().move_forward(-move_speed);
		}
		glutPostRedisplay();
		break;
	case 'a': 
	case 'A':
		printf("a pressed!\n");
		if (rotate_mode) {
			scene.camera_list[CAMERA_MAIN].get().rotate_v_axis(glm::radians(rotate_angle)); 
		}
		else {
			scene.camera_list[CAMERA_MAIN].get().move_right(-move_speed);
		}
		glutPostRedisplay();
		break;
	case 'd':
	case 'D':
		printf("d pressed!\n");
		if (rotate_mode) {
			scene.camera_list[CAMERA_MAIN].get().rotate_v_axis(glm::radians(-rotate_angle));
		}
		else {
			scene.camera_list[CAMERA_MAIN].get().move_right(move_speed);
		}
		glutPostRedisplay();
		break;
	case 'w': 
	case 'W':
		printf("w pressed!\n");
		if (rotate_mode) {
			scene.camera_list[CAMERA_MAIN].get().rotate_u_axis(glm::radians(rotate_angle));
		}
		else {
			scene.camera_list[CAMERA_MAIN].get().move_up(move_speed);
		}
		glutPostRedisplay();
		break;
	case 's': 
	case 'S':
		printf("s pressed!\n");
		if (rotate_mode) {
			scene.camera_list[CAMERA_MAIN].get().rotate_u_axis(glm::radians(-rotate_angle));
		}
		else {
			scene.camera_list[CAMERA_MAIN].get().move_up(-move_speed);
		}
		glutPostRedisplay();
		break;
	case 'r':
	case 'R':
		if (rotate_mode) {
			printf("Changed to Move mode\n");
			rotate_mode = !rotate_mode;
		}
		else {
			printf("Changed to Rotate mode\n");
			rotate_mode = !rotate_mode;
		}
		break;
	case '=':
		if (dynamic_camera.camera_id == CAMERA_DYNAMIC && dynamic_camera.flag_zoom) {
			dynamic_camera.zoom_in_out(-2.0f); 
		}
		break;
	case '-': 
		if (dynamic_camera.camera_id == CAMERA_DYNAMIC && dynamic_camera.flag_zoom) {
			dynamic_camera.zoom_in_out(2.0f); 
		}
		break;
	case 'f': 
	case 'F':
		show_camera_frame = !show_camera_frame;
		fprintf(stdout, "Camera Frame: %s\n", show_camera_frame ? "ON" : "OFF");
		break;
	case 't':
	case 'T':
		scene.current_cctv_camera_idx = (scene.current_cctv_camera_idx + 1) % 3;
		fprintf(stdout, "Switching to CCTV Camera %d\n", scene.current_cctv_camera_idx);
		break;
	}

}

void special_keyboard(int key, int x, int y) {

	glutPostRedisplay(); 
}

void mouse(int button, int state, int x, int y) {
	float zoom_speed = 1.0f; 

	if (scene.camera_list.empty() || scene.camera_list.size() <= CAMERA_MAIN) {
		fprintf(stderr, "Error: Main camera not initialized for zoom.\n");
		return;
	}

	if (button == GLUT_LEFT_BUTTON) { 
		if (state == GLUT_DOWN) {
			mouse_left_button_down = true;
			scene.first_mouse_move = true; 
		}
		else if (state == GLUT_UP) {
			mouse_left_button_down = false;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON) { 
		
	}

	else if (button == 3) { 
		scene.camera_list[CAMERA_MAIN].get().zoom_in_out(-zoom_speed); 
		glutPostRedisplay();
	}
	else if (button == 4) { 
		scene.camera_list[CAMERA_MAIN].get().zoom_in_out(zoom_speed); 
		glutPostRedisplay();
	}
}

void mouse_motion(int x, int y) {
	if (!mouse_left_button_down) {
		return;
	}
	if (scene.first_mouse_move) { 
		scene.last_mouse_x = x;
		scene.last_mouse_y = y;
		scene.first_mouse_move = false;
	}

	float xoffset = x - scene.last_mouse_x; 
	float yoffset = scene.last_mouse_y - y; 

	xoffset *= -1.0f;						

	scene.last_mouse_x = x;
	scene.last_mouse_y = y;

	xoffset *= scene.mouse_sensitivity;
	yoffset *= scene.mouse_sensitivity;

	scene.camera_yaw += xoffset;
	scene.camera_pitch += yoffset;

	if (scene.camera_pitch > 89.0f) {
		scene.camera_pitch = 89.0f;
	}
	if (scene.camera_pitch < -89.0f) {
		scene.camera_pitch = -89.0f;
	}

	if (camera_ID_mapper[CAMERA_DYNAMIC] < scene.camera_list.size()) {
		Camera& dynamic_camera = scene.camera_list[camera_ID_mapper[CAMERA_DYNAMIC]].get();

		glm::vec3 front;
		front.x = cos(glm::radians(scene.camera_yaw)) * cos(glm::radians(scene.camera_pitch));
		front.y = sin(glm::radians(scene.camera_yaw)) * cos(glm::radians(scene.camera_pitch));
		front.z = sin(glm::radians(scene.camera_pitch));
		dynamic_camera.cam_view.naxis = glm::normalize(front); 

		glm::vec3 world_up = glm::vec3(0.0f, 0.0f, 1.0f); 
		dynamic_camera.cam_view.uaxis = glm::normalize(glm::cross(dynamic_camera.cam_view.naxis, world_up));
		dynamic_camera.cam_view.vaxis = glm::normalize(glm::cross(dynamic_camera.cam_view.uaxis, dynamic_camera.cam_view.naxis));

		dynamic_camera.ViewMatrix = dynamic_camera.create_ViewMatrix_from_cam_view();
	}
	int center_x = scene.window.width / 2;
	int center_y = scene.window.height / 2;
	if (x != center_x || y != center_y) {
		glutWarpPointer(center_x, center_y);
		scene.last_mouse_x = center_x;
		scene.last_mouse_y = center_y;
	}

	glutPostRedisplay();
}


void reshape(int width, int height) {
	scene.window.width = width;
	scene.window.height = height;
	scene.window.aspect_ratio = (float)width / height;
	scene.create_camera_list(scene.window.width, scene.window.height, scene.window.aspect_ratio);
	glutPostRedisplay();
}

void timer_scene(int index) {
	scene.clock(0);
	glutPostRedisplay();
	glutTimerFunc(100, timer_scene, 0);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special_keyboard);
	glutReshapeFunc(reshape);
 	glutTimerFunc(100, timer_scene, 0);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_motion);
	glutPassiveMotionFunc(NULL);
//	glutCloseFunc(cleanup_OpenGL_stuffs or else); // Do it yourself!!!
}

void initialize_OpenGL(void) {
	glEnable(GL_DEPTH_TEST); // Default state
	 
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClearColor(0.12f, 0.18f, 0.12f, 1.0f);
}

void initialize_renderer(void) {
	register_callbacks();
	initialize_OpenGL();
	scene.initialize();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

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

void print_message(const char * m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170/AIE4012 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) { 
	char program_name[256] = "Sogang CSE4170/AIE4120 Our_House_GLSL_V_0.55";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: I, O, P, Q, W, E, A, S, D, R, =, -, F" };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	glutSetCursor(GLUT_CURSOR_NONE); 
	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	if (!scene.camera_list.empty() && scene.camera_list.size() > CAMERA_MAIN) {
		scene.camera_list[CAMERA_MAIN].get().flag_move = true;
		scene.camera_list[CAMERA_MAIN].get().flag_rotate_axes = true;
		scene.camera_list[CAMERA_MAIN].get().flag_zoom = true; 
	}
	else {
		fprintf(stderr, "Error: Main camera not initialized or camera_list is empty.\n");
	}


	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
