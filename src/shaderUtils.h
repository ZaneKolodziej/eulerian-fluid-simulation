//Basically a helper function as a header file

#pragma once

#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <string>
#include <fstream>
#include <vector>

namespace ShaderUtilities {
GLuint compileShaderStage(GLenum type, const char* source);
GLuint linkProgram(std::vector<GLuint> shaders);
std::string readFile(const std::string& path);

void setFloat (GLuint shaderProgram, const std::string& name, float value);
void setInt (GLuint shaderProgram, const std::string& name, int value);
void setVec2 (GLuint shaderProgram, const std::string& name, float x, float y);
void setMat4 (GLuint shaderProgram, const std::string& name, const glm::mat4& matrix);
}