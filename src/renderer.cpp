#include <glad/glad.h>

#include <stb_image_write.h>

#include "constants.h"
#include "model.h"
#include "renderable.h"
#include "renderer.h"

Renderer::Renderer(int width, int height) : shader(vertexShaderPath, fragmentShaderPath), width(width), height(height), 
											camera(glm::vec3(0.f, 0.f, 0.f), (float)width, (float)height), 
											UBO(-1), FBO(-1), texture(-1), RBO(-1) {
	initUniformbuffer();
	initFramebuffer();
	initTexture();
	initRenderbuffer();
	attachToFramebuffer();
}

void Renderer::start() {
	bindFramebuffer();
	shader.use();
	camera.processInput();
	shader.setUniform("model", camera.model);
	shader.setUniform("view", camera.view);
	shader.setUniform("projection", camera.projection);
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

unsigned int Renderer::render(Renderable& renderable, Animation* animation, double timeElapsed) {
	updateUniformbuffer(renderable.mesh, animation, timeElapsed);
	renderable.draw();
	return texture;
}

void Renderer::stop() const {
	unbindFramebuffer();
}

void Renderer::resizeBuffer(int width, int height) {
	this->width = width;
	this->height = height;
	camera.updateDimensions(static_cast<float>(width), static_cast<float>(height));
	resizeTexture();
	resizeRenderbuffer();
	attachToFramebuffer();
}

void Renderer::updateAlphaThreshold(float threshold) {
	shader.setUniform("alphaThreshold", threshold);
}

void Renderer::writeTextureToFile(const std::string& filename) {
	bindFramebuffer();
	GLubyte* pixels = new GLubyte[width * height * 4];
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	stbi_flip_vertically_on_write(true);
	stbi_write_png(filename.c_str(), width, height, 4, pixels, 4 * width);
	delete[] pixels;
	unbindFramebuffer();
}

void Renderer::initUniformbuffer() {
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * constants::MAX_BONES, nullptr, GL_DYNAMIC_DRAW);
	shader.bindUBO("boneTransforms", 0, UBO);
}

void Renderer::initFramebuffer() {
	glGenFramebuffers(1, &FBO);
}

void Renderer::initTexture() {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::initRenderbuffer() {
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Renderer::attachToFramebuffer() const {
	bindFramebuffer();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	unbindFramebuffer();
}

void Renderer::bindFramebuffer() const {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void Renderer::unbindFramebuffer() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::resizeTexture() {
	glDeleteTextures(1, &texture);
	initTexture();
}

void Renderer::resizeRenderbuffer() {
	glDeleteRenderbuffers(1, &RBO);
	initRenderbuffer();
}

void Renderer::updateUniformbuffer(Mesh* mesh, Animation* animation, double timeElapsed) {
	std::vector<glm::mat4> transforms = animator.getMeshTransforms(mesh, animation, timeElapsed);
	void* bufferData = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(bufferData, transforms.data(), transforms.size() * sizeof(glm::mat4));
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}
