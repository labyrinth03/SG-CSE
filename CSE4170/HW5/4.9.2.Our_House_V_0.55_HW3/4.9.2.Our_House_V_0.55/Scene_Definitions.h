#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Shaders/LoadShaders.h"
#include <FreeImage/FreeImage.h>
#include "Camera.h"

#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))
#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

#define N_MAX_STATIC_OBJECTS		10
#define N_MAX_DYNAMIC_OBJECTS		10
#define N_MAX_CAMERAS		10
#define N_MAX_SHADERS		10

extern unsigned int static_object_ID_mapper[N_MAX_STATIC_OBJECTS];
extern unsigned int dynamic_object_ID_mapper[N_MAX_DYNAMIC_OBJECTS];
extern unsigned int camera_ID_mapper[N_MAX_CAMERAS];
extern unsigned int shader_ID_mapper[N_MAX_SHADERS];

struct Material {
	glm::vec4 emission, ambient, diffuse, specular;
	GLfloat exponent;
};

// 광원 정보를 담을 구조체 정의
struct Lighting {
	glm::vec3 position; // 월드 공간 위치
	glm::vec3 ambient;  // 주변광 색상/세기
	glm::vec3 diffuse;  // 확산광 색상/세기
	glm::vec3 specular; // 정반사광 색상/세기
	glm::vec3 direction; // 스폿 광원의 방향 (눈 공간에서만 사용)
	float cutoff;        // 내부 원뿔 각도의 코사인 값
	float outer_cutoff;  // 외부 원뿔 각도의 코사인 값
	bool light_on;
	float constant;
	float linear;
	float quadratic;
};

enum STATIC_OBJECT_ID {
	STATIC_OBJECT_TOWER = 0,
	STATIC_OBJECT_IRONMAN, STATIC_OBJECT_OPTIMUS, STATIC_OBJECT_GODZILLA,
	STATIC_OBJECT_TANK, STATIC_OBJECT_TABLE, STATIC_OBJECT_LIGHT,
	STATIC_OBJECT_TEAPOT, STATIC_OBJECT_NEW_CHAIR, 
	 STATIC_OBJECT_NEW_PICTURE, STATIC_OBJECT_FRAME, STATIC_OBJECT_COW, STATIC_OBJECT_BUILDING
};

enum DYNAMIC_OBJECT_ID {
	DYNAMIC_OBJECT_TIGER = 0, DYNAMIC_OBJECT_COW_1, DYNAMIC_OBJECT_COW_2, DYNAMIC_OBJECT_BEN,
	DYNAMIC_OBJECT_WOLF
};

enum SHADER_ID { SHADER_SIMPLE = 0, SHADER_GOURAUD, SHADER_PHONG };

struct Shader {
	ShaderInfo shader_info[3];
	GLuint h_ShaderProgram; // handle to shader program

	GLint loc_ModelViewProjectionMatrix;
	GLint loc_ModelMatrix; // 모델 행렬 (월드 공간 변환에 필요)
	GLint loc_ViewMatrix;
	GLint loc_ModelViewMatrix; // 필요한 경우
	GLint loc_NormalMatrix; // 노멀 변환용 행렬

	// 재질 속성
	GLint loc_material_emission;
	GLint loc_material_ambient;
	GLint loc_material_diffuse;
	GLint loc_material_specular;
	GLint loc_material_exponent;

	GLint loc_light_pos_ws;    // 월드 공간 광원 위치
	GLint loc_light_ambient;  // 추가
	GLint loc_light_diffuse;  // 추가
	GLint loc_light_specular; // 추가
	GLint loc_light_on;

	// 추가: 눈 좌표계 광원 관련 유니폼
	GLint loc_eye_light_pos_es;     // 눈 공간 광원 위치 (ES: Eye Space)
	GLint loc_eye_light_ambient;    // 추가
	GLint loc_eye_light_diffuse;    // 추가
	GLint loc_eye_light_specular;   // 추가
	GLint loc_eye_light_direction; // 새로 추가
	GLint loc_eye_light_cutoff;       // 새로 추가
	GLint loc_eye_light_outer_cutoff; // 새로 추가

	GLint loc_object_light_pos_es; // 늑대 고정 광원: 눈 공간 위치
	GLint loc_object_light_ambient;
	GLint loc_object_light_diffuse;
	GLint loc_object_light_specular;
	GLint loc_object_light_on; // 늑대 광원 활성화 여부


