#pragma once

#include <chrono>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "constants.h"
#include "renderer.h"

using TimePoint = decltype(std::chrono::system_clock::now() + std::chrono::duration<double, std::ratio<1, constants::FPS>>{ 0 });

struct Animation;
struct Mesh;
struct Model;

class Viewport {
public:
	Viewport(std::vector<int>& selectedMeshes);
	~Viewport();
	void draw(TimePoint currentTime);
	void changeModel(Model* model, TimePoint startTime);
	void changeAnimation(Animation* animation, TimePoint startTime);

private:
	bool handleResize();
	void shaderOptionsWindow();
	void animationOptionsWindow(double& timeElapsed);
	void exportOptionsWindow(double timeElapsed);
	std::string exportedFilename(double timeElapsed);

	int width, height;
	Animation* animation;
	TimePoint startTime;
	Renderer renderer;
	std::vector<Renderable> renderables;
	std::vector<int>& selectedMeshes;
};