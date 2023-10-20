#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
	std::fstream vertexShaderFile(vertexShaderPath);
	std::stringstream vertStringStream;
	vertStringStream << vertexShaderFile.rdbuf();
	std::string vertexShaderString = vertStringStream.str();
	const char* vertexShaderSource = vertexShaderString.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

	glCompileShader(vertexShader);

	checkCompileError(vertexShader, "Vertex");

	std::fstream fragmentShaderFile(fragmentShaderPath);
	std::stringstream fragStringStream;
	fragStringStream << fragmentShaderFile.rdbuf();
	std::string fragmentShaderString = fragStringStream.str();
	const char* fragmentShaderSource = fragmentShaderString.c_str();

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

	glCompileShader(fragmentShader);

	checkCompileError(fragmentShader, "Fragment");

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		char errorLog[1024];
		glGetProgramInfoLog(program, 1024, nullptr, &errorLog[0]);
		std::cerr << "Linking shaders failed with error: " << errorLog << '\n';
		glDeleteProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	else {
		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
	}

}

void Shader::use() const {
	glUseProgram(program);
}

void Shader::setUniform(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
}

void Shader::setUniform(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::setUniform(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::setUniform(const std::string& name, const glm::mat4& value) const {
	glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::bindUBO(const std::string& name, GLuint bindingPoint, GLuint ubo) const {
	GLuint blockIndex = glGetUniformBlockIndex(program, name.c_str());
	glUniformBlockBinding(program, blockIndex, bindingPoint);
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);
}

void Shader::checkCompileError(GLuint shader, const std::string type) const {
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		char errorLog[1024];
		glGetShaderInfoLog(shader, 1024, nullptr, &errorLog[0]);
		std::cerr << type << " Shader compilation failed with error: " << errorLog << '\n';
		glDeleteShader(shader);
	}
}
