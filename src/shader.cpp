#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <string>
#include <fstream>
#include "shader.h"
#include "shaderUtils.h"


//Constructor
Shader::Shader(const std::string& vertPath, const std::string& fragPath) {
    GLuint vertexShader;
    GLuint fragmentShader;
    //Read the contents of the shaders
    std::string vertexShaderSource = ShaderUtilities::readFile(vertPath);
    std::string fragmentShaderSource = ShaderUtilities::readFile(fragPath);
    //---Compile the shaders---
    //Vertex shader
    vertexShader = ShaderUtilities::compileShaderStage(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    //Fragment shader
    fragmentShader = ShaderUtilities::compileShaderStage(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());
    //Link the shaders
    shaderProgram = ShaderUtilities::linkProgram(std::vector<GLuint> { vertexShader, fragmentShader });
    //Cleanup/delete old shader IDs
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

//Destructor
Shader::~Shader() {
    glDeleteProgram(shaderProgram);
}

//Use (it really is just a shortned glUseProgram)
void Shader::use() {
    glUseProgram(shaderProgram);
}


void Shader::setFloat(const std::string& name, float value) {
    ShaderUtilities::setFloat(shaderProgram, name, value); 
}

void Shader::setInt(const std::string& name, int value) {
    ShaderUtilities::setInt(shaderProgram, name, value);
}

void Shader::setVec2(const std::string& name, float x, float y) {
    ShaderUtilities::setVec2(shaderProgram, name, x, y);
}

void Shader::setMat4(const std::string& name, glm::mat4& matrix) {
    ShaderUtilities::setMat4(shaderProgram, name, matrix);
}