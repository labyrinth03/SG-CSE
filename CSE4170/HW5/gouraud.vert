#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;

uniform mat4 u_ModelViewProjectionMatrix;
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix; // 추가: View Matrix
uniform mat3 u_NormalMatrix;

// 광원 속성 (Light 구조체)
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light u_light; // 월드 광원
uniform Light u_eye_light; // 눈 공간 광원
uniform Light u_object_light; // **추가: 오브젝트 광원**



// 재질 속성 (Gouraud는 개별 유니폼을 사용한다고 가정)
uniform vec4 u_material_emission;
uniform vec4 u_material_ambient;
uniform vec4 u_material_diffuse;
uniform vec4 u_material_specular;
uniform float u_material_exponent; // shininess

// 카메라 위치 (월드 공간)
uniform vec3 u_camera_pos_ws;

uniform int u_light_on; // 월드 광원 on/off
uniform int u_eye_light_on; // 눈 공간 광원 on/off
uniform int u_object_light_on; // **추가: 오브젝트 광원 on/off**

out vec2 fs_texcoord;
out vec4 fs_color; // 정점별로 계산된 최종 색상

vec3 calculate_lighting_vert(vec3 light_pos, vec3 light_ambient, vec3 light_diffuse, vec3 light_specular,
                             vec3 vert_pos, vec3 vert_normal, vec3 camera_pos, float material_exponent, bool is_world_space) {
    
    vec3 ambient_comp;
    vec3 diffuse_comp;
    vec3 specular_comp;

    vec3 L; // Light direction
    vec3 V; // View direction
    vec3 R; // Reflected light direction

    if (is_world_space) {
        L = normalize(light_pos - vert_pos);
        V = normalize(camera_pos - vert_pos);
    } else { // 눈 공간 (vert_pos는 눈 공간, 카메라 위치는 원점)
        L = normalize(light_pos - vert_pos);
        V = normalize(-vert_pos); // 카메라가 눈 공간의 원점에 있다고 가정
    }

    // Ambient
    ambient_comp = u_material_ambient.rgb * light_ambient;

    // Diffuse
    float diff_factor = max(dot(vert_normal, L), 0.0);
    diffuse_comp = u_material_diffuse.rgb * light_diffuse * diff_factor;

    // Specular
    R = reflect(-L, vert_normal);
    float spec_factor = pow(max(dot(V, R), 0.0), material_exponent);
    specular_comp = u_material_specular.rgb * light_specular * spec_factor;

    return ambient_comp + diffuse_comp + specular_comp;
}

void main() {
    gl_Position = u_ModelViewProjectionMatrix * vec4(a_position, 1.0);

    vec3 position_ws = vec3(u_ModelMatrix * vec4(a_position, 1.0));
    vec3 normal_ws = normalize(u_NormalMatrix * a_normal);

    // 눈 공간에서의 정점 위치와 노멀 계산 (calculate_lighting_vert 함수 호출 시 필요)
    vec3 position_es = vec3(u_ViewMatrix * u_ModelMatrix * vec4(a_position, 1.0));
    vec3 normal_es = normalize(mat3(u_ViewMatrix) * u_NormalMatrix * a_normal);

    vec3 final_lighting_color = u_material_emission.rgb;

    // 월드 공간 광원 계산
    if (u_light_on == 1) {
        final_lighting_color += calculate_lighting_vert(
            u_light.position, u_light.ambient, u_light.diffuse, u_light.specular, 
            position_ws, normal_ws, u_camera_pos_ws, u_material_exponent, true // 월드 공간 광원
        );
    }

    // 눈 공간 광원 계산
    if (u_eye_light_on == 1) {
        final_lighting_color += calculate_lighting_vert(
            u_eye_light.position, u_eye_light.ambient, u_eye_light.diffuse, u_eye_light.specular, 
            position_es, normal_es, vec3(0.0, 0.0, 0.0), u_material_exponent, false // 눈 공간 광원
        );
    }

    if (u_object_light_on == 1) {
        // 오브젝트 광원의 속성이 월드 공간인지 눈 공간인지에 따라 아래 코드 선택
        // 예시: 월드 공간 광원이라고 가정
        final_lighting_color += calculate_lighting_vert(
            u_object_light.position, u_object_light.ambient, u_object_light.diffuse, u_object_light.specular,
            position_ws, normal_ws, u_camera_pos_ws, u_material_exponent, true // 월드 공간 광원 (가정)
        );
        // 만약 오브젝트 광원이 눈 공간 광원이라면 (u_eye_light와 유사하게)
        /*
        final_lighting_color += calculate_lighting_vert(
            u_object_light.position, u_object_light.ambient, u_object_light.diffuse, u_object_light.specular,
            position_es, normal_es, vec3(0.0, 0.0, 0.0), u_material_exponent, false // 눈 공간 광원 (가정)
        );
        */
    }

    fs_color = vec4(final_lighting_color, 1.0);
    fs_texcoord = a_texcoord;
}