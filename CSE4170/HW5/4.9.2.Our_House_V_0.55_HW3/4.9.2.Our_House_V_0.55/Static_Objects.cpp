#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"

extern Scene scene;
// 텍스처 로드 함수 (재사용을 위해 외부 또는 Static_Object::load_texture로 만들 수도 있음)
GLuint loadTexture(const char* filename) {
	fprintf(stdout, "[loadTexture] Attempting to load texture from: %s\n", filename);

	FREE_IMAGE_FORMAT tx_file_format;
	FIBITMAP* tx_pixmap = NULL;
	FIBITMAP* tx_pixmap_32 = NULL;

	// 1. 파일 형식 감지
	tx_file_format = FreeImage_GetFileType(filename, 0);
	if (tx_file_format == FIF_UNKNOWN) {
		// 확장자를 기반으로 다시 시도
		tx_file_format = FreeImage_GetFIFFromFilename(filename);
	}

	if (tx_file_format == FIF_UNKNOWN) {
		fprintf(stderr, "[loadTexture] Error: Unknown image format or file not found for %s\n", filename);
		return 0; // 텍스처 로드 실패
	}

	// 2. 이미지 로드
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	if (!tx_pixmap) {
		fprintf(stderr, "[loadTexture] Error: Could not load image %s. Check file path and existence.\n", filename);
		return 0; // 텍스처 로드 실패
	}
	fprintf(stdout, "[loadTexture] Image '%s' loaded successfully. Original BPP: %d\n", filename, FreeImage_GetBPP(tx_pixmap));

	// 3. 이미지 수직 뒤집기 (OpenGL 좌표계에 맞춤)
	FreeImage_FlipVertical(tx_pixmap);

	// 4. 32비트 (RGBA)로 변환
	if (FreeImage_GetBPP(tx_pixmap) == 32) {
		tx_pixmap_32 = tx_pixmap; // 이미 32비트이면 원본 사용
		fprintf(stdout, "[loadTexture] Image is already 32-bit.\n");
	}
	else {
		fprintf(stdout, "[loadTexture] Converting image from %d bits to 32 bits...\n", FreeImage_GetBPP(tx_pixmap));
		tx_pixmap_32 = FreeImage_ConvertTo32Bits(tx_pixmap);
		if (!tx_pixmap_32) {
			fprintf(stderr, "[loadTexture] Error: Could not convert image to 32-bit: %s\n", filename);
			FreeImage_Unload(tx_pixmap); // 원본 tx_pixmap 해제
			return 0; // 텍스처 로드 실패
		}
	}

	// 5. 텍스처 정보 추출
	int width = FreeImage_GetWidth(tx_pixmap_32);
	int height = FreeImage_GetHeight(tx_pixmap_32);
	GLvoid* data = FreeImage_GetBits(tx_pixmap_32);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// 6. OpenGL에 텍스처 데이터 전송 (GL_BGRA 사용)
	// FreeImage_ConvertTo32Bits는 일반적으로 B, G, R, A 순서로 바이트를 반환합니다.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	fprintf(stdout, "[loadTexture] Loaded %dx%d RGBA texture into graphics memory for '%s'.\n", width, height, filename);

	// 7. 텍스처 필터링 및 미니맵 설정
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D); // 미니맵 생성

	if (textureID > 0) {
		scene.loaded_texture_ids.push_back(textureID);
	}

	// 8. FreeImage 비트맵 객체 해제
	// tx_pixmap_32가 tx_pixmap과 다르면 tx_pixmap_32만 언로드,
	// 같으면 (원본이 32비트였으면) tx_pixmap_32도 언로드하면 tx_pixmap도 같이 해제됨.
	// FreeImage_ConvertTo32Bits가 원본을 복사해서 새 포인터를 반환하는 것이 일반적이므로 이 분기 처리가 안전함.
	if (tx_pixmap_32 != tx_pixmap) { // 새로운 32비트 맵이 생성되었다면
		FreeImage_Unload(tx_pixmap);     // 원본 맵 해제
	}
	FreeImage_Unload(tx_pixmap_32);  // 32비트 맵 해제

	glBindTexture(GL_TEXTURE_2D, 0); // 텍스처 언바인딩

	fprintf(stdout, "[loadTexture] Texture ID %d created for '%s'.\n\n", textureID, filename);
	return textureID;
}

