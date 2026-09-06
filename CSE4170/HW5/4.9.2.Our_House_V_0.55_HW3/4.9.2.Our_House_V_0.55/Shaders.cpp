#include "Scene_Definitions.h"

void Shader_Simple::prepare_shader() {
	shader_info[0] = { GL_VERTEX_SHADER, "Shaders/simple.vert" };
	shader_info[1] = { GL_FRAGMENT_SHADER, "Shaders/simple.frag" };
	shader_info[2] = { GL_NONE, NULL };

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
	glUseProgram(0);

}

void Shader_Gouraud::prepare_shader() {
    shader_info[0] = { GL_VERTEX_SHADER, "Shaders/gouraud.vert" };
    shader_info[1] = { GL_FRAGMENT_SHADER, "Shaders/gouraud.frag" };
    shader_info[2] = { GL_NONE, NULL };

    h_ShaderProgram = LoadShaders(shader_info);
    glUseProgram(h_ShaderProgram);

    loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
    loc_ModelMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelMatrix");
    loc_ViewMatrix = glGetUniformLocation(h_ShaderProgram, "u_ViewMatrix"); // ViewMatrix도 쉐이더로 넘길 경우
    loc_NormalMatrix = glGetUniformLocation(h_ShaderProgram, "u_NormalMatrix"); // 노멀 변환용 유니폼
    // Phong은 ViewMatrix를 직접 사용하지 않을 수 있으나, 카메라 위치를 위해 필요

    loc_light_pos_ws = glGetUniformLocation(h_ShaderProgram, "u_light.position"); // 이름 변경: u_light.position
    loc_light_ambient = glGetUniformLocation(h_ShaderProgram, "u_light.ambient");    // 추가
    loc_light_diffuse = glGetUniformLocation(h_ShaderProgram, "u_light.diffuse");    // 추가
    loc_light_specular = glGetUniformLocation(h_ShaderProgram, "u_light.specular");  // 추가
    loc_light_on = glGetUniformLocation(h_ShaderProgram, "u_light_on");

    loc_eye_light_pos_es = glGetUniformLocation(h_ShaderProgram, "u_eye_light.position"); // 눈 공간 광원 위치
    loc_eye_light_ambient = glGetUniformLocation(h_ShaderProgram, "u_eye_light.ambient");
    loc_eye_light_diffuse = glGetUniformLocation(h_ShaderProgram, "u_eye_light.diffuse");
    loc_eye_light_specular = glGetUniformLocation(h_ShaderProgram, "u_eye_light.specular");
    loc_eye_light_on = glGetUniformLocation(h_ShaderProgram, "u_eye_light_on");

    loc_object_light_pos_es = glGetUniformLocation(h_ShaderProgram, "u_object_light.position");
    loc_object_light_ambient = glGetUniformLocation(h_ShaderProgram, "u_object_light.ambient");
    loc_object_light_diffuse = glGetUniformLocation(h_ShaderProgram, "u_object_light.diffuse");
    loc_object_light_specular = glGetUniformLocation(h_ShaderProgram, "u_object_light.specular");
    loc_eye_light_direction = glGetUniformLocation(h_ShaderProgram, "u_eye_light.direction");
    loc_eye_light_cutoff = glGetUniformLocation(h_ShaderProgram, "u_eye_light.cutoff");
    loc_eye_light_outer_cutoff = glGetUniformLocation(h_ShaderProgram, "u_eye_light.outer_cutoff");

    loc_object_light_on = glGetUniformLocation(h_ShaderProgram, "u_object_light_on"); // 쉐이더의 uniform 이름과 일치
    loc_object_light_constant = glGetUniformLocation(h_ShaderProgram, "u_object_light.constant");
    loc_object_light_linear = glGetUniformLocation(h_ShaderProgram, "u_object_light.linear");
    loc_object_light_quadratic = glGetUniformLocation(h_ShaderProgram, "u_object_light.quadratic");

    loc_is_transparent = glGetUniformLocation(h_ShaderProgram, "u_is_transparent");
    loc_alpha = glGetUniformLocation(h_ShaderProgram, "u_alpha");

    loc_scope_effect_on = glGetUniformLocation(h_ShaderProgram, "u_scope_effect_on");
    loc_viewport_info = glGetUniformLocation(h_ShaderProgram, "u_viewport_info");
    loc_red_velvet_on = glGetUniformLocation(h_ShaderProgram, "u_red_velvet_on");


    loc_camera_pos_ws = glGetUniformLocation(h_ShaderProgram, "u_camera_pos_ws");

    loc_material_emission = glGetUniformLocation(h_ShaderProgram, "u_material_emission");
    loc_material_ambient = glGetUniformLocation(h_ShaderProgram, "u_material_ambient");
    loc_material_diffuse = glGetUniformLocation(h_ShaderProgram, "u_material_diffuse");
    loc_material_specular = glGetUniformLocation(h_ShaderProgram, "u_material_specular");
    loc_material_exponent = glGetUniformLocation(h_ShaderProgram, "u_material_exponent");

    loc_texture_sampler = glGetUniformLocation(h_ShaderProgram, "u_texture_sampler");
    loc_has_texture = glGetUniformLocation(h_ShaderProgram, "u_has_texture"); // 쉐이더의 has_texture uniform 이름 확인

    // 중요: 쉐이더 유니폼 위치가 유효한지 확인하는 디버깅 코드
#define CHECK_UNIFORM(loc_var) if (loc_var == -1) { fprintf(stderr, "[ERROR] Gouraud Shader: Uniform '%s' not found.\n", #loc_var); }
    CHECK_UNIFORM(loc_ModelViewProjectionMatrix);
    CHECK_UNIFORM(loc_ModelMatrix);
    CHECK_UNIFORM(loc_NormalMatrix);
    CHECK_UNIFORM(loc_ViewMatrix);
    CHECK_UNIFORM(loc_light_pos_ws);
    CHECK_UNIFORM(loc_light_ambient);
    CHECK_UNIFORM(loc_light_diffuse);
    CHECK_UNIFORM(loc_light_specular);
    CHECK_UNIFORM(loc_light_on);
    CHECK_UNIFORM(loc_eye_light_pos_es);
    CHECK_UNIFORM(loc_eye_light_ambient);
    CHECK_UNIFORM(loc_eye_light_diffuse);
    CHECK_UNIFORM(loc_eye_light_specular);
    CHECK_UNIFORM(loc_eye_light_on);
    CHECK_UNIFORM(loc_camera_pos_ws);
    CHECK_UNIFORM(loc_material_emission);
    CHECK_UNIFORM(loc_material_ambient);
    CHECK_UNIFORM(loc_material_diffuse);
    CHECK_UNIFORM(loc_material_specular);
    CHECK_UNIFORM(loc_material_exponent);
    CHECK_UNIFORM(loc_texture_sampler);
    CHECK_UNIFORM(loc_has_texture);
    CHECK_UNIFORM(loc_object_light_pos_es);
    CHECK_UNIFORM(loc_object_light_ambient);
    CHECK_UNIFORM(loc_object_light_diffuse);
    CHECK_UNIFORM(loc_object_light_specular);
    CHECK_UNIFORM(loc_object_light_on);
#undef CHECK_UNIFORM

    glUseProgram(0);
}

