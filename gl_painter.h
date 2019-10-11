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

// 用来编译shader的类
class Shader {

public:
	Shader(const string shader_code, GLenum shader_type);
	GLuint get_shader_ID();

private:
	GLuint _obj;
};

/*
*	这个类中全部是静态函数
*	主要是调用OpenGL现有的函数
*	做一些OpenGL的初始化工作
*	对绘制图像进行了封装
*/
class GL_PAINTER {

public:
	GL_PAINTER();
	// 初始化OpenGL 编译shader等
	static void init();

	// 避免反复初始化
	static bool if_init();

	// 加载纹理
	static GLuint load_BMP_texture(const char *imagepath);
	static GLuint loadCubemap(vector<std::string> faces);

	// paint函数通过参数mode对绘图任务做了映射，可通过mode来控制绘制什么图形
	// attr为该图形的一些属性
	static void paint(GLfloat *attr, int mode);

	//销毁，放置位置
	static bool canUseDestroyCube, canUsePutCube;
	static int destroyCube[3], putCube[3];

private:
	static bool is_init;
	static GLuint VAO, VBO, EBO, skyboxVAO, skyboxVBO, waterVAO, waterVBO;
	static GLuint shader_program, simpleDepthShader_program, shader_program_sb, shader_program_water;
	static GLuint texture, cubemapTexture;
	static GLuint depthMap;
    static GLuint depthMapFBO;

	//计算销毁，放置位置
	static void getDesPutCube();

	// 这里是个玄学... 在使用原生OpenGL绘制时第一次分辨率会有问题
	// 通过做一次无用的绘制解决
	static void draw_init();	// pull the gl_window to right position

	// 演示用三角形
	static void draw_tri();


	// 画一个正方体，调试摄像机用
	static void draw_cube();


	// 环境？
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