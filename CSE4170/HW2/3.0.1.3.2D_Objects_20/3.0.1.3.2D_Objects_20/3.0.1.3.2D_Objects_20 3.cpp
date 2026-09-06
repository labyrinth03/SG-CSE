#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <random>


#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

														  // include glm/*.hpp only if necessary
														  //#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0

int win_width = 0, win_height = 0;
float centerx = 0.0f, centery = 0.0f, rotate_angle = 0.0f;
float mouse_x = 0.0f;
float mouse_y = 0.0f;

GLfloat axes[4][2];
GLfloat axes_color[3] = { 0.0f, 0.0f, 0.0f };
GLuint VBO_axes, VAO_axes;

void prepare_axes(void) { // Draw axes in their MC.
	axes[0][0] = -win_width / 2.5f; axes[0][1] = 0.0f;
	axes[1][0] = win_width / 2.5f; axes[1][1] = 0.0f;
	axes[2][0] = 0.0f; axes[2][1] = -win_height / 2.5f;
	axes[3][0] = 0.0f; axes[3][1] = win_height / 2.5f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_axes(void) {
	axes[0][0] = -win_width / 2.25f; axes[1][0] = win_width / 2.25f;
	axes[2][1] = -win_height / 2.25f;
	axes[3][1] = win_height / 2.25f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_axes(void) {
	glUniform3fv(loc_primitive_color, 1, axes_color);
	glBindVertexArray(VAO_axes);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);
}

GLfloat line[2][2];
GLfloat line_color[3] = { 1.0f, 0.0f, 0.0f };
GLuint VBO_line, VAO_line;

void prepare_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height;
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f;
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_line);
	glBindVertexArray(VAO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height;
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f;
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_line(void) { // Draw line in its MC.
					   // y = x - win_height/4
	glUniform3fv(loc_primitive_color, 1, line_color);
	glBindVertexArray(VAO_line);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

#define AIRPLANE_BIG_WING 0
#define AIRPLANE_SMALL_WING 1
#define AIRPLANE_BODY 2
#define AIRPLANE_BACK 3
#define AIRPLANE_SIDEWINDER1 4
#define AIRPLANE_SIDEWINDER2 5
#define AIRPLANE_CENTER 6
GLfloat big_wing[6][2] = { { 0.0, 0.0 },{ -20.0, 15.0 },{ -20.0, 20.0 },{ 0.0, 23.0 },{ 20.0, 20.0 },{ 20.0, 15.0 } };
GLfloat small_wing[6][2] = { { 0.0, -18.0 },{ -11.0, -12.0 },{ -12.0, -7.0 },{ 0.0, -10.0 },{ 12.0, -7.0 },{ 11.0, -12.0 } };
GLfloat body[5][2] = { { 0.0, -25.0 },{ -6.0, 0.0 },{ -6.0, 22.0 },{ 6.0, 22.0 },{ 6.0, 0.0 } };
GLfloat back[5][2] = { { 0.0, 25.0 },{ -7.0, 24.0 },{ -7.0, 21.0 },{ 7.0, 21.0 },{ 7.0, 24.0 } };
GLfloat sidewinder1[5][2] = { { -20.0, 10.0 },{ -18.0, 3.0 },{ -16.0, 10.0 },{ -18.0, 20.0 },{ -20.0, 20.0 } };
GLfloat sidewinder2[5][2] = { { 20.0, 10.0 },{ 18.0, 3.0 },{ 16.0, 10.0 },{ 18.0, 20.0 },{ 20.0, 20.0 } };
GLfloat center[1][2] = { { 0.0, 0.0 } };
GLfloat airplane_color[7][3] = {
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // big_wing
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // small_wing
	{ 111 / 255.0f,  85 / 255.0f, 157 / 255.0f },  // body
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // back
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder1
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder2
	{ 255 / 255.0f,   0 / 255.0f,   0 / 255.0f }   // center
};

GLuint VBO_airplane, VAO_airplane;

int airplane_clock = 0;
float airplane_s_factor = 1.0f;

void prepare_airplane() {
	GLsizeiptr buffer_size = sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2) + sizeof(center);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(big_wing), big_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing), sizeof(small_wing), small_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing), sizeof(body), body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body), sizeof(back), back);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back),
		sizeof(sidewinder1), sidewinder1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1), sizeof(sidewinder2), sidewinder2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2), sizeof(center), center);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_airplane);
	glBindVertexArray(VAO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_airplane() { // Draw airplane in its MC.
	glBindVertexArray(VAO_airplane);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BIG_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SMALL_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BACK]);
	glDrawArrays(GL_TRIANGLE_FAN, 17, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER1]);
	glDrawArrays(GL_TRIANGLE_FAN, 22, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER2]);
	glDrawArrays(GL_TRIANGLE_FAN, 27, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_CENTER]);
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 32, 1);
	glPointSize(1.0);
	glBindVertexArray(0);
}

//shirt
#define SHIRT_LEFT_BODY 0
#define SHIRT_RIGHT_BODY 1
#define SHIRT_LEFT_COLLAR 2
#define SHIRT_RIGHT_COLLAR 3
#define SHIRT_FRONT_POCKET 4
#define SHIRT_BUTTON1 5
#define SHIRT_BUTTON2 6
#define SHIRT_BUTTON3 7
#define SHIRT_BUTTON4 8
GLfloat left_body[6][2] = { { 0.0, -9.0 },{ -8.0, -9.0 },{ -11.0, 8.0 },{ -6.0, 10.0 },{ -3.0, 7.0 },{ 0.0, 9.0 } };
GLfloat right_body[6][2] = { { 0.0, -9.0 },{ 0.0, 9.0 },{ 3.0, 7.0 },{ 6.0, 10.0 },{ 11.0, 8.0 },{ 8.0, -9.0 } };
GLfloat left_collar[4][2] = { { 0.0, 9.0 },{ -3.0, 7.0 },{ -6.0, 10.0 },{ -4.0, 11.0 } };
GLfloat right_collar[4][2] = { { 0.0, 9.0 },{ 4.0, 11.0 },{ 6.0, 10.0 },{ 3.0, 7.0 } };
GLfloat front_pocket[6][2] = { { 5.0, 0.0 },{ 4.0, 1.0 },{ 4.0, 3.0 },{ 7.0, 3.0 },{ 7.0, 1.0 },{ 6.0, 0.0 } };
GLfloat button1[3][2] = { { -1.0, 6.0 },{ 1.0, 6.0 },{ 0.0, 5.0 } };
GLfloat button2[3][2] = { { -1.0, 3.0 },{ 1.0, 3.0 },{ 0.0, 2.0 } };
GLfloat button3[3][2] = { { -1.0, 0.0 },{ 1.0, 0.0 },{ 0.0, -1.0 } };
GLfloat button4[3][2] = { { -1.0, -3.0 },{ 1.0, -3.0 },{ 0.0, -4.0 } };

GLfloat shirt_color[9][3] = {
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f }
};

GLuint VBO_shirt, VAO_shirt;
void prepare_shirt() {
	GLsizeiptr buffer_size = sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2) + sizeof(button3) + sizeof(button4);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(left_body), left_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body), sizeof(right_body), right_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body), sizeof(left_collar), left_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar), sizeof(right_collar), right_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar),
		sizeof(front_pocket), front_pocket);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket), sizeof(button1), button1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1), sizeof(button2), button2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2), sizeof(button3), button3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2) + sizeof(button3), sizeof(button4), button4);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_shirt);
	glBindVertexArray(VAO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_shirt() {
	glBindVertexArray(VAO_shirt);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_FRONT_POCKET]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON1]);
	glDrawArrays(GL_TRIANGLE_FAN, 26, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON2]);
	glDrawArrays(GL_TRIANGLE_FAN, 29, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON3]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON4]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);
	glBindVertexArray(0);
}

//house
#define HOUSE_ROOF 0
#define HOUSE_BODY 1
#define HOUSE_CHIMNEY 2
#define HOUSE_DOOR 3
#define HOUSE_WINDOW 4

GLfloat roof[3][2] = { { -12.0, 0.0 },{ 0.0, 12.0 },{ 12.0, 0.0 } };
GLfloat house_body[4][2] = { { -12.0, -14.0 },{ -12.0, 0.0 },{ 12.0, 0.0 },{ 12.0, -14.0 } };
GLfloat chimney[4][2] = { { 6.0, 6.0 },{ 6.0, 14.0 },{ 10.0, 14.0 },{ 10.0, 2.0 } };
GLfloat door[4][2] = { { -8.0, -14.0 },{ -8.0, -8.0 },{ -4.0, -8.0 },{ -4.0, -14.0 } };
GLfloat window[4][2] = { { 4.0, -6.0 },{ 4.0, -2.0 },{ 8.0, -2.0 },{ 8.0, -6.0 } };

GLfloat house_color[5][3] = {
	{ 200 / 255.0f, 39 / 255.0f, 42 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 233 / 255.0f, 113 / 255.0f, 23 / 255.0f },
	{ 44 / 255.0f, 180 / 255.0f, 49 / 255.0f }
};