void Shader_Phong::prepare_shader() {
    shader_info[0] = { GL_VERTEX_SHADER, "Shaders/phong.vert" };
    shader_info[1] = { GL_FRAGMENT_SHADER, "Shaders/phong.frag" };
    shader_info[2] = { GL_NONE, NULL };

    h_ShaderProgram = LoadShaders(shader_info);
    glUseProgram(h_ShaderProgram);

    loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
    loc_ModelMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelMatrix");
    loc_ViewMatrix = glGetUniformLocation(h_ShaderProgram, "u_ViewMatrix"); // ViewMatrix도 쉐이더로 넘길 경우
    loc_NormalMatrix = glGetUniformLocation(h_ShaderProgram, "u_NormalMatrix"); // 노멀 변환용 유니폼
    // Phong은 ViewMatrix를 직접 사용하지 않을 수 있으나, 카메라 위치를 위해 필요


    loc_light_pos_ws = glGetUniformLocation(h_ShaderProgram, "u_light.position"); // 이름 변경: u_light.position
    loc_light_ambient = glGetUniformLocation(h_ShaderProgram, "u_light.ambient");    // 추가
    loc_light_diffuse = glGetUniformLocation(h_ShaderProgram, "u_light.diffuse");    // 추가
    loc_light_specular = glGetUniformLocation(h_ShaderProgram, "u_light.specular");  // 추가
    loc_light_on = glGetUniformLocation(h_ShaderProgram, "u_light_on");


    loc_eye_light_pos_es = glGetUniformLocation(h_ShaderProgram, "u_eye_light.position"); // 눈 공간 광원 위치
    loc_eye_light_ambient = glGetUniformLocation(h_ShaderProgram, "u_eye_light.ambient");
    loc_eye_light_diffuse = glGetUniformLocation(h_ShaderProgram, "u_eye_light.diffuse");
    loc_eye_light_specular = glGetUniformLocation(h_ShaderProgram, "u_eye_light.specular");
    loc_eye_light_direction = glGetUniformLocation(h_ShaderProgram, "u_eye_light.direction"); // 새로 추가
    loc_eye_light_cutoff = glGetUniformLocation(h_ShaderProgram, "u_eye_light.cutoff");       // 새로 추가
    loc_eye_light_outer_cutoff = glGetUniformLocation(h_ShaderProgram, "u_eye_light.outer_cutoff"); // 새로 추가
    loc_eye_light_on = glGetUniformLocation(h_ShaderProgram, "u_eye_light_on");

    loc_object_light_pos_es = glGetUniformLocation(h_ShaderProgram, "u_object_light.position");
    loc_object_light_ambient = glGetUniformLocation(h_ShaderProgram, "u_object_light.ambient");
    loc_object_light_diffuse = glGetUniformLocation(h_ShaderProgram, "u_object_light.diffuse");
    loc_object_light_specular = glGetUniformLocation(h_ShaderProgram, "u_object_light.specular");
    loc_object_light_on = glGetUniformLocation(h_ShaderProgram, "u_object_light_on"); // 쉐이더의 uniform 이름과 일치
    loc_object_light_constant = glGetUniformLocation(h_ShaderProgram, "u_object_light.constant");
    loc_object_light_linear = glGetUniformLocation(h_ShaderProgram, "u_object_light.linear");
    loc_object_light_quadratic = glGetUniformLocation(h_ShaderProgram, "u_object_light.quadratic");

    loc_is_transparent = glGetUniformLocation(h_ShaderProgram, "u_is_transparent");
    loc_alpha = glGetUniformLocation(h_ShaderProgram, "u_alpha");

    loc_scope_effect_on = glGetUniformLocation(h_ShaderProgram, "u_scope_effect_on");
    loc_viewport_info = glGetUniformLocation(h_ShaderProgram, "u_viewport_info");
    loc_red_velvet_on = glGetUniformLocation(h_ShaderProgram, "u_red_velvet_on");

    loc_camera_pos_ws = glGetUniformLocation(h_ShaderProgram, "u_camera_pos_ws");

    loc_material_emission = glGetUniformLocation(h_ShaderProgram, "u_material.emission");
    loc_material_ambient = glGetUniformLocation(h_ShaderProgram, "u_material.ambient");
    loc_material_diffuse = glGetUniformLocation(h_ShaderProgram, "u_material.diffuse");
    loc_material_specular = glGetUniformLocation(h_ShaderProgram, "u_material.specular");
    loc_material_exponent = glGetUniformLocation(h_ShaderProgram, "u_material.exponent");
    loc_texture_sampler = glGetUniformLocation(h_ShaderProgram, "u_texture_sampler");
    loc_has_texture = glGetUniformLocation(h_ShaderProgram, "u_has_texture"); // 쉐이더의 has_texture uniform 이름 확인

    // 중요: 쉐이더 유니폼 위치가 유효한지 확인하는 디버깅 코드
#define CHECK_UNIFORM(loc_var) if (loc_var == -1) { fprintf(stderr, "[ERROR] Phong Shader: Uniform '%s' not found.\n", #loc_var); }
    CHECK_UNIFORM(loc_ModelViewProjectionMatrix);
    CHECK_UNIFORM(loc_ModelMatrix);
    CHECK_UNIFORM(loc_NormalMatrix);
    CHECK_UNIFORM(loc_ViewMatrix);
    CHECK_UNIFORM(loc_light_pos_ws);
    CHECK_UNIFORM(loc_light_ambient);
    CHECK_UNIFORM(loc_light_diffuse);
    CHECK_UNIFORM(loc_light_specular);
    CHECK_UNIFORM(loc_light_on);
    CHECK_UNIFORM(loc_eye_light_pos_es);
    CHECK_UNIFORM(loc_eye_light_ambient);
    CHECK_UNIFORM(loc_eye_light_diffuse);
    CHECK_UNIFORM(loc_eye_light_specular);
    CHECK_UNIFORM(loc_eye_light_on);
    CHECK_UNIFORM(loc_camera_pos_ws);
    CHECK_UNIFORM(loc_material_emission);
    CHECK_UNIFORM(loc_material_ambient);
    CHECK_UNIFORM(loc_material_diffuse);
    CHECK_UNIFORM(loc_material_specular);
    CHECK_UNIFORM(loc_material_exponent);
    CHECK_UNIFORM(loc_texture_sampler);
    CHECK_UNIFORM(loc_has_texture);
    CHECK_UNIFORM(loc_object_light_pos_es);
    CHECK_UNIFORM(loc_object_light_ambient);
    CHECK_UNIFORM(loc_object_light_diffuse);
    CHECK_UNIFORM(loc_object_light_specular);
    CHECK_UNIFORM(loc_object_light_on);
#undef CHECK_UNIFORM

    glUseProgram(0);
}

