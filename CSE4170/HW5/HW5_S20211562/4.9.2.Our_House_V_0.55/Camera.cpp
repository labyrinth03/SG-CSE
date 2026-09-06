#define _CRT_SECURE_NO_WARNINGS

#include "Camera.h"
#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

void Perspective_Camera::define_camera(int win_width, int win_height, float win_aspect_ratio) {
	glm::mat3 R33_t;
	glm::mat4 T;
 
	switch (camera_id) {
	case CAMERA_MAIN:
		flag_valid = true;
		flag_move = true; // yes. the main camera is permitted to move

		// let's use glm funtions to set up the initial camera pose
		ViewMatrix = glm::lookAt(glm::vec3(-300.0f, -300.0f, 200.0f), glm::vec3(125.0f, 80.0f, 25.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // initial pose for main camera
		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);

		cam_view.naxis = -cam_view.naxis;

		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]); // why does this work?

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 15.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;  
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;

		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		view_port.x = 200; view_port.y = 200; view_port.w = win_width - 200; view_port.h = win_height - 200;
		break;

	case CAMERA_CC0:
		flag_valid = true;
		flag_move = false; 
		flag_rotate_axes = false;
		flag_zoom = false;

		ViewMatrix = glm::lookAt(glm::vec3(15.0f, 15.0f, 50.0f), glm::vec3(55.0f, 60.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
		cam_view.naxis = -cam_view.naxis; 

		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]);

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 60.0f * TO_RADIAN; 
		cam_proj.params.pers.aspect = win_aspect_ratio; 
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;
		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		view_port.x = 0; view_port.y = 0; view_port.w = 0; view_port.h = 0;
		break;

	case CAMERA_CC1: 
		flag_valid = true;
		flag_move = false;
		flag_rotate_axes = false;
		flag_zoom = false;

		ViewMatrix = glm::lookAt(glm::vec3(70.0f, 125.0f, 50.0f), glm::vec3(145.0f, 155.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
		cam_view.naxis = -cam_view.naxis;

		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]);

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 50.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;
		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		view_port.x = 0; view_port.y = 0; view_port.w = 0; view_port.h = 0;
		break;

	case CAMERA_CC2: 
		flag_valid = true;
		flag_move = false;
		flag_rotate_axes = false;
		flag_zoom = false;

		ViewMatrix = glm::lookAt(glm::vec3(225.0f, 15.0f, 50.0f), glm::vec3(205.0f, 35.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		cam_view.uaxis = glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		cam_view.vaxis = glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		cam_view.naxis = glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]);
		cam_view.naxis = -cam_view.naxis;

		R33_t = glm::transpose(glm::mat3(ViewMatrix));
		T = glm::mat4(R33_t) * ViewMatrix;
		cam_view.pos = -glm::vec3(T[3][0], T[3][1], T[3][2]);

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 70.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;
		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		view_port.x = 0; view_port.y = 0; view_port.w = 0; view_port.h = 0;
		break;


	case CAMERA_DYNAMIC:
		break;
	}
}

void Camera::set_ViewMatrix(void) {
	ViewMatrix = create_ViewMatrix_from_cam_view();
	cam_view.naxis = glm::normalize(cam_view.naxis);
	cam_view.uaxis = glm::normalize(glm::cross(cam_view.naxis, cam_view.vaxis));
	cam_view.vaxis = glm::normalize(glm::cross(cam_view.uaxis, cam_view.naxis));
}

void Camera::move_forward(float delta) {
	if (flag_move) {
		cam_view.pos += delta * cam_view.naxis;
		set_ViewMatrix();
	}
}

void Camera::move_right(float delta) {
	if (flag_move) {
		cam_view.pos += delta * cam_view.uaxis;
		set_ViewMatrix();
	}
}

void Camera::move_up(float delta) {
	if (flag_move) {
		cam_view.pos += delta * cam_view.vaxis;
		set_ViewMatrix();
	}
}

void Camera::rotate_u_axis(float angle) {
	if (flag_rotate_axes) {
		cam_view.naxis = glm::normalize(glm::rotate(cam_view.naxis, angle, cam_view.uaxis));
		cam_view.vaxis = glm::normalize(glm::cross(cam_view.uaxis, cam_view.naxis));
		set_ViewMatrix(); 
	}
}

void Camera::rotate_v_axis(float angle) {
	if (flag_rotate_axes) {
		cam_view.naxis = glm::normalize(glm::rotate(cam_view.naxis, angle, cam_view.vaxis));
		cam_view.uaxis = glm::normalize(glm::cross(cam_view.naxis, cam_view.vaxis));
		set_ViewMatrix();
	}
}