GLuint VBO_house, VAO_house;
void prepare_house() {
	GLsizeiptr buffer_size = sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door)
		+ sizeof(window);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(roof), roof);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof), sizeof(house_body), house_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body), sizeof(chimney), chimney);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney), sizeof(door), door);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door),
		sizeof(window), window);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_house);
	glBindVertexArray(VAO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_house() {
	glBindVertexArray(VAO_house);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_ROOF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 3, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_CHIMNEY]);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_DOOR]);
	glDrawArrays(GL_TRIANGLE_FAN, 11, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glBindVertexArray(0);
}

//car
#define CAR_BODY 0
#define CAR_FRAME 1
#define CAR_WINDOW 2
#define CAR_LEFT_LIGHT 3
#define CAR_RIGHT_LIGHT 4
#define CAR_LEFT_WHEEL 5
#define CAR_RIGHT_WHEEL 6

GLfloat car_body[4][2] = { { -16.0, -8.0 },{ -16.0, 0.0 },{ 16.0, 0.0 },{ 16.0, -8.0 } };
GLfloat car_frame[4][2] = { { -10.0, 0.0 },{ -10.0, 10.0 },{ 10.0, 10.0 },{ 10.0, 0.0 } };
GLfloat car_window[4][2] = { { -8.0, 0.0 },{ -8.0, 8.0 },{ 8.0, 8.0 },{ 8.0, 0.0 } };
GLfloat car_left_light[4][2] = { { -9.0, -6.0 },{ -10.0, -5.0 },{ -9.0, -4.0 },{ -8.0, -5.0 } };
GLfloat car_right_light[4][2] = { { 9.0, -6.0 },{ 8.0, -5.0 },{ 9.0, -4.0 },{ 10.0, -5.0 } };
GLfloat car_left_wheel[4][2] = { { -10.0, -12.0 },{ -10.0, -8.0 },{ -6.0, -8.0 },{ -6.0, -12.0 } };
GLfloat car_right_wheel[4][2] = { { 6.0, -12.0 },{ 6.0, -8.0 },{ 10.0, -8.0 },{ 10.0, -12.0 } };

GLfloat car_color[7][3] = {
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 216 / 255.0f, 208 / 255.0f, 174 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f }
};

GLuint VBO_car, VAO_car;
void prepare_car() {
	GLsizeiptr buffer_size = sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel) + sizeof(car_right_wheel);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car_body), car_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body), sizeof(car_frame), car_frame);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame), sizeof(car_window), car_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window), sizeof(car_left_light), car_left_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light),
		sizeof(car_right_light), car_right_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light), sizeof(car_left_wheel), car_left_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel), sizeof(car_right_wheel), car_right_wheel);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car);
	glBindVertexArray(VAO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car() {
	glBindVertexArray(VAO_car);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_FRAME]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);

	glBindVertexArray(0);
}

//draw cocktail
#define COCKTAIL_NECK 0
#define COCKTAIL_LIQUID 1
#define COCKTAIL_REMAIN 2
#define COCKTAIL_STRAW 3
#define COCKTAIL_DECO 4

GLfloat neck[6][2] = { { -6.0, -12.0 },{ -6.0, -11.0 },{ -1.0, 0.0 },{ 1.0, 0.0 },{ 6.0, -11.0 },{ 6.0, -12.0 } };
GLfloat liquid[6][2] = { { -1.0, 0.0 },{ -9.0, 4.0 },{ -12.0, 7.0 },{ 12.0, 7.0 },{ 9.0, 4.0 },{ 1.0, 0.0 } };
GLfloat remain[4][2] = { { -12.0, 7.0 },{ -12.0, 10.0 },{ 12.0, 10.0 },{ 12.0, 7.0 } };
GLfloat straw[4][2] = { { 7.0, 7.0 },{ 12.0, 12.0 },{ 14.0, 12.0 },{ 9.0, 7.0 } };
GLfloat deco[8][2] = { { 12.0, 12.0 },{ 10.0, 14.0 },{ 10.0, 16.0 },{ 12.0, 18.0 },{ 14.0, 18.0 },{ 16.0, 16.0 },{ 16.0, 14.0 },{ 14.0, 12.0 } };

GLfloat cocktail_color[5][3] = {
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 0 / 255.0f, 63 / 255.0f, 122 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 191 / 255.0f, 255 / 255.0f, 0 / 255.0f },
	{ 218 / 255.0f, 165 / 255.0f, 32 / 255.0f }
};

GLuint VBO_cocktail, VAO_cocktail;
void prepare_cocktail() {
	GLsizeiptr buffer_size = sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw)
		+ sizeof(deco);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(neck), neck);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck), sizeof(liquid), liquid);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid), sizeof(remain), remain);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain), sizeof(straw), straw);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw),
		sizeof(deco), deco);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_cocktail);
	glBindVertexArray(VAO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cocktail() {
	glBindVertexArray(VAO_cocktail);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_NECK]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_LIQUID]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_REMAIN]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_STRAW]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_DECO]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 8);

	glBindVertexArray(0);
}

//draw car2
#define CAR2_BODY 0
#define CAR2_FRONT_WINDOW 1
#define CAR2_BACK_WINDOW 2
#define CAR2_FRONT_WHEEL 3
#define CAR2_BACK_WHEEL 4
#define CAR2_LIGHT1 5
#define CAR2_LIGHT2 6

GLfloat car2_body[8][2] = { { -18.0, -7.0 },{ -18.0, 0.0 },{ -13.0, 0.0 },{ -10.0, 8.0 },{ 10.0, 8.0 },{ 13.0, 0.0 },{ 18.0, 0.0 },{ 18.0, -7.0 } };
GLfloat car2_front_window[4][2] = { { -10.0, 0.0 },{ -8.0, 6.0 },{ -2.0, 6.0 },{ -2.0, 0.0 } };
GLfloat car2_back_window[4][2] = { { 0.0, 0.0 },{ 0.0, 6.0 },{ 8.0, 6.0 },{ 10.0, 0.0 } };
GLfloat car2_front_wheel[8][2] = { { -11.0, -11.0 },{ -13.0, -8.0 },{ -13.0, -7.0 },{ -11.0, -4.0 },{ -7.0, -4.0 },{ -5.0, -7.0 },{ -5.0, -8.0 },{ -7.0, -11.0 } };
GLfloat car2_back_wheel[8][2] = { { 7.0, -11.0 },{ 5.0, -8.0 },{ 5.0, -7.0 },{ 7.0, -4.0 },{ 11.0, -4.0 },{ 13.0, -7.0 },{ 13.0, -8.0 },{ 11.0, -11.0 } };
GLfloat car2_light1[3][2] = { { -18.0, -1.0 },{ -17.0, -2.0 },{ -18.0, -3.0 } };
GLfloat car2_light2[3][2] = { { -18.0, -4.0 },{ -17.0, -5.0 },{ -18.0, -6.0 } };

GLfloat car2_color[7][3] = {
	{ 100 / 255.0f, 141 / 255.0f, 159 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f }
};

GLuint VBO_car2, VAO_car2;
void prepare_car2() {
	GLsizeiptr buffer_size = sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel) + sizeof(car2_light1) + sizeof(car2_light2);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car2_body), car2_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body), sizeof(car2_front_window), car2_front_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window), sizeof(car2_back_window), car2_back_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window), sizeof(car2_front_wheel), car2_front_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel),
		sizeof(car2_back_wheel), car2_back_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel), sizeof(car2_light1), car2_light1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel) + sizeof(car2_light1), sizeof(car2_light2), car2_light2);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car2);
	glBindVertexArray(VAO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car2() {
	glBindVertexArray(VAO_car2);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT1]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT2]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);

	glBindVertexArray(0);
}

// hat
#define HAT_LEAF 0
#define HAT_BODY 1
#define HAT_STRIP 2
#define HAT_BOTTOM 3

GLfloat hat_leaf[4][2] = { { 3.0, 20.0 },{ 3.0, 28.0 },{ 9.0, 32.0 },{ 9.0, 24.0 } };
GLfloat hat_body[4][2] = { { -19.5, 2.0 },{ 19.5, 2.0 },{ 15.0, 20.0 },{ -15.0, 20.0 } };
GLfloat hat_strip[4][2] = { { -20.0, 0.0 },{ 20.0, 0.0 },{ 19.5, 2.0 },{ -19.5, 2.0 } };
GLfloat hat_bottom[4][2] = { { 25.0, 0.0 },{ -25.0, 0.0 },{ -25.0, -4.0 },{ 25.0, -4.0 } };

GLfloat hat_color[4][3] = {
	{ 167 / 255.0f, 255 / 255.0f, 55 / 255.0f },
{ 255 / 255.0f, 144 / 255.0f, 32 / 255.0f },
{ 255 / 255.0f, 40 / 255.0f, 33 / 255.0f },
{ 255 / 255.0f, 144 / 255.0f, 32 / 255.0f }
};

GLuint VBO_hat, VAO_hat;

void prepare_hat() {
	GLsizeiptr buffer_size = sizeof(hat_leaf) + sizeof(hat_body) + sizeof(hat_strip) + sizeof(hat_bottom);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_hat);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_hat);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(hat_leaf), hat_leaf);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf), sizeof(hat_body), hat_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf) + sizeof(hat_body), sizeof(hat_strip), hat_strip);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf) + sizeof(hat_body) + sizeof(hat_strip), sizeof(hat_bottom), hat_bottom);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_hat);
	glBindVertexArray(VAO_hat);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_hat);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_hat() {
	glBindVertexArray(VAO_hat);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_LEAF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_STRIP]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_BOTTOM]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glBindVertexArray(0);
}