void Shader_Phong::set_lights(const Lighting& world_light, bool world_light_on,
    const Lighting& eye_light, bool eye_light_on, const Lighting& object_light, bool object_light_on, glm::vec3 instance_light_pos_es,
    glm::vec3 camera_pos_ws) {

    // 월드 공간 광원 설정
    if (loc_light_pos_ws != -1) glUniform3fv(loc_light_pos_ws, 1, glm::value_ptr(world_light.position));
    if (loc_light_ambient != -1) glUniform3fv(loc_light_ambient, 1, glm::value_ptr(world_light.ambient));
    if (loc_light_diffuse != -1) glUniform3fv(loc_light_diffuse, 1, glm::value_ptr(world_light.diffuse));
    if (loc_light_specular != -1) glUniform3fv(loc_light_specular, 1, glm::value_ptr(world_light.specular));
    if (loc_light_on != -1) glUniform1i(loc_light_on, world_light_on ? 1 : 0);

    // 눈 공간 광원 (스폿 광원) 설정
   // if (loc_eye_light_pos_es != -1) glUniform3fv(loc_eye_light_pos_es, 1, glm::value_ptr(eye_light_pos_es));
    //if (loc_eye_light_direction != -1) glUniform3fv(loc_eye_light_direction, 1, glm::value_ptr(eye_light_dir_es));
    if (loc_eye_light_pos_es != -1) glUniform3fv(loc_eye_light_pos_es, 1, glm::value_ptr(eye_light.position));
    if (loc_eye_light_ambient != -1) glUniform3fv(loc_eye_light_ambient, 1, glm::value_ptr(eye_light.ambient));
    if (loc_eye_light_diffuse != -1) glUniform3fv(loc_eye_light_diffuse, 1, glm::value_ptr(eye_light.diffuse));
    if (loc_eye_light_specular != -1) glUniform3fv(loc_eye_light_specular, 1, glm::value_ptr(eye_light.specular));
    if (loc_eye_light_direction != -1) glUniform3fv(loc_eye_light_direction, 1, glm::value_ptr(eye_light.direction));
    if (loc_eye_light_cutoff != -1) glUniform1f(loc_eye_light_cutoff, eye_light.cutoff);
    if (loc_eye_light_outer_cutoff != -1) glUniform1f(loc_eye_light_outer_cutoff, eye_light.outer_cutoff);
    if (loc_eye_light_on != -1) glUniform1i(loc_eye_light_on, eye_light_on ? 1 : 0);

    glUniform3fv(loc_object_light_pos_es, 1, glm::value_ptr(instance_light_pos_es));
    glUniform3fv(loc_object_light_ambient, 1, glm::value_ptr(object_light.ambient));
    glUniform3fv(loc_object_light_diffuse, 1, glm::value_ptr(object_light.diffuse));
    glUniform3fv(loc_object_light_specular, 1, glm::value_ptr(object_light.specular));
    glUniform1i(loc_object_light_on, object_light_on ? 1 : 0);
    glUniform1f(loc_object_light_constant, object_light.constant);
    glUniform1f(loc_object_light_linear, object_light.linear);
    glUniform1f(loc_object_light_quadratic, object_light.quadratic);

    // 카메라 위치 (월드 공간)
    if (loc_camera_pos_ws != -1) glUniform3fv(loc_camera_pos_ws, 1, glm::value_ptr(camera_pos_ws));

}

