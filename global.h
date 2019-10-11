#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include "utils.h"

#define KEY_NONE 0
// ��Ϸ״̬����200��ʼ��
#define GAME_FALLING 200
#define GAME_RUNNING 201
#define GAME_END 202

// ��ͼ�����
#define MAX_X 256
#define MAX_Y 32
#define MAX_Z 256

/*
 * �޸ġ���ӻ�ɾ�����¶���ʱ����֪����Ҫ�޸ĵĵط���
 * global.cpp		setCurHolding()
 * utils.cpp		get_cube_name()
*/
// �������ͣ��������0����������0�������װ�ε�С��0

#define CUBE_WATER_MIN -15
#define CUBE_WATER_MAX -11
#define CUBE_SUN -3
#define CUBE_CLOUD -2
#define CUBE_WATER -1
#define CUBE_AIR 0
#define CUBE_EARTH 1
#define CUBE_STONE 2
#define CUBE_GRASS 3 //�ݵ�
#define CUBE_TREE 4 //����
#define CUBE_LEAF 5 //��Ҷ
#define CUBE_FENCE 6//դ��

#define MAX_PIG 5
#define CUBE_PIG 80
#define PIG_MOVE_FORWARD 81
#define PIG_STOP 82
#define PIG_OFFSET 83
#define PIG_ROTATE 84

// ����������Ϣ��2�ķǸ������η�
#define KEY_W 1
#define KEY_S 2
#define KEY_A 4
#define KEY_D 8
#define KEY_SPACE 16
#define KEY_X 32
#define KEY_ESC 64

// ������ؼ�
#define SIZE_WEIGHT		1.0
#define INIT_WIDTH		500
#define INIT_HEIGHT		500

extern int gameState; // ��Ϸ״̬

// �ֳַ�������
extern int cubeTypeHolding;

// ��ͼ
extern int mapData[MAX_X][MAX_Y][MAX_Z]; // ��ͼ
extern const float cubeSize; // �����С

// ����
/*extern int windowWidth; // ���ڿ��
extern int windowHeight; // ���ڸ߶�
extern int windowLeftX, windowTopY; // �������϶���λ��*/
extern int windowCenterX;
extern int windowCenterY; // ��������λ��

extern int viewport_width;
extern int viewport_height;

// ���
extern glm::vec3 cameraPos; // ���λ��
extern glm::vec3 cameraFront; // �����Ұ����ǰ����������ȷ����Ұ����
extern glm::vec3 cameraUp; // ���ͷƫ������
extern float cameraForwardAngle; // �����ǰ�Ƕ�
extern float cameraSpeed; // ����ƶ��ٶ�
extern float cameraYaw; // ������ӽǶ�
extern float cameraPitch; // ��������Ƕ�
/*extern glm::vec3 viewPos; // �۾�λ��
extern glm::vec3 viewCenter; // ��Ұ����
extern glm::vec3 viewUp; // ͷƫ�Ƕ�*/

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


//��ѭ���Ļ�õ�ͼ����
extern int getMapData(int i, int j, int k);
//д��ͼ����
extern void setMapData(int i, int j, int k, int type);
// �����ֳַ�������
extern void setCurHolding(bool forward);