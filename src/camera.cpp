#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "camera.h"

Camera::Camera(glm::vec3 cameraPos, float width, float height) : cameraPos(cameraPos), width(width), height(height) {
	left = -width / (2 * scale);
	right = -left;
	bottom = 0.f;
	top = height / scale;
	projection = glm::ortho(left, right, bottom, top, -10.0f, 10.f);
	model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	view = glm::translate(glm::mat4(1.0f), -cameraPos);
}

void Camera::processInput() {
	if (ImGui::IsKeyDown(ImGuiKey_W))
		cameraPos.y += moveSpeed;
	if (ImGui::IsKeyDown(ImGuiKey_A))
		cameraPos.x -= moveSpeed;
	if (ImGui::IsKeyDown(ImGuiKey_S))
		cameraPos.y -= moveSpeed;
	if (ImGui::IsKeyDown(ImGuiKey_D))
		cameraPos.x += moveSpeed;
	if (ImGui::IsKeyDown(ImGuiKey_UpArrow))
		scale -= zoomSpeed;
	if (ImGui::IsKeyDown(ImGuiKey_DownArrow))
		scale += zoomSpeed;
	update();
}

void Camera::update() {
	left = -width / (2 * scale);
	right = -left;
	bottom = 0.f;
	top = height / scale;
	projection = glm::ortho(left, right, bottom, top, -10.0f, 10.f);
	view = glm::translate(glm::mat4(1.0f), -cameraPos);
}

void Camera::updateDimensions(float width, float height) {
	this->width = width;
	this->height = height;
	left = -width / (2 * scale);
	right = -left;
	bottom = 0.f;
	top = height / scale;
	projection = glm::ortho(left, right, bottom, top, -10.0f, 10.f);
}
