#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;

uniform mat4 u_ModelViewProjectionMatrix;
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat3 u_NormalMatrix; // C++에서 계산된 (모델->눈 공간) 변환 행렬

// 프래그먼트 쉐이더로 전달될 값들
out vec3 fs_normal_ws;     // 월드 공간 노멀
out vec3 fs_position_ws;   // 월드 공간 정점 위치
out vec3 fs_position_es;   // 눈 공간 정점 위치
out vec3 fs_normal_es;     // 눈 공간 노멀
out vec2 fs_texcoord;      // 텍스처 좌표

void main() {
    // 최종 클립 공간 위치 계산
    gl_Position = u_ModelViewProjectionMatrix * vec4(a_position, 1.0);

    // 1. 월드 공간으로 위치와 노멀 변환
    fs_position_ws = vec3(u_ModelMatrix * vec4(a_position, 1.0));
    fs_normal_ws = normalize(mat3(transpose(inverse(u_ModelMatrix))) * a_normal);

    // 2. 눈 공간으로 위치와 노멀 변환
    fs_position_es = vec3(u_ViewMatrix * vec4(fs_position_ws, 1.0));
    fs_normal_es = normalize(u_NormalMatrix * a_normal); // C++에서 넘어온 행렬을 직접 사용

    // 3. 텍스처 좌표 전달
    fs_texcoord = a_texcoord;
}