// cake
#define CAKE_FIRE 0
#define CAKE_CANDLE 1
#define CAKE_BODY 2
#define CAKE_BOTTOM 3
#define CAKE_DECORATE 4

GLfloat cake_fire[4][2] = { { -0.5, 14.0 },{ -0.5, 13.0 },{ 0.5, 13.0 },{ 0.5, 14.0 } };
GLfloat cake_candle[4][2] = { { -1.0, 8.0 } ,{ -1.0, 13.0 },{ 1.0, 13.0 },{ 1.0, 8.0 } };
GLfloat cake_body[4][2] = { { 8.0, 5.0 },{ -8.0, 5.0 } ,{ -8.0, 8.0 },{ 8.0, 8.0 } };
GLfloat cake_bottom[4][2] = { { -10.0, 1.0 },{ -10.0, 5.0 },{ 10.0, 5.0 },{ 10.0, 1.0 } };
GLfloat cake_decorate[4][2] = { { -10.0, 0.0 },{ -10.0, 1.0 },{ 10.0, 1.0 },{ 10.0, 0.0 } };

GLfloat cake_color[5][3] = {
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
{ 255 / 255.0f, 204 / 255.0f, 0 / 255.0f },
{ 255 / 255.0f, 102 / 255.0f, 255 / 255.0f },
{ 255 / 255.0f, 102 / 255.0f, 255 / 255.0f },
{ 102 / 255.0f, 51 / 255.0f, 0 / 255.0f }
};

GLuint VBO_cake, VAO_cake;

void prepare_cake() {
	int size = sizeof(cake_fire);
	GLsizeiptr buffer_size = sizeof(cake_fire) * 5;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_cake);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cake);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, size, cake_fire);
	glBufferSubData(GL_ARRAY_BUFFER, size, size, cake_candle);
	glBufferSubData(GL_ARRAY_BUFFER, size * 2, size, cake_body);
	glBufferSubData(GL_ARRAY_BUFFER, size * 3, size, cake_bottom);
	glBufferSubData(GL_ARRAY_BUFFER, size * 4, size, cake_decorate);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_cake);
	glBindVertexArray(VAO_cake);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cake);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cake() {
	glBindVertexArray(VAO_cake);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_FIRE]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_CANDLE]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_BOTTOM]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, cake_color[CAKE_DECORATE]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glBindVertexArray(0);
}

// sword

#define SWORD_BODY 0
#define SWORD_BODY2 1
#define SWORD_HEAD 2
#define SWORD_HEAD2 3
#define SWORD_IN 4
#define SWORD_DOWN 5
#define SWORD_BODY_IN 6

GLfloat sword_body[4][2] = { { -6.0, 0.0 },{ -6.0, -4.0 },{ 6.0, -4.0 },{ 6.0, 0.0 } };
GLfloat sword_body2[4][2] = { { -2.0, -4.0 },{ -2.0, -6.0 } ,{ 2.0, -6.0 },{ 2.0, -4.0 } };
GLfloat sword_head[4][2] = { { -2.0, 0.0 },{ -2.0, 16.0 } ,{ 2.0, 16.0 },{ 2.0, 0.0 } };
GLfloat sword_head2[3][2] = { { -2.0, 16.0 },{ 0.0, 19.46 } ,{ 2.0, 16.0 } };
GLfloat sword_in[4][2] = { { -0.3, 0.7 },{ -0.3, 15.3 } ,{ 0.3, 15.3 },{ 0.3, 0.7 } };
GLfloat sword_down[4][2] = { { -2.0, -6.0 } ,{ 2.0, -6.0 },{ 4.0, -8.0 },{ -4.0, -8.0 } };
GLfloat sword_body_in[4][2] = { { 0.0, -1.0 } ,{ 1.0, -2.732 },{ 0.0, -4.464 },{ -1.0, -2.732 } };

GLfloat sword_color[7][3] = {
	{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f }
};

GLuint VBO_sword, VAO_sword;

void prepare_sword() {
	GLsizeiptr buffer_size = sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down) + sizeof(sword_body_in);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sword_body), sword_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body), sizeof(sword_body2), sword_body2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2), sizeof(sword_head), sword_head);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head), sizeof(sword_head2), sword_head2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2), sizeof(sword_in), sword_in);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in), sizeof(sword_down), sword_down);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down), sizeof(sword_body_in), sword_body_in);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_sword);
	glBindVertexArray(VAO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_sword() {
	glBindVertexArray(VAO_sword);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY2]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD2]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 3);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_DOWN]);
	glDrawArrays(GL_TRIANGLE_FAN, 19, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 23, 4);

	glBindVertexArray(0);
}

GLfloat shield[6][2] = {
	{ -10.0f, 10.0f },   // 왼쪽 위
	{ 0.0f, 15.0f },     // 중앙 위
	{ 10.0f, 10.0f },    // 오른쪽 위
	{ 10.0f, -5.0f },    // 오른쪽 아래
	{ 0.0f, -15.0f },    // 아래 중앙 (뾰족)
	{ -10.0f, -5.0f }    // 왼쪽 아래
};
GLuint VBO_shield, VAO_shield;
GLfloat shield_color[3] = { 0.6f, 0.6f, 0.8f }; // 약간 푸른 회색

void prepare_shield() {
	// Initialize vertex buffer object
	glGenBuffers(1, &VBO_shield);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shield);
	glBufferData(GL_ARRAY_BUFFER, sizeof(shield), shield, GL_STATIC_DRAW);

	// Initialize vertex array object
	glGenVertexArrays(1, &VAO_shield);
	glBindVertexArray(VAO_shield);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shield);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void draw_shield() {
	glBindVertexArray(VAO_shield);

	glUniform3fv(loc_primitive_color, 1, shield_color);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6); // 6개 점으로 TRIANGLE_FAN
	glBindVertexArray(0);
}

GLfloat crown[17][2] = {
	{0.0f, 7.0f},
	{2.0f, 5.0f},
	{1.0f, 4.0f},
	{3.0f, 1.0f},
	{5.0f, 2.0f},
	{4.0f, 3.0f},
	{8.0f, 4.0f},
	{6.0f, -1.0f},
	{0.0f, -4.0f},
	{-6.0f, -1.0f},
	{-8.0f, 4.0f},
	{-4.0f, 3.0f},
	{-5.0f, 2.0f},
	{-3.0f, 1.0f},
	{-1.0f, 4.0f},
	{-2.0f, 5.0f},
	{0.0f, 7.0f}
};
GLfloat crown_star[9][2] = {
	{0.0f, 2.0f},
	{0.4f, 0.4f},
	{2.0f, 0.0f},
	{0.4f, -0.4f},
	{0.0f, -2.0f},
	{-0.4f, -0.4f},
	{-2.0f, 0.0f},
	{-0.4f, 0.4f},
	{0.0f, 2.0f}
};
GLuint VBO_crown, VAO_crown;
GLfloat crown_color[3] = { 0.2706f, 0.2706f, 0.2549f }; // 노란색
GLfloat star_color1[3] = { 1.0f, 0.2f, 0.8f}; //분홍색
GLfloat star_color2[3] = { 0.7421f, 0.9218f, 0.9960f}; //하늘색


