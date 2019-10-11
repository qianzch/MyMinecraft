#include "camera.h"



Camera::Camera()
{
}


Camera::~Camera()
{
}


void Camera::cameraInit() {
	cameraPos = glm::vec3(1.0f, 2.0f, 5.0f);
	cameraFront = glm::normalize(glm::vec3(-1.0f, -2.0f, -5.0f));
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraForwardAngle = -90.0f;
	cameraSpeed = 0.01f;
	cameraYaw = -90.0f;
	cameraPitch = 0.0f;
}

void Camera::cameraResize(int newWidth, int newHeight) {
	windowCenterX = newWidth / 2;
	windowCenterY = newHeight / 2;
}

void Camera::cameraMove(int keyInput) {
	glm::vec3 oldCameraPos = cameraPos; // 保存旧坐标，如果碰撞就返回
	cameraForwardAngle = glm::degrees(atan2(cameraFront.z, cameraFront.x));
	switch (keyInput) {
	case KEY_W:
		cameraPos.x += cameraSpeed * glm::cos(glm::radians((float)cameraForwardAngle));
		cameraPos.z += cameraSpeed * glm::sin(glm::radians((float)cameraForwardAngle));
		break;
	case KEY_S:
		cameraPos.x += cameraSpeed * glm::cos(glm::radians((float)cameraForwardAngle + 180));
		cameraPos.z += cameraSpeed * glm::sin(glm::radians((float)cameraForwardAngle + 180));
		break;
	case KEY_A:
		cameraPos.x += cameraSpeed * glm::cos(glm::radians((float)cameraForwardAngle - 90));
		cameraPos.z += cameraSpeed * glm::sin(glm::radians((float)cameraForwardAngle - 90));
		break;
	case KEY_D:
		cameraPos.x += cameraSpeed * glm::cos(glm::radians((float)cameraForwardAngle + 90));
		cameraPos.z += cameraSpeed * glm::sin(glm::radians((float)cameraForwardAngle + 90));
		break;
	case KEY_SPACE:
		cameraPos.y += cameraSpeed;
		cameraFallSpeed = 0.04;
		break;
	case KEY_X:
		cameraPos.y -= cameraSpeed;
		cameraFallSpeed = 0.04;
		break;
	}
	if (cameraHit()) {
		cameraPos = oldCameraPos;
	}
}

void Camera::cameraRotate(int xpos, int ypos) {
	float xoffset = xpos - windowCenterX;
	float yoffset = windowCenterY - ypos; // reversed since y-coordinates go from bottom to top

	float sensitivity = 0.05f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	cameraYaw += xoffset;
	cameraPitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (cameraPitch > 89.0f)
		cameraPitch = 89.0f;
	else if (cameraPitch < -89.0f)
		cameraPitch = -89.0f;

	glm::vec3 front;
	front.y = sin(glm::radians(cameraPitch));
	//front.y = -0.95f * (ypos - windowHeight / 2) / (windowHeight / 2);
	front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
	front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
	cameraFront = glm::normalize(front);
}

bool Camera::cameraHit() {
	int cx = (cameraPos.x + cubeSize*0.5) / cubeSize;
	int cy = (cameraPos.y +  cubeSize * 0.5- cubeSize * 1.1) / cubeSize;
	int cz = (cameraPos.z + cubeSize * 0.5) / cubeSize;

	if (getMapData(cx,cy,cz) > 0) {
		return true;
	}
	else {
		cy++;
		if (getMapData(cx,cy,cz) > 0) {
			return true;
		}
	}
	
	return false;
}

bool Camera::cameraPlat() {
	int cx = (cameraPos.x + cubeSize * 0.5) / cubeSize;
	int cy = (cameraPos.y + cubeSize * 0.5 - cubeSize * 1.1) / cubeSize;
	int cz = (cameraPos.z + cubeSize * 0.5) / cubeSize;

	if (getMapData(cx, cy-1, cz) > 0) {
		return true;
	}
	

	return false;
}

float Camera::cameraFallSpeed = 0.04;

void Camera::cameraFall() {
	float old_y = cameraPos.y;
	cameraPos.y -= cameraFallSpeed;
	if (cameraHit()) {
		cameraPos.y = old_y;
		cameraFallSpeed /= 2;
		if (cameraFallSpeed < 0.04) {
			cameraFallSpeed = 0.04;
		}
	}
	else {
		cameraFallSpeed += 0.07;
	}
}


glm::vec3 Camera::getEye() {
	return cameraPos;
}

glm::vec3 Camera::getCenter() {
	return cameraPos + cameraFront;
}

glm::vec3 Camera::getUp() {
	return cameraUp;
}

void Camera::cameraTranslate(int xpos, int ypos) {
	float xoffset = xpos - windowCenterX;
	float yoffset = windowCenterY - ypos;
	cameraPos.x += xoffset * 0.005;
	cameraPos.y += yoffset * 0.005;
}