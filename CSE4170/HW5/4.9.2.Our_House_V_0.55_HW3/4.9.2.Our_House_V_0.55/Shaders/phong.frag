#version 330 core

// --- in, uniform, struct 선언 (이전과 동일) ---
in vec3 fs_position_ws;
in vec3 fs_normal_ws;
in vec3 fs_position_es;
in vec3 fs_normal_es;
in vec2 fs_texcoord;

struct Light {
    vec3 position; vec3 ambient; vec3 diffuse; vec3 specular;
    vec3 direction; float cutoff; float outer_cutoff;
    float constant; float linear; float quadratic;
};
uniform Light u_light, u_eye_light, u_object_light;

struct Material {
    vec4 emission; vec4 ambient; vec4 diffuse; vec4 specular;
    float exponent;
};
uniform Material u_material;

uniform vec3 u_camera_pos_ws;
uniform sampler2D u_texture_sampler;
uniform int u_has_texture;
uniform int u_light_on, u_eye_light_on, u_object_light_on;
uniform bool u_is_transparent;
uniform float u_alpha;
uniform bool u_scope_effect_on;
uniform bool u_red_velvet_on;
uniform vec4 u_viewport_info;

out vec4 FragColor;

// --- calculate_lighting 함수 (이전과 동일) ---
vec3 calculate_lighting(vec3 light_pos, vec3 light_ambient, vec3 light_diffuse, vec3 light_specular,
                        float light_constant, float light_linear, float light_quadratic,
                        vec3 frag_pos, vec3 frag_normal, vec3 camera_pos, bool is_world_space,
                        bool is_spot_light, vec3 spot_direction, float spot_cutoff, float spot_outer_cutoff) {
    vec3 N = normalize(frag_normal);
    vec3 L = normalize(light_pos - frag_pos);
    vec3 V = is_world_space ? normalize(camera_pos - frag_pos) : normalize(-frag_pos);
    vec3 R = reflect(-L, N);
    float diff_factor = max(dot(N, L), 0.0);
    float spec_factor = pow(max(dot(V, R), 0.0), u_material.exponent);
    vec3 ambient_comp = u_material.ambient.rgb * light_ambient;
    vec3 diffuse_comp = u_material.diffuse.rgb * light_diffuse * diff_factor;
    vec3 specular_comp = u_material.specular.rgb * light_specular * spec_factor;
    float distance = length(light_pos - frag_pos);
    float attenuation = 1.0 / (light_constant + light_linear * distance + light_quadratic * (distance * distance));
    vec3 attenuated_light = (diffuse_comp + specular_comp) * attenuation;
    vec3 final_light_color = ambient_comp + attenuated_light;
    if (is_spot_light) {
        float theta = dot(normalize(frag_pos - light_pos), normalize(spot_direction));
        float intensity = smoothstep(spot_outer_cutoff, spot_cutoff, theta);
        final_light_color = ambient_comp + attenuated_light * intensity;
    }
    return final_light_color;
}

void main() {
    // 1. 조명 계산
    vec3 total_lighting_color = u_material.emission.rgb;
    if (u_light_on == 1) {
        total_lighting_color += calculate_lighting(u_light.position, u_light.ambient, u_light.diffuse, u_light.specular, 1.0, 0.0, 0.0, fs_position_ws, fs_normal_ws, u_camera_pos_ws, true, false, vec3(0.0), 0.0, 0.0);
    }
    if (u_eye_light_on == 1) {
        total_lighting_color += calculate_lighting(u_eye_light.position, u_eye_light.ambient, u_eye_light.diffuse, u_eye_light.specular, 1.0, 0.0, 0.0, fs_position_es, fs_normal_es, vec3(0.0), false, true, u_eye_light.direction, u_eye_light.cutoff, u_eye_light.outer_cutoff);
    }
    if (u_object_light_on == 1) {
        total_lighting_color += calculate_lighting(u_object_light.position, u_object_light.ambient, u_object_light.diffuse, u_object_light.specular, u_object_light.constant, u_object_light.linear, u_object_light.quadratic, fs_position_es, fs_normal_es, vec3(0.0), false, false, vec3(0.0), 0.0, 0.0);
    }

    // 2. 임시 변수에 최종 색상 저장
    vec4 final_color = vec4(total_lighting_color, 1.0);

    // 3. 텍스처 적용
    if (u_has_texture == 1) {
        final_color.rgb *= texture(u_texture_sampler, fs_texcoord).rgb;
    }

    // 4. 레드벨벳 필터 적용
    if (u_red_velvet_on) {
        float luminance = dot(final_color.rgb, vec3(0.299, 0.587, 0.114));
        vec3 grayscale = vec3(luminance);
        float red_dominance = final_color.r - max(final_color.g, final_color.b);
        float mix_factor = smoothstep(0.1, 0.25, red_dominance);
        final_color.rgb = mix(grayscale, final_color.rgb, mix_factor);
    }

    // 5. 투명도 설정
    if (u_is_transparent) {
        final_color.a = u_alpha;
    }
    
    // 6. 최종 결정된 색상을 FragColor에 할당
    FragColor = final_color;

    // ▼▼▼▼▼▼▼▼▼▼ 6. 스코프 효과 로직 수정 ▼▼▼▼▼▼▼▼▼▼
    // 모든 계산이 끝난 최종 FragColor 위에 덧씌웁니다.
    if (u_scope_effect_on) {
        vec2 center = u_viewport_info.xy + u_viewport_info.zw * 0.5;
        float radius = min(u_viewport_info.z, u_viewport_info.w) * 0.4;
        
        float dist = distance(gl_FragCoord.xy, center);

        // 원의 바깥쪽이면 무조건 검은색으로 덮어씁니다.
        if (dist > radius) {
            discard;
        }
    }
}