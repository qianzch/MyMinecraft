#pragma once

//以照相机为中心，绘制周围的距离
#define MAX_ENV_HALF_X 25
#define MAX_ENV_HALF_Y 15
#define MAX_ENV_HALF_Z 25
//绘制区域的尺寸
#define MAX_ENV_X (MAX_ENV_HALF_X*2+1)
#define MAX_ENV_Y (MAX_ENV_HALF_Y*2+1)
#define MAX_ENV_Z (MAX_ENV_HALF_Z*2+1)

//绘制环境
class EnvPainter
{
public:
	EnvPainter();
	~EnvPainter();

	//绘制前的检查
	void checkBeforeDraw();
	//数据发生更新或照相机位置变更时时，重新生成光照等环境信息
	void flushData();

	//照相机所在的方块
	int cameraInCubeX, cameraInCubeY, cameraInCubeZ;
	//绘制区域[0][0][0]对应的方块x,y,z值
	int envBeginX, envBeginY, envBeginZ;
	//环境光强度
	int light[MAX_ENV_X][MAX_ENV_Y][MAX_ENV_Z];
	//可视与否/需要绘制的不透明方块
	int visible[MAX_ENV_X][MAX_ENV_Y][MAX_ENV_Z];
	//需要绘制的透明方块
	int alpha[MAX_ENV_X][MAX_ENV_Y][MAX_ENV_Z];

};

