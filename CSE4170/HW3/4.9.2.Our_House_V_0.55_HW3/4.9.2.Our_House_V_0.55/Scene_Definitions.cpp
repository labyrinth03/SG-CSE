#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"
#include <glm/gtc/type_ptr.hpp>

unsigned int static_object_ID_mapper[N_MAX_STATIC_OBJECTS];
unsigned int dynamic_object_ID_mapper[N_MAX_DYNAMIC_OBJECTS];
unsigned int camera_ID_mapper[N_MAX_CAMERAS];
unsigned int shader_ID_mapper[N_MAX_SHADERS];

void Axis_Object::define_axis() {
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_axes), &vertices_axes[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Axis_Object::draw_axis(Shader_Simple* shader_simple, glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix) {
#define WC_AXIS_LENGTH		60.0f
	glm::mat4 ModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(WC_AXIS_LENGTH, WC_AXIS_LENGTH, WC_AXIS_LENGTH));
	glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;

	glUseProgram(shader_simple->h_ShaderProgram);
	glUniformMatrix4fv(shader_simple->loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);


	glBindVertexArray(VAO);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
	glUseProgram(0);
}

#define WC_AXIS_LENGTH        60.0f
void Axis_Object::draw_axis(Shader_Simple* shader_simple, const glm::vec3& camera_pos,
	const glm::vec3& camera_uaxis, const glm::vec3& camera_vaxis, const glm::vec3& camera_naxis,
	glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, float scale) {
	glm::mat4 ModelMatrix = glm::mat4(
		glm::vec4(camera_uaxis, 0.0f),  
		glm::vec4(camera_vaxis, 0.0f),  
		glm::vec4(camera_naxis * -1.0f, 0.0f), 
		glm::vec4(camera_pos, 1.0f)              
	);

	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(WC_AXIS_LENGTH, WC_AXIS_LENGTH, WC_AXIS_LENGTH));


	glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;

	glUseProgram(shader_simple->h_ShaderProgram); 
	glUniformMatrix4fv(shader_simple->loc_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));

	glBindVertexArray(VAO);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);

	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);

	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
	glUseProgram(0); 
}


void Scene::clock(int clock_id) { // currently one clock
	time_stamp = ++time_stamp % UINT_MAX;
}

void Scene::build_static_world() {
	static_geometry_data.building.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.building);

	static_geometry_data.table.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.table);

	static_geometry_data.light.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.light);

	static_geometry_data.teapot.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.teapot);

	static_geometry_data.new_chair.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.new_chair);

	static_geometry_data.frame.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.frame);

	static_geometry_data.new_picture.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.new_picture);

	//static_geometry_data.cow.define_object();
	//static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	//static_objects.push_back(static_geometry_data.cow);

	static_geometry_data.tower.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.tower);

	static_geometry_data.ironman.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.ironman);

	static_geometry_data.optimus.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.optimus);

	static_geometry_data.godzilla.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.godzilla);

	static_geometry_data.tank.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.tank);
}

void Scene::build_dynamic_world() {								
	//dynamic_geometry_data.tiger_d.define_object();
	//dynamic_object_ID_mapper[DYNAMIC_OBJECT_TIGER] = dynamic_objects.size();
	//dynamic_objects.push_back(dynamic_geometry_data.tiger_d);

	//// two different moving cows
	//dynamic_geometry_data.cow_d_1.define_object();
	//dynamic_object_ID_mapper[DYNAMIC_OBJECT_COW_1] = dynamic_objects.size();
	//dynamic_objects.push_back(dynamic_geometry_data.cow_d_1);

	//dynamic_geometry_data.cow_d_2.define_object();
	//dynamic_object_ID_mapper[DYNAMIC_OBJECT_COW_2] = dynamic_objects.size();
	//dynamic_objects.push_back(dynamic_geometry_data.cow_d_2);

	dynamic_geometry_data.ben_d.define_object();
	dynamic_object_ID_mapper[DYNAMIC_OBJECT_BEN] = dynamic_objects.size();
	dynamic_objects.push_back(dynamic_geometry_data.ben_d);

	dynamic_geometry_data.wolf_d.define_object();
	dynamic_object_ID_mapper[DYNAMIC_OBJECT_WOLF] = dynamic_objects.size();
	dynamic_objects.push_back(dynamic_geometry_data.wolf_d);
}

