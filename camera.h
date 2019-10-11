#pragma once
#include "global.h"

class Camera
{
public:
	Camera();
	~Camera();
	// 相机设置
	static void cameraInit(); // 相机初始化
	static void cameraResize(int newWidth, int newHeight);

	// 相机相关函数
	static void cameraMove(int keyInput); // 相机位置移动
	static void cameraRotate(int xpos, int ypos); // 相机视野角度旋转

    // 相机自身的物理引擎
	static bool cameraHit(); // 碰撞检测
	static bool cameraPlat(); // 站在平面上
	static void cameraFall(); // 因重力下坠

	// 获取相机的位置
	static glm::vec3 getEye();
	static glm::vec3 getCenter();
	static glm::vec3 getUp();

	// 随便写，debug用
	static void cameraTranslate(int xpos, int ypos);

	static float cameraFallSpeed;
};