void Camera::rotate_n_axis(float angle) {
	if (flag_rotate_axes) {
		cam_view.uaxis = glm::normalize(glm::rotate(cam_view.uaxis, angle, cam_view.naxis));
		cam_view.vaxis = glm::normalize(glm::cross(cam_view.uaxis, cam_view.naxis));
		set_ViewMatrix(); 
	}
}

void Camera::zoom_in_out(float delta_fovy) {
	if (!flag_zoom) return;

	if (cam_proj.projection_type == CAMERA_PROJECTION_PERSPECTIVE) {
		cam_proj.params.pers.fovy += delta_fovy * TO_RADIAN;

		const float min_fovy_rad = 1.0f * TO_RADIAN;
		const float max_fovy_rad = 170.0f * TO_RADIAN;

		cam_proj.params.pers.fovy = glm::clamp(cam_proj.params.pers.fovy, min_fovy_rad, max_fovy_rad);

		ProjectionMatrix = glm::perspective(
			cam_proj.params.pers.fovy,
			cam_proj.params.pers.aspect,
			cam_proj.params.pers.n,
			cam_proj.params.pers.f
		);
	}
}

glm::mat4 Camera::create_ViewMatrix_from_cam_view() {
	return glm::lookAt(cam_view.pos, cam_view.pos + cam_view.naxis, cam_view.vaxis);
}

void Perspective_Camera::define_camera(Camera_ID id, glm::vec3 default_pos, glm::vec3 default_target, glm::vec3 default_up, float default_fovy, float default_aspect, float default_near, float default_far) {
	camera_id = id; 
	if (camera_id == CAMERA_DYNAMIC) {
		flag_valid = true;
		flag_move = true;         
		flag_rotate_axes = false;  
		flag_zoom = true;         
		flag_mouse_look = true;    
	}
	else {
		flag_valid = false; 
		flag_move = false;
		flag_rotate_axes = false;
		flag_zoom = false;
		flag_mouse_look = false;
	}

	cam_view.pos = default_pos;
	glm::mat4 temp_view = glm::lookAt(default_pos, default_target, default_up);
	cam_view.uaxis = glm::vec3(temp_view[0][0], temp_view[1][0], temp_view[2][0]);
	cam_view.vaxis = glm::vec3(temp_view[0][1], temp_view[1][1], temp_view[2][1]);
	cam_view.naxis = -glm::vec3(temp_view[0][2], temp_view[1][2], temp_view[2][2]); 

	cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
	cam_proj.params.pers.fovy = default_fovy * TO_RADIAN; 
	cam_proj.params.pers.aspect = default_aspect;
	cam_proj.params.pers.n = default_near;
	cam_proj.params.pers.f = default_far;
	ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
		cam_proj.params.pers.n, cam_proj.params.pers.f);

	set_ViewMatrix();

	view_port.x = 0; view_port.y = 0; view_port.w = 0; view_port.h = 0; 
}


void Orthographic_Camera::define_camera(Camera_ID id, glm::vec3 default_pos, glm::vec3 default_target, glm::vec3 default_up,
	float left, float right, float bottom, float top, float n, float f) {
	camera_id = id;

	flag_valid = true;
	flag_move = false;
	flag_rotate_axes = false;
	flag_zoom = false;          
	flag_mouse_look = false;

	cam_view.pos = default_pos;
	glm::mat4 temp_view = glm::lookAt(default_pos, default_target, default_up);
	cam_view.uaxis = glm::vec3(temp_view[0][0], temp_view[1][0], temp_view[2][0]);
	cam_view.vaxis = glm::vec3(temp_view[0][1], temp_view[1][1], temp_view[2][1]);
	cam_view.naxis = -glm::vec3(temp_view[0][2], temp_view[1][2], temp_view[2][2]);
	set_ViewMatrix();

	cam_proj.projection_type = CAMERA_PROJECTION_ORTHOGRAPHIC;
	cam_proj.params.ortho.left = left;
	cam_proj.params.ortho.right = right;
	cam_proj.params.ortho.botton = bottom;
	cam_proj.params.ortho.top = top;
	cam_proj.params.ortho.n = n;
	cam_proj.params.ortho.f = f;
	ProjectionMatrix = glm::ortho(left, right, bottom, top, n, f);

	view_port.x = 0; view_port.y = 0; view_port.w = 0; view_port.h = 0; 
}