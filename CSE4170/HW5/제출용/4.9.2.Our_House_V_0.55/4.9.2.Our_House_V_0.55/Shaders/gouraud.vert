#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;

uniform mat4 u_ModelViewProjectionMatrix;
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat3 u_NormalMatrix;

// 광원 속성 (Light 구조체)
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
    float cutoff;
    float outer_cutoff;
    float constant;
    float linear;
    float quadratic;
};
uniform Light u_light;
uniform Light u_eye_light;
uniform Light u_object_light;

// 재질 속성
uniform vec4 u_material_emission;
uniform vec4 u_material_ambient;
uniform vec4 u_material_diffuse;
uniform vec4 u_material_specular;
uniform float u_material_exponent;

// 카메라 위치 (월드 공간)
uniform vec3 u_camera_pos_ws;

uniform int u_light_on;
uniform int u_eye_light_on;
uniform int u_object_light_on;

out vec2 fs_texcoord;
out vec4 fs_color;
out vec3 fs_position_es;
out vec3 fs_normal_es;

vec3 calculate_lighting_vert(vec3 light_pos, vec3 light_ambient, vec3 light_diffuse, vec3 light_specular,
                             float light_constant, float light_linear, float light_quadratic,
                             bool is_spot_light, vec3 spot_direction, float spot_cutoff, float spot_outer_cutoff,
                             vec3 vert_pos, vec3 vert_normal, vec3 camera_pos, float material_exponent, bool is_world_space) {
    
    vec3 N = normalize(vert_normal);
    vec3 L = normalize(light_pos - vert_pos);
    vec3 V = is_world_space ? normalize(camera_pos - vert_pos) : normalize(-vert_pos);
    vec3 R = reflect(-L, N);

    float diff_factor = max(dot(N, L), 0.0);
    float spec_factor = pow(max(dot(V, R), 0.0), material_exponent);
    
    vec3 ambient_comp = u_material_ambient.rgb * light_ambient;
    vec3 diffuse_comp = u_material_diffuse.rgb * light_diffuse * diff_factor;
    vec3 specular_comp = u_material_specular.rgb * light_specular * spec_factor;

    float distance = length(light_pos - vert_pos);
    float attenuation = 1.0 / (light_constant + light_linear * distance + light_quadratic * (distance * distance));

    vec3 attenuated_light = (diffuse_comp + specular_comp) * attenuation;

    // 스포트라이트 계산 로직 추가
    if (is_spot_light) {
        float theta = dot(normalize(vert_pos - light_pos), normalize(spot_direction));
        float intensity = smoothstep(spot_outer_cutoff, spot_cutoff, theta);
        attenuated_light *= intensity;
    }

    return ambient_comp + attenuated_light;
}

void main() {
    gl_Position = u_ModelViewProjectionMatrix * vec4(a_position, 1.0);

    vec3 position_ws = vec3(u_ModelMatrix * vec4(a_position, 1.0));
    vec3 normal_ws = normalize(u_NormalMatrix * a_normal);

    vec3 position_es = vec3(u_ViewMatrix * u_ModelMatrix * vec4(a_position, 1.0));
    vec3 normal_es = normalize(mat3(u_ViewMatrix) * u_NormalMatrix * a_normal);

    vec3 final_lighting_color = u_material_emission.rgb;

    if (u_light_on == 1) {
        final_lighting_color += calculate_lighting_vert(
            u_light.position, u_light.ambient, u_light.diffuse, u_light.specular,
            1.0, 0.0, 0.0, // 감쇠 없음
            false, vec3(0.0), 0.0, 0.0, // 스포트라이트 아님
            position_ws, normal_ws, u_camera_pos_ws, u_material_exponent, true
        );
    }

    if (u_eye_light_on == 1) {
        final_lighting_color += calculate_lighting_vert(
            u_eye_light.position, u_eye_light.ambient, u_eye_light.diffuse, u_eye_light.specular,
            1.0, 0.0, 0.0, // 감쇠 없음
            true, u_eye_light.direction, u_eye_light.cutoff, u_eye_light.outer_cutoff, // 스포트라이트임
            position_es, normal_es, vec3(0.0, 0.0, 0.0), u_material_exponent, false
        );
    }

    if (u_object_light_on == 1) {
        final_lighting_color += calculate_lighting_vert(
            u_object_light.position, u_object_light.ambient, u_object_light.diffuse, u_object_light.specular,
            u_object_light.constant, u_object_light.linear, u_object_light.quadratic,
            false, vec3(0.0), 0.0, 0.0, // 스포트라이트 아님
            position_es, normal_es, vec3(0.0, 0.0, 0.0), u_material_exponent, false
        );
    }
    fs_position_es = vec3(u_ViewMatrix * u_ModelMatrix * vec4(a_position, 1.0));
    fs_normal_es = normalize(mat3(u_ViewMatrix) * u_NormalMatrix * a_normal);
    fs_color = vec4(final_lighting_color, 1.0);
    fs_texcoord = a_texcoord;
}