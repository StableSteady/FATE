#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
	Camera(glm::vec3 cameraPos, float width, float height);
	void processInput();
	void updateDimensions(float width, float height);
	glm::mat4 model, view, projection;

private:
	void update();

	glm::vec3 cameraPos;
	float left, right, bottom, top;
	float width, height;
	float moveSpeed = 0.05f;
	float zoomSpeed = 10.f;
	float scale = 450.0f;
};