void prepare_crown() {

	// Initialize vertex buffer object
	glGenBuffers(1, &VBO_crown);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_crown);
	glBufferData(GL_ARRAY_BUFFER, sizeof(crown), crown, GL_STATIC_DRAW);

	// Initialize vertex array object
	glGenVertexArrays(1, &VAO_crown);
	glBindVertexArray(VAO_crown);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_crown);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void draw_crown() {
	glBindVertexArray(VAO_crown);

	glUniform3fv(loc_primitive_color, 1, crown_color);

	std::vector<GLfloat> triangles;
	for (size_t i = 0; i < 16; ++i) {
		// 첫 번째 점을 기준으로 삼각형을 만들기
		if (i == 0 || i == 1 || i == 14 || i == 15) {
			triangles.push_back(0.0f);  // 중심 (0,0)
			triangles.push_back(5.0f);
			triangles.push_back(crown[i][0]);  // 점 i
			triangles.push_back(crown[i][1]);
			triangles.push_back(crown[i + 1][0]);  // 다음 점
			triangles.push_back(crown[i + 1][1]);
		}
		else if (i == 3) {
			triangles.push_back(0.0f);  // 
			triangles.push_back(0.0f);
			triangles.push_back(6.0f);  // 점 i
			triangles.push_back(-1.0f);
			triangles.push_back(crown[i + 1][0]);  // 다음 점
			triangles.push_back(crown[i + 1][1]);
		}
		else if (i == 4 || i == 5) {
			triangles.push_back(6.5f);  // 중심 (0,0)
			triangles.push_back(2.5f);
			triangles.push_back(crown[i][0]);  // 점 i
			triangles.push_back(crown[i][1]);
			triangles.push_back(crown[i + 1][0]);  // 다음 점
			triangles.push_back(crown[i + 1][1]);
		}
		else if (i == 6) {
			triangles.push_back(5.0f);  // 중심 (0,0)
			triangles.push_back(2.0f);
			triangles.push_back(crown[i][0]);  // 점 i
			triangles.push_back(crown[i][1]);
			triangles.push_back(crown[i + 1][0]);  // 다음 점
			triangles.push_back(crown[i + 1][1]);
		}
		else if (i == 9) {
			triangles.push_back(-5.0f);  // 중심 (0,0)
			triangles.push_back(2.0f);
			triangles.push_back(crown[i][0]);  // 점 i
			triangles.push_back(crown[i][1]);
			triangles.push_back(crown[i + 1][0]);  // 다음 점
			triangles.push_back(crown[i + 1][1]);
		}
		else if (i == 10 || i == 11) {
			triangles.push_back(-6.5f);  // 중심 (0,0)
			triangles.push_back(2.5f);
			triangles.push_back(crown[i][0]);  // 점 i
			triangles.push_back(crown[i][1]);
			triangles.push_back(crown[i + 1][0]);  // 다음 점
			triangles.push_back(crown[i + 1][1]);
		}
		else if (i == 12) {
			triangles.push_back(0.0f);  // 중심 (0,0)
			triangles.push_back(0.0f);
			triangles.push_back(crown[i][0]);  // 점 i
			triangles.push_back(crown[i][1]);
			triangles.push_back(-6.0f);  // 다음 점
			triangles.push_back(-1.0f);
		}
		else {
			triangles.push_back(0.0f);  // 중심 (0,0)
			triangles.push_back(0.0f);
			triangles.push_back(crown[i][0]);  // 점 i
			triangles.push_back(crown[i][1]);
			triangles.push_back(crown[i + 1][0]);  // 다음 점
			triangles.push_back(crown[i + 1][1]);
		}

	}

	// 빈 공간 보수
	triangles.push_back(0.0f);  // 중심 (0,0)
	triangles.push_back(5.0f);
	triangles.push_back(1.0f);  // 점 i
	triangles.push_back(4.0f);
	triangles.push_back(-1.0f);  // 다음 점
	triangles.push_back(4.0f);

	triangles.push_back(0.0f);  // 중심 (0,0)
	triangles.push_back(0.0f);
	triangles.push_back(1.0f);  // 점 i
	triangles.push_back(4.0f);
	triangles.push_back(-1.0f);  // 다음 점
	triangles.push_back(4.0f);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_crown);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(GLfloat), triangles.data(), GL_STATIC_DRAW);

	// 삼각형을 그리기
	glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);


	triangles.clear();
	// 별 분홍색 부분
	glUniform3fv(loc_primitive_color, 1, star_color1);
	for (size_t i = 0; i < 8; ++i) {
		if (i == 0 || i == 6 || i == 7) {
			triangles.push_back(0.0f);  
			triangles.push_back(0.0f);
			triangles.push_back(crown_star[i][0]); 
			triangles.push_back(crown_star[i][1]);
			triangles.push_back(crown_star[i + 1][0]);  
			triangles.push_back(crown_star[i + 1][1]);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO_crown);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(GLfloat), triangles.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);

	triangles.clear();
	// 별 하늘색 부분
	glUniform3fv(loc_primitive_color, 1, star_color2);
	for (size_t i = 0; i < 8; ++i) {
		if (!(i == 0 || i == 6 || i == 7)) {
			triangles.push_back(0.0f);
			triangles.push_back(0.0f);
			triangles.push_back(crown_star[i][0]);
			triangles.push_back(crown_star[i][1]);
			triangles.push_back(crown_star[i + 1][0]);
			triangles.push_back(crown_star[i + 1][1]);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO_crown);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(GLfloat), triangles.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);

	glBindVertexArray(0);
}

GLfloat grail[19][2] = {
	{4.0f, 0.0f},
	{1.0f, 2.0f},
	{1.0f, 3.0f},
	{1.7f, 3.3f},
	{2.0f, 4.0f},
	{3.0f, 4.3f},
	{4.0f, 5.0f},
	{4.5f, 6.5f},
	{5.0f, 8.0f},
	{-5.0f, 8.0f},
	{-4.5f, 6.0f},
	{-4.0f, 5.0f},
	{-3.0f, 4.3f},
	{-2.0f, 4.0f},
	{-1.7f, 3.3f},
	{-1.0f, 3.0f},
	{-1.0f, 2.0f},
	{-4.0f, 0.0f},
	{4.0f, 0.0f}
};
GLfloat grail_star[9][2] = {
	{0.0f, 7.0f},
	{0.4f, 5.4f},
	{2.0f, 5.0f},
	{0.4f, 4.6f},
	{0.0f, 3.0f},
	{-0.4f, 4.6f},
	{-2.0f, 5.0f},
	{-0.4f, 5.4f},
	{0.0f, 7.0f}
};
GLfloat grail_color[3] = { 0.2627f, 0.4118f, 0.9137f }; // 노란색
GLuint VBO_grail, VAO_grail;

void prepare_grail() {

	// Initialize vertex buffer object
	glGenBuffers(1, &VBO_grail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grail);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grail), grail, GL_STATIC_DRAW);

	// Initialize vertex array object
	glGenVertexArrays(1, &VAO_grail);
	glBindVertexArray(VAO_grail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grail);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void draw_grail() {
	glBindVertexArray(VAO_grail);

	glUniform3fv(loc_primitive_color, 1, grail_color);

	std::vector<GLfloat> triangles;
	for (size_t i = 0; i < 18; ++i) {
		if (i == 0 || i == 1 || i == 15 || i == 16 || i == 17) {
			triangles.push_back(0.0f);  // 중심 (0,0)
			triangles.push_back(0.0f);
			triangles.push_back(grail[i][0]);  // 점 i
			triangles.push_back(grail[i][1]);
			triangles.push_back(grail[i + 1][0]);  // 다음 점
			triangles.push_back(grail[i + 1][1]);
		}
		else {
			triangles.push_back(0.0f);  // 중심 (0,0)
			triangles.push_back(6.0f);
			triangles.push_back(grail[i][0]);  // 점 i
			triangles.push_back(grail[i][1]);
			triangles.push_back(grail[i + 1][0]);  // 다음 점
			triangles.push_back(grail[i + 1][1]);
		}

	}

	// 빈 공간 보수
	triangles.push_back(0.0f);  // 중심 (0,0)
	triangles.push_back(0.0f);
	triangles.push_back(1.0f);  // 점 i
	triangles.push_back(3.0f);
	triangles.push_back(-1.0f);  // 다음 점
	triangles.push_back(3.0f);

	triangles.push_back(0.0f);  // 중심 (0,0)
	triangles.push_back(6.0f);
	triangles.push_back(1.0f);  // 점 i
	triangles.push_back(3.0f);
	triangles.push_back(-1.0f);  // 다음 점
	triangles.push_back(3.0f);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_grail);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(GLfloat), triangles.data(), GL_STATIC_DRAW);

	// 삼각형을 그리기
	glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);


	triangles.clear();
	// 별 분홍색 부분
	glUniform3fv(loc_primitive_color, 1, star_color1);
	for (size_t i = 0; i < 8; ++i) {
		if (i == 0 || i == 6 || i == 7) {
			triangles.push_back(0.0f);
			triangles.push_back(5.0f);
			triangles.push_back(grail_star[i][0]);
			triangles.push_back(grail_star[i][1]);
			triangles.push_back(grail_star[i + 1][0]);
			triangles.push_back(grail_star[i + 1][1]);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grail);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(GLfloat), triangles.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);

	triangles.clear();
	// 별 하늘색 부분
	glUniform3fv(loc_primitive_color, 1, star_color2);
	for (size_t i = 0; i < 8; ++i) {
		if (!(i == 0 || i == 6 || i == 7)) {
			triangles.push_back(0.0f);
			triangles.push_back(5.0f);
			triangles.push_back(grail_star[i][0]);
			triangles.push_back(grail_star[i][1]);
			triangles.push_back(grail_star[i + 1][0]);
			triangles.push_back(grail_star[i + 1][1]);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO_grail);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(GLfloat), triangles.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);

	glBindVertexArray(0);
}

GLfloat hourglass[19][2] = {
	{3.5f, 0.0f},
	{3.5f, 1.0f},
	{3.0f, 1.0f},
	{2.0f, 3.0f},
	{1.0f, 4.0f},
	{2.0f, 5.0f},
	{3.0f, 7.0f},
	{3.5f, 7.0f},
	{3.5f, 8.0f},
	{-3.5f, 8.0f},
	{-3.5f, 7.0f},
	{-3.0f, 7.0f},
	{-2.0f, 5.0f},
	{-1.0f, 4.0f},
	{-2.0f, 3.0f},
	{-3.0f, 1.0f},
	{-3.5f, 1.0f},
	{-3.5f, 0.0f},
	{3.5f, 0.0f}
};
GLfloat hourglass_bottom[5][2] = {
	{3.5f, 0.0f},
	{3.5f, 1.0f},
	{-3.5f, 1.0f},
	{-3.5f, 0.0f},
	{3.5f, 0.0f}
};
GLfloat hourglass_top[5][2] = {
	{3.5f, 7.0f},
	{3.5f, 8.0f},
	{-3.5f, 8.0f},
	{-3.5f, 7.0f},
	{3.5f, 7.0f}
};
GLfloat hourglass_sand[9][2] = {
	{2.0f, 1.0f},
	{0.2f, 2.0f},
	{0.2f, 4.2f},
	{1.0f, 5.0f},
	{-1.0f, 5.0f},
	{-0.2f, 4.2f},
	{-0.2f, 2.0f},
	{-2.0f, 1.0f},
	{2.0f, 1.0f}
};
GLfloat hourglass_color[3] = { 0.3333f, 0.1804f, 0.1882f }; // 노란색
GLfloat sand_color[3] = { 1.0f, 0.9529f, 0.4314f }; // 빨간색
GLfloat wood_color[3] = { 0.9176f, 0.7098f, 0.6627f }; // 노란색
GLuint VBO_hourglass , VAO_hourglass;

