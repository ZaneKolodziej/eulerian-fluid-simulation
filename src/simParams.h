#pragma once

#include <ImGui/imgui.h>
#include <vector>

struct UIParams {
    float radius = 0.00075f;
    float viscosity = 0.0f;
    int displayMode = 0;
    bool debugMode = false;
    int screenWidth = 1280;
    int simRenderWidth = 720;
    int simRenderHeight = 720;
};

bool drawSideUI(UIParams& params, int sideBarX) {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
        ImGui::SetNextWindowPos(ImVec2(sideBarX, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(params.screenWidth - params.simRenderWidth, params.simRenderHeight), ImGuiCond_Always);
        ImGui::Begin("Simulation Controls", nullptr, windowFlags);
        //ImGui UI elements go in here
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::SliderFloat("Radius", &params.radius, 0.0001f, 0.005f, "%.5f");
            ImGui::SliderFloat("Viscosity", &params.viscosity, 0.0f, 0.5f, "%.5f");
            ImGui::RadioButton("Dye", &params.displayMode, 0);
            ImGui::RadioButton("Velocity", &params.displayMode, 1);
            ImGui::RadioButton("Divergence", &params.displayMode, 2);
            bool resetClicked = ImGui::Button("Reset", ImVec2(-1, 35));
        ImGui::End();
        return resetClicked;
};

void resetSim(const std::vector<float>& initBuffer, const std::vector<float>& initDyeBuffer, GLuint textureA, GLuint textureB, GLuint dyeTextureA, GLuint dyeTextureB, int simWidth, int simHeight){
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
};