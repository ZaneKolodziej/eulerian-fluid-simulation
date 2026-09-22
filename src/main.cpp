#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE //Removes ordering conflicts (at least the wiki says it does)
#include <GLFW/glfw3.h>
#include <glm/glm/gtc/matrix_transform.hpp>

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <ImGui/backends/imgui_impl_glfw.h>

#include <stdio.h>
#include <utility>
#include <vector>

#include "shaderUtils.h"
#include "shader.h"
#include "mesh.h"
#include "computeShader.h"
#include "simParams.h"

//Error callback initialization
void error_callback(int error, const char* description)
 {
     fprintf(stderr, "Error: %s\n", description);
 }

//Key Callback initialization
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
 {
    if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, 1); 
 }

//Declare any const variables
int simWidth = 512;
int simHeight = 512;

int simRenderWidth = 720;
int simRenderHeight = 720;

int windowWidth = 1280;
int windowHeight = 720;
//initialize main
int main() {

 glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        glfwTerminate();
        fprintf(stderr, "Failed to initialize GLFW\n"); //GLFW initialization failure
        return -1;
    }

    //---Create the window---

    //Points to the openGL version used in program? (I think this is called context?)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    //Initialize window
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Fluid Simulation", NULL, NULL);
    //Handle window initialization failure
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "Failed to initialize Window\n"); //Window initialization failure
        return -1;
    }
    glfwSetKeyCallback(window, key_callback);
    //Make the context current for openGL
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
     //Initialize extension library loader for openGL (glad) and check if it loaded properly
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) //gladLoadGLLoader expects a function loader than can load OpenGl extensions. The cast (GLADloadproc) tells GLAD that glfwGetProcAddress is a function pointer with the correct signatrue for loading openGL functions.
    {
        glfwTerminate();
        fprintf(stderr, "Failed to initialize glad loader\n"); //Glad loader failure
        return -1;
    }
    
    //ImGui initialization
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430 core");

    //---Initialize compute shaders---
    //Compute shader texture 1
    GLuint textureA = 0, textureB = 0;
    //Dye shader texture
    GLuint dyeTextureA = 0, dyeTextureB = 0;

    //Advection
    ComputeShader advectionShader("shaders/advection.comp");
    advectionShader.initTextures(simWidth, simHeight, textureA, textureB);
    //Diffusion
    ComputeShader diffusionShader("shaders/diffuse.comp");
    //diffusionShader.initTextures(simWidth, simHeight, textureA, textureB);
    //Divergence
    ComputeShader divergenceShader("shaders/divergence.comp");
    //divergenceShader.initTextures(simWidth, simHeight, textureA, textureB);
    //Pressure-solver
    ComputeShader pressureSolver("shaders/pressureSolver.comp");
    //divergenceShader.initTextures(simWidth, simHeight, textureA, textureB);
    //Projection
    ComputeShader projectionShader("shaders/projection.comp");
    //projectionShader.initTextures(simWidth, simHeight, textureA, textureB);
    //Advect dye
    ComputeShader advectDyeShader("shaders/advectDye.comp");
    advectDyeShader.initTextures(simWidth, simHeight, dyeTextureA, dyeTextureB);
    //Splat, doesn't actually need it's own textures initialized since it modifies existing ones
    ComputeShader splatShader("shaders/splat.comp");

    GLuint currentDyeSource = dyeTextureA;
    fprintf(stderr, "dyeTextureA=%u currentDyeSource=%u\n", dyeTextureA, currentDyeSource);
    GLuint currentDyeDest   = dyeTextureB;
    fprintf(stderr, "TextureB=%u currentDyeDest=%u\n", dyeTextureB, currentDyeDest);

    GLuint currentSource = textureA;
    fprintf(stderr, "textureA=%u currentSource=%u\n", textureA, currentSource);
    GLuint currentDest   = textureB;
    fprintf(stderr, "textureB=%u currentDest=%u\n", textureB, currentDest);

    std::vector<float> initBuffer(simWidth * simHeight * 4, 0.0f);
        for (size_t i = 0; i < initBuffer.size(); i += 4) {
        initBuffer[i + 0] = 0.2f; // Velocity X (.r)
        initBuffer[i + 1] = 0.2f; // Velocity Y (.g)
        initBuffer[i + 2] = 0.0f; // Divergence (.b)
        initBuffer[i + 3] = 0.0f; // Pressure (.a)
    }
    std::vector<float> initDyeBuffer(simWidth * simHeight * 4, 0.0f);
        for (size_t i = 0; i < initDyeBuffer.size(); i += 4) {
        initDyeBuffer[i + 3] = 1.0f; // Alpha (.a)
    }
    glBindTexture(GL_TEXTURE_2D, textureA);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, simWidth, simHeight, GL_RGBA, GL_FLOAT, initBuffer.data());

    glBindTexture(GL_TEXTURE_2D, textureB);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, simWidth, simHeight, GL_RGBA, GL_FLOAT, initBuffer.data());

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, dyeTextureA);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, simWidth, simHeight, GL_RGBA, GL_FLOAT, initDyeBuffer.data());

    glBindTexture(GL_TEXTURE_2D, dyeTextureB);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, simWidth, simHeight, GL_RGBA, GL_FLOAT, initDyeBuffer.data());

    glBindTexture(GL_TEXTURE_2D, 0);

    //Deltatime calculations
    double lastTime = glfwGetTime();

    //Input handling "constants"
    double oldXPos, oldYPos;
    glfwGetCursorPos(window, &oldXPos, &oldYPos);

    //Workgroup size calculations (workgroups are 16x16)
    unsigned int workGroupSize = 16;
    unsigned int numGroupsX = (simWidth  + workGroupSize - 1) / workGroupSize;
    unsigned int numGroupsY = (simHeight + workGroupSize - 1) / workGroupSize;

    //Create shader and mesh instances
    Shader shader("shaders/shader.vert", "shaders/shader.frag");
    Mesh mesh;
    UIParams params;

    //Mesh/view stuff?
    glm::mat4 projection = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -1.0f, 1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;
    shader.use();
    shader.setMat4 ("mvp", mvp);
    //---Render Loop---
    while (!glfwWindowShouldClose(window))
    {   //ImGui stuff
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        static float f = 0.0f;

        int fbWidth;
        int fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        glViewport(0, 0, fbWidth, fbHeight);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //Deltatime calculations (real-time edition)
        double currentTime = glfwGetTime();
        double dt = currentTime - lastTime;
        lastTime = currentTime;

        //Force calculations
        const float baseForce = 3.0f;
        const float baseRadius = 0.00075f;
        float forceScale = baseForce * (baseRadius*baseRadius) / (params.radius*params.radius);

        //---Input handling---
        //Mouse position (coordinates normalized)
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        double dx = xpos - oldXPos;
        double dy = ypos - oldYPos;

        float normY = 1.0f - (ypos / simRenderHeight);
        float normX = (xpos / simRenderWidth);
        if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) && !(xpos > simRenderWidth || xpos < 0 || ypos > simRenderHeight || ypos < 0))
        {
            //---Splatsesesesees---
            //Velocity pass
            splatShader.use();
            splatShader.setInt("splatMode", 1);
            splatShader.setVec2("point", normX, normY);
            splatShader.setVec2("force", dx * baseForce * forceScale, dy * -baseForce * forceScale);
            splatShader.setFloat("dyeAmount", 0.0f);
            //Needs to be changed to integrate with ImGui stuff later for variable radius size
            splatShader.setFloat("radius", params.radius);
            splatShader.bindImages(currentSource, currentDest);
            splatShader.dispatch(numGroupsX, numGroupsY);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            std::swap(currentSource, currentDest);
            //Dye pass
            splatShader.use();
            splatShader.setInt("splatMode", 0);
            splatShader.setVec2("point", normX, normY);
            splatShader.setVec2("force", 0.8f, 0.8f); //Used for dye .rg channels instead of velocity
            splatShader.setFloat("dyeAmount", 0.8f);
            //Needs to be changed to integrate with ImGui stuff later for variable radius size
            splatShader.setFloat("radius", params.radius);
            splatShader.bindImages(currentDyeSource, currentDyeDest);
            splatShader.dispatch(numGroupsX, numGroupsY);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            std::swap(currentDyeSource, currentDyeDest);
        }
        oldXPos = xpos;
        oldYPos = ypos;

        //---Passeseseseses---
        //1. Advection
        advectionShader.use();
        advectionShader.setFloat("dt", dt);
        advectionShader.bindImages(currentSource, currentDest);
        advectionShader.dispatch(numGroupsX, numGroupsY);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        std::swap(currentSource, currentDest);

        //2. Diffusion
        diffusionShader.use();
        diffusionShader.setFloat("dt", dt);
        diffusionShader.setFloat("viscosity", params.viscosity);
        diffusionShader.setInt("diffuseMode", 0);
        diffusionShader.bindImages(currentSource, currentDest);
        diffusionShader.dispatch(numGroupsX, numGroupsY);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        std::swap(currentSource, currentDest);

        //3. Divergence
        divergenceShader.use();
        divergenceShader.bindImages(currentSource, currentDest);
        divergenceShader.dispatch(numGroupsX, numGroupsY);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        std::swap(currentSource, currentDest);

        //4. Pressure Solver
        pressureSolver.use();
        pressureSolver.setFloat("omega", 1.0);
        const int redBlackIters = 60;
        GLint locIsBlackPass = glGetUniformLocation(pressureSolver.shaderProgram, "isBlackPass");

        glBindImageTexture(0, currentSource, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, currentSource, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        for (int i = 0; i < redBlackIters; i++)
        {
            //Red pass (isBlackPass = 0 (shocker I know))
            glUniform1i(locIsBlackPass, 0);
            pressureSolver.dispatch(numGroupsX, numGroupsY);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

            //Black pass (isBlackPass = 1 (again, shocker I know))
            glUniform1i(locIsBlackPass, 1);
            pressureSolver.dispatch(numGroupsX, numGroupsY);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        }

        //5. Projection
        projectionShader.use();
        projectionShader.setFloat("dt", dt);
        projectionShader.bindImages(currentSource, currentDest);
        projectionShader.dispatch(numGroupsX, numGroupsY);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        std::swap(currentSource, currentDest);

        //Advect dye
        advectDyeShader.use();
        advectDyeShader.setFloat("dt", dt);
        glBindImageTexture(0, currentSource, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, currentDyeSource, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(2, currentDyeDest, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        advectDyeShader.dispatch(numGroupsX, numGroupsY);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        std::swap(currentDyeSource, currentDyeDest);

        //Diffuse dye
        diffusionShader.use();
        diffusionShader.setFloat("dt", dt);
        diffusionShader.setFloat("viscosity", params.viscosity);
        diffusionShader.setInt("diffuseMode", 1);
        diffusionShader.bindImages(currentDyeSource, currentDyeDest);
        diffusionShader.dispatch(numGroupsX, numGroupsY);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        std::swap(currentDyeSource, currentDyeDest);

        //ImGui Implementations
        bool resetRequested = drawSideUI(params, simRenderWidth);
        if (resetRequested == true) {
            resetSim(initBuffer, initDyeBuffer, textureA, textureB, dyeTextureA, dyeTextureB, simWidth, simHeight);
        };
        //fprintf(stderr, "displayMode = %d\n", params.displayMode);
        //Visualize/Draw pipeline

        shader.use();
        GLuint renderTexture = (params.displayMode == 0) ? currentDyeSource : currentSource;
        shader.setInt("displayMode", params.displayMode);
        //fprintf(stderr, "displayMode = %d\n", params.displayMode);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderTexture);
        shader.setInt("texture0", 0);
        glBindVertexArray(mesh.VAO);
        glViewport(0, 0, simRenderWidth, simRenderHeight);
        glDrawArrays(GL_TRIANGLES, 0, 6);
            //Surprize! ImGui time
            glViewport(0, 0, fbWidth, fbHeight);
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }
    //Cleanup!
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

}   