void prepare_hourglass() {

	// Initialize vertex buffer object
	glGenBuffers(1, &VBO_hourglass);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_hourglass);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hourglass), hourglass, GL_STATIC_DRAW);

	// Initialize vertex array object
	glGenVertexArrays(1, &VAO_hourglass);
	glBindVertexArray(VAO_hourglass);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_hourglass);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void draw_hourglass() {
	glBindVertexArray(VAO_hourglass);

	glUniform3fv(loc_primitive_color, 1, hourglass_color);

	std::vector<GLfloat> triangles;
	for (size_t i = 0; i < 18; ++i) {
		if (i < 4 || i > 12) {
			triangles.push_back(0.0f);  // 중심 (0,0)
			triangles.push_back(1.0f);
			triangles.push_back(hourglass[i][0]);  // 점 i
			triangles.push_back(hourglass[i][1]);
			triangles.push_back(hourglass[i + 1][0]);  // 다음 점
			triangles.push_back(hourglass[i + 1][1]);
		}
		else {
			triangles.push_back(0.0f);  // 중심 (0,0)
			triangles.push_back(7.0f);
			triangles.push_back(hourglass[i][0]);  // 점 i
			triangles.push_back(hourglass[i][1]);
			triangles.push_back(hourglass[i + 1][0]);  // 다음 점
			triangles.push_back(hourglass[i + 1][1]);
		}

	}

	// 빈 공간 보수
	triangles.push_back(0.0f);  
	triangles.push_back(1.0f);
	triangles.push_back(1.0f);  
	triangles.push_back(4.0f);
	triangles.push_back(-1.0f);  
	triangles.push_back(4.0f);

	triangles.push_back(0.0f);  
	triangles.push_back(7.0f);
	triangles.push_back(1.0f);  
	triangles.push_back(4.0f);
	triangles.push_back(-1.0f);  
	triangles.push_back(4.0f);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_hourglass);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(GLfloat), triangles.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);


	triangles.clear();
	// 모래 부분
	glUniform3fv(loc_primitive_color, 1, sand_color);
	for (size_t i = 0; i < 8; ++i) {
		if (i == 0 || i  == 6 || i == 7) {
			triangles.push_back(0.0f);
			triangles.push_back(2.0f);
			triangles.push_back(hourglass_sand[i][0]);
			triangles.push_back(hourglass_sand[i][1]);
			triangles.push_back(hourglass_sand[i + 1][0]);
			triangles.push_back(hourglass_sand[i + 1][1]);
		}
		else if (i == 1 || i == 5) {
			triangles.push_back(0.0f);
			triangles.push_back(3.0f);
			triangles.push_back(hourglass_sand[i][0]);
			triangles.push_back(hourglass_sand[i][1]);
			triangles.push_back(hourglass_sand[i + 1][0]);
			triangles.push_back(hourglass_sand[i + 1][1]);
		}
		else{
			triangles.push_back(0.0f);
			triangles.push_back(4.2f);
			triangles.push_back(hourglass_sand[i][0]);
			triangles.push_back(hourglass_sand[i][1]);
			triangles.push_back(hourglass_sand[i + 1][0]);
			triangles.push_back(hourglass_sand[i + 1][1]);
		}
	}
	// 빈 공간 보수
	triangles.push_back(0.0f);
	triangles.push_back(3.0f);
	triangles.push_back(0.2f);
	triangles.push_back(2.0f);
	triangles.push_back(-0.2f);
	triangles.push_back(2.0f);

	triangles.push_back(0.0f);
	triangles.push_back(3.0f);
	triangles.push_back(0.2f);
	triangles.push_back(4.2f);
	triangles.push_back(-0.2f);
	triangles.push_back(4.2f);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_hourglass);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(GLfloat), triangles.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);




	triangles.clear();
	// bottom
	glUniform3fv(loc_primitive_color, 1, wood_color);
	for (size_t i = 0; i < 4; ++i) {
			triangles.push_back(0.0f);
			triangles.push_back(0.5f);
			triangles.push_back(hourglass_bottom[i][0]);
			triangles.push_back(hourglass_bottom[i][1]);
			triangles.push_back(hourglass_bottom[i + 1][0]);
			triangles.push_back(hourglass_bottom[i + 1][1]);
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO_hourglass);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(GLfloat), triangles.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);

	triangles.clear();
	// top
	glUniform3fv(loc_primitive_color, 1, wood_color);
	for (size_t i = 0; i < 4; ++i) {
		triangles.push_back(0.0f);
		triangles.push_back(7.5f);
		triangles.push_back(hourglass_top[i][0]);
		triangles.push_back(hourglass_top[i][1]);
		triangles.push_back(hourglass_top[i + 1][0]);
		triangles.push_back(hourglass_top[i + 1][1]);
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO_hourglass);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(GLfloat), triangles.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);



	glBindVertexArray(0);
}

GLfloat feather[22][2] = {
	{0.25f, 0.0f},
	{0.25f, 2.0f},
	{0.5f, 2.0f},
	{1.0f, 3.0f},
	{0.5f, 4.0f},
	{1.5f, 5.5f},
	{2.0f, 7.0f},
	{1.0f, 8.0f},
	{2.0f, 12.0f},
	{1.0f, 14.0f},
	{0.0f, 15.0f},
	{-1.0f, 14.0f},
	{-2.0f, 12.0f},
	{-1.0f, 8.0f},
	{-2.0f, 8.0f},
	{-1.0f, 5.0f},
	{-0.5f, 4.0f},
	{-1.0f, 3.0f},
	{-0.5f, 2.0f},
	{-0.25f, 2.0f},
	{-0.25f, 0.0f},
	{0.25f, 0.0f}
};
GLfloat feather_line[5][2] = {
	{0.15f, 6.0f},
	{0.15f, 11.0f},
	{-0.15f, 11.0f},
	{-0.15f, 6.0f},
	{0.15f, 6.0f}
};
GLfloat feather_color[3] = { 0.5529f, 0.1333f, 0.2392f }; // 노란색
GLfloat featherline_color[3] = { 0.8275f, 0.4275f, 0.5098f }; // 빨간색
GLuint VBO_feather, VAO_feather;

void prepare_feather() {

	// Initialize vertex buffer object
	glGenBuffers(1, &VBO_feather);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_feather);
	glBufferData(GL_ARRAY_BUFFER, sizeof(feather), feather, GL_STATIC_DRAW);

	// Initialize vertex array object
	glGenVertexArrays(1, &VAO_feather);
	glBindVertexArray(VAO_feather);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_feather);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void draw_feather() {
	glBindVertexArray(VAO_feather);

	glUniform3fv(loc_primitive_color, 1, feather_color);

	std::vector<GLfloat> triangles;
	for (size_t i = 0; i < 21; ++i) {
		if (i < 4 || i > 15) {
			triangles.push_back(0.0f);  // 중심 (0,0)
			triangles.push_back(4.0f);
			triangles.push_back(feather[i][0]);  // 점 i
			triangles.push_back(feather[i][1]);
			triangles.push_back(feather[i + 1][0]);  // 다음 점
			triangles.push_back(feather[i + 1][1]);
		}
		else {
			triangles.push_back(0.0f);  // 중심 (0,0)
			triangles.push_back(8.0f);
			triangles.push_back(feather[i][0]);  // 점 i
			triangles.push_back(feather[i][1]);
			triangles.push_back(feather[i + 1][0]);  // 다음 점
			triangles.push_back(feather[i + 1][1]);
		}

	}

	// 빈 공간 보수
	triangles.push_back(0.0f);
	triangles.push_back(8.0f);
	triangles.push_back(0.5f);
	triangles.push_back(4.0f);
	triangles.push_back(-0.5f);
	triangles.push_back(4.0f);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_feather);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(GLfloat), triangles.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);


	triangles.clear();
	// 선 부분
	glUniform3fv(loc_primitive_color, 1, featherline_color);
	for (size_t i = 0; i < 4; ++i) {
			triangles.push_back(0.0f);
			triangles.push_back(8.5f);
			triangles.push_back(feather_line[i][0]);
			triangles.push_back(feather_line[i][1]);
			triangles.push_back(feather_line[i + 1][0]);
			triangles.push_back(feather_line[i + 1][1]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO_feather);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(GLfloat), triangles.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);



	glBindVertexArray(0);
}

