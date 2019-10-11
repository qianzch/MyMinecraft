#include "EnvPainter.h"
#include "global.h"
#include <cmath>

#include <queue>
using namespace std;

struct Point {
	int x, y, z;
};

EnvPainter::EnvPainter()
{
	cameraInCubeX = -1;
	cameraInCubeY = -1;
	cameraInCubeZ = -1;




}


EnvPainter::~EnvPainter()
{
}

void EnvPainter::checkBeforeDraw() {
	if (floor(cameraPos.x / cubeSize) != cameraInCubeX || floor(cameraPos.y / cubeSize) != cameraInCubeY || floor(cameraPos.z / cubeSize) != cameraInCubeZ) {
		//绘制中心位置变了，重新生成环境信息



		flushData();
	}
}

void EnvPainter::flushData() {
	cameraInCubeX = cameraPos.x / cubeSize;
	cameraInCubeY = cameraPos.y / cubeSize;
	cameraInCubeZ = cameraPos.z / cubeSize;

	/*envBeginX = (cameraInCubeX - MAX_ENV_HALF_X + MAX_X) % MAX_X;
	envBeginY = max((cameraInCubeY - MAX_ENV_HALF_Y ), 0 );
	envBeginZ = (cameraInCubeZ - MAX_ENV_HALF_Z + MAX_Z) % MAX_Z;

	static int _opt_count = -1;
	_opt_count++;
	if (_opt_count == 10) _opt_count = 0;
	if (_opt_count != 0) return;

	for (int i = 0; i < MAX_ENV_X; i++) {
		for (int j = 0; j < MAX_ENV_Y; j++) {
			for (int k = 0; k < MAX_ENV_Z; k++) {
				light[i][j][k] = 0;
				visible[i][j][k] = -1;
				alpha[i][j][k] = -1;
			}
		}
	}

	queue<Point> lightPoints;
	for (int i = 0; i < MAX_ENV_X; i++) {
		 for (int k = 0; k < MAX_ENV_Z; k++){
			 int SunshineLight = 15;
			 for (int j = MAX_Y - 1; j >= envBeginY; j--) {
				 int cubeType = getMapData(envBeginX + i, j, envBeginZ + k);
				 if (cubeType != CUBE_AIR) {
					 //不是空气时，阳光光强发生变化
					 if (cubeType == CUBE_STONE) {
						 SunshineLight = 0;
					 }
				 }	
				 if (j < envBeginY + MAX_ENV_Y) {
					light[i][j - envBeginY][k] = SunshineLight;
					if (SunshineLight > 0) {
						Point p;
						p.x = i;
						p.y = j - envBeginY;
						p.z = k;
						lightPoints.push(p);
					}
				 }
			 }
		}
	}
	//通过广度优先搜索计算环境光
	Point p,np;
	while (!lightPoints.empty()) {
		p = lightPoints.front();
		lightPoints.pop();


		queue<Point> nps;
		np = p; np.x--; nps.push(np);
		np = p; np.x++; nps.push(np);
		np = p; np.y--; nps.push(np);
		np = p; np.y++; nps.push(np);
		np = p; np.z--; nps.push(np);
		np = p; np.z++; nps.push(np);

		while (!nps.empty()) {
			np = nps.front(); nps.pop();
			int i, j, k;
			i = np.x; j = np.y; k = np.z;
			if (i >= 0 && i<MAX_ENV_X && j >= 0 && j<MAX_ENV_Y && k >= 0 && k<MAX_ENV_Z && light[i][j][k] < light[p.x][p.y][p.z]) {
				int cubeType = getMapData(envBeginX + i, envBeginY + j, envBeginZ + k);
				int newLight = light[i][j][k];
				if (cubeType == CUBE_AIR) {
					newLight = light[p.x][p.y][p.z] - 1;
				}
				if (cubeType == CUBE_WATER) {
					newLight = light[p.x][p.y][p.z] - 2;
				}
				// ...

				if (newLight > light[i][j][k]) {
					light[i][j][k] = newLight;
					lightPoints.push(np);
				}

			}
		}

	}

	queue<Point> vps;
	Point vp,nvp;
	vp.x = MAX_ENV_HALF_X;
	vp.y = MAX_ENV_HALF_Y;
	vp.z = MAX_ENV_HALF_Z;
	vps.push(vp);

	while (!vps.empty()) {
		vp = vps.front();
		vps.pop();

		if (vp.x<0 || vp.x >= MAX_ENV_X || vp.y<0 || vp.y >= MAX_ENV_Y || vp.z<0 || vp.z >= MAX_ENV_Z || visible[vp.x][vp.y][vp.z] != -1) {
			continue;
		}

		visible[vp.x][vp.y][vp.z] = (getMapData(vp.x + envBeginX, vp.y + envBeginY, vp.z + envBeginZ) == CUBE_AIR?0:1);
		if (visible[vp.x][vp.y][vp.z] == 0) {
			nvp = vp; nvp.x++; vps.push(nvp);
			nvp = vp; nvp.x--; vps.push(nvp);
			nvp = vp; nvp.y++; vps.push(nvp);
			nvp = vp; nvp.y--; vps.push(nvp);
			nvp = vp; nvp.z++; vps.push(nvp);
			nvp = vp; nvp.z--; vps.push(nvp);
		}
	}*/

}