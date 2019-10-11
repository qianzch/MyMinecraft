#pragma once

//�������Ϊ���ģ�������Χ�ľ���
#define MAX_ENV_HALF_X 25
#define MAX_ENV_HALF_Y 15
#define MAX_ENV_HALF_Z 25
//��������ĳߴ�
#define MAX_ENV_X (MAX_ENV_HALF_X*2+1)
#define MAX_ENV_Y (MAX_ENV_HALF_Y*2+1)
#define MAX_ENV_Z (MAX_ENV_HALF_Z*2+1)

//���ƻ���
class EnvPainter
{
public:
	EnvPainter();
	~EnvPainter();

	//����ǰ�ļ��
	void checkBeforeDraw();
	//���ݷ������»������λ�ñ��ʱʱ���������ɹ��յȻ�����Ϣ
	void flushData();

	//��������ڵķ���
	int cameraInCubeX, cameraInCubeY, cameraInCubeZ;
	//��������[0][0][0]��Ӧ�ķ���x,y,zֵ
	int envBeginX, envBeginY, envBeginZ;
	//������ǿ��
	int light[MAX_ENV_X][MAX_ENV_Y][MAX_ENV_Z];
	//�������/��Ҫ���ƵĲ�͸������
	int visible[MAX_ENV_X][MAX_ENV_Y][MAX_ENV_Z];
	//��Ҫ���Ƶ�͸������
	int alpha[MAX_ENV_X][MAX_ENV_Y][MAX_ENV_Z];

};

