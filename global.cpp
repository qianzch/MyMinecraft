#include "global.h"

int gameState = GAME_FALLING; // 游戏状态

int mapData[MAX_X][MAX_Y][MAX_Z] = { 0 }; // 地图
const float cubeSize = 2.0f; // 方块大小

int cubeTypeHolding = CUBE_GRASS;
int viewport_width = 800;
int viewport_height = 555;

/*
int windowWidth = 500; // 窗口宽度
int windowHeight = 500; // 窗口高度

int windowLeftX = 635; // 窗口左上顶点横坐标
int windowTopY = 230; // 窗口左上顶点纵坐标*/
int windowCenterX = INIT_WIDTH; // 窗口中心位置横坐标
int windowCenterY = INIT_HEIGHT; // 窗口中心位置纵坐标


glm::vec3 cameraPos = glm::vec3(cubeSize * 15, cubeSize * 20, cubeSize * 15);  // 相机位置
glm::vec3 cameraFront = glm::normalize(glm::vec3(-1.0f, -2.0f, -5.0f));  // 相机视野中心前偏角，用以确定视野中心
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);  // 相机头偏角向量
float cameraForwardAngle = -90.0f;  // 相机向前角度
float cameraSpeed = 1.0f;  // 相机移动速度
float cameraYaw = -90.0f;  // 相机侧视角度
float cameraPitch = 0.0f;  // 相机俯仰角度
/*glm::vec3 viewPos;
glm::vec3 viewCenter;
glm::vec3 viewUp;*/

Animal pigs[MAX_PIG];

//带循环的获得地图数据
int getMapData(int i, int j, int k) {
	if (j >= 0 && j < MAX_Y) {
		return mapData[(i % MAX_X + MAX_X) % MAX_X][j][(k % MAX_Z + MAX_Z) % MAX_Z];
	}
	else {
		return CUBE_AIR;
	}
}
//写地图数据
void setMapData(int i, int j, int k, int type) {
	if (j >= 0 && j < MAX_Y) {
		mapData[(i % MAX_X + MAX_X) % MAX_X][j][(k % MAX_Z + MAX_Z) % MAX_Z] = type;
	}
}
// 更改手持方块类型
void setCurHolding(bool forward) {
	// no CUBE_AIR & CUBE_SUN
	static const int cube_type[] = {
		CUBE_CLOUD,		CUBE_WATER,		CUBE_EARTH,
		CUBE_STONE,		CUBE_GRASS,		CUBE_TREE,
		CUBE_LEAF
	};
	static int index = 2;
	index += forward ? 1 : -1;
	if (index < 0) index = 6;
	else index %= 7;
	cubeTypeHolding = cube_type[index];
	string tip = "Currently holding cube: " + UTILS::get_cube_name(cube_type[index]);
	UTILS::logd(tip.c_str());
}

Animal::Animal()
{
	int x = 80 + rand() % 10 - 5, y = 15, z = 41 + rand() % 10 - 5;
	animalPos = glm::vec3(x*cubeSize, y*cubeSize, z*cubeSize);
	animalCoordinate = glm::vec3(x, y, z);
	//animalFront = glm::normalize(glm::vec3(1.0, 0.0, 0.0));
	animalStepCount = 0;
	animalState = PIG_MOVE_FORWARD;
	animalForwardAngle = -90.0f;
	animalSpeed = cubeSize / 15;
	animalYaw = -90.0f;
}

Animal::Animal(int x, int y, int z)
{
	animalPos = glm::vec3(x*cubeSize, y*cubeSize, z*cubeSize);
	animalCoordinate = glm::vec3(x, y + 1, z);
	//animalFront = glm::normalize(glm::vec3(1.0, 0.0, 0.0));
	animalStepCount = 0;
	animalState = PIG_MOVE_FORWARD;
	animalForwardAngle = -90.0f;
	animalSpeed = cubeSize / 15;
	animalYaw = -90.0f;
}

