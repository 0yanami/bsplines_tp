#ifndef CAMERAH
#define CAMERAH

#ifndef GLM_H
#define GLM_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif

class Camera {
private:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	float yaw;
	float pitch;
	float lastX;
	float lastY;
	float fov;
	
public:
	Camera(glm::vec3 cameraPos, double fov, float windowWidth, float windowHeight);

	glm::vec3 getPos();
	glm::vec3 getDir();
	glm::mat4 getView();
	float getFov();

	float offsetPitch(float _offset, float sensitivity);
	float offsetYaw(float _offset, float sensitivity);
	float offsetFov(float _fov);

	void updateDirection();

	void moveX(float _offset);
	void moveY(float _offset);
	void moveZ(float _offset);
};

#endif
