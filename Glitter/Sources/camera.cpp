#include "camera.hpp"

Camera::Camera(glm::vec3 _cameraPos, double _fov, float _windowWidth, float _windowHeight) :
fov(_fov),
cameraPos(_cameraPos)
{
	yaw = -90.0;
	pitch = -0.0;
	lastX = _windowHeight / 2;
	lastY = _windowWidth / 2;
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 Camera::getPos() {
	return cameraPos;
}

glm::vec3 Camera::getDir() {
	return cameraFront;
}


glm::mat4 Camera::getView()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

//direction
void Camera::updateDirection()
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw));
	direction.y = sin(glm::radians(pitch)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

float Camera::offsetPitch(float _offset, float _sensitivity)
{
	if (pitch + _offset > 45) {
		pitch = 45.0f;
	}
	if (pitch + _offset < -45) {
		pitch = -45.0f;
	}
	
	pitch += _offset * _sensitivity;
	updateDirection();
	return pitch;
}

float Camera::offsetYaw(float _offset, float _sensitivity)
{
	yaw += _offset * _sensitivity;
	if (yaw < 0)
		yaw += 360;
	if (yaw > 360)
		yaw -= 360;
	updateDirection();
	return yaw;
}

//fov
float Camera::offsetFov(float _offset)
{
	if (fov + _offset > 90) {
		fov = 90.0f;
	}
	else if (fov + _offset < 1) {
		fov = 1.0f;
	}
	fov += _offset;
	return fov;
}

float Camera::getFov()
{
	return this->fov;
}

//position
void Camera::moveX(float _offset)
{
	cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * _offset;
}

void Camera::moveY(float _offset)
{
	cameraPos += glm::normalize(cameraUp) * _offset;
}

void Camera::moveZ(float _offset)
{
	cameraPos += cameraFront * _offset;
}