GLfloat flower[30][2] = {
	{0.0f, 1.0f},
	{1.0f, 0.0f},
	{2.2f, 0.2f},
	{3.0f, 1.0f},
	{2.8f, 1.5f},
	{1.0f, 2.0f},
	{2.0f, 2.3f},
	{3.0f, 3.0f},
	{3.2f, 3.7f},
	{3.1f, 4.2f},
	{2.7f, 4.4f},
	{1.0f, 3.5f},
	{0.8f, 5.0f},
	{0.5f, 6.0f},
	{0.0f, 5.9f},
	{-0.5f, 6.0f},
	{-0.8f, 5.0f},
	{-1.0f, 4.0f},
	{-1.0f, 3.5f},
	{-1.5f, 3.8f},
	{-2.5f, 4.0f},
	{-3.5f, 3.5f},
	{-4.0f, 3.0f},
	{-3.6f, 2.3f},
	{-2.0f, 2.0f},
	{-3.0f, 1.4f},
	{-2.8f, 0.6f},
	{-2.0f, 0.0f},
	{-1.0f, 0.2f},
	{0.0f, 1.0f}
};
GLfloat leaves[15][2] = {
	{0.0f, 1.0f},
	{-0.1f, 0.0f},
	{-0.35f, -0.7f},
	{0.6f, -0.65f},
	{1.3f, -1.0f},
	{2.0f, -1.7f},
	{1.1f, -1.9f},
	{0.3f, -1.8f},
	{-0.5f, -1.5f},
	{-1.6f, -1.8f},
	{-2.7f, -2.0f},
	{-2.0f, -1.5f},
	{-1.2f, -0.8f},
	{-1.0f, 0.2f},
	{0.0f, 1.0f}
};
GLfloat stamen[7][2] = {
	{-0.5f, 2.8f},
	{-0.2f, 2.5f},
	{0.45f, 2.3f},
	{-0.3f, 2.15f},
	{-0.8f, 1.9f},
	{-0.5f, 2.4f},
	{-0.5f, 2.8f}
};
GLfloat flower_color[3] = { 0.2353f, 0.6980f, 0.9176f }; // 노란색
GLfloat leaves_color[3] = { 0.7765f, 0.8745f, 0.2353f }; // 빨간색
GLfloat stamen_color[3] = { 1.0f, 1.0f, 1.0f }; // 임시
GLuint VBO_flower, VAO_flower;

void prepare_flower() {

	// Initialize vertex buffer object
	glGenBuffers(1, &VBO_flower);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_flower);
	glBufferData(GL_ARRAY_BUFFER, sizeof(flower), flower, GL_STATIC_DRAW);

	// Initialize vertex array object
	glGenVertexArrays(1, &VAO_flower);
	glBindVertexArray(VAO_flower);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_flower);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void draw_flower() {
	glBindVertexArray(VAO_flower);

	glUniform3fv(loc_primitive_color, 1, flower_color);

	std::vector<GLfloat> triangles;
	for (size_t i = 0; i < 29; ++i) {
		if (i < 5) {
			triangles.push_back(0.5f);  // 중심 (0,0)
			triangles.push_back(1.5f);
			triangles.push_back(flower[i][0]);  // 점 i
			triangles.push_back(flower[i][1]);
			triangles.push_back(flower[i + 1][0]);  // 다음 점
			triangles.push_back(flower[i + 1][1]);
		}
		if (i >= 5  && i < 11) {
			triangles.push_back(1.0f);  // 중심 (0,0)
			triangles.push_back(2.75f);
			triangles.push_back(flower[i][0]);  // 점 i
			triangles.push_back(flower[i][1]);
			triangles.push_back(flower[i + 1][0]);  // 다음 점
			triangles.push_back(flower[i + 1][1]);
		}
		if (i >= 11  && i < 18) {
			triangles.push_back(0.0f);  // 중심 (0,0)
			triangles.push_back(3.5f);
			triangles.push_back(flower[i][0]);  // 점 i
			triangles.push_back(flower[i][1]);
			triangles.push_back(flower[i + 1][0]);  // 다음 점
			triangles.push_back(flower[i + 1][1]);
		}
		if (i >= 18 && i < 24) {
			triangles.push_back(-1.5f);  // 중심 (0,0)
			triangles.push_back(2.75f);
			triangles.push_back(flower[i][0]);  // 점 i
			triangles.push_back(flower[i][1]);
			triangles.push_back(flower[i + 1][0]);  // 다음 점
			triangles.push_back(flower[i + 1][1]);
		}
		else {
			triangles.push_back(-1.0f);  // 중심 (0,0)
			triangles.push_back(1.5f);
			triangles.push_back(flower[i][0]);  // 점 i
			triangles.push_back(flower[i][1]);
			triangles.push_back(flower[i + 1][0]);  // 다음 점
			triangles.push_back(flower[i + 1][1]);
		}

	}

	// 빈 공간 보수
	triangles.push_back(-1.0f);
	triangles.push_back(3.5f);
	triangles.push_back(-2.0f);
	triangles.push_back(2.0f);
	triangles.push_back(-1.0f);
	triangles.push_back(1.0f);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_flower);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(GLfloat), triangles.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);


	triangles.clear();
	// 선 부분
	glUniform3fv(loc_primitive_color, 1, leaves_color);
	for (size_t i = 0; i < 14; ++i) {
		if (i < 2 || i > 11) {
			triangles.push_back(-0.7f);
			triangles.push_back(-0.8f);
			triangles.push_back(leaves[i][0]);
			triangles.push_back(leaves[i][1]);
			triangles.push_back(leaves[i + 1][0]);
			triangles.push_back(leaves[i + 1][1]);
		} 
		else if (i > 2 && i < 8) {
			triangles.push_back(-0.5f);
			triangles.push_back(-1.0f);
			triangles.push_back(leaves[i][0]);
			triangles.push_back(leaves[i][1]);
			triangles.push_back(leaves[i + 1][0]);
			triangles.push_back(leaves[i + 1][1]);
		}
		else {
			triangles.push_back(-0.7f);
			triangles.push_back(-0.8f);
			triangles.push_back(leaves[i][0]);
			triangles.push_back(leaves[i][1]);
			triangles.push_back(leaves[i + 1][0]);
			triangles.push_back(leaves[i + 1][1]);
		}
	}

	// 빈 공간 보수
	triangles.push_back(-0.5f);
	triangles.push_back(-1.5f);
	triangles.push_back(0.5f);
	triangles.push_back(-0.65f);
	triangles.push_back(-0.8f);
	triangles.push_back(-0.5f);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_flower);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(GLfloat), triangles.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);


	triangles.clear();
	// 선 부분
	glUniform3fv(loc_primitive_color, 1, stamen_color);
	for (size_t i = 0; i < 6; ++i) {
		triangles.push_back(-0.3f);
		triangles.push_back(2.3f);
		triangles.push_back(stamen[i][0]);
		triangles.push_back(stamen[i][1]);
		triangles.push_back(stamen[i + 1][0]);
		triangles.push_back(stamen[i + 1][1]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO_flower);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(GLfloat), triangles.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);

	glBindVertexArray(0);
}









struct FlyingFeather {
	glm::vec2 start_pos; // 깃털의 시작 위치 (화면 바깥의 무작위 위치)
	glm::vec2 target_pos; // 깃털의 목표 위치 (마우스 클릭 위치)
	float start_time;    // 이동 애니메이션이 시작된 scene_timer 시간
	float duration;      // 시작 위치에서 목표 위치까지 이동하는 데 걸리는 시간 (초)
};

// 현재 날아다니는 깃털 목록
std::vector<FlyingFeather> active_flying_feathers;
std::mt19937 rng; // Mersenne Twister 엔진
std::uniform_real_distribution<float> dist_width; // 너비 범위 (-win_width/2 to win_width/2)
std::uniform_real_distribution<float> dist_height; // 높이 범위 (-win_height/2 to win_height/2)
std::uniform_int_distribution<int> dist_side; // 무작위 측면 선택 (0:상, 1:하, 2:좌, 3:우)

std::vector<glm::vec2> clicked_flower_positions;

int mouse_right_click = 0;
int day = 1;
int cocktail_wave = 0;

float scene_timer = 0.0f;
float house_scaling_factor = 1.0f;
float random_height;

float hat_scaling_factor = 0.5f;

float cocktail_scaling_factor = 1.0f;

//sin
float frequency = 10.0f;
float amplitude = 50.0f;

float fixed_x;
float fixed_y;


float orbit_angle_main = 0.0f; // 모든 오브젝트에 공통으로 적용될 주 공전 각도
float orbit_radius = 80.0f; // 공전 궤도 반지름 (원하는 값으로 조절)
float orbit_speed = 1.0f; // 공전 속도 (원하는 값으로 조절)

// 각 오브젝트의 초기 각도 오프셋 (5개 오브젝트이므로 360 / 5 = 72도 간격)
float flower_angle_offset = 0.0f;
float feather_angle_offset = 72.0f;
float hourglass_angle_offset = 144.0f;
float grail_angle_offset = 216.0f;
float crown_angle_offset = 288.0f;

// 오브젝트 자체 회전을 위한 변수
float self_rotation_angle = 0.0f; // 오브젝트 자체의 회전 각도
float self_rotation_speed = 3.0f; // 오브젝트 자체 회전 속도 (원하는 값으로 조절)


float sword_rotation_angle = 0.0f;
float sword_scaling_factor = 1.0f;
int scaling_direction = 1;

