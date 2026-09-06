#version 330 core

// 버텍스 쉐이더에서 보간된 값들
in vec2 fs_texcoord; // 텍스처 좌표 (버텍스 쉐이더로부터)
in vec4 fs_color;    // 정점별로 계산되어 보간된 최종 색상

// 텍스처 관련 유니폼
uniform sampler2D u_texture_sampler; // 텍스처 샘플러
uniform int u_has_texture;           // 텍스처 사용 여부 (0: 없음, 1: 있음)

out vec4 FragColor; // 최종 출력 색상

void main() {
    vec4 final_pixel_color = fs_color; // 버텍스 쉐이더에서 계산된 보간된 색상을 기본으로 사용

    // 텍스처가 있다면 텍스처 색상을 샘플링하여 적용
    if (u_has_texture == 1) {
        vec4 tex_color = texture(u_texture_sampler, fs_texcoord);
        final_pixel_color.rgb *= tex_color.rgb; // 텍스처 색상을 기존 색상에 곱하여 적용
    }

    FragColor = final_pixel_color;
}