	GLint loc_eye_light_on;         // '4' 키로 제어되는 눈 공간 광원 on/off
	GLint loc_camera_pos_ws;   // 월드 공간 카메라 위치
	// 광원 ON/OFF 플래그 (세 광원 모두 여기에 통합 가능)
	GLint loc_light_enabled;         // 월드 광원 on/off
	GLint loc_light_color; // 추가: 광원 색상 유니폼 위치
	// 텍스처 관련 (Phong 쉐이더에서 텍스처를 사용한다면 필요)
	GLint loc_texture_sampler;
	GLint loc_has_texture; // 텍스처가 있는지 여부를 쉐이더에 알리는 플래그

	// 투명도 관련
	GLint loc_transparent_enabled;
	GLint loc_transparency_alpha; 

	GLint loc_eye_light_enabled;
	GLint loc_object_light_enabled;
	GLint loc_eye_light_pos_ws;
	GLint loc_object_light_pos_ms;

	GLint loc_object_light_constant;
	GLint loc_object_light_linear;
	GLint loc_object_light_quadratic;

	GLint loc_is_transparent; // 투명 모드 여부 전달용
	GLint loc_alpha;          // 알파 값 전달용

	GLint loc_scope_effect_on; // 007 스코프 효과 on/off 전달용

	GLint loc_viewport_info; // 뷰포트의 (x, y, 너비, 높이) 전달용
	GLint loc_red_velvet_on;  // 레드벨벳 필터 on/off 전달용

	Shader() {
		h_ShaderProgram = NULL;
		shader_info[0] = shader_info[1] = shader_info[2] = { NULL, NULL };
		loc_ModelViewProjectionMatrix = -1;
		loc_ModelMatrix = -1; // 모델 행렬 (월드 공간 변환에 필요)
		loc_ViewMatrix = -1;
		loc_ModelViewMatrix = -1; // 필요한 경우
		loc_NormalMatrix = -1; // 노멀 변환용 행렬

		// 재질 속성
		loc_material_emission = -1;
		loc_material_ambient = -1;
		loc_material_diffuse = -1;
		loc_material_specular = -1;
		loc_material_exponent = -1;

		loc_light_pos_ws = -1;    // 월드 공간 광원 위치
		loc_light_ambient = -1;  // 추가
		loc_light_diffuse = -1;  // 추가
		loc_light_specular = -1; // 추가
		loc_light_on = -1;

		// 추가: 눈 좌표계 광원 관련 유니폼
		loc_eye_light_pos_es = -1;     // 눈 공간 광원 위치 (ES: Eye Space)
		loc_eye_light_ambient = -1;    // 추가
		loc_eye_light_diffuse = -1;    // 추가
		loc_eye_light_specular = -1;   // 추가
		loc_eye_light_direction = -1; // 새로 추가
		loc_eye_light_cutoff = -1;       // 새로 추가
		loc_eye_light_outer_cutoff = -1; // 새로 추가
		loc_eye_light_on = -1;         // '4' 키로 제어되는 눈 공간 광원 on/off

		loc_object_light_pos_es = -1; // 늑대 고정 광원: 눈 공간 위치
		loc_object_light_ambient = -1;
		loc_object_light_diffuse = -1;
		loc_object_light_specular = -1;
		loc_object_light_on = -1; // 늑대 광원 활성화 여부

		loc_camera_pos_ws = -1;   // 월드 공간 카메라 위치

		// 광원 ON/OFF 플래그 (세 광원 모두 여기에 통합 가능)
		loc_light_enabled = -1;         // 월드 광원 on/off
		loc_light_color = -1; // 추가: 광원 색상 유니폼 위치

		// 텍스처 관련 (Phong 쉐이더에서 텍스처를 사용한다면 필요)
		loc_texture_sampler;
		loc_has_texture; // 텍스처가 있는지 여부를 쉐이더에 알리는 플래그

		// 투명도 관련
		loc_transparent_enabled = -1;
		loc_transparency_alpha = -1;

		loc_eye_light_enabled = -1;
		loc_object_light_enabled = -1;
		loc_eye_light_pos_ws = -1;
		loc_object_light_pos_ms = -1;

		loc_object_light_constant = -1;
		loc_object_light_linear = -1;
		loc_object_light_quadratic = -1;
		loc_is_transparent = -1;
		loc_alpha = -1;
		loc_scope_effect_on = -1;

		loc_viewport_info = -1;
		loc_red_velvet_on = -1;
	}
	virtual void prepare_shader() = 0;
};

