#include <glad/glad.h>
#include<glm/glm/glm.hpp>
#include "computeShader.h"
#include "shaderUtils.h"
#include <string>
#include <vector>


    //Constructor
    ComputeShader::ComputeShader(const std::string& computePath) {
        GLuint computeShader;
        //Read the shader's contents
        std::string computeShaderSource = ShaderUtilities::readFile(computePath);
        //Compile the shader
        computeShader = ShaderUtilities::compileShaderStage(GL_COMPUTE_SHADER, computeShaderSource.c_str());
        //Link the shader
        shaderProgram = ShaderUtilities::linkProgram(std::vector<GLuint> {computeShader});
        //Clean up
        glDeleteShader(computeShader);
    };
    //Deconstructor
    ComputeShader::~ComputeShader() {
        glDeleteProgram(shaderProgram);
    };

    void ComputeShader::initTextures (int width, int height, GLuint &outTexA, GLuint &outTexB){
            // Generate two texture handles
        glGenTextures(1, &outTexA);
        glGenTextures(1, &outTexB);

        // Setup helper function
        GLuint textures[2] = { outTexA, outTexB };
        for (int i = 0; i < 2; i++) {
            glBindTexture(GL_TEXTURE_2D, textures[i]);

            // Allocate storage for a 32-bit RGBA texture (no initial data needed, pass nullptr)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

            // Set texture filtering (GL_LINEAR allows for smooth sampling if moving to sample2D later)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Set wrapping to clamp to edge so boundaries don't wrap around the screen
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    };

    void ComputeShader::dispatch (unsigned int numGroupsX, unsigned int numGroupsY){
        //use compiled compute shader
        glUseProgram(shaderProgram);

        //execute compute shader with the given work group counts
        glDispatchCompute(numGroupsX, numGroupsY, 1);

        //wait for compute shader writes to finish before textures are sampled elsewhere
        //ensures that both image2d and texture samplers wait for completion
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

        //unbind the program
        glUseProgram(0);
    };

    void ComputeShader::bindImages (unsigned int sourceTexID, unsigned int destTexID){
        //Bind source texture to slot 0 with read permissions
        glBindImageTexture(0, sourceTexID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        //Bind destination texture to slot 1 with write permissions
        glBindImageTexture(1, destTexID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    };
    //Use (it really is just a shortned glUseProgram)
    void ComputeShader::use() {
    glUseProgram(shaderProgram);
}

    void ComputeShader::setFloat(const std::string& name, float value) {
        ShaderUtilities::setFloat(shaderProgram, name, value); 
    };

    void ComputeShader::setInt(const std::string& name, int value) {
        ShaderUtilities::setInt(shaderProgram, name, value);
    };

    void ComputeShader::setVec2(const std::string& name, float x, float y) {
        ShaderUtilities::setVec2(shaderProgram, name, x, y);
    };

    void ComputeShader::setMat4(const std::string& name, glm::mat4& matrix) {
        ShaderUtilities::setMat4(shaderProgram, name, matrix);
    };