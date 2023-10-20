#pragma once

#include <memory>

#include "animationSelector.h"
#include "meshSelector.h"
#include "modelSelector.h"

struct GLFWwindow;
struct Model;
class Viewport;

class FATE {
public:
	FATE();
	~FATE();
	void run();

private:
	void initGLFW();
	void destroyGLFW() const;

	void initImGui() const;
	void destroyImGui() const;

	void createImGuiFrame() const;
	void createImGuiDockspace() const;
	void renderImGui() const;

	GLFWwindow* window;
	ModelSelector modelSelector;
	MeshSelector meshSelector;
	AnimationSelector animationSelector;

	std::unique_ptr<Model> model;
	std::unique_ptr<Viewport> viewport;

	static constexpr int SCREEN_WIDTH = 1280, SCREEN_HEIGHT = 720;
};