struct Shader_Simple : Shader {
	GLint loc_primitive_color;
	void prepare_shader();
};

struct Shader_Gouraud : Shader {
	GLint loc_primitive_color;
	void prepare_shader();
	void set_lights(const Lighting& world_light, bool world_light_on,
		const Lighting& eye_light, bool eye_light_on, 
		const Lighting& object_light, bool object_light_on, glm::vec3 instance_light_pos_es,
		glm::vec3 camera_pos_ws);
};
struct Shader_Phong : Shader {
	GLint loc_primitive_color;
	void prepare_shader();
	void set_lights(const Lighting& world_light, bool world_light_on,
		const Lighting& eye_light, bool eye_light_on, 
		const Lighting& object_light, bool object_light_on, glm::vec3 instance_light_pos_es,
		glm::vec3 camera_pos_ws);
};

struct Shader_Data {
	Shader_Simple shader_simple;
	Shader_Gouraud shader_gouraud; // 추가
	Shader_Phong shader_phong;     // 추가
};



struct Instance {
	glm::mat4 ModelMatrix;
	Material material;
	GLuint texture_id; // 텍스처 ID 추가
	Lighting object_light;
	bool transparent_on; // '6' 키로 제어될 투명 모드 on/off
	float alpha;         // 불투명도 (0.0: 완전 투명, 1.0: 완전 불투명)
	Instance() : ModelMatrix(1.0f), texture_id(0), transparent_on(false), alpha(1.0f) {} // 기본값 0 (텍스처 없음)

};

struct Axis_Object {
	GLuint VBO, VAO;
	GLfloat vertices_axes[6][3] = {
		{ 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f }
	};
	GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } };

	void define_axis();
	void draw_axis(Shader_Simple* shader_simple, glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix);
	void draw_axis(Shader_Simple* shader_simple, const glm::vec3& camera_pos,
		const glm::vec3& camera_uaxis, const glm::vec3& camera_vaxis, const glm::vec3& camera_naxis,
		glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, float scale = 10.0f); // scale 인자 추가
};

struct Static_Object { // an object that does not move
	STATIC_OBJECT_ID object_id;
	char filename[512]; // where to read geometry data
	int n_fields; // 3/6/8 where3 floats for vertex, 3 floats for normal, and 2 floats for texcoord
	int n_triangles;
	GLfloat* vertices; // pointer to vertex array data
	GLuint VBO, VAO; // handles to vertex buffer object and vertex array object
	GLenum front_face_mode; // clockwise or counter-clockwise

	std::vector<Instance> instances;
	bool flag_valid;

	Static_Object() {}
	Static_Object(STATIC_OBJECT_ID _object_id) : object_id(_object_id) {
		instances.clear();
	}
	void read_geometry(int bytes_per_primitive);  
	void prepare_geom_of_static_object();
	//void draw_object(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
	//	std::vector<std::reference_wrapper<Shader>>& shader_list);
	// Static_Object.h 또는 Scene_Definitions.h의 관련 함수 선언 수정
	void draw_object(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
		std::vector<std::reference_wrapper<Shader>>& shader_list, glm::vec3 light_pos_ws, glm::vec3 camera_pos_ws); // light/camera pos 추가


};


struct Building : public Static_Object { 
	Building(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};

struct Table : public Static_Object { 
	Table(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};

struct Light : public Static_Object { 
	Light(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};
struct Teapot : public Static_Object { 
	Teapot(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};
struct New_Chair : public Static_Object { 
	New_Chair(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};

struct Frame : public Static_Object { 
	Frame(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};

struct New_Picture : public Static_Object { 
	New_Picture(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};
struct Cow : public Static_Object { 
	Cow(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};

struct Tower : public Static_Object {
	Tower(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object();
};

struct Ironman : public Static_Object {
	Ironman(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object();
};

struct Optimus : public Static_Object {
	Optimus(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object();
};

struct Godzilla : public Static_Object {
	Godzilla(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object();
};

struct Tank : public Static_Object {
	Tank(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object();
};

struct Static_Geometry_Data {
	Building building{ STATIC_OBJECT_BUILDING };
	Table table{ STATIC_OBJECT_TABLE };
	Light light{ STATIC_OBJECT_LIGHT };
	Teapot teapot{ STATIC_OBJECT_TEAPOT };
	New_Chair new_chair{ STATIC_OBJECT_NEW_CHAIR };
	Frame frame{ STATIC_OBJECT_FRAME };
	New_Picture new_picture{ STATIC_OBJECT_NEW_PICTURE };
	Cow cow{ STATIC_OBJECT_COW };
	Tower tower{ STATIC_OBJECT_TOWER };
	Ironman ironman{ STATIC_OBJECT_IRONMAN };
	Optimus optimus{ STATIC_OBJECT_OPTIMUS };
	Godzilla godzilla{ STATIC_OBJECT_GODZILLA };
	Tank tank{ STATIC_OBJECT_TANK };
};

struct Dynamic_Object { // an object that moves
	DYNAMIC_OBJECT_ID object_id;
	std::vector<Static_Object> object_frames;
	bool flag_valid;