void Static_Object::read_geometry(int bytes_per_primitive) {
	FILE* fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error: cannot open the object file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(&n_triangles, sizeof(int), 1, fp);
	vertices = (float*)malloc(n_triangles * bytes_per_primitive);
	if (vertices == NULL) {
		fprintf(stderr, "Error: cannot allocate memory for the geometry file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(vertices, bytes_per_primitive, n_triangles, fp); // assume the data file has no faults.
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);
}

void Static_Object::prepare_geom_of_static_object() {
	int i, n_bytes_per_vertex, n_bytes_per_triangle;
	char filename[512];

	n_bytes_per_vertex = n_fields * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	read_geometry(n_bytes_per_triangle);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, n_triangles * n_bytes_per_triangle, vertices, GL_STATIC_DRAW);

	free(vertices);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	// vertex normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// vertex texture coordinates (n_fields가 8일 때만 활성화)
	if (n_fields == 8) { // 3 (pos) + 3 (normal) + 2 (texcoord)
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
	else {
		glDisableVertexAttribArray(2); // 텍스처 좌표가 없으면 비활성화
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	printf("nfields is %d\n", n_fields);
}

void Building::define_object() {
	Material* cur_material;
	strcpy(filename, "Data/Building1_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	instances.back().ModelMatrix = glm::mat4(1.0f);

	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.135f, 0.2225f, 0.1575f, 1.0f);
	cur_material->diffuse = glm::vec4(0.54f, 0.89f, 0.63f, 1.0f);
	cur_material->specular = glm::vec4(0.316228f, 0.316228f, 0.316228f, 1.0f);
	cur_material->exponent = 128.0f * 0.1f;
}

void Table::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;

	strcpy(filename, "Data/Table_vn.geom");
	n_fields = 6;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(215.0f, 25.0f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.5f, 0.5f, 0.5f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.1f, 0.3f, 0.1f, 1.0f);
	cur_material->diffuse = glm::vec4(0.4f, 0.6f, 0.3f, 1.0f);
	cur_material->specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	cur_material->exponent = 15.0f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(198.0f, 120.0f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.8f, 0.6f, 0.6f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	cur_material->diffuse = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	cur_material->specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	cur_material->exponent = 128.0f * 0.078125f;
}

void Light::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/Light_vn.geom");
	n_fields = 6;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 100.0f, 49.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 47.5f, 49.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 130.0f, 49.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(190.0f, 60.0f, 49.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(210.0f, 112.5f, 49.0));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;

}

void Teapot::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/Teapotn_vn.geom");
	n_fields = 6;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(193.0f, 120.0f, 11.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(2.0f, 2.0f, 2.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.1745f, 0.01175f, 0.01175f, 1.0f);
	cur_material->diffuse = glm::vec4(0.61424f, 0.04136f, 0.04136f, 1.0f);
	cur_material->specular = glm::vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
	cur_material->exponent = 128.0f * 0.6;
}

void New_Chair::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/new_chair_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 104.0f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.8f, 0.8f, 0.8f));
	*cur_MM = glm::rotate(*cur_MM, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.05f, 0.05f, 0.0f, 1.0f);
	cur_material->diffuse = glm::vec4(0.5f, 0.5f, 0.4f, 1.0f);
	cur_material->specular = glm::vec4(0.7f, 0.7f, 0.04f, 1.0f);
	cur_material->exponent = 128.0f * 0.078125f;
}