void display(void) {
	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);

	if (scene_timer < 0.9f) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distrib(-2, 2);
		random_height = distrib(gen);

		house_scaling_factor += 0.01f;
		hat_scaling_factor += 0.001f;


	}
	else {
		random_height = 0.0f;
	}



	orbit_angle_main += orbit_speed;
	if (orbit_angle_main >= 360.0f) orbit_angle_main -= 360.0f;

	self_rotation_angle += self_rotation_speed;
	if (self_rotation_angle >= 360.0f) self_rotation_angle -= 360.0f;


	if (scene_timer < 1.0f) {
		fixed_x = win_width / 2.0f;
		fixed_y = win_height / 2.0f;
		//모래시계
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(fixed_x - 30.0f, fixed_y - 30.0f, 0.0f)); // 위치
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(sword_rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f)); // 회전
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -20.0f, 0.0f)); // 위치
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f, 5.0f, 1.0f)); // 크기 조정
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_hourglass();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 30.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f + house_scaling_factor, 2.0f + house_scaling_factor, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -200.0f + random_height, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(4.0f, 4.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_car();

		float hat_x = scene_timer * 100.0f;
		float hat_y = sin(scene_timer * frequency) * amplitude;
		float hat_x_speed = 1000.0f; // Hat의 x축 이동 속도 (effective_time에 곱해지는 값)
		float hat_time_offset = 0.1f;
		int NUM_HATS = 17;

		for (int i = 0; i < NUM_HATS; ++i) {
			// 각 Hat의 유효 시간 계산 (시간 오프셋 적용)
			float effective_time = scene_timer - (float)i * hat_time_offset;	
			if (effective_time < 0.0f) {	//아직 안 나온 건 SKIP
				continue;
			}
			float hat_x = effective_time * hat_x_speed;							// 각 Hat의 x, y 위치 계산 (사인 파동 함수 사용)
			float hat_y = sin(effective_time * frequency) * amplitude;

			float vx = hat_x_speed;												// 사인 곡선의 접선 벡터 계산
			float vy = amplitude * frequency * cos(effective_time * frequency);

			float tangent_angle_rad = atan2(vy, vx);
			float tangent_angle_deg = glm::degrees(tangent_angle_rad);

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(hat_x - 400.0f, hat_y + 100.0f, 0.0f)); // 3. 계산된 위치로 이동
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(tangent_angle_deg), glm::vec3(0.0f, 0.0f, 1.0f)); // 2. 접선 각도만큼 회전
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(hat_scaling_factor, hat_scaling_factor, 1.0f)); // 1. 크기 조절
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix; // ViewProjectionMatrix 곱하기
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_hat();
		}
	}
	else {
		if (mouse_right_click == 1 || mouse_right_click == 6) {
			// 꽃 그리기 (검 주위를 공전)
			// 변환 순서 (코드 작성 순서): Translate (검 위치) -> Rotate (공전) -> Translate (궤도 반경) -> Scale
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(mouse_x, mouse_y, 0.0f)); // 4. 검의 현재 위치로 이동
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(orbit_angle_main + flower_angle_offset), glm::vec3(0.0f, 0.0f, 1.0f)); // 3. 공전 회전 (주 각도 + 오프셋)
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(orbit_radius, 0.0f, 0.0f)); // 2. 공전 궤도 반경만큼 이동
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(self_rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f)); // 2. 오브젝트 자체 회전
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f)); // 1. 크기 조절 (원하는 크기로 조절)
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -2.3f, 0.0f)); // 위치

			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_flower();


			for (const auto& flower_pos : clicked_flower_positions) {
				ModelMatrix = glm::mat4(1.0f); // 새로운 ModelMatrix 시작

				// 저장된 꽃 위치로 translation
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(flower_pos.x, flower_pos.y, 0.0f));

				// 꽃 크기 조절 (원하는 크기로 조절)
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f, 5.0f, 1.0f)); // 적절한 스케일 값 사용

				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_flower(); // 꽃 그리기 함수 호출
			}


		}
		if (mouse_right_click == 2 || mouse_right_click == 6) {
			// 깃털 그리기 (검 주위를 공전)
			// 변환 순서 (코드 작성 순서): Translate (검 위치) -> Rotate (공전) -> Translate (궤도 반경) -> Scale
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(mouse_x, mouse_y, 0.0f)); // 4. 검의 현재 위치로 이동
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(orbit_angle_main + feather_angle_offset), glm::vec3(0.0f, 0.0f, 1.0f)); // 3. 공전 회전 (주 각도 + 오프셋)
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(orbit_radius, 0.0f, 0.0f)); // 2. 공전 궤도 반경만큼 이동 (예: x축 방향)
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(self_rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f)); // 2. 오브젝트 자체 회전
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f)); // 1. 깃털 크기 조절 (원하는 크기로 조절)
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -7.5f, 0.0f)); // 위치

			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_feather();

			float current_time = scene_timer;
			// 비행이 끝나지 않은 깃털을 걸러낼 임시 벡터
			std::vector<FlyingFeather> still_flying_feathers;

			for (const auto& feather : active_flying_feathers) {
				float elapsed_time = current_time - feather.start_time;

				// 애니메이션 지속 시간 동안만 이동 및 그리기
				if (elapsed_time < feather.duration) {
					float t = elapsed_time / feather.duration; // 선형 보간을 위한 비율 (0.0에서 1.0까지 증가)
					glm::vec2 current_pos = feather.start_pos * (1.0f - t) + feather.target_pos * t; // 현재 위치 계산: 시작 위치에서 목표 위치로 이동


					glm::vec2 direction = feather.target_pos - current_pos; // 날아가는 방향 벡터 계산
					float angle_radians = atan2(direction.y, direction.x);  // 방향 벡터의 각도 계산 (라디안)
					float angle_degrees = glm::degrees(angle_radians);      // 각도를 도로 변환
					float rotation_angle_z = angle_degrees - 90.0f; // 90도 빼기



					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(current_pos.x, current_pos.y, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation_angle_z), glm::vec3(0.0f, 0.0f, 1.0f)); // 2. 날아가는 방향으로 회전
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0f, 5.0f, 1.0f)); // 적절한 스케일 값 사용
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_feather(); // 깃털 그리기 함수 호출


					still_flying_feathers.push_back(feather);  // 비행이 끝나지 않은 깃털은 다음 프레임을 위해 저장

				}
			}
			active_flying_feathers = still_flying_feathers; // 비행이 끝난 깃털 제거 후 벡터 업데이트

		}
		if (mouse_right_click == 3 || mouse_right_click == 6) {
			// 모래시계 그리기 (검 주위를 공전)
			// 변환 순서 (코드 작성 순서): Translate (검 위치) -> Rotate (공전) -> Translate (궤도 반경) -> Scale
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(mouse_x, mouse_y, 0.0f)); // 4. 검의 현재 위치로 이동
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(orbit_angle_main + hourglass_angle_offset), glm::vec3(0.0f, 0.0f, 1.0f)); // 3. 공전 회전 (주 각도 + 오프셋)
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(orbit_radius, 0.0f, 0.0f)); // 2. 공전 궤도 반경만큼 이동
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(self_rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f)); // 2. 오브젝트 자체 회전
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f)); // 1. 크기 조절 (원하는 크기로 조절)
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -4.0f, 0.0f)); // 헛돌지 않는 위치

			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_hourglass();
		}
		if (mouse_right_click == 4 || mouse_right_click == 6) {
			// 성배 그리기 (검 주위를 공전)
			// 변환 순서 (코드 작성 순서): Translate (검 위치) -> Rotate (공전) -> Translate (궤도 반경) -> Scale
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(mouse_x, mouse_y, 0.0f)); // 4. 검의 현재 위치로 이동
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(orbit_angle_main + grail_angle_offset), glm::vec3(0.0f, 0.0f, 1.0f)); // 3. 공전 회전 (주 각도 + 오프셋)
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(orbit_radius, 0.0f, 0.0f)); // 2. 공전 궤도 반경만큼 이동
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(self_rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f)); // 2. 오브젝트 자체 회전
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f)); // 1. 크기 조절 (원하는 크기로 조절)
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -4.0f, 0.0f)); // 위치

			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_grail();

			if (cocktail_wave) {
				float cocktail_x = scene_timer * 100.0f;
				float cocktail_y = sin(scene_timer * frequency) * amplitude;
				float cocktail_x_speed = 1000.0f; // Hat의 x축 이동 속도 (effective_time에 곱해지는 값)
				float cocktail_time_offset = 0.1f;
				int NUM_COCKTAILS = 17;

				for (int i = 0; i < NUM_COCKTAILS; ++i) {
					// 각 Hat의 유효 시간 계산 (시간 오프셋 적용)
					float effective_time = scene_timer - (float)i * cocktail_time_offset;
					if (effective_time < 0.0f) {	//아직 안 나온 건 SKIP
						continue;
					}
					float cocktail_x = effective_time * cocktail_x_speed;							// 각 Hat의 x, y 위치 계산 (사인 파동 함수 사용)
					float cocktail_y = sin(effective_time * frequency) * amplitude;

					float vx = cocktail_x_speed;												// 사인 곡선의 접선 벡터 계산
					float vy = amplitude * frequency * cos(effective_time * frequency);

					float tangent_angle_rad = atan2(vy, vx);
					float tangent_angle_deg = glm::degrees(tangent_angle_rad);

					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(cocktail_x - 400.0f, cocktail_y - 100.0f, 0.0f)); // 3. 계산된 위치로 이동
					ModelMatrix = glm::rotate(ModelMatrix, glm::radians(tangent_angle_deg), glm::vec3(0.0f, 0.0f, 1.0f)); // 2. 접선 각도만큼 회전
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(hat_scaling_factor, hat_scaling_factor, 1.0f)); // 1. 크기 조절
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix; // ViewProjectionMatrix 곱하기
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_cocktail();

					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(cocktail_x - 400.0f, cocktail_y - 200.0f, 0.0f)); // 3. 계산된 위치로 이동
					ModelMatrix = glm::rotate(ModelMatrix, glm::radians(tangent_angle_deg), glm::vec3(0.0f, 0.0f, 1.0f)); // 2. 접선 각도만큼 회전
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(hat_scaling_factor, hat_scaling_factor, 1.0f)); // 1. 크기 조절
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix; // ViewProjectionMatrix 곱하기
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_cocktail();

					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(cocktail_x - 400.0f, cocktail_y + 100.0f, 0.0f)); // 3. 계산된 위치로 이동
					ModelMatrix = glm::rotate(ModelMatrix, glm::radians(tangent_angle_deg), glm::vec3(0.0f, 0.0f, 1.0f)); // 2. 접선 각도만큼 회전
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(hat_scaling_factor, hat_scaling_factor, 1.0f)); // 1. 크기 조절
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix; // ViewProjectionMatrix 곱하기
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_cocktail();

					ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(cocktail_x - 400.0f, cocktail_y + 200.0f, 0.0f)); // 3. 계산된 위치로 이동
					ModelMatrix = glm::rotate(ModelMatrix, glm::radians(tangent_angle_deg), glm::vec3(0.0f, 0.0f, 1.0f)); // 2. 접선 각도만큼 회전
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(hat_scaling_factor, hat_scaling_factor, 1.0f)); // 1. 크기 조절
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix; // ViewProjectionMatrix 곱하기
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_cocktail();
				}
			}



		}
		if (mouse_right_click == 5 || mouse_right_click == 6) {
			// 왕관 그리기 (검 주위를 공전)
			// 변환 순서 (코드 작성 순서): Translate (검 위치) -> Rotate (공전) -> Translate (궤도 반경) -> Scale
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(mouse_x, mouse_y, 0.0f)); // 4. 검의 현재 위치로 이동
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(orbit_angle_main + crown_angle_offset), glm::vec3(0.0f, 0.0f, 1.0f)); // 3. 공전 회전 (주 각도 + 오프셋)
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(orbit_radius, 0.0f, 0.0f)); // 2. 공전 궤도 반경만큼 이동
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(self_rotation_angle), glm::vec3(0.0f, 0.0f, 1.0f)); // 2. 오브젝트 자체 회전
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f)); // 1. 크기 조절 (원하는 크기로 조절)
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -1.5f, 0.0f)); // 위치

			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_crown();
		}

		if (cocktail_scaling_factor < 10.0f) {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(cocktail_scaling_factor, cocktail_scaling_factor, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_cocktail();
		}
		else {
			cocktail_scaling_factor = 1.0f;
			//scene_timer = 1.0f;
			cocktail_wave = 1;
		}




	}


	ModelMatrix = glm::mat4(1.0f);
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes();























	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(mouse_x + 10, mouse_y - 10, 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // 회전
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 3.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_sword();

	glFlush();

	scene_timer += 0.002f;
	sword_rotation_angle += 1.0f; // 회전 속도 증가
	if (sword_rotation_angle >= 360.0f) sword_rotation_angle -= 360.0f;
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;

	}
}