	Dynamic_Object() {}
	Dynamic_Object(DYNAMIC_OBJECT_ID _object_id) : object_id(_object_id) {
		object_frames.clear();
	}

	glm::vec3 prev_direction = glm::vec3(0.0f, 1.0f, 0.0f);
	float prev_yaw = 0.0f; 
	float wolf_prev_yaw; // DYNAMIC_OBJECT_WOLF의 이전 yaw 값을 저장

	//std::vector<Instance> instances;

	void draw_object(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
		std::vector<std::reference_wrapper<Shader>>& shader_list, int time_stamp, glm::vec3 light_pos_ws, glm::vec3 camera_pos_ws); // light/camera pos 추가	
	void toggle_current_frame_light(); // 새로운 함수 선언
};

struct Tiger_D : public Dynamic_Object { 
	Tiger_D(DYNAMIC_OBJECT_ID _object_id) : Dynamic_Object(_object_id) {}
	void define_object(); 
};

struct Cow_D : public Dynamic_Object { 
	Cow_D(DYNAMIC_OBJECT_ID _object_id) : Dynamic_Object(_object_id) {}
	void define_object(); 
};

struct Ben_D : public Dynamic_Object {
	Ben_D(DYNAMIC_OBJECT_ID _object_id) : Dynamic_Object(_object_id) {}
	void define_object();
};

struct Wolf_D : public Dynamic_Object {
	Wolf_D(DYNAMIC_OBJECT_ID _object_id) : Dynamic_Object(_object_id) {}
	void define_object();
};

struct Dynamic_Geometry_Data {
	Tiger_D tiger_d{ DYNAMIC_OBJECT_TIGER };
	Cow_D cow_d_1{ DYNAMIC_OBJECT_COW_1 };
	Cow_D cow_d_2{ DYNAMIC_OBJECT_COW_2 };
	Ben_D ben_d{ DYNAMIC_OBJECT_BEN };
	Wolf_D wolf_d{ DYNAMIC_OBJECT_WOLF };
};

struct Window {
	int width, height;
	float aspect_ratio;
};

struct Scene {
	unsigned int time_stamp;
	Static_Geometry_Data static_geometry_data;
	std::vector<std::reference_wrapper<Static_Object>> static_objects;

	Dynamic_Geometry_Data dynamic_geometry_data;
	std::vector<std::reference_wrapper<Dynamic_Object>> dynamic_objects;

	Camera_Data camera_data;
	std::vector<std::reference_wrapper<Camera>> camera_list;

	Shader_Data shader_data;
	std::vector<std::reference_wrapper<Shader>> shader_list;
	SHADER_ID shader_kind;

	Window window; // for a better code, this must be defined in another structure!

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	Axis_Object axis_object;

	int current_cctv_camera_idx;
	bool show_cctv_viewport;


	bool  first_mouse_move = true; 
	float last_mouse_x;           
	float last_mouse_y;           
	float mouse_sensitivity = 0.1f; 
	float camera_pitch = 0.0f;   
	float camera_yaw = -90.0f;   

	GLuint ironman_texture_id;
	GLuint wolf_texture_id; // 늑대 텍스처도 있다면 함께 관리

	std::vector<GLuint> loaded_texture_ids; // 로드된 모든 텍스처 ID를 저장할 벡터

	SHADER_ID current_shading_mode; // 추가
	glm::vec3 world_light_pos; // 월드 공간 광원 위치 (일단 하나만 가정)

	
	glm::vec3 light_color; // 추가: 광원 색상 (세기 조절용)

	Lighting main_light;
	bool light_on; // 추가: 광원 on/off 상태