Animal::Animal(float x, float y, float z)
{
	animalPos = glm::vec3(x, y + cubeSize * 0, z);
	animalCoordinate = glm::vec3((int)x / cubeSize, (int)y / cubeSize + 1, (int)z / cubeSize);
	//animalFront = glm::normalize(glm::vec3(1.0, 0.0, 0.0));
	animalStepCount = 0;
	animalState = PIG_MOVE_FORWARD;
	animalForwardAngle = -90.0f;
	animalSpeed = cubeSize / 15;
	animalYaw = -90.0f;
}



Animal::~Animal()
{
}

void Animal::animalMoveForward() {
	glm::vec3 oldPos = animalPos;
	glm::vec3 oldCoordinate = animalCoordinate;
	//animalForwardAngle = glm::degrees(atan2(animalFront.z, animalFront.x));
	animalPos.x += animalSpeed * glm::cos(glm::radians((float)animalForwardAngle));
	animalPos.z -= animalSpeed * glm::sin(glm::radians((float)animalForwardAngle));
	animalCoordinate.x = (int)animalPos.x / cubeSize;
	animalCoordinate.z = (int)animalPos.z / cubeSize;
	if (animalOut(animalCoordinate.x, animalCoordinate.y, animalCoordinate.z)) {
		//animalPos = oldPos;
		//animalState = PIG_OFFSET;
		animalRotate(PIG_ROTATE);
	}
	if (animalCoordinate != oldCoordinate) {
		if (animalHit()) {
			//animalRotate(PIG_ROTATE);
			animalPos = oldPos;
			animalCoordinate.x = (int)animalPos.x / cubeSize;
			animalCoordinate.z = (int)animalPos.z / cubeSize;
		}
		else {
			setMapData(oldCoordinate.x, oldCoordinate.y, oldCoordinate.z, CUBE_AIR);
			setMapData(animalCoordinate.x, animalCoordinate.y, oldCoordinate.z, CUBE_PIG);
		}
	}
}

bool Animal::animalHit() {
	int cx = animalPos.x / cubeSize;
	int cy = animalPos.y / cubeSize;
	int cz = animalPos.z / cubeSize;

	if (getMapData(cx, cy, cz) > 0) {
		return true;
	}

	/*if (getMapData(cx + 1, cy, cz) == CUBE_PIG) return true;
	if (getMapData(cx - 1, cy, cz) == CUBE_PIG) return true;
	if (getMapData(cx, cy, cz + 1) == CUBE_PIG) return true;
	if (getMapData(cx, cy, cz - 1) == CUBE_PIG) return true;*/

	return false;
}

void Animal::animalRotate(int type) {
	if (type == PIG_OFFSET) {
		animalForwardAngle += rand() % 3 + 1;
	}
	else {
		animalForwardAngle += rand() % 90 + 90;
	}
}

int Animal::animalChooseMode() {
	int mode = rand() % 100;
	if (mode < 75) {
		return PIG_MOVE_FORWARD;
	}
	else if (mode >= 75 && mode < 80) {
		return PIG_STOP;
	}
	else if (mode >= 80 && mode < 100) {
		return PIG_OFFSET;
	}
	else {
		return PIG_OFFSET; // PIG_ROTATE
	}
}

void Animal::animalMove() {
	animalStepCount++;
	if (animalStepCount >= 30) {
		int mode = animalChooseMode();
		switch (mode) {
		case PIG_MOVE_FORWARD:
			animalState = PIG_MOVE_FORWARD;
			animalMoveForward();
			break;
		case PIG_STOP:
			animalState = PIG_STOP;
			break;
		case PIG_OFFSET:
			animalState = PIG_OFFSET;
			animalRotate(PIG_OFFSET);
			break;
		case PIG_ROTATE:
			animalRotate(PIG_ROTATE);
			break;
		default:
			animalMoveForward();
			break;
		}
		animalStepCount = 0;
	}
	else {
		switch (animalState) {
		case PIG_MOVE_FORWARD: animalMoveForward(); break;
		case PIG_STOP: break;
		case PIG_OFFSET: animalRotate(PIG_OFFSET); break;
		default: break;
		}
	}
}

bool Animal::animalOut(int x, int y, int z) {
	int xdx = x - 80, xdz = z - 41;
	if (xdx > 15 || xdx < -15 || xdz > 15 || xdz < -15) {
		return true;
	}
	else {
		return false;
	}
}