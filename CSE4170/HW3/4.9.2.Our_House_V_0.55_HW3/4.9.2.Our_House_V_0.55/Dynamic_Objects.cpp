#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"
#include <glm/gtx/quaternion.hpp>

void Tiger_D::define_object() {
#define N_TIGER_FRAMES 12
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = true;

	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		object_frames.emplace_back();
		sprintf(object_frames[i].filename, "Data/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);
		object_frames[i].n_fields = 8;
		object_frames[i].front_face_mode = GL_CW;
		object_frames[i].prepare_geom_of_static_object();

		object_frames[i].instances.emplace_back();
		cur_MM = &(object_frames[i].instances.back().ModelMatrix);
		*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
		cur_material = &(object_frames[i].instances.back().material);
		cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
		cur_material->diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
		cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
		cur_material->exponent = 128.0f * 0.21794872f;
	}
}

void Ben_D::define_object() {
#define N_BEN_FRAMES 30
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = true;

	for (int i = 0; i < N_BEN_FRAMES; i++) {
		object_frames.emplace_back();
		sprintf(object_frames[i].filename, "Data/dynamic_objects/ben/ben_vntm_%d%d.geom", i / 10, i % 10);
		object_frames[i].n_fields = 8;
		object_frames[i].front_face_mode = GL_CW;
		object_frames[i].prepare_geom_of_static_object();

		object_frames[i].instances.emplace_back();
		cur_MM = &(object_frames[i].instances.back().ModelMatrix);
		*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
		cur_material = &(object_frames[i].instances.back().material);
		cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
		cur_material->diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
		cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
		cur_material->exponent = 128.0f * 0.21794872f;
	}
}

void Wolf_D::define_object() {
#define N_WOLF_FRAMES 16
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = true;

	for (int i = 0; i < N_WOLF_FRAMES; i++) {
		object_frames.emplace_back();
		sprintf(object_frames[i].filename, "Data/dynamic_objects/wolf/wolf_%d%d_vnt.geom", i / 10, i % 10);
		object_frames[i].n_fields = 8;
		object_frames[i].front_face_mode = GL_CW;
		object_frames[i].prepare_geom_of_static_object();

		object_frames[i].instances.emplace_back();
		cur_MM = &(object_frames[i].instances.back().ModelMatrix);
		*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
		cur_material = &(object_frames[i].instances.back().material);
		cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
		cur_material->diffuse = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
		cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
		cur_material->exponent = 128.0f * 0.21794872f;
	}
}

void Cow_D::define_object() {
#define N_FRAMES_COW_1 1
#define N_FRAMES_COW_2 1
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = true;
	switch (object_id) {

		int n_frames;
	case DYNAMIC_OBJECT_COW_1:
		n_frames = N_FRAMES_COW_1;
		for (int i = 0; i < n_frames; i++) {
			object_frames.emplace_back();
			strcpy(object_frames[i].filename, "Data/cow_vn.geom");
			object_frames[i].n_fields = 6;
			object_frames[i].front_face_mode = GL_CCW;
			object_frames[i].prepare_geom_of_static_object();
			object_frames[i].instances.emplace_back();
			cur_MM = &(object_frames[i].instances.back().ModelMatrix);
			*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(30.0f, 30.0f, 30.0f));
			cur_material = &(object_frames[i].instances.back().material);
			cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
			cur_material->diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
			cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
			cur_material->exponent = 128.0f * 0.21794872f;
		}
		break;
	case DYNAMIC_OBJECT_COW_2:
		n_frames = N_FRAMES_COW_2;
		for (int i = 0; i < n_frames; i++) {
			object_frames.emplace_back();
			strcpy(object_frames[i].filename, "Data/cow_vn.geom");
			object_frames[i].n_fields = 6;
			object_frames[i].front_face_mode = GL_CCW;
			object_frames[i].prepare_geom_of_static_object();

			object_frames[i].instances.emplace_back();
			cur_MM = &(object_frames[i].instances.back().ModelMatrix);
			*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(30.0f, 30.0f, 30.0f));
			cur_material = &(object_frames[i].instances.back().material);
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			cur_material->ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
			cur_material->diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
			cur_material->specular = glm::vec4(0.774597f, 0.774597f, 0.774597f, 1.0f);
			cur_material->exponent = 128.0f * 0.6f;
		}
		break;
	}
}