void Scene::create_camera_list(int win_width, int win_height, float win_aspect_ratio) {
	camera_list.clear();
	// main camera
	camera_data.cam_main.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_MAIN] = camera_list.size();
	camera_list.push_back(camera_data.cam_main);


	// CCTV Camera 0
	Perspective_Camera cam_cc0(CAMERA_CC0);
	camera_data.cam_cc0.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_CC0] = camera_list.size();
	camera_list.push_back(camera_data.cam_cc0);

	// CCTV Camera 1
	Perspective_Camera cam_cc1(CAMERA_CC1);
	camera_data.cam_cc1.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_CC1] = camera_list.size();
	camera_list.push_back(camera_data.cam_cc1);

	// CCTV Camera 2
	Perspective_Camera cam_cc2(CAMERA_CC2);
	camera_data.cam_cc2.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_CC2] = camera_list.size();
	camera_list.push_back(camera_data.cam_cc2);

	camera_data.cam_dynamic.define_camera(
		CAMERA_DYNAMIC,
		glm::vec3(115.0f, 80.0f, 50.0f),  
		glm::vec3(115.0f, 120.0f, 0.0f),  
		glm::vec3(0.0f, 0.0f, 1.0f), 
		60.0f,                      
		win_aspect_ratio,
		0.1f, 1000.0f              
	);
	camera_ID_mapper[CAMERA_DYNAMIC] = camera_list.size();
	camera_list.push_back(std::ref(camera_data.cam_dynamic));

	camera_data.cam_view_front.define_camera(
		CAMERA_VIEW_FRONT,
		glm::vec3(120.0f, -150.0f, 25.0f), 
		glm::vec3(120.0f, 0.0f, 25.0f),   
		glm::vec3(0.0f, 0.0f, 1.0f),     
		-120.0f, 120.0f, -120.0f, 120.0f, 
		0.1f, 1000.0f                  
	);
	camera_ID_mapper[CAMERA_VIEW_FRONT] = camera_list.size();
	camera_list.push_back(camera_data.cam_view_front);


	camera_data.cam_view_side_right.define_camera(
		CAMERA_VIEW_SIDE_RIGHT,
		glm::vec3(250.0f, 80.0f, 25.0f),
		glm::vec3(0.0f, 80.0f, 25.0f),  
		glm::vec3(0.0f, 0.0f, 1.0f),  
		-80.0f, 80.0f, -80.0f, 80.0f,
		0.1f, 1000.0f
	);
	camera_ID_mapper[CAMERA_VIEW_SIDE_RIGHT] = camera_list.size();
	camera_list.push_back(camera_data.cam_view_side_right);

	camera_data.cam_view_top.define_camera(
		CAMERA_VIEW_TOP,
		glm::vec3(120.0f, 80.0f, 60.0f),
		glm::vec3(120.0f, 80.0f, 20.0f),
		glm::vec3(0.0f, 1.0f, 0.0f), 
		-120.0f, 120.0f, -120.0f, 120.0f, 
		0.1f, 1000.0f
	);
	camera_ID_mapper[CAMERA_VIEW_TOP] = camera_list.size();
	camera_list.push_back(camera_data.cam_view_top);






}

void Scene::build_shader_list() {
	shader_data.shader_simple.prepare_shader();
	shader_ID_mapper[SHADER_SIMPLE] = shader_list.size();
	shader_list.push_back(shader_data.shader_simple);
}

void Scene::initialize() {
	axis_object.define_axis();
	build_static_world();
	build_dynamic_world();
	create_camera_list(window.width, window.height, window.aspect_ratio);
	build_shader_list();
}

void Scene::draw_static_world() {
	glm::mat4 ModelViewProjectionMatrix;
	for (auto static_object = static_objects.begin(); static_object != static_objects.end(); static_object++) {
		if (static_object->get().flag_valid == false) continue;
		static_object->get().draw_object(ViewMatrix, ProjectionMatrix, shader_kind, shader_list);
	}
}

void Scene::draw_dynamic_world() {
	glm::mat4 ModelViewProjectionMatrix;
	for (auto dynamic_object = dynamic_objects.begin(); dynamic_object != dynamic_objects.end(); dynamic_object++) {
		if (dynamic_object->get().flag_valid == false) continue;
		dynamic_object->get().draw_object(ViewMatrix, ProjectionMatrix, shader_kind, shader_list, time_stamp);
	}
}

void Scene::draw_axis() {
	axis_object.draw_axis(static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[SHADER_SIMPLE]].get()),
		ViewMatrix, ProjectionMatrix);
}

void Scene::draw_world() {
	draw_axis();
	draw_static_world();
	draw_dynamic_world();
}

void Scene::draw_camera_axes_in_viewport(Camera& viewer_camera) {
	auto* shader_simple = static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[SHADER_SIMPLE]].get());

	for (int i = CAMERA_MAIN; i <= CAMERA_DYNAMIC; ++i) {
		if (camera_ID_mapper[i] >= camera_list.size()) continue;

		Camera& target_cam = camera_list[camera_ID_mapper[i]].get();
		if (!target_cam.flag_valid) continue;

		if (viewer_camera.camera_id == CAMERA_DYNAMIC && target_cam.camera_id == CAMERA_DYNAMIC)
			continue;

		axis_object.draw_axis(
			shader_simple,
			target_cam.cam_view.pos,
			target_cam.cam_view.uaxis,
			target_cam.cam_view.vaxis,
			target_cam.cam_view.naxis,
			viewer_camera.ViewMatrix,
			viewer_camera.ProjectionMatrix,
			10.0f
		);
	}
}
