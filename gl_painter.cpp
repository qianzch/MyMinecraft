#include "gl_painter.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
using namespace std;

Shader::Shader(const string shader_code_file, GLenum shader_type) {
	ifstream f_in;
	f_in.open(shader_code_file.c_str(), std::ios::in | std::ios::binary);
	if (!f_in.is_open())
		throw std::runtime_error(std::string("Failed to open file: ") + shader_code_file);
	// redirect buffer, "buffer" then can be used as "f_in"
	stringstream buffer;
	buffer << f_in.rdbuf();
	string code_str = buffer.str();
	const char* code = code_str.c_str();
	f_in.close();

	_obj = glCreateShader(shader_type);
	glShaderSource(_obj, 1, (const GLchar**)&code, NULL);
	glCompileShader(_obj);
	GLint status;
	glGetShaderiv(_obj, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		string msg("Compile failure in shader:\n");

		GLint infoLogLength;
		glGetShaderiv(_obj, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* strInfoLog = new char[infoLogLength + 1];
		glGetShaderInfoLog(_obj, infoLogLength, NULL, strInfoLog);
		msg += strInfoLog;
		delete[] strInfoLog;

		glDeleteShader(_obj); _obj = 0;
		throw std::runtime_error(msg);
	}
}
GLuint Shader::get_shader_ID() {
	return _obj;
}

bool GL_PAINTER::is_init(false);
GLuint GL_PAINTER::VAO(0);
GLuint GL_PAINTER::VBO(0);
GLuint GL_PAINTER::EBO(0);
GLuint GL_PAINTER::skyboxVAO(0);
GLuint GL_PAINTER::skyboxVBO(0);
GLuint GL_PAINTER::waterVAO(0);
GLuint GL_PAINTER::waterVBO(0);
GLuint GL_PAINTER::shader_program(0);
GLuint GL_PAINTER::shader_program_sb(0);  //sb=skybox
GLuint GL_PAINTER::shader_program_water(0);
GLuint GL_PAINTER::simpleDepthShader_program(0);
GLuint GL_PAINTER::texture(0);
GLuint GL_PAINTER::cubemapTexture(0);
GLuint GL_PAINTER::depthMap(0);
GLuint GL_PAINTER::depthMapFBO(0);


GL_PAINTER::GL_PAINTER() {}

void GL_PAINTER::init() {
	glewExperimental = GL_TRUE;
	glewInit();

	if (!is_init) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		
		glGenFramebuffers(1, &depthMapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

		Shader v_shader(VERTEX_SHADER_FILE, GL_VERTEX_SHADER);
		Shader f_shader(FRAGMENT_SHADER_FILE, GL_FRAGMENT_SHADER);

		Shader v_simpleDepthShader("shadow_mapping_depth.vs", GL_VERTEX_SHADER);
		Shader f_simpleDepthShader("shadow_mapping_depth.fs", GL_FRAGMENT_SHADER);

		Shader v_shader_sb(SKYBOX_VS_FILE, GL_VERTEX_SHADER);
		Shader f_shader_sb(SKYBOX_FS_FILE, GL_FRAGMENT_SHADER);

		Shader v_shader_water(WATER_VS_FILE, GL_VERTEX_SHADER);
		Shader f_shader_water(WATER_FS_FILE, GL_FRAGMENT_SHADER);

		GLuint v_shader_id = v_shader.get_shader_ID();
		GLuint f_shader_id = f_shader.get_shader_ID();
		shader_program = glCreateProgram();
		glAttachShader(shader_program, v_shader_id);
		glAttachShader(shader_program, f_shader_id);
		glLinkProgram(shader_program);

		GLuint v_simpleDepthShader_id = v_simpleDepthShader.get_shader_ID();
		GLuint f_simpleDepthShader_id = f_simpleDepthShader.get_shader_ID();
		simpleDepthShader_program = glCreateProgram();
		glAttachShader(simpleDepthShader_program, v_simpleDepthShader_id);
		glAttachShader(simpleDepthShader_program, f_simpleDepthShader_id);
		glLinkProgram(simpleDepthShader_program);
		//glUseProgram(shader_program);

		
		GLuint v_shader_sb_id = v_shader_sb.get_shader_ID();
		GLuint f_shader_sb_id = f_shader_sb.get_shader_ID();
		shader_program_sb = glCreateProgram();
		glAttachShader(shader_program_sb, v_shader_sb_id);
		glAttachShader(shader_program_sb, f_shader_sb_id);
		glLinkProgram(shader_program_sb);

		GLuint v_shader_water_id = v_shader_water.get_shader_ID();
		GLuint f_shader_water_id = f_shader_water.get_shader_ID();
		shader_program_water = glCreateProgram();
		glAttachShader(shader_program_water, v_shader_water_id);
		glAttachShader(shader_program_water, f_shader_water_id);
		glLinkProgram(shader_program_water);
	}
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	is_init = true;

	texture = load_BMP_texture(BLOCK_TEXTURE_FILE);

	vector<std::string> faces
	{
		SKYBOX_FIFE_1,
		SKYBOX_FIFE_2,
		SKYBOX_FIFE_3,
		SKYBOX_FIFE_4,
		SKYBOX_FIFE_5,
		SKYBOX_FIFE_6,
	};
	cubemapTexture = loadCubemap(faces);

}

bool GL_PAINTER::if_init() { return is_init; }

void GL_PAINTER::paint(GLfloat *attr, int mode) {
	glClearColor(0.38, 0.55, 0.71, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (!is_init)
		throw logic_error("GL_PAINTER must be initialized first !");

	if (attr != nullptr) { 
		Camera::cameraFall();
		draw_skybox();
		draw_environment();
	}
	else draw_init(); // attr为nullptr时用来解决玄学bug
	
}

void GL_PAINTER::draw_init() {
	glBegin(GL_POINT);
	glVertex3f(1.5, 1.5, 0);
	glEnd();
}

double inTriangleDepth(glm::vec4 &p0, glm::vec4 &p1, glm::vec4 &p2, glm::vec2 &pt) {
	glm::vec2 v1, v2, vt;
	v1 = glm::vec2(p1.x - p0.x, p1.y - p0.y);
	v2 = glm::vec2(p2.x - p0.x, p2.y - p0.y);
	vt = glm::vec2(pt.x - p0.x, pt.y - p0.y);
	if (glm::abs(v1.x*v2.y - v2.x*v1.y) < 0.0001) {
		return -10000;
	}
	double u = (vt.x*v2.y - vt.y*v2.x) / (v1.x*v2.y - v1.y*v2.x);
	double v = (vt.x*v1.y - vt.y*v1.x) / (v2.x*v1.y - v2.y*v1.x);
	if (u >= 0 && v >= 0 && u + v <= 1) {
		return p0.z + u*(p1.z - p0.z) + v*(p2.z - p0.z);
	}
	return -10000;
}

bool GL_PAINTER::canUseDestroyCube = false;
int GL_PAINTER::destroyCube[3] = { 0,0,0 };
bool GL_PAINTER::canUsePutCube = false;
int GL_PAINTER::putCube[3] = { 0,0,0 };

bool isEmptyToMouse(int type) {
	if (type == CUBE_AIR) {
		return true;
	}
	if (type >= CUBE_WATER_MIN && type <= CUBE_WATER_MAX) {
		return true;
	}
	return false;
}

void GL_PAINTER::getDesPutCube() {
	const int searchArea = 5;
	float cubePoint[searchArea*searchArea*searchArea * 6][3];
	float matchCube[3];
	float nearestZ = 10000;

	canUseDestroyCube = false;
	canUsePutCube = false;

	int i, j, k, l;
	int cameraInCubeX = cameraPos.x / cubeSize;
	int cameraInCubeY = cameraPos.y / cubeSize;
	int cameraInCubeZ = cameraPos.z / cubeSize;
	glm::mat4 projection, view, model;
	glm::vec3 viewPos, viewCenter, viewUp;
	viewPos = Camera::getEye();
	viewCenter = Camera::getCenter();
	viewUp = Camera::getUp();
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 1000.0f);
	view = glm::lookAt(viewPos, viewCenter, viewUp);


	for (i = cameraInCubeX - searchArea; i <= cameraInCubeX + searchArea; i++) {
		for (j = cameraInCubeY - searchArea; j <= cameraInCubeY + searchArea; j++) {
			for (k = cameraInCubeZ - searchArea; k <= cameraInCubeZ + searchArea; k++) {

				if (isEmptyToMouse(getMapData(i, j, k))) {
					continue;
				}

				for (l = 0; l < 6; l++) {
					glm::vec4 p1;
					glm::vec4 p2;
					glm::vec4 p3;
					glm::vec4 p4;

					if (l == 0) {
						if (!isEmptyToMouse(getMapData(i + 1, j, k))) continue;
						p1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
						p2 = glm::vec4(1.0f, 1.0f, -1.0f, 1.0f);
						p3 = glm::vec4(1.0f, -1.0f, -1.0f, 1.0f);
						p4 = glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
					}
					if (l == 1) {
						if (!isEmptyToMouse(getMapData(i - 1, j, k))) continue;
						p1 = glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
						p2 = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
						p3 = glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f);
						p4 = glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
					}
					if (l == 2) {
						if (!isEmptyToMouse(getMapData(i, j + 1, k))) continue;
						p1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
						p2 = glm::vec4(1.0f, 1.0f, -1.0f, 1.0f);
						p3 = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
						p4 = glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
					}
					if (l == 3) {
						if (!isEmptyToMouse(getMapData(i, j - 1, k))) continue;
						p1 = glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
						p2 = glm::vec4(1.0f, -1.0f, -1.0f, 1.0f);
						p3 = glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f);
						p4 = glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
					}
					if (l == 4) {
						if (!isEmptyToMouse(getMapData(i, j, k + 1))) continue;
						p1 = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
						p2 = glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
						p3 = glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
						p4 = glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
					}
					if (l == 5) {
						if (!isEmptyToMouse(getMapData(i, j, k - 1))) continue;
						p1 = glm::vec4(1.0f, 1.0f, -1.0f, 1.0f);
						p2 = glm::vec4(1.0f, -1.0f, -1.0f, 1.0f);
						p3 = glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f);
						p4 = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
					}
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(cubeSize * (i), cubeSize * (j), cubeSize * (k)));
					model = glm::scale(model, glm::vec3(cubeSize / 2, cubeSize / 2, cubeSize / 2));

					p1 = projection * view * model * p1;
					p2 = projection * view * model * p2;
					p3 = projection * view * model * p3;
					p4 = projection * view * model * p4;

					double depth1 = inTriangleDepth(p1, p2, p3, glm::vec2(0, 0));
					double depth2 = inTriangleDepth(p1, p4, p3, glm::vec2(0, 0));

					if (depth1 >= -1 || depth2 >= -1) {
						double oZ;
						if (depth1 < -1) {
							oZ = depth2;
						}
						else if (depth2 < -1) {
							oZ = depth1;
						}
						else {
							oZ = glm::min(depth1, depth2);
						}


						if (oZ < nearestZ) {
							canUseDestroyCube = canUsePutCube = true;
							destroyCube[0] = i;
							destroyCube[1] = j;
							destroyCube[2] = k;
							putCube[0] = i;
							putCube[1] = j;
							putCube[2] = k;
							if (l == 0) {
								putCube[0]++;
							}
							else if (l == 1) {
								putCube[0]--;
							}
							else if (l == 2) {
								putCube[1]++;
							}
							else if (l == 3) {
								putCube[1]--;
							}
							else if (l == 4) {
								putCube[2]++;
							}
							else if (l == 5) {
								putCube[2]--;
							}
							nearestZ = oZ;
						}
					}

				}


			}
		}
	}

}