void Dynamic_Object::draw_object(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
	std::vector<std::reference_wrapper<Shader>>& shader_list, int time_stamp) {
	int cur_object_index = time_stamp % object_frames.size();
	Static_Object& cur_object = object_frames[cur_object_index];
	glFrontFace(cur_object.front_face_mode);

	float rotation_angle = 0.0f;
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	switch (object_id) {
	//case DYNAMIC_OBJECT_TIGER:
	//	rotation_angle = (time_stamp % 360) * TO_RADIAN;
	//	ModelMatrix = glm::rotate(ModelMatrix, -rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	//	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(100.0f, 0.0f, 0.0f));
	//	break;
	//case DYNAMIC_OBJECT_COW_1:
	//	rotation_angle = (2 * time_stamp % 360) * TO_RADIAN;
	//	ModelMatrix = glm::rotate(ModelMatrix, -rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f));
	//	break;
	//case DYNAMIC_OBJECT_COW_2:
	//	rotation_angle = (5 * time_stamp % 360) * TO_RADIAN;
	//	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(100.0f, 50.0f, 0.0f));
	//	ModelMatrix = glm::rotate(ModelMatrix, rotation_angle, glm::vec3(1.0f, 0.0f, 0.0f));
	//	break;
	case DYNAMIC_OBJECT_BEN: {
		glm::vec3 forward_points[] = {
			glm::vec3(30.0f, 80.0f, 0.0f),   
			glm::vec3(80.0f, 80.0f, 0.0f),
			glm::vec3(80.0f, 105.0f, 0.0f),   
			glm::vec3(170.0f, 105.0f, 0.0f),  
			glm::vec3(170.0f, 145.0f, 0.0f)  
		};
		const int num_forward_segments = 4; 

		std::vector<glm::vec3> full_path_points;
		for (int i = 0; i <= num_forward_segments; ++i) {
			full_path_points.push_back(forward_points[i]);
		}
		for (int i = num_forward_segments - 1; i >= 0; --i) { // P4 다음에 P3, P2, P1, P0 순으로 역순 추가
			full_path_points.push_back(forward_points[i]);
		}
		const int num_full_segments = full_path_points.size() - 1; 

		
		std::vector<float> segment_lengths(num_full_segments);		
		float total_path_length = 0.0f;
		for (int i = 0; i < num_full_segments; ++i) {
			segment_lengths[i] = glm::length(full_path_points[i + 1] - full_path_points[i]);
			total_path_length += segment_lengths[i];
		}

		const float move_speed = 1.0f; // 이동 속도 
		const int total_frames_for_path = (int)(total_path_length / move_speed);
		int current_time_in_path = time_stamp % total_frames_for_path;
		if (current_time_in_path < 0) current_time_in_path += total_frames_for_path;
		float current_distance = (float)current_time_in_path * move_speed;
		if (current_distance > total_path_length - move_speed / 2.0f && current_distance <= total_path_length) {
			current_distance = total_path_length;
		}

		glm::vec3 current_pos;
		float target_yaw = 0.0f; 
		float accumulated_length = 0.0f;
		int current_segment_idx = -1;

		for (int i = 0; i < num_full_segments; ++i) {
			if (current_distance < accumulated_length + segment_lengths[i]) {
				current_segment_idx = i;
				break;
			}
			accumulated_length += segment_lengths[i];
		}

		if (current_segment_idx == -1) {
			current_segment_idx = num_full_segments - 1;
		}

		float segment_t;
		if (segment_lengths[current_segment_idx] > 0.001f) {
			segment_t = (current_distance - accumulated_length) / segment_lengths[current_segment_idx];
		}
		else {
			segment_t = 0.0f; 
		}
		segment_t = glm::clamp(segment_t, 0.0f, 1.0f);

		const float rotation_smoothing_factor = 0.1f; 
		

		if (time_stamp == 0 || (current_time_in_path == 0 && time_stamp > 0)) {
			current_pos = full_path_points[0];
			glm::vec3 initial_direction = glm::normalize(full_path_points[1] - full_path_points[0]);
			prev_yaw = atan2(initial_direction.y, initial_direction.x);
		}
		else {
			current_pos = glm::mix(full_path_points[current_segment_idx], full_path_points[current_segment_idx + 1], segment_t);
		}

		glm::vec3 direction;
		if (current_segment_idx == num_full_segments - 1 && segment_t >= 1.0f - 0.001f) { 
			direction = glm::normalize(full_path_points[1] - full_path_points[0]); 
		}
		else {
			direction = glm::normalize(full_path_points[current_segment_idx + 1] - full_path_points[current_segment_idx]);
		}
		target_yaw = atan2(direction.y, direction.x); 

		float angle_diff = target_yaw - prev_yaw;
		if (angle_diff > 3.14f) angle_diff -= 2 * 3.14f;
		if (angle_diff < -3.14f) angle_diff += 2 * 3.14f;

		float current_yaw = prev_yaw + angle_diff * rotation_smoothing_factor;
		ModelMatrix = glm::translate(glm::mat4(1.0f), current_pos);
		ModelMatrix = glm::rotate(ModelMatrix, current_yaw, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 ben_base_align = glm::rotate(glm::mat4(1.0f), 90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ben_base_align = glm::rotate(ben_base_align, 180.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f)); 
		ben_base_align = glm::rotate(ben_base_align, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f)); 

		ModelMatrix = ModelMatrix * ben_base_align;
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(100.0f, 100.0f, 100.0f)); 

		prev_yaw = current_yaw;

		break;
	}
	case DYNAMIC_OBJECT_WOLF: {
		glm::vec3 wolf_points[] = {
			glm::vec3(210.0f, 25.0f, 0.0f),   
			glm::vec3(180.0f, 25.0f, 0.0f),   
			glm::vec3(180.0f, 70.0f, 0.0f),   
			glm::vec3(218.0f, 70.0f, 0.0f),   
			glm::vec3(218.0f, 145.0f, 0.0f),   
		};
		const int num_wolf_segments_in_loop = 4;

		std::vector<glm::vec3> wolf_full_path_points;
		for (int i = 0; i <= num_wolf_segments_in_loop; ++i) {
			wolf_full_path_points.push_back(wolf_points[i]);
		}
		for (int i = num_wolf_segments_in_loop - 1; i >= 0; --i) { 
			wolf_full_path_points.push_back(wolf_points[i]);
		}
		const int num_wolf_full_segments = wolf_full_path_points.size() - 1;

		std::vector<float> wolf_segment_lengths(num_wolf_full_segments);
		float wolf_total_path_length = 0.0f;
		for (int i = 0; i < num_wolf_full_segments; ++i) {
			wolf_segment_lengths[i] = glm::length(wolf_full_path_points[i + 1] - wolf_full_path_points[i]);
			wolf_total_path_length += wolf_segment_lengths[i];
		}

		const float wolf_move_speed = 2.0f; 

		const int wolf_total_frames_for_path = (int)(wolf_total_path_length / wolf_move_speed);

		int wolf_current_time_in_path = time_stamp % wolf_total_frames_for_path;
		if (wolf_current_time_in_path < 0) wolf_current_time_in_path += wolf_total_frames_for_path;

		float wolf_current_distance = (float)wolf_current_time_in_path * wolf_move_speed;

		if (wolf_current_distance > wolf_total_path_length - wolf_move_speed / 2.0f && wolf_current_distance <= wolf_total_path_length + 0.001f) {
			wolf_current_distance = wolf_total_path_length;
		}

		glm::vec3 wolf_current_pos;
		float wolf_target_yaw = 0.0f;
		float wolf_accumulated_length = 0.0f;
		int wolf_current_segment_idx = -1;

		for (int i = 0; i < num_wolf_full_segments; ++i) {
			if (wolf_current_distance < wolf_accumulated_length + wolf_segment_lengths[i]) {
				wolf_current_segment_idx = i;
				break;
			}
			wolf_accumulated_length += wolf_segment_lengths[i];
		}

		if (wolf_current_segment_idx == -1) {
			wolf_current_segment_idx = num_wolf_full_segments - 1;
		}

		float wolf_segment_t;
		if (wolf_segment_lengths[wolf_current_segment_idx] > 0.001f) {
			wolf_segment_t = (wolf_current_distance - wolf_accumulated_length) / wolf_segment_lengths[wolf_current_segment_idx];
		}
		else {
			wolf_segment_t = 0.0f;
		}
		wolf_segment_t = glm::clamp(wolf_segment_t, 0.0f, 1.0f);

		const float wolf_rotation_smoothing_factor = 0.1f; 

		static float wolf_prev_yaw = 0.0f; 

		if (time_stamp == 0 || (wolf_current_time_in_path == 0 && time_stamp > 0)) {
			wolf_current_pos = wolf_full_path_points[0]; 
			glm::vec3 initial_direction = glm::normalize(wolf_full_path_points[1] - wolf_full_path_points[0]);
			wolf_prev_yaw = atan2(initial_direction.y, initial_direction.x); 
		}
		else {
			wolf_current_pos = glm::mix(wolf_full_path_points[wolf_current_segment_idx], wolf_full_path_points[wolf_current_segment_idx + 1], wolf_segment_t);
		}

		glm::vec3 wolf_direction;
		if (wolf_current_segment_idx == num_wolf_full_segments - 1 && wolf_segment_t >= 1.0f - 0.001f) {
			wolf_direction = glm::normalize(wolf_full_path_points[1] - wolf_full_path_points[0]); 
		}
		else {
			wolf_direction = glm::normalize(wolf_full_path_points[wolf_current_segment_idx + 1] - wolf_full_path_points[wolf_current_segment_idx]);
		}
		wolf_target_yaw = atan2(wolf_direction.y, wolf_direction.x);

		float wolf_angle_diff = wolf_target_yaw - wolf_prev_yaw;
		if (wolf_angle_diff > 3.14f) wolf_angle_diff -= 2 * 3.14f;
		if (wolf_angle_diff < -3.14f) wolf_angle_diff += 2 * 3.14f;

		float wolf_current_yaw = wolf_prev_yaw + wolf_angle_diff * wolf_rotation_smoothing_factor;

		ModelMatrix = glm::translate(glm::mat4(1.0f), wolf_current_pos); 
		ModelMatrix = glm::rotate(ModelMatrix, wolf_current_yaw, glm::vec3(0.0f, 0.0f, 1.0f)); 


		glm::mat4 wolf_base_align = glm::rotate(glm::mat4(1.0f), 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		wolf_base_align = glm::rotate(wolf_base_align, 90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		wolf_base_align = glm::scale(wolf_base_align, glm::vec3(50.0f, 50.0f, 50.0f));


		ModelMatrix = ModelMatrix * wolf_base_align;

		wolf_prev_yaw = wolf_current_yaw; 

		break;
		}
	}

	for (int i = 0; i < cur_object.instances.size(); i++) {
		glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix * cur_object.instances[i].ModelMatrix;
		switch (shader_kind) {
		case SHADER_SIMPLE:
			Shader_Simple* shader_simple_ptr = static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[shader_kind]].get());
			glUseProgram(shader_simple_ptr->h_ShaderProgram);
			glUniformMatrix4fv(shader_simple_ptr->loc_ModelViewProjectionMatrix, 1, GL_FALSE,
				&ModelViewProjectionMatrix[0][0]);
			glUniform3f(shader_simple_ptr->loc_primitive_color, cur_object.instances[i].material.diffuse.r,
				cur_object.instances[i].material.diffuse.g, cur_object.instances[i].material.diffuse.b);
			break;
		}
		glBindVertexArray(cur_object.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * cur_object.n_triangles);
		glBindVertexArray(0);
		glUseProgram(0);
	}
}