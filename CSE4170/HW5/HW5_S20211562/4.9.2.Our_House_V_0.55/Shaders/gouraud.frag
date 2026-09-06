#version 330 core

in vec2 fs_texcoord;
in vec4 fs_color;
in vec3 fs_position_es;
in vec3 fs_normal_es;

uniform sampler2D u_texture_sampler;
uniform int u_has_texture;
uniform bool u_is_transparent;
uniform float u_alpha;
uniform bool u_scope_effect_on;
uniform bool u_red_velvet_on;
uniform vec4 u_viewport_info;

out vec4 FragColor;

void main() {
    // 1. 임시 변수에 최종 색상 저장
    vec4 final_color = fs_color;

    // 2. 텍스처 적용
    if (u_has_texture == 1) {
        final_color.rgb *= texture(u_texture_sampler, fs_texcoord).rgb;
    }

    // 3. 레드벨벳 필터 적용
    if (u_red_velvet_on) {
        float luminance = dot(final_color.rgb, vec3(0.299, 0.587, 0.114));
        vec3 grayscale = vec3(luminance);
        float red_dominance = final_color.r - max(final_color.g, final_color.b);
        float mix_factor = smoothstep(0.1, 0.25, red_dominance);
        final_color.rgb = mix(grayscale, final_color.rgb, mix_factor);
    }

    // 4. 투명도 설정
    if (u_is_transparent) {
        final_color.a = u_alpha;
    }

    // 5. 최종 결정된 색상을 FragColor에 할당
    FragColor = final_color;

    // 6. 스코프 효과는 모든 계산이 끝난 최종 FragColor 위에 덧씌움
    if (u_scope_effect_on) {
        vec2 center = u_viewport_info.xy + u_viewport_info.zw * 0.5;
        float radius = min(u_viewport_info.z, u_viewport_info.w) * 0.4;

        float dist = distance(gl_FragCoord.xy, center);


        if (dist > radius) {
            discard;
        }
    }
}