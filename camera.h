#pragma once
#include "global.h"

class Camera
{
public:
	Camera();
	~Camera();
	// �������
	static void cameraInit(); // �����ʼ��
	static void cameraResize(int newWidth, int newHeight);

	// �����غ���
	static void cameraMove(int keyInput); // ���λ���ƶ�
	static void cameraRotate(int xpos, int ypos); // �����Ұ�Ƕ���ת

    // ����������������
	static bool cameraHit(); // ��ײ���
	static bool cameraPlat(); // վ��ƽ����
	static void cameraFall(); // ��������׹

	// ��ȡ�����λ��
	static glm::vec3 getEye();
	static glm::vec3 getCenter();
	static glm::vec3 getUp();

	// ���д��debug��
	static void cameraTranslate(int xpos, int ypos);

	static float cameraFallSpeed;
};

