#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>

#include "utils.h"
#include "camera.h"

#define VERTEX_SHADER_FILE		"vertex_shader.glsl"
#define FRAGMENT_SHADER_FILE	"fragment_shader.glsl"
#define SKYBOX_FS_FILE	"skybox_fs.glsl"
#define SKYBOX_VS_FILE	"skybox_vs.glsl"
#define WATER_FS_FILE	"water_fs.glsl"
#define WATER_VS_FILE	"water_vs.glsl"
#define BLOCK_TEXTURE_FILE		"./Resources/block_texture.bmp"
#define SKYBOX_FIFE_1  "./Resources/skybox/right.jpg"
#define SKYBOX_FIFE_2  "./Resources/skybox/left.jpg"
#define SKYBOX_FIFE_3  "./Resources/skybox/top.jpg"
#define SKYBOX_FIFE_4  "./Resources/skybox/bottom.jpg"
#define SKYBOX_FIFE_5  "./Resources/skybox/back.jpg"
#define SKYBOX_FIFE_6  "./Resources/skybox/front.jpg"

using namespace std;

// ��������shader����
class Shader {

public:
	Shader(const string shader_code, GLenum shader_type);
	GLuint get_shader_ID();

private:
	GLuint _obj;
};

/*
*	�������ȫ���Ǿ�̬����
*	��Ҫ�ǵ���OpenGL���еĺ���
*	��һЩOpenGL�ĳ�ʼ������
*	�Ի���ͼ������˷�װ
*/
class GL_PAINTER {

public:
	GL_PAINTER();
	// ��ʼ��OpenGL ����shader��
	static void init();

	// ���ⷴ����ʼ��
	static bool if_init();

	// ��������
	static GLuint load_BMP_texture(const char *imagepath);
	static GLuint loadCubemap(vector<std::string> faces);

	// paint����ͨ������mode�Ի�ͼ��������ӳ�䣬��ͨ��mode�����ƻ���ʲôͼ��
	// attrΪ��ͼ�ε�һЩ����
	static void paint(GLfloat *attr, int mode);

	//���٣�����λ��
	static bool canUseDestroyCube, canUsePutCube;
	static int destroyCube[3], putCube[3];

private:
	static bool is_init;
	static GLuint VAO, VBO, EBO, skyboxVAO, skyboxVBO, waterVAO, waterVBO;
	static GLuint shader_program, simpleDepthShader_program, shader_program_sb, shader_program_water;
	static GLuint texture, cubemapTexture;
	static GLuint depthMap;
    static GLuint depthMapFBO;

	//�������٣�����λ��
	static void getDesPutCube();

	// �����Ǹ���ѧ... ��ʹ��ԭ��OpenGL����ʱ��һ�ηֱ��ʻ�������
	// ͨ����һ�����õĻ��ƽ��
	static void draw_init();	// pull the gl_window to right position

	// ��ʾ��������
	static void draw_tri();


	// ��һ�������壬�����������
	static void draw_cube();


	// ������
	static void draw_skybox();
	static void draw_environment();

	static void draw_cube_fence(GLfloat *ver, int i, int j, int k);
	static void draw_cube_sun(GLfloat *ver, int i, int j, int k);
	static void draw_cube_cloud(GLfloat *ver, int i, int j, int k);
	static void draw_cube_water(GLfloat *ver, int i, int j, int k);
	static void draw_cube_earth(GLfloat *ver, int i, int j, int k);
	static void draw_cube_stone(GLfloat *ver, int i, int j, int k);
	static void draw_cube_grass(GLfloat *ver, int i, int j, int k);
	static void draw_cube_tree(GLfloat *ver, int i, int j, int k);
	static void draw_cube_leaf(GLfloat *ver, int i, int j, int k);
	static void draw_cube_pig(GLfloat *ver, Animal pig, int time);
	static void waterFlow();
};