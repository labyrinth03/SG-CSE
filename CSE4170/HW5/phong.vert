#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord; // 텍스처 좌표 어트리뷰트

uniform mat4 u_ModelViewProjectionMatrix;
uniform mat4 u_ModelMatrix; // 월드 공간 위치, 노멀 계산용
uniform mat4 u_ViewMatrix; // 추가: View Matrix
uniform mat3 u_NormalMatrix; // 노멀 변환용 (ModelMatrix의 역행렬 전치)

// 광원 속성 (Light 구조체) - 버텍스 쉐이더에서는 직접 사용하지 않으므로 제거 (프래그먼트 쉐이더로 직접 전달)
/*
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light u_light; // 월드 광원
uniform Light u_eye_light; // 눈 공간 광원 (위치는 이미 눈 공간)
*/

// 광원 및 카메라 위치 (월드 공간) - 버텍스 쉐이더에서 직접 사용되지 않으므로 제거
// uniform vec3 u_light_pos_ws;
// uniform vec3 u_camera_pos_ws;

// 추가: 광원 on/off 유니폼 - 버텍스 쉐이더에서 사용되지 않으므로 제거
// uniform int u_light_on;
// uniform int u_eye_light_on;

// 프래그먼트 쉐이더로 전달될 값들
out vec3 fs_normal_ws;        // 월드 공간 노멀
out vec3 fs_position_ws;      // 월드 공간 정점 위치
out vec3 fs_position_es;      // 추가: 눈 공간 정점 위치 (Phong Fragment에서 필요)
out vec3 fs_normal_es;        // 추가: 눈 공간 노멀 (Phong Fragment에서 필요)
out vec2 fs_texcoord;         // 텍스처 좌표

void main() {
    gl_Position = u_ModelViewProjectionMatrix * vec4(a_position, 1.0);

    // 월드 공간 변환
    fs_position_ws = vec3(u_ModelMatrix * vec4(a_position, 1.0));
    fs_normal_ws = normalize(u_NormalMatrix * a_normal);

    // 눈 공간 변환 (ViewMatrix 사용)
    fs_position_es = vec3(u_ViewMatrix * u_ModelMatrix * vec4(a_position, 1.0)); // ModelViewMatrix의 결과
    fs_normal_es = normalize(mat3(u_ViewMatrix) * u_NormalMatrix * a_normal); // ViewMatrix의 상위 3x3 부분을 노멀에 적용

    // 텍스처 좌표 전달
    fs_texcoord = a_texcoord;
}