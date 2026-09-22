//Holds the stuff for computeShaders specifically

#pragma once

#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <string>
#include <vector>

struct ComputeShader {
    GLuint shaderProgram;

    //Constructor
    ComputeShader(const std::string& computePath);
    //Deconstructor
    ~ComputeShader();
    void initTextures (int width, int height, GLuint &outTexA, GLuint &outTexB);
    void dispatch (unsigned int numGroupsX, unsigned int numGroupsY);
    void bindImages (unsigned int sourceTexID, unsigned int destTexID);
    void use();

    void setFloat (const std::string& name, float value);
    void setInt (const std::string& name, int value);
    void setVec2 (const std::string& name, float x, float y);
    void setMat4 (const std::string& name, glm::mat4& matrix);
};