void Shader_Gouraud::set_lights(const Lighting& world_light, bool world_light_on,
    const Lighting& eye_light, bool eye_light_on, const Lighting& object_light, bool object_light_on, glm::vec3 instance_light_pos_es,
    glm::vec3 camera_pos_ws) {

    // 월드 공간 광원 설정
    if (loc_light_pos_ws != -1) glUniform3fv(loc_light_pos_ws, 1, glm::value_ptr(world_light.position));
    if (loc_light_ambient != -1) glUniform3fv(loc_light_ambient, 1, glm::value_ptr(world_light.ambient));
    if (loc_light_diffuse != -1) glUniform3fv(loc_light_diffuse, 1, glm::value_ptr(world_light.diffuse));
    if (loc_light_specular != -1) glUniform3fv(loc_light_specular, 1, glm::value_ptr(world_light.specular));
    if (loc_light_on != -1) glUniform1i(loc_light_on, world_light_on ? 1 : 0);

    // 눈 공간 광원 (스폿 광원) 설정
    //if (loc_eye_light_pos_es != -1) glUniform3fv(loc_eye_light_pos_es, 1, glm::value_ptr(eye_light_pos_es));
    //if (loc_eye_light_direction != -1) glUniform3fv(loc_eye_light_direction, 1, glm::value_ptr(eye_light_dir_es));
    if (loc_eye_light_pos_es != -1) glUniform3fv(loc_eye_light_pos_es, 1, glm::value_ptr(eye_light.position));
    if (loc_eye_light_ambient != -1) glUniform3fv(loc_eye_light_ambient, 1, glm::value_ptr(eye_light.ambient));
    if (loc_eye_light_diffuse != -1) glUniform3fv(loc_eye_light_diffuse, 1, glm::value_ptr(eye_light.diffuse));
    if (loc_eye_light_specular != -1) glUniform3fv(loc_eye_light_specular, 1, glm::value_ptr(eye_light.specular));
    if (loc_eye_light_direction != -1) glUniform3fv(loc_eye_light_direction, 1, glm::value_ptr(eye_light.direction));
    if (loc_eye_light_cutoff != -1) glUniform1f(loc_eye_light_cutoff, eye_light.cutoff);
    if (loc_eye_light_outer_cutoff != -1) glUniform1f(loc_eye_light_outer_cutoff, eye_light.outer_cutoff);
    if (loc_eye_light_on != -1) glUniform1i(loc_eye_light_on, eye_light_on ? 1 : 0);

    glUniform3fv(loc_object_light_pos_es, 1, glm::value_ptr(instance_light_pos_es));
    glUniform3fv(loc_object_light_ambient, 1, glm::value_ptr(object_light.ambient));
    glUniform3fv(loc_object_light_diffuse, 1, glm::value_ptr(object_light.diffuse));
    glUniform3fv(loc_object_light_specular, 1, glm::value_ptr(object_light.specular));
    //glUniform1i(loc_object_light_on, object_light.light_on ? 1 : 0);
    glUniform1i(loc_object_light_on, object_light_on ? 1 : 0);     // 올바른 코드
    glUniform1f(loc_object_light_constant, object_light.constant);
    glUniform1f(loc_object_light_linear, object_light.linear);
    glUniform1f(loc_object_light_quadratic, object_light.quadratic);


    // 카메라 위치 (월드 공간)
    if (loc_camera_pos_ws != -1) glUniform3fv(loc_camera_pos_ws, 1, glm::value_ptr(camera_pos_ws));

}