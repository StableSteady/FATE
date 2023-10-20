#include <cmath>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "model.h"
#include "renderable.h"
#include "viewport.h"

Viewport::Viewport(std::vector<int>& selectedMeshes) : width(0), height(0), animation(nullptr), renderer(0, 0), selectedMeshes(selectedMeshes) {
}

Viewport::~Viewport() {
}

void Viewport::draw(TimePoint currentTime) {
	ImGui::Begin("Viewport");

	if (handleResize() == false or animation == nullptr) {
		ImGui::End();
		return;
	}

	unsigned int texture = 0;
	double timeElapsed = std::fmod(std::chrono::duration<double>(currentTime - startTime).count(), animation->duration);
	animationOptionsWindow(timeElapsed);
	renderer.start();
	for (int i = 0; i < renderables.size(); ++i) {
		if (selectedMeshes[i] != 0) {
			texture = renderer.render(renderables[i], animation, timeElapsed);
		}
	}
	renderer.stop();
	ImGui::Image((void*)(intptr_t)texture, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
	exportOptionsWindow(timeElapsed);

	ImGui::End();
	shaderOptionsWindow();
}

void Viewport::changeModel(Model* model, TimePoint startTime) {
	selectedMeshes.clear();
	selectedMeshes.resize(model->meshes.size());
	renderables.clear();
	for (Mesh& mesh : model->meshes) {
		renderables.emplace_back(model, &mesh);
	}
	this->animation = &model->animations[0];
	this->startTime = startTime;
}

void Viewport::changeAnimation(Animation* animation, TimePoint startTime) {
	this->animation = animation;
	this->startTime = startTime;
}

bool Viewport::handleResize() {
	ImVec2 size = ImGui::GetContentRegionAvail();

	if (size.x != width or size.y != height) {
		if (size.x == 0 or size.y == 0) {
			return false;
		}

		width = size.x;
		height = size.y;
		renderer.resizeBuffer(width, height);

		return true;
	}
	return true;
}

void Viewport::shaderOptionsWindow() {
	ImGui::Begin("Shader options");

	static float threshold = 0.6f;
	ImGui::SliderFloat("Alpha threshold", &threshold, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_NoInput);
	renderer.updateAlphaThreshold(threshold);

	ImGui::End();
}

void Viewport::animationOptionsWindow(double& timeElapsed) {
	ImGui::Begin("Animation options");

	if (!animation) {
		ImGui::End();
		return;
	}

	static bool animate = true;
	if (ImGui::Button("Play/Pause")) {
		animate = !animate;
	}

	static float t = 0.f;
	t = animate ? static_cast<float>(timeElapsed) : t;
	ImGui::SameLine();
	ImGui::SliderFloat("Duration", &t, 0.f, static_cast<float>(animation->duration));
	if (t != static_cast<float>(timeElapsed)) { // If user changed time manually
		startTime = std::chrono::system_clock::now() - std::chrono::duration<double>(static_cast<double>(t));
	}
	
	timeElapsed = static_cast<double>(t);

	ImGui::End();
}

void Viewport::exportOptionsWindow(double timeElapsed) {
	ImGui::Begin("Export");
	
	if (ImGui::Button("Export current frame")) {
		renderer.writeTextureToFile(exportedFilename(timeElapsed));
	}

	ImGui::End();
}

std::string Viewport::exportedFilename(double timeElapsed) {
	std::string filename;
	for (int i = 0; i < selectedMeshes.size(); ++i) {
		if (i == 1) {
			filename += renderables[i].mesh->name;
		}
	}
	filename += animation->name;
	filename.append("_");
	filename += std::to_string(timeElapsed);
	filename.append(".png");
	return filename;
}
