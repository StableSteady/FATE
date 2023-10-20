#pragma once

#include <vector>

#include "animator.h"
#include "camera.h"
#include "shader.h"

struct Renderable;

class Renderer {
public:
	Renderer(int width, int height);
	void start();
	[[nodiscard]] unsigned int render(Renderable& renderable, Animation* animation, double timeElapsed);
	void stop() const;
	void resizeBuffer(int width, int height);

	void updateAlphaThreshold(float threshold);
	void writeTextureToFile(const std::string& filename);

private:
	void initUniformbuffer();
	void initFramebuffer();
	void initTexture();
	void initRenderbuffer();
	void attachToFramebuffer() const;
	void bindFramebuffer() const;
	void unbindFramebuffer() const;
	void resizeTexture();
	void resizeRenderbuffer();

	void updateUniformbuffer(Mesh* mesh, Animation* animation, double timeElapsed);

	Animator animator;
	Shader shader;
	int width, height;
	Camera camera;
	unsigned int UBO, FBO, texture, RBO;

	static constexpr const char* vertexShaderPath = "vertex.glsl";
	static constexpr const char* fragmentShaderPath = "fragment.glsl";
};