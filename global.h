#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include "utils.h"

#define KEY_NONE 0
// 游戏状态，从200开始用
#define GAME_FALLING 200
#define GAME_RUNNING 201
#define GAME_END 202

// 地图长宽高
#define MAX_X 256
#define MAX_Y 32
#define MAX_Z 256

/*
 * 修改、添加或删除以下定义时，已知的需要修改的地方：
 * global.cpp		setCurHolding()
 * utils.cpp		get_cube_name()
*/
// 方块类型，固体大于0，空气等于0，流体和装饰的小于0

#define CUBE_WATER_MIN -15
#define CUBE_WATER_MAX -11
#define CUBE_SUN -3
#define CUBE_CLOUD -2
#define CUBE_WATER -1
#define CUBE_AIR 0
#define CUBE_EARTH 1
#define CUBE_STONE 2
#define CUBE_GRASS 3 //草地
#define CUBE_TREE 4 //树干
#define CUBE_LEAF 5 //树叶
#define CUBE_FENCE 6//栅栏

#define MAX_PIG 5
#define CUBE_PIG 80
#define PIG_MOVE_FORWARD 81
#define PIG_STOP 82
#define PIG_OFFSET 83
#define PIG_ROTATE 84

// 键盘输入消息，2的非负整数次方
#define KEY_W 1
#define KEY_S 2
#define KEY_A 4
#define KEY_D 8
#define KEY_SPACE 16
#define KEY_X 32
#define KEY_ESC 64

// 窗口与控件
#define SIZE_WEIGHT		1.0
#define INIT_WIDTH		500
#define INIT_HEIGHT		500

extern int gameState; // 游戏状态

// 手持方块类型
extern int cubeTypeHolding;

// 地图
extern int mapData[MAX_X][MAX_Y][MAX_Z]; // 地图
extern const float cubeSize; // 方块大小

// 窗口
/*extern int windowWidth; // 窗口宽度
extern int windowHeight; // 窗口高度
extern int windowLeftX, windowTopY; // 窗口左上顶点位置*/
extern int windowCenterX;
extern int windowCenterY; // 窗口中心位置

extern int viewport_width;
extern int viewport_height;

// 相机
extern glm::vec3 cameraPos; // 相机位置
extern glm::vec3 cameraFront; // 相机视野中心前移量，用以确定视野中心
extern glm::vec3 cameraUp; // 相机头偏角向量
extern float cameraForwardAngle; // 相机向前角度
extern float cameraSpeed; // 相机移动速度
extern float cameraYaw; // 相机侧视角度
extern float cameraPitch; // 相机俯仰角度
/*extern glm::vec3 viewPos; // 眼睛位置
extern glm::vec3 viewCenter; // 视野中心
extern glm::vec3 viewUp; // 头偏角度*/

class Animal
{
public:
	Animal();
	Animal(int, int, int);
	Animal(float, float, float);
	~Animal();
	glm::vec3 animalCoordinate;
	glm::vec3 animalPos;
	//glm::vec3 animalFront;
	int animalStepCount;
	int animalState;
	float animalForwardAngle;
	float animalSpeed;
	float animalYaw;

	void animalMoveForward();
	bool animalHit();
	void animalRotate(int type);
	int animalChooseMode();
	void animalMove();
	bool animalOut(int, int, int);
};

extern Animal pigs[MAX_PIG];


//带循环的获得地图数据
extern int getMapData(int i, int j, int k);
//写地图数据
extern void setMapData(int i, int j, int k, int type);
// 更改手持方块类型
extern void setCurHolding(bool forward);