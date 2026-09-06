#version 330 core

in vec3 fs_position_ws;      // 월드 공간 정점 위치
in vec3 fs_normal_ws;        // 월드 공간 노멀
in vec3 fs_position_es;      // 눈 공간 정점 위치
in vec3 fs_normal_es;        // 눈 공간 노멀
in vec2 fs_texcoord;

// 광원 속성 (Light 구조체)
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse; // 'diffuses' -> 'diffuse'로 수정
    vec3 specular;
    vec3 direction; // 스폿 광원의 방향 (눈 공간에서만 의미)
    float cutoff;   // 내부 원뿔 각도의 코사인 값
    float outer_cutoff; // 외부 원뿔 각도의 코사인 값
};

uniform Light u_light; // 월드 광원
uniform Light u_eye_light; // 눈 공간 광원
uniform Light u_object_light; // 오브젝트 광원 추가

// 재질 속성
struct Material {
    vec4 emission;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float exponent; // shininess
};
uniform Material u_material;

// 카메라 위치 (월드 공간)
uniform vec3 u_camera_pos_ws;

// 텍스처 관련 유니폼
uniform sampler2D u_texture_sampler;
uniform int u_has_texture;

// 광원 on/off 유니폼
uniform int u_light_on; // 월드 광원 on/off
uniform int u_eye_light_on; // 눈 공간 광원 on/off
uniform int u_object_light_on; // 오브젝트 광원 on/off 추가

out vec4 FragColor;

vec3 calculate_lighting(vec3 light_pos, vec3 light_ambient, vec3 light_diffuse, vec3 light_specular,
                         vec3 frag_pos, vec3 frag_normal, vec3 camera_pos, bool is_world_space,
                         bool is_spot_light, vec3 spot_direction, float spot_cutoff, float spot_outer_cutoff) {
    
    vec3 L; // Light direction (정점에서 광원 방향으로)
    vec3 V; // View direction (정점에서 카메라 방향으로)
    vec3 N = normalize(frag_normal); // 정규화된 법선 벡터

    if (is_world_space) { // 월드 공간 광원 계산
        L = normalize(light_pos - frag_pos);
        V = normalize(camera_pos - frag_pos);
    } else { // 눈 공간 광원 계산 (u_eye_light는 이미 눈 공간에 있으므로 카메라 위치는 (0,0,0)으로 간주
        L = normalize(light_pos - frag_pos);
        V = normalize(0.0 - frag_pos); // 정점에서 카메라(눈 공간 원점)를 향하는 벡터
    }
    
    // Ambient
    vec3 ambient_comp = u_material.ambient.rgb * light_ambient;

    // Diffuse
    float diff_factor = max(dot(N, L), 0.0); // N과 L이 이루는 각도
    vec3 diffuse_comp = u_material.diffuse.rgb * light_diffuse * diff_factor;

    // Specular
    vec3 R = reflect(-L, N); // 빛의 방향 L을 반사
    float spec_factor = pow(max(dot(V, R), 0.0), u_material.exponent);
    vec3 specular_comp = u_material.specular.rgb * light_specular * spec_factor;

    vec3 final_light_color = ambient_comp + diffuse_comp + specular_comp;

    // 스폿 광원 효과 적용 (눈 공간 광원에만 해당)
    if (is_spot_light) {
        // 스폿 광원의 방향 (눈 공간)
        vec3 spotDir = normalize(spot_direction); // u_eye_light.direction (0,0,-1)
        
        // 정점에서 광원으로 향하는 벡터의 반대 (빛이 들어오는 방향)
        // L은 이미 frag_pos - light_pos 로 계산되었으므로, 빛이 나가는 방향
        // 빛이 '들어오는' 방향은 light_pos - frag_pos 이므로, -L과 같음
        vec3 light_to_frag_vec = normalize(frag_pos - light_pos); // 광원 위치에서 프래그먼트를 향하는 벡터

        // 스폿 광원 방향과 프래그먼트 방향 벡터의 내적
        float theta = dot(light_to_frag_vec, spotDir); // spotDir은 (0,0,-1)로 설정될 예정

        // 원뿔 범위 내에 있는지 확인
        if (theta > spot_outer_cutoff) { // 외부 원뿔 범위보다 안쪽에 있다면 (내적이 클수록 각도가 작음)
            float intensity = 1.0;
            if (theta < spot_cutoff) { // 내부 원뿔과 외부 원뿔 사이
                intensity = smoothstep(spot_outer_cutoff, spot_cutoff, theta);
            }
            final_light_color *= intensity; // 빛 강도 조절
        } else { // 외부 원뿔 밖에 있다면 빛 없음
            final_light_color = vec3(0.0);
        }
    }

    return final_light_color;
}


void main() {
    vec3 final_lighting_color = u_material.emission.rgb; // Emission은 항상 적용

    // 월드 공간 광원 계산
    if (u_light_on == 1) {
        final_lighting_color += calculate_lighting(
            u_light.position, u_light.ambient, u_light.diffuse, u_light.specular,
            fs_position_ws, fs_normal_ws, u_camera_pos_ws, true,
                        false, vec3(0.0), 0.0, 0.0 // 스폿 광원 아님
        );
    }

    // 눈 공간 광원 계산
    if (u_eye_light_on == 1) {
        // 눈 공간 광원 계산 시, 정점 위치와 노멀도 눈 공간 좌표 (fs_position_es, fs_normal_es)를 사용
        final_lighting_color += calculate_lighting(
            u_eye_light.position, u_eye_light.ambient, u_eye_light.diffuse, u_eye_light.specular,
            fs_position_es, fs_normal_es, vec3(0.0, 0.0, 0.0), false,
            true, u_eye_light.direction, u_eye_light.cutoff, u_eye_light.outer_cutoff
        );
    }

    if (u_object_light_on == 1) {
    // 오브젝트 광원이 월드 공간에 있다고 가정하고 예시
    final_lighting_color += calculate_lighting(
        u_object_light.position, u_object_light.ambient, u_object_light.diffuse, u_object_light.specular,
        fs_position_ws, fs_normal_ws, u_camera_pos_ws, true, // 월드 공간 광원
        true, vec3(0.0), 0.0, 0.0 // 스폿 광원 아님
    );
    // 만약 오브젝트 광원이 눈 공간 광원이라면 (u_eye_light와 유사하게)
    /*
    final_lighting_color += calculate_lighting(
        u_object_light.position, u_object_light.ambient, u_object_light.diffuse, u_object_light.specular,
        fs_position_es, fs_normal_es, vec3(0.0, 0.0, 0.0), false, // 눈 공간 광원
        false, vec3(0.0), 0.0, 0.0 // 스폿 광원 아님 (스폿 광원이라면 direction, cutoff, outer_cutoff도 전달)
    );
    */
}

    vec4 final_output_color = vec4(final_lighting_color, 1.0);

    // 텍스처 적용
    if (u_has_texture == 1) {
        vec4 tex_color = texture(u_texture_sampler, fs_texcoord);
        final_output_color.rgb *= tex_color.rgb; // 텍스처 색상을 최종 조명 색상에 곱함
    }

    FragColor = final_output_color;
}