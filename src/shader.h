#pragma once

#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <stdio.h>
#include <string>



struct Shader {
    GLuint shaderProgram;
    //Constructor
    Shader(const std::string& vertPath, const std::string& fragPath);
    //Destructor
    ~Shader();
    //Third evil option, use
    void use();
    //---Uniform Setters---
    void setFloat (const std::string& name, float value);
    void setInt (const std::string& name, int value);
    void setVec2 (const std::string& name, float x, float y);
    void setMat4 (const std::string& name, glm::mat4& matrix);
};