	Lighting eye_space_light; // 추가: 눈 좌표계 광원 (4번 키)
	bool eye_light_on;     // 추가: 눈 좌표계 광원 on/off
	glm::vec3 eye_light_world_pos;
	glm::vec3 eye_light_world_dir;

	glm::vec3 eye_light_eye_pos;
	glm::vec3 eye_light_eye_dir;

	glm::vec3 object_light_eye_pos;
	Lighting object_light;
	bool object_light_on;

	bool scope_effect_on; // '9' 키로 제어될 스코프 효과 플래그
	bool red_velvet_on;    // '0' 키로 제어될 레드벨벳 필터 플래그

	Scene() {
		time_stamp = 0;
		static_objects.clear();
		shader_list.clear();
		shader_kind = SHADER_SIMPLE;
		current_cctv_camera_idx = 0; 
		show_cctv_viewport = true; 
		ViewMatrix = ProjectionMatrix = glm::mat4(1.0f);
		ironman_texture_id = 0;
		wolf_texture_id = 0;
		loaded_texture_ids.clear(); // 생성자에서 벡터 초기화

		world_light_pos = glm::vec3(120.0f, 80.0f, 50.0f); // 예시: (0, 10, 0)에 광원 배치
		light_on = true; // 초기에는 광원 켜짐
		light_color = glm::vec3(1.0f, 1.0f, 1.0f); // 초기 광원 색상 (흰색, 최대 세기)
		main_light.position = glm::vec3(0.0f, 10.0f, 0.0f); // 월드 공간 위치
		main_light.ambient = glm::vec3(0.1f, 0.1f, 0.1f); // 약한 주변광
		main_light.diffuse = glm::vec3(0.6f, 0.6f, 0.6f); // 흰색 확산광
		main_light.specular = glm::vec3(0.7f, 0.7f, 0.7f); // 흰색 정반사광 (강하게)


		// 추가: 눈 좌표계 광원 초기 설정
		// 이 위치는 카메라에 상대적인 위치입니다.
		// 예를 들어, 카메라 오른쪽 위 (카메라의 로컬 X축 +1, Y축 +1)
		eye_space_light.position = glm::vec3(0.0f, 0.0f, 0.0f); // 카메라에서 약간 오른쪽 위, 앞으로 (뒤로 가면 안보임)
		eye_space_light.ambient = glm::vec3(0.05f, 0.05f, 0.05f); // 좀 더 약한 주변광
		eye_space_light.diffuse = glm::vec3(0.0f, 0.5f, 1.0f); // 푸른색 디퓨즈 (눈 좌표계 광원임을 시각적으로 구분)
		eye_space_light.specular = glm::vec3(0.5f, 0.5f, 1.0f); // 푸른색 스페큘러
		eye_space_light.direction = glm::vec3(0.0f, 0.0f, -1.0f);
		eye_space_light.cutoff = glm::cos(glm::radians(5.0f)); // 내부 원뿔 각도 (예: 12.5도)
		eye_space_light.outer_cutoff = glm::cos(glm::radians(7.5f)); // 외부 원뿔 각도 (예: 17.5도)
		eye_space_light.light_on = true; // 이 부분이 true여야 합니다!

		eye_light_world_pos = glm::vec3(0.0f);
		eye_light_world_dir = glm::vec3(0.0f, 0.0f, -1.0f);

		eye_light_eye_pos = glm::vec3(0.0f);
		eye_light_eye_dir = glm::vec3(0.0f, 0.0f, -1.0f);
		//eye_space_light.is_spot_light = true; // 스폿 광원임을 명시 (쉐이더에서 사용)

		eye_light_on = true; // 초기에는 꺼진 상태 (사용자가 켜도록)

		scope_effect_on = false; // 기본값은 스코프 효과 OFF
		red_velvet_on = false; // 기본값은 필터 OFF
	}

	void draw_camera_axes_in_viewport(Camera& viewer_camera);

	void clock(int clock_id);
	void build_static_world();
	void build_dynamic_world();
	void create_camera_list(int win_width, int win_height, float win_aspect_ratio);
	void build_shader_list();
	void initialize();
	void draw_static_world(const glm::vec3& viewer_pos_ws);
	void draw_dynamic_world(const glm::vec3& viewer_pos_ws);
	void draw_axis();
	void draw_world();
};

GLuint loadTexture(const char* filename);




	 
 