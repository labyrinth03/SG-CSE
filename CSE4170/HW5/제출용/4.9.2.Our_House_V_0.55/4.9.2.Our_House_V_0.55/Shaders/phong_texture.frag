#version 400 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords; 

uniform vec3 u_light_pos_ws;
uniform vec3 u_camera_pos_ws;

uniform vec4 u_material_emission;
uniform vec4 u_material_ambient;
uniform vec4 u_material_diffuse;
uniform vec4 u_material_specular;
uniform float u_material_exponent;

uniform sampler2D u_texture_sampler; // 텍스처 샘플러 유니폼

out vec4 color;

void main() {
    // 텍스처 색상 가져오기
    vec4 tex_color = texture(u_texture_sampler, TexCoords);

    vec3 norm = normalize(Normal);
    vec3 light_dir = normalize(u_light_pos_ws - FragPos);
    vec3 view_dir = normalize(u_camera_pos_ws - FragPos);

    vec4 ambient_color = u_material_ambient * tex_color; // 텍스처 색상 적용
    float diff = max(dot(norm, light_dir), 0.0);
    vec4 diffuse_color = u_material_diffuse * diff * tex_color; // 텍스처 색상 적용

    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), u_material_exponent);
    vec4 specular_color = u_material_specular * spec;

    color = u_material_emission + ambient_color + diffuse_color + specular_color;
}