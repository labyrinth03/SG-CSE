#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

enum Camera_Projection_TYPE {
	CAMERA_PROJECTION_PERSPECTIVE = 0, CAMERA_PROJECTION_ORTHOGRAPHIC
};

enum Camera_ID {
	CAMERA_MAIN = 0,
	CAMERA_CC0, CAMERA_CC1, CAMERA_CC2, CAMERA_DYNAMIC, CAMERA_VIEW_FRONT, CAMERA_VIEW_SIDE_RIGHT, 
	CAMERA_VIEW_TOP, N_MAX_CAMERAS
};

struct Camera_View {
	glm::vec3 pos;
	glm::vec3 uaxis, vaxis, naxis;
};
 
struct Camera_Projection {
	Camera_Projection_TYPE projection_type;
	union {
		struct {
			float fovy, aspect, n, f;
		} pers;
		struct {
			float left, right, botton, top, n, f;
		} ortho;
	} params;
};

struct View_Port {
	float x, y, w, h;
};

struct Camera {
	Camera_ID camera_id;
	Camera_View cam_view;
	glm::mat4 ViewMatrix;
	Camera_Projection cam_proj;
	glm::mat4 ProjectionMatrix;
	View_Port view_port;

	Camera(Camera_ID _camera_id) : camera_id(_camera_id) {}
	bool flag_valid;
	bool flag_move; 
	bool flag_rotate_axes; 
	bool flag_zoom; 
	bool flag_mouse_look;


	void set_ViewMatrix(void); 
	void move_forward(float delta);
	void move_right(float delta);
	void move_up(float delta);

	void rotate_u_axis(float angle); 
	void rotate_v_axis(float angle); 
	void rotate_n_axis(float angle); 
	void zoom_in_out(float delta_fovy); 

	glm::mat4 create_ViewMatrix_from_cam_view();
};

struct Perspective_Camera : public Camera {
	Perspective_Camera(Camera_ID _camera_id) : Camera(_camera_id) {}
	void define_camera(int win_width, int win_height, float win_aspect_ratio);
	void define_camera(Camera_ID id, glm::vec3 default_pos, glm::vec3 default_target, glm::vec3 default_up, float default_fovy, float default_aspect, float default_near, float default_far);
};

struct Orthographic_Camera : public Camera {
	Orthographic_Camera(Camera_ID _camera_id) : Camera(_camera_id) {}
	void define_camera(int win_width, int win_height, float win_aspect_ratio);
	void define_camera(Camera_ID id, glm::vec3 default_pos, glm::vec3 default_target, glm::vec3 default_up,
		float left, float right, float bottom, float top, float n, float f);
};

struct Camera_Data {
	Perspective_Camera cam_main { CAMERA_MAIN };
	Perspective_Camera cam_cc0 { CAMERA_CC0 };
	Perspective_Camera cam_cc1{ CAMERA_CC1 };
	Perspective_Camera cam_cc2{ CAMERA_CC2 };
	Perspective_Camera cam_dynamic{ CAMERA_DYNAMIC };
	Orthographic_Camera cam_view_front{ CAMERA_VIEW_FRONT };
	Orthographic_Camera cam_view_side_right{ CAMERA_VIEW_SIDE_RIGHT };
	Orthographic_Camera cam_view_top{ CAMERA_VIEW_TOP };
};