void Frame::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/Frame_vn.geom");
	n_fields = 6;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(188.0f, 116.0f, 30.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.6f, 0.6f, 0.6f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.19125f, 0.0735f, 0.0225f, 1.0f);
	cur_material->diffuse = glm::vec4(0.7038f, 0.27048f, 0.0828f, 1.0f);
	cur_material->specular = glm::vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
	cur_material->exponent = 128.0f * 0.1f;
}

void New_Picture::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/new_picture_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(189.5f, 116.0f, 30.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(13.5f * 0.6f, 13.5f * 0.6f, 13.5f * 0.6f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
	cur_material->diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	cur_material->specular = glm::vec4(0.774597f, 0.774597f, 0.774597f, 1.0f);
	cur_material->exponent = 128.0f * 0.6f;
}

void Cow::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/cow_vn.geom");
	n_fields = 6;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(215.0f, 100.0f, 9.5f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(30.0f, 30.0f, 30.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	cur_material->diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
	cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	cur_material->exponent = 0.21794872f * 0.6f;
}

void Tower::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/woodTower_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 135.0f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(5.0f, 5.0f, 5.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	cur_material->diffuse = glm::vec4(0.957f, 0.643f, 0.376f, 1.0f);
	cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	cur_material->exponent = 0.21794872f * 0.6f;
}

void Ironman::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/ironman_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 140.0f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(5.0f, 5.0f, 5.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	cur_material->diffuse = glm::vec4(0.5f, 0.1f, 0.1f, 1.0f);
	cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	cur_material->exponent = 0.21794872f * 0.6f;

	instances.back().texture_id = loadTexture("Data/ironman_tex2.jpg");
	printf("ironman tex id :%d\n", instances.back().texture_id);
}

void Optimus::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/optimus_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, 70.0f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.03f, 0.03f, 0.03f));
	*cur_MM = glm::rotate(*cur_MM, -270.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	*cur_MM = glm::rotate(*cur_MM, -90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	cur_material->diffuse = glm::vec4(0.1f, 0.2f, 0.5f, 1.0f);
	cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	cur_material->exponent = 0.21794872f * 0.6f;
}

void Godzilla::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/godzilla_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(35.0f, 40.0f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.05f, 0.05f, 0.05f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	cur_material->diffuse = glm::vec4(0.0f, 0.5f, 0.0f, 1.0f);
	cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	cur_material->exponent = 0.21794872f * 0.6f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 45.0f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.04f, 0.04f, 0.04f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	cur_material->diffuse = glm::vec4(0.5f, 0.0f, 0.0f, 1.0f);
	cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	cur_material->exponent = 0.21794872f * 0.6f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 45.0f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.04f, 0.04f, 0.04f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	cur_material->diffuse = glm::vec4(0.0f, 0.0f, 0.5f, 1.0f);
	cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	cur_material->exponent = 0.21794872f * 0.6f;
}

void Tank::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/tank_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(213.0f, 24.5f, 10.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(2.5f, 2.5f, 2.5f));
	*cur_MM = glm::rotate(*cur_MM, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	*cur_MM = glm::translate(*cur_MM, glm::vec3(0.0f, -15.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	cur_material->diffuse = glm::vec4(0.0f, 0.3f, 0.0f, 1.0f);
	cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	cur_material->exponent = 0.21794872f * 0.6f;
}

void print_mat4(const char* string, glm::mat4 M) {
	fprintf(stdout, "\n***** %s ******\n", string);
	for (int i = 0; i < 4; i++)
		fprintf(stdout, "*** COL[%d] (%f, %f, %f, %f)\n", i, M[i].x, M[i].y, M[i].z, M[i].w);
	fprintf(stdout, "**************\n\n");
}
void Static_Object::draw_object(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
	std::vector<std::reference_wrapper<Shader>>& shader_list, glm::vec3 light_pos_ws, glm::vec3 camera_pos_ws) {

	glFrontFace(front_face_mode);

	// Pass 1: 불투명(Opaque) 객체들을 먼저 그립니다.
	for (int i = 0; i < instances.size(); i++) {
		Instance& instance = instances[i];
		if (instance.transparent_on) continue;

		Shader* current_shader = &shader_list[shader_ID_mapper[shader_kind]].get();
		glUseProgram(current_shader->h_ShaderProgram);

		glm::mat4 ModelMatrix = instance.ModelMatrix;
		glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(current_shader->loc_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));

		if (shader_kind == SHADER_GOURAUD || shader_kind == SHADER_PHONG) {
			glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
			glm::mat3 NormalMatrix = glm::transpose(glm::inverse(glm::mat3(ModelViewMatrix)));

			Lighting temp_eye_light = scene.eye_space_light;
			temp_eye_light.position = scene.eye_light_eye_pos;
			temp_eye_light.direction = scene.eye_light_eye_dir;

			if (shader_kind == SHADER_PHONG) {
				Shader_Phong* phong_shader = static_cast<Shader_Phong*>(current_shader);
				phong_shader->set_lights(
					scene.main_light, scene.light_on, temp_eye_light, scene.eye_light_on,
					scene.object_light, scene.object_light_on, scene.object_light_eye_pos, camera_pos_ws
				);
				// ... Phong 쉐이더 유니폼 설정 (이전과 동일) ...
				glUniformMatrix4fv(phong_shader->loc_ModelMatrix, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
				glUniformMatrix4fv(phong_shader->loc_ViewMatrix, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
				glUniformMatrix3fv(phong_shader->loc_NormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
				glUniform4fv(phong_shader->loc_material_emission, 1, glm::value_ptr(instance.material.emission));
				glUniform4fv(phong_shader->loc_material_ambient, 1, glm::value_ptr(instance.material.ambient));
				glUniform4fv(phong_shader->loc_material_diffuse, 1, glm::value_ptr(instance.material.diffuse));
				glUniform4fv(phong_shader->loc_material_specular, 1, glm::value_ptr(instance.material.specular));
				glUniform1f(phong_shader->loc_material_exponent, instance.material.exponent);
				glUniform1i(phong_shader->loc_is_transparent, 0);

				glUniform1i(phong_shader->loc_red_velvet_on, scene.red_velvet_on);
				if (phong_shader->loc_texture_sampler != -1 && phong_shader->loc_has_texture != -1) {
					if (instance.texture_id != 0) {
						glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, instance.texture_id);
						glUniform1i(phong_shader->loc_texture_sampler, 0); glUniform1i(phong_shader->loc_has_texture, 1);
					}
					else { glUniform1i(phong_shader->loc_has_texture, 0); }
				}
			}
			else { // SHADER_GOURAUD
				Shader_Gouraud* gouraud_shader = static_cast<Shader_Gouraud*>(current_shader);
				gouraud_shader->set_lights(
					scene.main_light, scene.light_on, temp_eye_light, scene.eye_light_on,
					scene.object_light, scene.object_light_on, scene.object_light_eye_pos, camera_pos_ws
				);
				// ... Gouraud 쉐이더 유니폼 설정 (이전과 동일) ...
				glUniformMatrix4fv(gouraud_shader->loc_ModelMatrix, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
				glUniformMatrix4fv(gouraud_shader->loc_ViewMatrix, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
				glUniformMatrix3fv(gouraud_shader->loc_NormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
				glUniform4fv(gouraud_shader->loc_material_emission, 1, glm::value_ptr(instance.material.emission));
				glUniform4fv(gouraud_shader->loc_material_ambient, 1, glm::value_ptr(instance.material.ambient));
				glUniform4fv(gouraud_shader->loc_material_diffuse, 1, glm::value_ptr(instance.material.diffuse));
				glUniform4fv(gouraud_shader->loc_material_specular, 1, glm::value_ptr(instance.material.specular));
				glUniform1f(gouraud_shader->loc_material_exponent, instance.material.exponent);
				glUniform1i(gouraud_shader->loc_is_transparent, 0);

				glUniform1i(gouraud_shader->loc_red_velvet_on, scene.red_velvet_on);
				if (gouraud_shader->loc_texture_sampler != -1 && gouraud_shader->loc_has_texture != -1) {
					if (instance.texture_id != 0) {
						glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, instance.texture_id);
						glUniform1i(gouraud_shader->loc_texture_sampler, 0); glUniform1i(gouraud_shader->loc_has_texture, 1);
					}
					else { glUniform1i(gouraud_shader->loc_has_texture, 0); }
				}
			}
		}
		// ▼▼▼▼▼▼▼▼▼▼ 누락되었던 Simple 쉐이더 처리 로직 추가 ▼▼▼▼▼▼▼▼▼▼
		else if (shader_kind == SHADER_SIMPLE) {
			Shader_Simple* simple_shader = static_cast<Shader_Simple*>(current_shader);
			glUniform3f(simple_shader->loc_primitive_color, instance.material.diffuse.r,
				instance.material.diffuse.g, instance.material.diffuse.b);
		}
		// ▲▲▲▲▲▲▲▲▲▲ 여기까지 ▲▲▲▲▲▲▲▲▲▲

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * n_triangles);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Pass 2: 투명(Transparent) 객체들을 마지막에 그립니다.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	for (int i = 0; i < instances.size(); i++) {
		Instance& instance = instances[i];
		if (!instance.transparent_on) continue;

		Shader* current_shader = &shader_list[shader_ID_mapper[shader_kind]].get();
		glUseProgram(current_shader->h_ShaderProgram);

		glm::mat4 ModelMatrix;
		if (this->object_id == STATIC_OBJECT_TOWER) {
			// 1. 회전 각도를 시간에 따라 계산합니다. (속도를 조절하려면 0.5f 값을 변경)
			float angle = scene.time_stamp * 0.5f;

			// 2. Z축 중심의 순수한 회전 행렬을 만듭니다.
			glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

			// 3. 인스턴스의 기본 ModelMatrix에 계산된 회전 행렬을 곱하여 최종 ModelMatrix를 결정합니다.
			// 이렇게 하면 타워가 자신의 로컬 축을 기준으로 제자리에서 회전(자전)하게 됩니다.
			ModelMatrix = instance.ModelMatrix * rotation_matrix;
		}
		else {
			// 타워가 아닌 다른 투명 객체는 회전하지 않습니다.
			ModelMatrix = instance.ModelMatrix;
		}
		glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(current_shader->loc_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));

		if (shader_kind == SHADER_GOURAUD || shader_kind == SHADER_PHONG) {
			// ... (불투명 패스와 동일한 조명/재질/텍스처 설정 로직) ...
			// 유일한 차이점은 is_transparent와 alpha 유니폼을 설정하는 것입니다.
			glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
			glm::mat3 NormalMatrix = glm::transpose(glm::inverse(glm::mat3(ModelViewMatrix)));
			Lighting temp_eye_light = scene.eye_space_light;
			temp_eye_light.position = scene.eye_light_eye_pos;
			temp_eye_light.direction = scene.eye_light_eye_dir;
			if (shader_kind == SHADER_PHONG) {
				Shader_Phong* phong_shader = static_cast<Shader_Phong*>(current_shader);
				phong_shader->set_lights(
					scene.main_light, scene.light_on, temp_eye_light, scene.eye_light_on,
					scene.object_light, scene.object_light_on, scene.object_light_eye_pos, camera_pos_ws
				);
				// ... Phong 쉐이더 유니폼 설정 ...
				glUniformMatrix4fv(phong_shader->loc_ModelMatrix, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
				glUniformMatrix4fv(phong_shader->loc_ViewMatrix, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
				glUniformMatrix3fv(phong_shader->loc_NormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
				glUniform4fv(phong_shader->loc_material_emission, 1, glm::value_ptr(instance.material.emission));
				glUniform4fv(phong_shader->loc_material_ambient, 1, glm::value_ptr(instance.material.ambient));
				glUniform4fv(phong_shader->loc_material_diffuse, 1, glm::value_ptr(instance.material.diffuse));
				glUniform4fv(phong_shader->loc_material_specular, 1, glm::value_ptr(instance.material.specular));
				glUniform1f(phong_shader->loc_material_exponent, instance.material.exponent);
				glUniform1i(phong_shader->loc_is_transparent, 1);

				glUniform1i(phong_shader->loc_red_velvet_on, scene.red_velvet_on);
				glUniform1f(phong_shader->loc_alpha, instance.alpha);
				if (phong_shader->loc_texture_sampler != -1 && phong_shader->loc_has_texture != -1) {
					if (instance.texture_id != 0) {
						glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, instance.texture_id);
						glUniform1i(phong_shader->loc_texture_sampler, 0); glUniform1i(phong_shader->loc_has_texture, 1);
					}
					else { glUniform1i(phong_shader->loc_has_texture, 0); }
				}
			}
			else { // SHADER_GOURAUD
				Shader_Gouraud* gouraud_shader = static_cast<Shader_Gouraud*>(current_shader);
				gouraud_shader->set_lights(
					scene.main_light, scene.light_on, temp_eye_light, scene.eye_light_on,
					scene.object_light, scene.object_light_on, scene.object_light_eye_pos, camera_pos_ws
				);
				// ... Gouraud 쉐이더 유니폼 설정 ...
				glUniformMatrix4fv(gouraud_shader->loc_ModelMatrix, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
				glUniformMatrix4fv(gouraud_shader->loc_ViewMatrix, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
				glUniformMatrix3fv(gouraud_shader->loc_NormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
				glUniform4fv(gouraud_shader->loc_material_emission, 1, glm::value_ptr(instance.material.emission));
				glUniform4fv(gouraud_shader->loc_material_ambient, 1, glm::value_ptr(instance.material.ambient));
				glUniform4fv(gouraud_shader->loc_material_diffuse, 1, glm::value_ptr(instance.material.diffuse));
				glUniform4fv(gouraud_shader->loc_material_specular, 1, glm::value_ptr(instance.material.specular));
				glUniform1f(gouraud_shader->loc_material_exponent, instance.material.exponent);
				glUniform1i(gouraud_shader->loc_is_transparent, 1);

				glUniform1i(gouraud_shader->loc_red_velvet_on, scene.red_velvet_on);
				glUniform1f(gouraud_shader->loc_alpha, instance.alpha);
				if (gouraud_shader->loc_texture_sampler != -1 && gouraud_shader->loc_has_texture != -1) {
					if (instance.texture_id != 0) {
						glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, instance.texture_id);
						glUniform1i(gouraud_shader->loc_texture_sampler, 0); glUniform1i(gouraud_shader->loc_has_texture, 1);
					}
					else { glUniform1i(gouraud_shader->loc_has_texture, 0); }
				}
			}
		}
		// ▼▼▼▼▼▼▼▼▼▼ 누락되었던 Simple 쉐이더 처리 로직 추가 ▼▼▼▼▼▼▼▼▼▼
		else if (shader_kind == SHADER_SIMPLE) {
			Shader_Simple* simple_shader = static_cast<Shader_Simple*>(current_shader);
			// Simple 쉐이더는 투명도를 직접 지원하지 않으므로, 기본 색상만 설정합니다.
			// 만약 Simple 쉐이더도 투명하게 하고 싶다면 simple.frag 쉐이더도 수정해야 합니다.
			glUniform3f(simple_shader->loc_primitive_color, instance.material.diffuse.r,
				instance.material.diffuse.g, instance.material.diffuse.b);
		}
		// ▲▲▲▲▲▲▲▲▲▲ 여기까지 ▲▲▲▲▲▲▲▲▲▲

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * n_triangles);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	glUseProgram(0);
}