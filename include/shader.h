#pragma once

#include <filesystem>
#include <string>

#include <glad/glad.h>

#include <glm/glm.hpp>

class Shader {
public:
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	void use() const;
	void setUniform(const std::string& name, bool value) const;
	void setUniform(const std::string& name, int value) const;
	void setUniform(const std::string& name, float value) const;
	void setUniform(const std::string& name, const glm::mat4& value) const;
	void bindUBO(const std::string& name, GLuint bindingPoint, GLuint ubo) const;
private:
	GLuint program = 0;
	void checkCompileError(GLuint shader, const std::string type) const;
};
