#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include "shaderUtils.h"

GLuint ShaderUtilities::compileShaderStage(GLenum type, const char* source) {
    //"type" replaces a hard-coded "GL_VERTEX_BUFFER" or the like
    GLuint shader = glCreateShader(type);
    //Compile generic shader
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    //Error checking
    int success;
    char infoLog[512];
    //Vertex shader
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader Compile Failed:\n%s\n%u\n", infoLog, type); //shader initialization failure
        return 0; //0 is failure since no OpenGL shader is 0
    }
    //success return the compiled shader's ID to whatever called it
    return shader;
};  

GLuint ShaderUtilities::linkProgram(std::vector<GLuint> shaders) {
    GLuint shaderProgram = glCreateProgram();
    for (GLuint shader : shaders) {
        glAttachShader(shaderProgram, shader);
    }  
    glLinkProgram(shaderProgram);
    //Error checking
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "Shader Link Failed:\n%s\n", infoLog); //shader linkage failure
        return 0; //0 is failure since no OpenGL shader is 0
    }
    //success return the compiled shader's ID to whatever called it
    return shaderProgram;
};

//Might need to update this to be more robust
std::string ShaderUtilities::readFile(const std::string& path) {
    //Read file contents
    std::ifstream file(path);
    //Check if the file path works, if not return an empty string
    if (!file) {
        fprintf(stderr, "Failed to load File: %s\n", path.c_str()); //file loading failure
        return "";       
    }
    //Assigns a buffer for file path to live in temp
    std::stringstream buffer;
    buffer << file.rdbuf();
    //Contents of the file are in "contents"
    std::string contents = buffer.str();
    //Assuming all has gone well, return entire contents of read file
    return contents;
};

void ShaderUtilities::setFloat (GLuint shaderProgram, const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);  
};

void ShaderUtilities::setInt (GLuint shaderProgram, const std::string& name, int value){
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
};

void ShaderUtilities::setVec2 (GLuint shaderProgram, const std::string& name, float x, float y){
    glUniform2f(glGetUniformLocation(shaderProgram, name.c_str()), x, y);
};

void ShaderUtilities::setMat4 (GLuint shaderProgram, const std::string& name, const glm::mat4& matrix){
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
};