void mouse_button(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) { 
		if (state == GLUT_DOWN) { 
			printf("Left mouse button clicked at coordinates (%d, %d)\n", x, y); // 예시 출력
			float mouse_world_x = (float)x - win_width / 2.0f;
			float mouse_world_y = (float)(win_height - y) - win_height / 2.0f;
			if(mouse_right_click == 1 || mouse_right_click == 6) clicked_flower_positions.push_back(glm::vec2(mouse_world_x, mouse_world_y));
			
			
			if (mouse_right_click == 2 || mouse_right_click == 6) {
				float current_time = scene_timer; // 현재 시간
				float flight_duration = 0.1f; // 깃털 비행 시간 (초) - 원하는 속도에 따라 조절
				float outside_offset = 50.0f; // 화면 바깥으로 나가는 정도

				
				glm::vec2 target_pos = glm::vec2((float)x - win_width / 2.0f, (float)(win_height - y) - win_height / 2.0f); // 클릭된 위치를 목표 위치로 설정 (윈도우 좌표를 월드 좌표로 변환)

				
				glm::vec2 start_pos;	// 무작위 시작 위치 생성
				int side = dist_side(rng); // 0:상, 1:하, 2:좌, 3:우 무작위 선택

				switch (side) {
				case 0: // 상단
					start_pos = glm::vec2(dist_width(rng), win_height / 2.0f + outside_offset);
					break;
				case 1: // 하단
					start_pos = glm::vec2(dist_width(rng), -win_height / 2.0f - outside_offset);
					break;
				case 2: // 좌측
					start_pos = glm::vec2(-win_width / 2.0f - outside_offset, dist_height(rng));
					break;
				case 3: // 우측
					start_pos = glm::vec2(win_width / 2.0f + outside_offset, dist_height(rng));
					break;
				}
				// 새로운 날아다니는 깃털 생성 및 추가
				active_flying_feathers.push_back({ start_pos, target_pos, current_time, flight_duration });
			}
			if (mouse_right_click == 3 || mouse_right_click == 6) {
				if (day) {
					glClearColor(16 / 255.0f, 20 / 255.0f, 94 / 255.0f, 1.0f);
					day = 0;
				}
				else {
					glClearColor(239 / 255.0f, 205 / 255.0f, 32 / 255.0f, 1.0f);
					day = 1;
				}
			}
			if (mouse_right_click == 4 || mouse_right_click == 6) {
				cocktail_scaling_factor += 1.0f;
			}
			if (mouse_right_click == 5 || mouse_right_click == 6) {

				
			}
		}
		else if (state == GLUT_UP) { // 버튼이 떼어진 상태
			printf("Left mouse button released at coordinates (%d, %d)\n", x, y); // 예시 출력
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			printf("Right mouse button clicked at coordinates (%d, %d)\n", x, y); // 예시 출력
			if (mouse_right_click == 0) {
				mouse_right_click++;
			}
			else if (mouse_right_click == 1) {
				mouse_right_click++;
				clicked_flower_positions.clear();
			}
			else if (mouse_right_click == 2) {
				mouse_right_click++;
			}
			else if (mouse_right_click == 3) {
				mouse_right_click++;
			}
			else if (mouse_right_click == 4) {
				mouse_right_click++;
				cocktail_wave = 0;
			}
			else if (mouse_right_click == 5) {
				mouse_right_click++;
			}
			else if(mouse_right_click == 6){
				mouse_right_click = 0;
				clicked_flower_positions.clear();
			}

		}
		else if (state == GLUT_UP) { // 버튼이 떼어진 상태
			printf("Right mouse button released at coordinates (%d, %d)\n", x, y); // 예시 출력
		}
	}
	// 다른 마우스 버튼(GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON)에 대한 처리도 추가할 수 있습니다.
}



void mouse_motion(int x, int y) {
	mouse_x = (float)x - win_width / 2.0f;
	mouse_y = (float)(win_height - y) - win_height / 2.0f;

	//glutPostRedisplay();
};

void reshape(int width, int height) {
	win_width = width, win_height = height;

	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0,
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;


	dist_width = std::uniform_real_distribution<float>(-win_width / 2.0f, win_width / 2.0f);
	dist_height = std::uniform_real_distribution<float>(-win_height / 2.0f, win_height / 2.0f);


	update_axes();
	update_line();

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_line);
	glDeleteBuffers(1, &VBO_line);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);

	// Delete others here too!!!
}

void timer(int value) {					// timer 추가	
	glutPostRedisplay();
	glutTimerFunc(16, timer, 0);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutCloseFunc(cleanup);
	glutTimerFunc(0, timer, 0);
	glutPassiveMotionFunc(mouse_motion);
	glutMouseFunc(mouse_button);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(128 / 255.0f, 128 / 255.0f, 114 / 255.0f, 1.0f);				//배경 색상 설정
	ViewMatrix = glm::mat4(1.0f);

	std::random_device rd;
	rng.seed(rd());
	dist_width = std::uniform_real_distribution<float>(-win_width / 2.0f, win_width / 2.0f);
	dist_height = std::uniform_real_distribution<float>(-win_height / 2.0f, win_height / 2.0f);
	dist_side = std::uniform_int_distribution<int>(0, 3);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_line();
	//prepare_airplane();
	//prepare_shirt();
	prepare_house();
	prepare_car();
	prepare_cocktail();
	//prepare_car2();
	prepare_hat();
	//prepare_cake();
	prepare_sword();
	prepare_shield();
	prepare_crown();
	prepare_grail();
	prepare_hourglass();
	prepare_feather();
	prepare_flower();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
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



#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 2DObjects_GLSL_3.0.1.3";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC, S' "
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 500);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}