void GL_PAINTER::draw_skybox() {

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	int Location = glGetUniformLocation(shader_program_sb, "skybox");
	glUniform1i(Location, 1);

	// draw skybox as last
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	glUseProgram(shader_program_sb);
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 model;
	glm::vec3 viewPos, viewCenter, viewUp;
	glm::mat4 view1;
	viewPos = Camera::getEye();
	viewCenter = Camera::getCenter();
	viewUp = Camera::getUp();
	view1 = glm::lookAt(viewPos, viewCenter, viewUp);
	view = glm::mat4(glm::mat3(view1));
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(viewPos.x, viewPos.y, viewPos.z));
	model = glm::scale(model, glm::vec3(10000, 10000, 10000));
	projection = glm::perspective(glm::radians(45.0f), GLfloat(800/555), 0.01f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader_program_sb, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shader_program_sb, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_program_sb, "view"), 1, GL_FALSE, glm::value_ptr(view));
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}

GLuint GL_PAINTER::load_BMP_texture(const char *imagepath) {
	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height, bpp;
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = fopen(imagepath, "rb");

	// Read the header, i.e. the 54 first bytes

	// If less than 54 byes are read, problem
	if (fread(header, 1, 54, file) != 54)
		return 0;

	// A BMP files always begins with "BM"
	if (header[0] != 'B' || header[1] != 'M')
		return 0;

	// Make sure this is a 24bpp file
	if (*(int*)&(header[0x1E]) != 0) return 0;
	if (*(int*)&(header[0x1C]) != 24) return 0;

	// Read the information about the image
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	bpp = 3;

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54;

	// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	// Everything is in memory now, the file wan be closed
	fclose(file);

	// Swap Red and Blue component for each texel of the image
	unsigned char t;
	for (unsigned int i = 0; i < imageSize; i += 3) {
		t = data[i];
		data[i] = data[i + 2];
		data[i + 2] = t;
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, (bpp == 3 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE, data);

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	return textureID;
}
GLuint GL_PAINTER::loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

#include "EnvPainter.h"

void initMapData() {
	int mapX = MAX_X;
	int mapY = MAX_Y;
	int mapZ = MAX_Z;

	for (int i = 0; i < mapX; i++) {
		for (int j = 0; j < mapY; j++) {
			for (int k = 0; k < mapZ; k++) {
				mapData[i][j][k] = 0;
			}
		}
	}
	for (int i = 0; i < mapX; i++) {
		for (int j = 0; j < 14; j++) {
			for (int k = 0; k < mapZ; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}
	for (int i = 0; i < mapX; i++) {
		for (int j = 14; j < 15; j++) {
			for (int k = 0; k < mapZ; k++) {
				mapData[i][j][k] = 3;
			}
		}
	}
	for (int i = 8; i < 12; i++) {
		for (int j = 15; j < 20; j++) {
			for (int k = 8; k < 12; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}
	for (int i = 7; i < 13; i++) {
		for (int j = 15; j < 18; j++) {
			for (int k = 7; k < 13; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}
	for (int i = 6; i < 14; i++) {
		for (int j = 15; j < 16; j++) {
			for (int k = 6; k < 14; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}
	for (int i = 5; i < 15; i++) {
		for (int j = 14; j < 15; j++) {
			for (int k = 18; k < 33; k++) {
				mapData[i][j][k] = -1;
			}
		}
	}
	
	for (int i = 30; i < 41; i++) {
		for (int j = 15; j < 25; j++) {
			for (int k = 25; k < 26; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}
	for (int i = 30; i < 31; i++) {
		for (int j = 15; j < 25; j++) {
			for (int k = 26; k < 41; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}
	for (int i = 35; i < 70; i++) {
		for (int j = 15; j < 16; j++) {
			for (int k = 1; k < 15; k++) {
				mapData[i][j][k] = 3;
			}
		}
	}

	for (int i = 37; i < 70; i++) {
		for (int j = 16; j < 17; j++) {
			for (int k = 1; k < 13; k++) {
				mapData[i][j][k] = 3;
			}
		}
	}
	for (int i = 40; i < 70; i++) {
		for (int j = 17; j < 18; j++) {
			for (int k = 1; k < 11; k++) {
				mapData[i][j][k] = 3;
			}
		}
	}
	for (int i = 5; i < 24; i++) {
		for (int j = 15; j < 16; j++) {
			for (int k = 40; k < 60; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}
	for (int i = 6; i < 23; i++) {
		for (int j = 16; j < 17; j++) {
			for (int k = 41; k < 59; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}
	for (int i = 7; i < 22; i++) {
		for (int j = 17; j < 18; j++) {
			for (int k = 42; k < 58; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}
	for (int i = 8; i < 21; i++) {
		for (int j = 18; j < 19; j++) {
			for (int k = 43; k < 57; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}
	for (int i = 9; i < 20; i++) {
		for (int j = 18; j < 19; j++) {
			for (int k = 44; k < 56; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}
	for (int i = 10; i < 19; i++) {
		for (int j = 19; j < 20; j++) {
			for (int k = 45; k < 55; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}
	for (int i = 11; i < 18; i++) {
		for (int j = 20; j < 21; j++) {
			for (int k = 46; k < 54; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}
	for (int i = 12; i < 17; i++) {
		for (int j = 21; j < 22; j++) {
			for (int k = 47; k < 53; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}
	for (int i = 13; i < 16; i++) {
		for (int j = 22; j < 23; j++) {
			for (int k = 48; k < 52; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}
	for (int i = 14; i < 15; i++) {
		for (int j = 23; j < 24; j++) {
			for (int k = 49; k < 51; k++) {
				mapData[i][j][k] = 2;
			}
		}
	}

	for (int i = 66; i < 94; i++) {
		for (int j = 15; j < 16; j++) {
			for (int k = 26; k < 27; k++) {
				mapData[i][j][k] = CUBE_FENCE;
			}
		}
	}
	for (int i = 66; i < 67; i++) {
		for (int j = 15; j < 16; j++) {
			for (int k = 27; k < 57; k++) {
				mapData[i][j][k] = CUBE_FENCE;
			}
		}
	}
	for (int i = 66; i < 94; i++) {
		for (int j = 15; j < 16; j++) {
			for (int k = 56; k < 57; k++) {
				mapData[i][j][k] = CUBE_FENCE;
			}
		}
	}
	for (int i = 93; i < 94; i++) {
		for (int j = 15; j < 16; j++) {
			for (int k = 27; k < 57; k++) {
				mapData[i][j][k] = CUBE_FENCE;
			}
		}
	}

	mapData[45][17][9] = 4;
	mapData[45][18][9] = 4;
	mapData[45][19][9] = 4;
	mapData[45][20][9] = 4;
	mapData[45][21][9] = 4;
	mapData[45][22][9] = 4;
	mapData[45][23][9] = 4;

	mapData[43][22][7] = 5; mapData[44][22][7] = 5; mapData[45][22][7] = 5; mapData[46][22][7] = 5; mapData[47][22][7] = 5;
	mapData[43][22][8] = 5; mapData[44][22][8] = 5; mapData[45][22][8] = 5; mapData[46][22][8] = 5; mapData[47][22][8] = 5;
	mapData[43][22][9] = 5; mapData[44][22][9] = 5;                         mapData[46][22][9] = 5; mapData[47][22][9] = 5;
	mapData[43][22][10] = 5; mapData[44][22][10] = 5; mapData[45][22][10] = 5; mapData[46][22][10] = 5; mapData[47][22][10] = 5;
	mapData[43][22][11] = 5; mapData[44][22][11] = 5; mapData[45][22][11] = 5; mapData[46][22][11] = 5; mapData[47][22][11] = 5;
	mapData[44][23][8] = 5; mapData[45][23][8] = 5; mapData[46][23][8] = 5;
	mapData[44][23][9] = 5;                         mapData[46][23][9] = 5;
	mapData[44][23][10] = 5; mapData[45][23][10] = 5; mapData[46][23][10] = 5;

	mapData[38][16][13] = 4;
	mapData[38][17][13] = 4;
	mapData[38][18][13] = 4;
	mapData[38][19][13] = 4;
	mapData[38][20][13] = 4;
	mapData[38][21][13] = 4;
	mapData[38][22][13] = 4;
	mapData[38][23][13] = 4;
	mapData[38][24][13] = 4;

	mapData[34][21][9] = 5; mapData[35][21][9] = 5; mapData[36][21][9] = 5; mapData[37][21][9] = 5; mapData[38][21][9] = 5; mapData[39][21][9] = 5; mapData[40][21][9] = 5; mapData[41][21][9] = 5; mapData[42][21][9] = 5;
	mapData[34][21][10] = 5; mapData[35][21][10] = 5; mapData[36][21][10] = 5; mapData[37][21][10] = 5; mapData[38][21][10] = 5; mapData[39][21][10] = 5; mapData[40][21][10] = 5; mapData[41][21][10] = 5; mapData[42][21][10] = 5;
	mapData[34][21][11] = 5; mapData[35][21][11] = 5; mapData[36][21][11] = 5; mapData[37][21][11] = 5; mapData[38][21][11] = 5; mapData[39][21][11] = 5; mapData[40][21][11] = 5; mapData[41][21][11] = 5; mapData[42][21][11] = 5;
	mapData[34][21][12] = 5; mapData[35][21][12] = 5; mapData[36][21][12] = 5; mapData[37][21][12] = 5; mapData[38][21][12] = 5; mapData[39][21][12] = 5; mapData[40][21][12] = 5; mapData[41][21][12] = 5; mapData[42][21][12] = 5;
	mapData[34][21][13] = 5; mapData[35][21][13] = 5; mapData[36][21][13] = 5; mapData[37][21][13] = 5;                          mapData[39][21][13] = 5; mapData[40][21][13] = 5; mapData[41][21][13] = 5; mapData[42][21][13] = 5;
	mapData[34][21][14] = 5; mapData[35][21][14] = 5; mapData[36][21][14] = 5; mapData[37][21][14] = 5; mapData[38][21][14] = 5; mapData[39][21][14] = 5; mapData[40][21][14] = 5; mapData[41][21][14] = 5; mapData[42][21][14] = 5;
	mapData[34][21][15] = 5; mapData[35][21][15] = 5; mapData[36][21][15] = 5; mapData[37][21][15] = 5; mapData[38][21][15] = 5; mapData[39][21][15] = 5; mapData[40][21][15] = 5; mapData[41][21][15] = 5; mapData[42][21][15] = 5;
	mapData[34][21][16] = 5; mapData[35][21][16] = 5; mapData[36][21][16] = 5; mapData[37][21][16] = 5; mapData[38][21][16] = 5; mapData[39][21][16] = 5; mapData[40][21][16] = 5; mapData[41][21][16] = 5; mapData[42][21][16] = 5;
	mapData[34][21][17] = 5; mapData[35][21][17] = 5; mapData[36][21][17] = 5; mapData[37][21][17] = 5; mapData[38][21][17] = 5; mapData[39][21][17] = 5; mapData[40][21][17] = 5; mapData[41][21][17] = 5; mapData[42][21][17] = 5;

	mapData[35][22][10] = 5; mapData[36][22][10] = 5; mapData[37][22][10] = 5; mapData[38][22][10] = 5; mapData[39][22][10] = 5; mapData[40][22][10] = 5; mapData[41][22][10] = 5; 
	mapData[35][22][11] = 5; mapData[36][22][11] = 5; mapData[37][22][11] = 5; mapData[38][22][11] = 5; mapData[39][22][11] = 5; mapData[40][22][11] = 5; mapData[41][22][11] = 5;
	mapData[35][22][12] = 5; mapData[36][22][12] = 5; mapData[37][22][12] = 5; mapData[38][22][12] = 5; mapData[39][22][12] = 5; mapData[40][22][12] = 5; mapData[41][22][12] = 5;
	mapData[35][22][13] = 5; mapData[36][22][13] = 5; mapData[37][22][13] = 5;                          mapData[39][22][13] = 5; mapData[40][22][13] = 5; mapData[41][22][13] = 5;
	mapData[35][22][14] = 5; mapData[36][22][14] = 5; mapData[37][22][14] = 5; mapData[38][22][14] = 5; mapData[39][22][14] = 5; mapData[40][22][14] = 5; mapData[41][22][14] = 5;
	mapData[35][22][15] = 5; mapData[36][22][15] = 5; mapData[37][22][15] = 5; mapData[38][22][15] = 5; mapData[39][22][15] = 5; mapData[40][22][15] = 5; mapData[41][22][15] = 5;
	mapData[35][22][16] = 5; mapData[36][22][16] = 5; mapData[37][22][16] = 5; mapData[38][22][16] = 5; mapData[39][22][16] = 5; mapData[40][22][16] = 5; mapData[41][22][16] = 5;
	mapData[36][23][11] = 5; mapData[37][23][11] = 5; mapData[38][23][11] = 5; mapData[39][23][11] = 5; mapData[40][23][11] = 5;
	mapData[36][23][12] = 5; mapData[37][23][12] = 5; mapData[38][23][12] = 5; mapData[39][23][12] = 5; mapData[40][23][12] = 5;
	mapData[36][23][13] = 5; mapData[37][23][13] = 5;                          mapData[39][23][13] = 5; mapData[40][23][13] = 5;
	mapData[36][23][14] = 5; mapData[37][23][14] = 5; mapData[38][23][14] = 5; mapData[39][23][14] = 5; mapData[40][23][14] = 5;
	mapData[36][23][15] = 5; mapData[37][23][15] = 5; mapData[38][23][15] = 5; mapData[39][23][15] = 5; mapData[40][23][15] = 5;
	mapData[37][24][12] = 5; mapData[38][24][12] = 5; mapData[39][24][12] = 5;
	mapData[37][24][13] = 5;                          mapData[39][24][13] = 5;
	mapData[37][24][14] = 5; mapData[38][24][14] = 5; mapData[39][24][14] = 5;
	mapData[38][25][13] = 5;

	mapData[10][20][10] = 4;
	mapData[10][21][10] = 4;
	mapData[10][22][10] = 4;
	mapData[10][23][10] = 4;
	mapData[9][22][9] = 5; mapData[10][22][9] = 5; mapData[11][22][9] = 5;
	mapData[9][22][10] = 5;                        mapData[11][22][10] = 5;
	mapData[9][22][11] = 5; mapData[10][22][11] = 5; mapData[11][22][11] = 5;
	mapData[9][23][9] = 5; mapData[10][23][9] = 5; mapData[11][23][9] = 5;
	mapData[9][23][10] = 5;                        mapData[11][23][10] = 5;
	mapData[9][23][11] = 5; mapData[10][23][11] = 5; mapData[11][23][11] = 5;
	mapData[10][24][10] = 5;

	mapData[50][14][28] = 4;
	mapData[50][15][28] = 4;
	mapData[50][16][28] = 4;
	mapData[50][17][28] = 4;
	mapData[50][18][28] = 4;
	mapData[50][19][28] = 4;
	mapData[50][20][28] = 4;
	mapData[49][21][27] = 5; mapData[50][21][27] = 5; mapData[51][21][27] = 5;
	mapData[49][21][28] = 5; mapData[50][21][28] = 5; mapData[51][21][28] = 5;
	mapData[49][21][29] = 5; mapData[50][21][29] = 5; mapData[51][21][29] = 5;
	mapData[48][20][26] = 5; mapData[49][20][26] = 5; mapData[50][20][26] = 5; mapData[51][20][26] = 5; mapData[52][20][26] = 5;
	mapData[48][20][27] = 5; mapData[49][20][27] = 5; mapData[50][20][27] = 5; mapData[51][20][27] = 5; mapData[52][20][27] = 5;
	mapData[48][20][28] = 5; mapData[49][20][28] = 5;                          mapData[51][20][28] = 5; mapData[52][20][28] = 5;
	mapData[48][20][29] = 5; mapData[49][20][29] = 5; mapData[50][20][29] = 5; mapData[51][20][29] = 5; mapData[52][20][29] = 5;
	mapData[48][20][30] = 5; mapData[49][20][30] = 5; mapData[50][20][30] = 5; mapData[51][20][30] = 5; mapData[52][20][30] = 5;

	mapData[53][14][28] = 4;
	mapData[53][15][28] = 4;
	mapData[53][16][28] = 4;
	mapData[53][17][28] = 4;
	mapData[53][18][28] = 4;
	mapData[53][19][28] = 4;
	mapData[52][20][27] = 5; mapData[53][20][27] = 5; mapData[54][20][27] = 5;
	mapData[52][20][28] = 5; mapData[53][20][28] = 5; mapData[54][20][28] = 5;
	mapData[52][20][29] = 5; mapData[53][20][29] = 5; mapData[54][20][29] = 5;
	mapData[51][19][26] = 5; mapData[52][19][26] = 5; mapData[53][19][26] = 5; mapData[54][19][26] = 5; mapData[55][19][26] = 5;
	mapData[51][19][27] = 5; mapData[52][19][27] = 5; mapData[53][19][27] = 5; mapData[54][19][27] = 5; mapData[55][19][27] = 5;
	mapData[51][19][28] = 5; mapData[52][19][28] = 5;                          mapData[54][19][28] = 5; mapData[55][19][28] = 5;
	mapData[51][19][29] = 5; mapData[52][19][29] = 5; mapData[53][19][29] = 5; mapData[54][19][29] = 5; mapData[55][19][29] = 5;
	mapData[51][19][30] = 5; mapData[52][19][30] = 5; mapData[53][19][30] = 5; mapData[54][19][30] = 5; mapData[55][19][30] = 5;

	mapData[50][14][34] = 4;
	mapData[50][15][34] = 4;
	mapData[50][16][34] = 4;
	mapData[50][17][34] = 4;
	mapData[50][18][34] = 4;
	mapData[50][19][34] = 4;
	mapData[49][20][33] = 5; mapData[50][20][33] = 5; mapData[51][20][33] = 5;
	mapData[49][20][34] = 5; mapData[50][20][34] = 5; mapData[51][20][34] = 5;
	mapData[49][20][35] = 5; mapData[50][20][35] = 5; mapData[51][20][35] = 5;
	mapData[48][19][32] = 5; mapData[49][19][32] = 5; mapData[50][19][32] = 5; mapData[51][19][32] = 5; mapData[52][19][32] = 5;
	mapData[48][19][33] = 5; mapData[49][19][33] = 5; mapData[50][19][33] = 5; mapData[51][19][33] = 5; mapData[52][19][33] = 5;
	mapData[48][19][34] = 5; mapData[49][19][34] = 5;                          mapData[51][19][34] = 5; mapData[52][19][34] = 5;
	mapData[48][19][35] = 5; mapData[49][19][35] = 5; mapData[50][19][35] = 5; mapData[51][19][35] = 5; mapData[52][19][35] = 5;
	mapData[48][19][36] = 5; mapData[49][19][36] = 5; mapData[50][19][36] = 5; mapData[51][19][36] = 5; mapData[52][19][36] = 5;

	mapData[55][14][34] = 4;
	mapData[55][15][34] = 4;
	mapData[55][16][34] = 4;
	mapData[55][17][34] = 4;
	mapData[55][18][34] = 4;
	mapData[55][19][34] = 4;
	mapData[55][20][34] = 4;
	mapData[54][21][33] = 5; mapData[55][21][33] = 5; mapData[55][21][33] = 5;
	mapData[54][21][34] = 5; mapData[55][21][34] = 5; mapData[55][21][34] = 5;
	mapData[54][21][35] = 5; mapData[55][21][35] = 5; mapData[55][21][35] = 5;
	mapData[53][20][32] = 5; mapData[54][20][32] = 5; mapData[55][20][32] = 5; mapData[56][20][32] = 5; mapData[57][20][32] = 5;
	mapData[53][20][33] = 5; mapData[54][20][33] = 5; mapData[55][20][33] = 5; mapData[56][20][33] = 5; mapData[57][20][33] = 5;
	mapData[53][20][34] = 5; mapData[54][20][34] = 5;                          mapData[56][20][34] = 5; mapData[57][20][34] = 5;
	mapData[53][20][35] = 5; mapData[54][20][35] = 5; mapData[55][20][35] = 5; mapData[56][20][35] = 5; mapData[57][20][35] = 5;
	mapData[53][20][36] = 5; mapData[54][20][36] = 5; mapData[55][20][36] = 5; mapData[56][20][36] = 5; mapData[57][20][36] = 5;

	//mapData[25][30][10] = -3;

	//»­ÔÆ
    mapData[2][25][3] = -2; mapData[2][25][4] = -2; mapData[3][25][3] = -2; mapData[3][25][5] = -2;
	mapData[14][25][8] = -2; mapData[14][25][9] = -2; mapData[15][25][8] = -2; mapData[15][25][10] = -2;
	mapData[50][25][50] = -2; mapData[52][25][52] = -2;
	mapData[120][25][128] = -2; mapData[122][25][120] = -2;
	mapData[250][25][50] = -2; mapData[250][25][51] = -2;
}


void GL_PAINTER::draw_environment() {

	static EnvPainter env;

	static bool initMap = false;
	if (!initMap) {
		initMap = true;
		initMapData();
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// 现在每个三角形的格式为 x, y, z, u, v, unused
	static GLfloat cube_vertexes[] = {
		//立方体
		1.0,1.0,-1.0,1.0,0.0,-1.0,
		1.0,-1.0,-1.0,1.0,0.0,-1.0,
		-1.0,-1.0,-1.0,1.0,0.0,-1.0,
		-1.0,1.0,-1.0,1.0,0.0,-1.0,

		1.0,1.0,1.0,0.0,1.0,1.0,
		1.0,-1.0,1.0,0.0,1.0,1.0,
		-1.0,-1.0,1.0,0.0,1.0,1.0,
		-1.0,1.0,1.0,0.0,1.0,1.0,

		1.0,1.0,1.0,0.0,0.0,4.0,
		1.0,-1.0,1.0,0.0,0.0,4.0,
		1.0,-1.0,-1.0,0.0,0.0,4.0,
		1.0,1.0,-1.0,0.0,0.0,4.0,

		-1.0,1.0,1.0,1.0,1.0,-4.0,
		-1.0,-1.0,1.0,1.0,1.0,-4.0,
		-1.0,-1.0,-1.0,1.0,1.0,-4.0,
		-1.0,1.0,-1.0,1.0,1.0,-4.0,

		1.0,1.0,1.0,1.0,0.0,2.0,
		-1.0,1.0,1.0,1.0,0.0,2.0,
		-1.0,1.0,-1.0,1.0,0.0,2.0,
		1.0,1.0,-1.0,1.0,0.0,2.0,

		1.0,-1.0,1.0,0.0,1.0,-2.0,
		-1.0,-1.0,1.0,0.0,1.0,-2.0,
		-1.0,-1.0,-1.0,0.0,1.0,-2.0,
		1.0,-1.0,-1.0,0.0,1.0,-2.0
	};


	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertexes), &cube_vertexes, GL_STATIC_DRAW);
	glUseProgram(shader_program);
	glm::mat4 projection, view, model;
	glm::vec3 viewPos, viewCenter, viewUp;
	//Camera::cameraMove(KEY_W);
	viewPos = Camera::getEye();
	//Camera::cameraRotate(886, 481);
	viewCenter = Camera::getCenter();
	viewUp = Camera::getUp();
	/*viewPos = glm::vec3(1.0f, 2.0f, 5.0f);
	viewCenter = glm::vec3(0.0, 0.0, 0.0);
	viewUp = glm::vec3(0.0, 1.0, 0.0);*/
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 1000.0f);
	view = glm::lookAt(viewPos, viewCenter, viewUp);

	glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, glm::value_ptr(view));

	getDesPutCube();

	static int _opt_count = 0;
	_opt_count++;
	if (_opt_count > 10) {
		env.checkBeforeDraw();
		_opt_count = 0;
	}

	//shader part begin:
	
	
	static const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

	glEnable(GL_DEPTH_TEST);
	
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::vec3 lightPos = glm::vec3(cameraPos.x + cubeSize * 4,  cameraPos.y + 1.5 * cubeSize * MAX_Y, cameraPos.z + cubeSize * 5);
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 10.0f, far_plane = 300.0f;
	lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;

	glUseProgram(simpleDepthShader_program);
	glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader_program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	for (int i = env.cameraInCubeX - MAX_ENV_HALF_X + 1; i < env.cameraInCubeX + MAX_ENV_HALF_X - 1; i++) {
		for (int k = env.cameraInCubeZ - MAX_ENV_HALF_Z + 1; k < env.cameraInCubeZ + MAX_ENV_HALF_Z - 1; k++) {
			for (int j = env.cameraInCubeY - MAX_ENV_HALF_Y + 1; j < env.cameraInCubeY + MAX_ENV_HALF_Y - 1; j++) {
				int cubeType = getMapData(i, j, k);

				if (cubeType != CUBE_AIR) {
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(cubeSize * (i), cubeSize * (j), cubeSize * (k)));
					model = glm::scale(model, glm::vec3(cubeSize / 2, cubeSize / 2, cubeSize / 2));

					//glUseProgram(simpleDepthShader_program);
					glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));

					switch (cubeType) {
					case CUBE_STONE: draw_cube_stone(cube_vertexes, i, j, k); break;
					case CUBE_EARTH: draw_cube_earth(cube_vertexes, i, j, k); break;
					case CUBE_GRASS: draw_cube_grass(cube_vertexes, i, j, k); break;
					case CUBE_TREE: draw_cube_tree(cube_vertexes, i, j, k); break;
					case CUBE_LEAF: draw_cube_leaf(cube_vertexes, i, j, k); break;
					default: break;
					}
				}
			}
		}
	}

	for (int i = 0; i < MAX_PIG; i++) {
		if (pigs[i].animalCoordinate.x > env.cameraInCubeX - MAX_ENV_HALF_X && pigs[i].animalCoordinate.x < env.cameraInCubeX + MAX_ENV_HALF_X - 1
			&& pigs[i].animalCoordinate.z > env.cameraInCubeZ - MAX_ENV_HALF_Z && pigs[i].animalCoordinate.z < env.cameraInCubeZ + MAX_ENV_HALF_Z - 1)
				draw_cube_pig(cube_vertexes, pigs[i], 1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	int screenWidth = 800;
	int screenHeight = 555;
	glViewport(0, 0, viewport_width, viewport_height);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader_program);
	glm::mat4 _projection = glm::perspective(glm::radians(45.0f), (float)viewport_width / (float)viewport_height, 0.1f, 100.0f);
	glm::mat4 _view = glm::lookAt(viewPos, viewCenter, viewUp);
	glm::mat4 _model = glm::mat4(1.0f);

	glUniform1i(glGetUniformLocation(shader_program, "texture_sampler"), 0);
	glUniform1i(glGetUniformLocation(shader_program, "shadowMap"), 1);

	glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, glm::value_ptr(_view));
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(_model));

	glUniform3fv(glGetUniformLocation(shader_program, "viewPos"), 1, glm::value_ptr(viewPos));
	glUniform3fv(glGetUniformLocation(shader_program, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	for (int i = env.cameraInCubeX - MAX_ENV_HALF_X + 1; i < env.cameraInCubeX + MAX_ENV_HALF_X - 1; i++) {
		for (int k = env.cameraInCubeZ - MAX_ENV_HALF_Z + 1; k < env.cameraInCubeZ + MAX_ENV_HALF_Z - 1; k++) {
			for (int j = env.cameraInCubeY - MAX_ENV_HALF_Y + 1; j < env.cameraInCubeY + MAX_ENV_HALF_Y - 1; j++) {
				int cubeType = getMapData(i, j, k);

				if (cubeType != CUBE_AIR) {
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(cubeSize * (i), cubeSize * (j), cubeSize * (k)));
					model = glm::scale(model, glm::vec3(cubeSize / 2, cubeSize / 2, cubeSize / 2));

					//glUseProgram(shader_program);
					glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));

					switch (cubeType) {
					case CUBE_STONE: draw_cube_stone(cube_vertexes, i, j, k); break;
					case CUBE_EARTH: draw_cube_earth(cube_vertexes, i, j, k); break;
					case CUBE_CLOUD: draw_cube_cloud(cube_vertexes, i, j, k); break;
					case CUBE_GRASS: draw_cube_grass(cube_vertexes, i, j, k); break;
					case CUBE_TREE: draw_cube_tree(cube_vertexes, i, j, k); break;
					case CUBE_LEAF: draw_cube_leaf(cube_vertexes, i, j, k); break;
					case CUBE_FENCE: draw_cube_fence(cube_vertexes, i, j, k); break;
					//case CUBE_WATER: draw_cube_water(cube_vertexes, i, j, k); break;
					default: break;
					}
				}
			}
		}
	}

	for (int i = 0; i < MAX_PIG; i++) {
		if (pigs[i].animalCoordinate.x > env.cameraInCubeX - MAX_ENV_HALF_X && pigs[i].animalCoordinate.x < env.cameraInCubeX + MAX_ENV_HALF_X - 1
			&& pigs[i].animalCoordinate.z > env.cameraInCubeZ - MAX_ENV_HALF_Z && pigs[i].animalCoordinate.z < env.cameraInCubeZ + MAX_ENV_HALF_Z - 1)
				draw_cube_pig(cube_vertexes, pigs[i], 2);
		pigs[i].animalMove();
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);


	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(shader_program_water);

	glUniform1i(glGetUniformLocation(shader_program_water, "texture_sampler"), 0);
	glUniform1i(glGetUniformLocation(shader_program_water, "shadowMap"), 1);

	glUniformMatrix4fv(glGetUniformLocation(shader_program_water, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_program_water, "view"), 1, GL_FALSE, glm::value_ptr(_view));
	glUniformMatrix4fv(glGetUniformLocation(shader_program_water, "model"), 1, GL_FALSE, glm::value_ptr(_model));

	glUniform3fv(glGetUniformLocation(shader_program_water, "viewPos"), 1, glm::value_ptr(viewPos));
	glUniform3fv(glGetUniformLocation(shader_program_water, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniformMatrix4fv(glGetUniformLocation(shader_program_water, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	for (int i = env.cameraInCubeX - MAX_ENV_HALF_X + 1; i < env.cameraInCubeX + MAX_ENV_HALF_X - 1; i++) {
		for (int k = env.cameraInCubeZ - MAX_ENV_HALF_Z + 1; k < env.cameraInCubeZ + MAX_ENV_HALF_Z - 1; k++) {
			for (int j = env.cameraInCubeY - MAX_ENV_HALF_Y + 1; j < env.cameraInCubeY + MAX_ENV_HALF_Y - 1; j++) {
				int cubeType = getMapData(i, j, k);

				if (cubeType == CUBE_WATER || (cubeType >= CUBE_WATER_MIN && cubeType <= CUBE_WATER_MAX)) {
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(cubeSize * (i), cubeSize * (j), cubeSize * (k)));
					model = glm::scale(model, glm::vec3(cubeSize / 2, cubeSize / 2, cubeSize / 2));

					//glUseProgram(shader_program);
					glUniformMatrix4fv(glGetUniformLocation(shader_program_water, "model"), 1, GL_FALSE, glm::value_ptr(model));

					draw_cube_water(cube_vertexes, i, j, k);

				}
			}
		}
	}

	static int needFlowWater = 0;
	needFlowWater++;
	if (needFlowWater > 30) {
		waterFlow();
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	glDeleteTextures(1, &depthMap);
	//shader part end
}

bool needToDraw(int optType) {
	if (optType == CUBE_AIR) {
		return true;
	}
	if (optType == CUBE_FENCE) {
		return true;
	}
	if (optType == CUBE_WATER) {
		return true;
	}
	if (optType == CUBE_PIG) {
		return true;
	}
	if (optType >= CUBE_WATER_MIN && optType <= CUBE_WATER_MAX) {
		return true;
	}
	return false;
}

void GL_PAINTER::draw_cube_fence(GLfloat *ver, int i, int j, int k) {


	double u, v, size;
	UTILS::uv_map(0, 1, u, v, size);

	static const GLfloat uv[] = {
		// side * 6
		u + size,	v + size,	0,
		u + size,	v,			0,
		u,			v,			0,
		u,			v + size,	0,
	};

	// set uv cord
	// 6 sides
	for (int i = 0; i < 6; i++) {
		// 4 vertexes
		for (int j = 0; j < 4; j++) {
			ver[(i * 4 + j) * 6 + 3] = uv[j * 3];
			ver[(i * 4 + j) * 6 + 4] = uv[j * 3 + 1];
		}
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 144, ver, GL_STATIC_DRAW);

	glm::mat4 model;
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(cubeSize * (i), cubeSize * (j - 0.2), cubeSize * (k)));
	model = glm::scale(model, glm::vec3(cubeSize / 2 *  0.1 , cubeSize / 2 * 0.8, cubeSize / 2 * 0.1));
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	for (int a = 0; a < 6; a++) {
		glDrawArrays(GL_TRIANGLE_FAN, 4*a, 4);
	}

	if (getMapData(i - 1, j, k) == CUBE_FENCE) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(cubeSize * (i - 0.33), cubeSize * (j - 0.2), cubeSize * (k)));
		model = glm::scale(model, glm::vec3(cubeSize / 2 * 0.1, cubeSize / 2 * 0.8, cubeSize / 2 * 0.1));
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		for (int a = 0; a < 6; a++) {
			glDrawArrays(GL_TRIANGLE_FAN, 4 * a, 4);
		}
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(cubeSize * (i - 0.25), cubeSize * (j - 0.1), cubeSize * (k)));
		model = glm::scale(model, glm::vec3(cubeSize / 2 * 0.5, cubeSize / 2 * 0.08, cubeSize / 2 * 0.08));
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		for (int a = 0; a < 6; a++) {
			glDrawArrays(GL_TRIANGLE_FAN, 4 * a, 4);
		}
	}
	if (getMapData(i + 1, j, k) == CUBE_FENCE) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(cubeSize * (i + 0.33), cubeSize * (j - 0.2), cubeSize * (k)));
		model = glm::scale(model, glm::vec3(cubeSize / 2 * 0.1, cubeSize / 2 * 0.8, cubeSize / 2 * 0.1));
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		for (int a = 0; a < 6; a++) {
			glDrawArrays(GL_TRIANGLE_FAN, 4 * a, 4);
		}
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(cubeSize * (i + 0.25), cubeSize * (j - 0.1), cubeSize * (k)));
		model = glm::scale(model, glm::vec3(cubeSize / 2 * 0.5, cubeSize / 2 * 0.08, cubeSize / 2 * 0.08));
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		for (int a = 0; a < 6; a++) {
			glDrawArrays(GL_TRIANGLE_FAN, 4 * a, 4);
		}
	}
	if (getMapData(i, j, k - 1) == CUBE_FENCE) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(cubeSize * (i), cubeSize * (j - 0.2), cubeSize * (k - 0.33)));
		model = glm::scale(model, glm::vec3(cubeSize / 2 * 0.1, cubeSize / 2 * 0.8, cubeSize / 2 * 0.1));
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		for (int a = 0; a < 6; a++) {
			glDrawArrays(GL_TRIANGLE_FAN, 4 * a, 4);
		}
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(cubeSize * (i), cubeSize * (j - 0.1), cubeSize * (k - 0.25)));
		model = glm::scale(model, glm::vec3(cubeSize / 2 * 0.08, cubeSize / 2 * 0.08, cubeSize / 2 * 0.5));
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		for (int a = 0; a < 6; a++) {
			glDrawArrays(GL_TRIANGLE_FAN, 4 * a, 4);
		}
	}
	if (getMapData(i, j, k + 1) == CUBE_FENCE) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(cubeSize * (i), cubeSize * (j - 0.2), cubeSize * (k + 0.33)));
		model = glm::scale(model, glm::vec3(cubeSize / 2 * 0.1, cubeSize / 2 * 0.8, cubeSize / 2 * 0.1));
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		for (int a = 0; a < 6; a++) {
			glDrawArrays(GL_TRIANGLE_FAN, 4 * a, 4);
		}
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(cubeSize * (i), cubeSize * (j - 0.1), cubeSize * (k + 0.25)));
		model = glm::scale(model, glm::vec3(cubeSize / 2 * 0.08, cubeSize / 2 * 0.08, cubeSize / 2 * 0.5));
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		for (int a = 0; a < 6; a++) {
			glDrawArrays(GL_TRIANGLE_FAN, 4 * a, 4);
		}
	}

}

void GL_PAINTER::draw_cube_cloud(GLfloat *ver, int i, int j, int k) {
	bool _opt_if_continue = false;
	bool _opt_is_air[6] = {
		needToDraw(getMapData(i, j, k - 1)),
		needToDraw(getMapData(i, j, k + 1)),
		needToDraw(getMapData(i + 1, j, k)),
		needToDraw(getMapData(i - 1, j, k)),
		needToDraw(getMapData(i, j + 1, k)),
		needToDraw(getMapData(i, j - 1, k))
	};
	for (int i = 0; i < 6; i++)
		_opt_if_continue = _opt_if_continue || _opt_is_air[i];
	if (!_opt_if_continue) return;

	double u, v, size;
	UTILS::uv_map(3, 3, u, v, size);

	static const GLfloat uv[] = {
		// side * 6
		u + size,	v + size,	0,
		u + size,	v,			0,
		u,			v,			0,
		u,			v + size,	0,
	};

	// set uv cord
	// 6 sides
	for (int i = 0; i < 6; i++) {
		// 4 vertexes
		for (int j = 0; j < 4; j++) {
			ver[(i * 4 + j) * 6 + 3] = uv[j * 3];
			ver[(i * 4 + j) * 6 + 4] = uv[j * 3 + 1];
		}
	}
	// rebuffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 144, ver, GL_STATIC_DRAW);
	// bottom
	if (_opt_is_air[0]) glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	// top
	if (_opt_is_air[1]) glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	if (_opt_is_air[2])	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	if (_opt_is_air[3]) glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	if (_opt_is_air[4]) glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	if (_opt_is_air[5]) glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
}
void GL_PAINTER::draw_cube_sun(GLfloat *ver, int i, int j, int k) {
	bool _opt_if_continue = false;
	bool _opt_is_air[6] = {
		needToDraw(getMapData(i, j, k - 1)),
		needToDraw(getMapData(i, j, k + 1)),
		needToDraw(getMapData(i + 1, j, k)),
		needToDraw(getMapData(i - 1, j, k)),
		needToDraw(getMapData(i, j + 1, k)),
		needToDraw(getMapData(i, j - 1, k))
	};
	for (int i = 0; i < 6; i++)
		_opt_if_continue = _opt_if_continue || _opt_is_air[i];
	if (!_opt_if_continue) return;

	double u, v, size;
	UTILS::uv_map(1, 2, u, v, size);
	static const GLfloat uv[] = {
		// side * 6
		u + size,	v + size,	0,
		u + size,	v,			0,
		u,			v,			0,
		u,			v + size,	0,
	};

	// set uv cord
	// 6 sides
	for (int i = 0; i < 6; i++) {
		// 4 vertexes
		for (int j = 0; j < 4; j++) {
			ver[(i * 4 + j) * 6 + 3] = uv[j * 3];
			ver[(i * 4 + j) * 6 + 4] = uv[j * 3 + 1];
		}
	}
	// rebuffer data
	// bottom
	if (_opt_is_air[0]) glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	// top
	if (_opt_is_air[1]) glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	if (_opt_is_air[2])	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	if (_opt_is_air[3]) glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	if (_opt_is_air[4]) glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	if (_opt_is_air[5]) glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
}
void GL_PAINTER::draw_cube_earth(GLfloat *ver, int i, int j, int k) {
	bool _opt_if_continue = false;
	bool _opt_is_air[6] = {
		needToDraw(getMapData(i, j, k - 1)),
		needToDraw(getMapData(i, j, k + 1)),
		needToDraw(getMapData(i + 1, j, k)),
		needToDraw(getMapData(i - 1, j, k)),
		needToDraw(getMapData(i, j + 1, k)),
		needToDraw(getMapData(i, j - 1, k))
	};
	for (int i = 0; i < 6; i++)
		_opt_if_continue = _opt_if_continue || _opt_is_air[i];
	if (!_opt_if_continue) return;

	double u, v, size;
	UTILS::uv_map(0, 1, u, v, size);
	static const GLfloat uv[] = {
		// side * 6
		u + size,	v + size,	0,
		u + size,	v,			0,
		u,			v,			0,
		u,			v + size,	0,
	};

	// set uv cord
	// 6 sides
	for (int i = 0; i < 6; i++) {
		// 4 vertexes
		for (int j = 0; j < 4; j++) {
			ver[(i * 4 + j) * 6 + 3] = uv[j * 3];
			ver[(i * 4 + j) * 6 + 4] = uv[j * 3 + 1];
		}
	}
	// rebuffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 144, ver, GL_STATIC_DRAW);
	// bottom
	if (_opt_is_air[0]) glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	// top
	if (_opt_is_air[1]) glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	if (_opt_is_air[2])	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	if (_opt_is_air[3]) glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	if (_opt_is_air[4]) glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	if (_opt_is_air[5]) glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
}
void GL_PAINTER::draw_cube_stone(GLfloat *ver, int i, int j, int k) {
	bool _opt_if_continue = false;
	bool _opt_is_air[6] = {
		needToDraw(getMapData(i, j, k - 1)),
		needToDraw(getMapData(i, j, k + 1)),
		needToDraw(getMapData(i + 1, j, k)),
		needToDraw(getMapData(i - 1, j, k)),
		needToDraw(getMapData(i, j + 1, k)),
		needToDraw(getMapData(i, j - 1, k))
	};
	for (int i = 0; i < 6; i++)
		_opt_if_continue = _opt_if_continue || _opt_is_air[i];
	if (!_opt_if_continue) return;

	double u, v, size;
	UTILS::uv_map(0, 0, u, v, size);

	static const GLfloat uv[] = {
		// side * 6
		u + size,	v + size,	0,
		u + size,	v,			0,
		u,			v,			0,
		u,			v + size,	0,
	};

	// set uv cord
	// 6 sides
	for (int i = 0; i < 6; i++) {
		// 4 vertexes
		for (int j = 0; j < 4; j++) {
			ver[(i * 4 + j) * 6 + 3] = uv[j * 3];
			ver[(i * 4 + j) * 6 + 4] = uv[j * 3 + 1];
		}
	}
	// rebuffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 144, ver, GL_STATIC_DRAW);
	// bottom
	if(_opt_is_air[0]) glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	// top
	if (_opt_is_air[1]) glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	if (_opt_is_air[2])	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	if (_opt_is_air[3]) glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	if (_opt_is_air[4]) glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	if (_opt_is_air[5]) glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
}
void GL_PAINTER::draw_cube_tree(GLfloat *ver, int i, int j, int k) {
	bool _opt_if_continue = false;
	bool _opt_is_air[6] = {
		needToDraw(getMapData(i, j, k - 1)),
		needToDraw(getMapData(i, j, k + 1)),
		needToDraw(getMapData(i + 1, j, k)),
		needToDraw(getMapData(i - 1, j, k)),
		needToDraw(getMapData(i, j + 1, k)),
		needToDraw(getMapData(i, j - 1, k))
	};
	for (int i = 0; i < 6; i++)
		_opt_if_continue = _opt_if_continue || _opt_is_air[i];
	if (!_opt_if_continue) return;

	double u, v, size;
	UTILS::uv_map(1, 0, u, v, size);

	double u1, v1, size1;
	UTILS::uv_map(1, 1, u1, v1, size1);
	static const GLfloat uv[] = {
		// side * 6
		u + size,	v + size,	0,
		u + size,	v,			0,
		u,			v,			0,
		u,			v + size,	0,
	};

	static const GLfloat uv1[] = {
		// side * 6
		u1 + size1,	v1 + size1,	0,
		u1 + size1,	v1,			0,
		u1,			v1,			0,
		u1,			v1 + size1,	0,
	};

	// set uv cord
	// 6 sides
	for (int i = 4; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			ver[(i * 4 + j) * 6 + 3] = uv1[j * 3];
			ver[(i * 4 + j) * 6 + 4] = uv1[j * 3 + 1];
		}
	}
	for (int i = 0; i < 4; i++) {
		// 4 vertexes
		for (int j = 0; j < 4; j++) {
			ver[(i * 4 + j) * 6 + 3] = uv[j * 3];
			ver[(i * 4 + j) * 6 + 4] = uv[j * 3 + 1];
		}
	}
	// rebuffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 144, ver, GL_STATIC_DRAW);
	// bottom
	if (_opt_is_air[0]) glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	// top
	if (_opt_is_air[1]) glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	if (_opt_is_air[2])	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	if (_opt_is_air[3]) glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	if (_opt_is_air[4]) glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	if (_opt_is_air[5]) glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
}
void GL_PAINTER::draw_cube_grass(GLfloat *ver, int i, int j, int k) {
	bool _opt_if_continue = false;
	bool _opt_is_air[6] = {
		needToDraw(getMapData(i, j, k - 1)),
		needToDraw(getMapData(i, j, k + 1)),
		needToDraw(getMapData(i + 1, j, k)),
		needToDraw(getMapData(i - 1, j, k)),
		needToDraw(getMapData(i, j + 1, k)),
		needToDraw(getMapData(i, j - 1, k))
	};
	for (int i = 0; i < 6; i++)
		_opt_if_continue = _opt_if_continue || _opt_is_air[i];
	if (!_opt_if_continue) return;

	double u, v, size;
	UTILS::uv_map(2, 1, u, v, size);

	static const GLfloat uv[] = {
		// side * 6
		u + size,	v + size,	0,
		u + size,	v,			0,
		u,			v,			0,
		u,			v + size,	0,
	};

	double u1, v1, size1;
	UTILS::uv_map(2, 2, u1, v1, size1);
	static const GLfloat uv1[] = {
		// side * 6
		u1 + size1,	v1 + size1,	0,
		u1 + size1,	v1,			0,
		u1,			v1,			0,
		u1,			v1 + size1,	0,
	};

	double u2, v2, size2;
	UTILS::uv_map(2, 0, u2, v2, size2);
	static const GLfloat uv2[] = {
		// side * 6
		u2 + size2,	v2 + size2,	0,
		u2 + size2,	v2,			0,
		u2,			v2,			0,
		u2,			v2 + size2,	0,
	};

	// set uv cord
	// 6 sides
	for (int i = 0; i < 4; i++) {
		// 4 vertexes
		for (int j = 0; j < 4; j++) {
			ver[(i * 4 + j) * 6 + 3] = uv1[j * 3];
			ver[(i * 4 + j) * 6 + 4] = uv1[j * 3 + 1];
		}
	}
	
		
	for (int j = 0; j < 4; j++) {
		ver[(4 * 4 + j) * 6 + 3] = uv[j * 3];
		ver[(4 * 4 + j) * 6 + 4] = uv[j * 3 + 1];
	}
		
	for (int j = 0; j < 4; j++){
		ver[(5 * 4 + j) * 6 + 3] = uv2[j * 3];
		ver[(5 * 4 + j) * 6 + 4] = uv2[j * 3 + 1];
	}
	
	// rebuffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 144, ver, GL_STATIC_DRAW);
	// bottom
	if (_opt_is_air[0]) glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	// top
	if (_opt_is_air[1]) glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	if (_opt_is_air[2])	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	if (_opt_is_air[3]) glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	if (_opt_is_air[4]) glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	if (_opt_is_air[5]) glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
}
void GL_PAINTER::draw_cube_leaf(GLfloat *ver, int i, int j, int k) {
	bool _opt_if_continue = false;
	bool _opt_is_air[6] = {
		needToDraw(getMapData(i, j, k - 1)),
		needToDraw(getMapData(i, j, k + 1)),
		needToDraw(getMapData(i + 1, j, k)),
		needToDraw(getMapData(i - 1, j, k)),
		needToDraw(getMapData(i, j + 1, k)),
		needToDraw(getMapData(i, j - 1, k))
	};
	for (int i = 0; i < 6; i++)
		_opt_if_continue = _opt_if_continue || _opt_is_air[i];
	if (!_opt_if_continue) return;

	double u, v, size;
	UTILS::uv_map(3, 1, u, v, size);

	static const GLfloat uv[] = {
		// side * 6
		u + size,	v + size,	0,
		u + size,	v,			0,
		u,			v,			0,
		u,			v + size,	0,
	};

	// set uv cord
	// 6 sides
	for (int i = 0; i < 6; i++) {
		// 4 vertexes
		for (int j = 0; j < 4; j++) {
			ver[(i * 4 + j) * 6 + 3] = uv[j * 3];
			ver[(i * 4 + j) * 6 + 4] = uv[j * 3 + 1];
		}
	}
	// rebuffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 144, ver, GL_STATIC_DRAW);
	// bottom
	if (_opt_is_air[0]) glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	// top
	if (_opt_is_air[1]) glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	if (_opt_is_air[2])	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	if (_opt_is_air[3]) glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	if (_opt_is_air[4]) glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	if (_opt_is_air[5]) glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
}




bool needToDrawWater(int optType) {
	if (optType == CUBE_AIR) {
		return true;
	}
	if (optType == CUBE_WATER) {
		return false;
	}
	return false;
}

float waterHeight(int type) {
	if (type == CUBE_WATER) {
		return 1.0;
	}
	if (type >= CUBE_WATER_MIN && type <= CUBE_WATER_MAX) {
		return 1.0 * (type - CUBE_WATER_MIN) / (CUBE_WATER_MAX - CUBE_WATER_MIN) * 2.0 - 1.0;
	}
	return -1.0;
}

GLfloat waterPoint[] = {
	1.0,-1.0,1.0,
	1.0,-1.0,-1.0,
	-1.0,-1.0,-1.0,
	-1.0,-1.0,1.0,
	-1.0,1.0,1.0,
	-1.0,1.0,-1.0,
	1.0,1.0,-1.0,
	1.0,1.0,1.0,
	0.0,1.0,0.0
};

const int waterPlain[] = {
	1,2,5, 5,6,1,
	0,3,4, 4,7,0,
	0,1,6, 6,7,0,
	2,3,4, 4,5,2,
	0,1,2, 2,3,0,
	4,5,8, 5,6,8, 6,7,8, 7,4,8
};

const glm::vec3 waterNormal[]{
	glm::vec3(0.0,0.0,-1.0),
	glm::vec3(0.0,0.0,1.0),
	glm::vec3(1.0,0.0,0.0),
	glm::vec3(-1.0,0.0,0.0),
	glm::vec3(0.0,-1.0,0.0)
};

GLfloat waterVertice[6 * 3 * 2 * 6];

void copyWaterPoint(int vi, int pi) {
	waterVertice[vi * 6 + 0] = waterPoint[pi * 3 + 0];
	waterVertice[vi * 6 + 1] = waterPoint[pi * 3 + 1];
	waterVertice[vi * 6 + 2] = waterPoint[pi * 3 + 2];
}

void setWaterPointNormal(int vi, const glm::vec3 &normal) {
	waterVertice[vi * 6 + 3] = normal.x;
	waterVertice[vi * 6 + 4] = normal.y;
	waterVertice[vi * 6 + 5] = normal.z;
}

void GL_PAINTER::draw_cube_water(GLfloat *ver, int i, int j, int k) {
	bool _opt_if_continue = false;
	bool _opt_is_air[6] = {
		needToDrawWater(getMapData(i, j, k - 1)),
		needToDrawWater(getMapData(i, j, k + 1)),
		needToDrawWater(getMapData(i + 1, j, k)),
		needToDrawWater(getMapData(i - 1, j, k)),
		needToDrawWater(getMapData(i, j - 1, k)),
		//顶部
		needToDrawWater(getMapData(i, j + 1, k))
	};
	for (int i = 0; i < 6; i++)
		_opt_if_continue = _opt_if_continue || _opt_is_air[i];
	if (!_opt_if_continue) return;

	float h = waterHeight(getMapData(i, j, k));
	waterPoint[4 * 3 + 1] = waterPoint[5 * 3 + 1] = waterPoint[6 * 3 + 1] = waterPoint[7 * 3 + 1] = h;
	h = waterHeight(getMapData(i - 1, j, k + 1));
	if (waterPoint[4 * 3 + 1] < h) waterPoint[4 * 3 + 1] = h;
	h = waterHeight(getMapData(i - 1, j, k - 1));
	if (waterPoint[5 * 3 + 1] < h) waterPoint[5 * 3 + 1] = h;
	h = waterHeight(getMapData(i + 1, j, k - 1));
	if (waterPoint[6 * 3 + 1] < h) waterPoint[6 * 3 + 1] = h;
	h = waterHeight(getMapData(i + 1, j, k + 1));
	if (waterPoint[7 * 3 + 1] < h) waterPoint[7 * 3 + 1] = h;
	h = waterHeight(getMapData(i - 1, j, k));
	if (waterPoint[4 * 3 + 1] < h) waterPoint[4 * 3 + 1] = h;
	if (waterPoint[5 * 3 + 1] < h) waterPoint[5 * 3 + 1] = h;
	h = waterHeight(getMapData(i + 1, j, k));
	if (waterPoint[6 * 3 + 1] < h) waterPoint[6 * 3 + 1] = h;
	if (waterPoint[7 * 3 + 1] < h) waterPoint[7 * 3 + 1] = h;
	h = waterHeight(getMapData(i, j, k - 1));
	if (waterPoint[5 * 3 + 1] < h) waterPoint[5 * 3 + 1] = h;
	if (waterPoint[6 * 3 + 1] < h) waterPoint[6 * 3 + 1] = h;
	h = waterHeight(getMapData(i, j, k + 1));
	if (waterPoint[4 * 3 + 1] < h) waterPoint[4 * 3 + 1] = h;
	if (waterPoint[7 * 3 + 1] < h) waterPoint[7 * 3 + 1] = h;
	waterPoint[8 * 3 + 1] = (waterPoint[4 * 3 + 1] + waterPoint[5 * 3 + 1] + waterPoint[6 * 3 + 1] + waterPoint[7 * 3 + 1]) / 4.0;

	double u, v, size;
	UTILS::uv_map(3, 0, u, v, size);

	int pn = 0;
	for (int pin = 0; pin < 5; pin++) {
		if (_opt_is_air[pin]) {


			for (int a = 0; a < 6; a++) {
				copyWaterPoint(pn + a, waterPlain[pin * 6 + a]);
				setWaterPointNormal(pn + a, waterNormal[pin]);
			}
			pn += 6;

		}
	}

	if (_opt_is_air[5] || (getMapData(i, j, k) >= CUBE_WATER_MIN && getMapData(i, j, k) < CUBE_WATER_MAX)) {
		//注：
		//getMapData(i, j, k) < CUBE_WATER_MAX
		//如果水不满，则要绘制
		for (int b = 10; b < 14; b++) {
			glm::vec3 v1, v2, vn;
			for (int a = 0; a < 3; a++) {
				copyWaterPoint(pn + a, waterPlain[b * 3 + a]);
			}
			v1 = glm::vec3(waterVertice[(pn + 1) * 6 + 0] - waterVertice[(pn + 0) * 6 + 0], waterVertice[(pn + 1) * 6 + 1] - waterVertice[(pn + 0) * 6 + 1], waterVertice[(pn + 1) * 6 + 2] - waterVertice[(pn + 0) * 6 + 2]);
			v2 = glm::vec3(waterVertice[(pn + 1) * 6 + 0] - waterVertice[(pn + 2) * 6 + 0], waterVertice[(pn + 1) * 6 + 1] - waterVertice[(pn + 2) * 6 + 1], waterVertice[(pn + 1) * 6 + 2] - waterVertice[(pn + 2) * 6 + 2]);
			vn = glm::vec3(v1.y * v2.z - v2.y * v1.z, v1.z * v2.x - v2.z * v1.x, v1.x * v2.y - v2.x * v1.y);
			for (int a = 0; a < 3; a++) {
				setWaterPointNormal(pn + a, vn);
			}
			pn += 3;
		}
	}

	/*static const GLfloat uv[] = {
	// side * 6
	u + size,	v + size,	0,
	u + size,	v,			0,
	u,			v,			0,
	u + size,	v + size,	0,
	u,			v + size,	0,
	u,			v,			0
	};

	// set uv cord
	// 6 sides
	for (int i = 0; i < 6; i++) {
	// 4 vertexes
	for (int j = 0; j < 6; j++) {
	waterVertice[(i * 4 + j) * 6 + 3] = uv[j * 3];
	waterVertice[(i * 4 + j) * 6 + 4] = uv[j * 3 + 1];
	}
	}*/


	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * pn, waterVertice, GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, pn);

}

void GL_PAINTER::waterFlow() {
	int cx = cameraPos.x / cubeSize;
	int cy = cameraPos.y / cubeSize;
	int cz = cameraPos.z / cubeSize;

	static int dl = 1;
	dl = 1 - dl;

	const int dlx = 30;
	const int dly = 15;
	const int dlz = 30;

	for (int i = cx - dlx; i <= cx + dlx; i++) {
		for (int k = cz - dlz; k <= cz + dlz; k++) {
			int j = cy - dly;
			if ((i + j + k) % 2 != dl) {
				j++;
			}
			for (; j < cy + dly; j++) {
				int type = getMapData(i, j, k);
				if (type == CUBE_AIR || (type >= CUBE_WATER_MIN && type <= CUBE_WATER_MAX)) {
					int nt = CUBE_AIR;
					int ot;
					ot = getMapData(i, j + 1, k);
					if (ot == CUBE_WATER || (ot >= CUBE_WATER_MIN && ot <= CUBE_WATER_MAX)) {
						nt = CUBE_WATER_MAX;
					}
					int ots[4] = { ((getMapData(i - 1, j - 1, k)>0) ? getMapData(i - 1, j, k) : CUBE_AIR),
						((getMapData(i + 1, j - 1, k)>0) ? getMapData(i + 1, j, k) : CUBE_AIR),
						((getMapData(i, j - 1, k - 1)>0) ? getMapData(i, j, k - 1) : CUBE_AIR),
						((getMapData(i, j - 1, k + 1)>0) ? getMapData(i, j, k + 1) : CUBE_AIR) };
					for (int ind = 0; ind < 4; ind++) {
						if (ots[ind] == CUBE_WATER) {
							nt = CUBE_WATER_MAX - 1;
						}
						else if (ots[ind] > CUBE_WATER_MIN && ots[ind] <= CUBE_WATER_MAX) {
							if (ots[ind] - 1 > nt || nt == CUBE_AIR) {
								nt = ots[ind] - 1;
							}
						}
					}
					setMapData(i, j, k, nt);
				}
			}
		}
	}
}

void GL_PAINTER::draw_cube_pig(GLfloat *ver, Animal pig, int time) {
	float angle = pig.animalForwardAngle;
	//if (angle > 90.0f) angle -= 90.0f;
	//else if (angle < -90.0f) angle += 90.0f;

	//head
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, pig.animalPos);
	model = glm::rotate(model, glm::radians(pig.animalForwardAngle), glm::vec3(0.0, 1.0, 0.0));
	model = glm::translate(model, glm::vec3(1.5, 0.0, 0.0));
	model = glm::scale(model, glm::vec3(cubeSize / 2.2, cubeSize / 2.2, cubeSize / 2.2));
	if (time == 1) {
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	}
	else {
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	}
	double u, u2, v, v2, size;
	UTILS::uv_map(0, 2, u, v, size);
	UTILS::uv_map(0, 3, u2, v2, size);

	const GLfloat uv[] = {
		// side * 6
		u + size,	v + size,	0,
		u + size,	v,			0,
		u,			v,			0,
		u,			v + size,	0,
	};
	const GLfloat uv2[] = {
		// side * 6
		u2 + size,	v2 + size,	0,
		u2 + size,	v2,			0,
		u2,			v2,			0,
		u2,			v2 + size,	0,
	};

	// set uv cord
	// 6 sides
	for (int i = 0; i < 6; i++) {
		// 4 vertexes
		for (int j = 0; j < 4; j++) {
			ver[(i * 4 + j) * 6 + 3] = uv2[j * 3];
			ver[(i * 4 + j) * 6 + 4] = uv2[j * 3 + 1];
		}
	}
	for (int j = 0; j < 4; j++) {
		ver[(2 * 4 + j) * 6 + 3] = uv[j * 3];
		ver[(2 * 4 + j) * 6 + 4] = uv[j * 3 + 1];
	}
	// rebuffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 144, ver, GL_STATIC_DRAW);
	// bottom
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	// top
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	//body


	model = glm::mat4(1.0f);

	model = glm::translate(model, pig.animalPos);
	model = glm::rotate(model, glm::radians(pig.animalForwardAngle), glm::vec3(0.0, 1.0, 0.0));
	//model = glm::scale(model, glm::vec3(cubeSize / 2.6, cubeSize / 2.6, cubeSize / 1.3));
	model = glm::scale(model, glm::vec3(cubeSize / 1.3, cubeSize / 2.6, cubeSize / 2.6));
	if (time == 1) {
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	}
	else {
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	}


	// set uv cord
	// 6 sides
	for (int i = 0; i < 6; i++) {
		// 4 vertexes
		for (int j = 0; j < 4; j++) {
			ver[(i * 4 + j) * 6 + 3] = uv2[j * 3];
			ver[(i * 4 + j) * 6 + 4] = uv2[j * 3 + 1];
		}
	}
	// rebuffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 144, ver, GL_STATIC_DRAW);
	// bottom
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	// top
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	//if (time == 2) pig.animalMove();
	/*string s = UTILS::float_to_string(angle) + ", " + UTILS::float_to_string(pig.animalFront.x) + ", " + UTILS::float_to_string(pig.animalFront.z);
	UTILS::logd(s.c_str());*/
}