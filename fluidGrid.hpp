#pragma once
#include <vector>
#include "fluidSolver.hpp"  
#include "raylib.h"

void drawFluid(const FluidState& state, int screenWidth, int screenHeight) {
    //Determines the size of the individual cell, shouldn't need to be a float, but using for extra safety
    float cellSize = (float)screenHeight / state.height;
    for (int j = 0; j < state.height; j++) {
        for (int i = 0; i < state.width; i++) {
            float d = state.density[state.getIndex(i, j)];
            float scrX = (i) * cellSize;
            float scrY = (j) * cellSize;
            //Multiply the density value by 255 to get a color
            float scaledD = d * 255.0f;
            //Clamp density value if above or below accepted values
            if (scaledD < 0.0f) scaledD = 0.0f;
            if (scaledD > 255.0f) scaledD = 255.0f;
            //Make it readable for RayLib
            unsigned char (intensity) = (unsigned char) scaledD;
            //Create the color
            Color c = CLITERAL(Color){intensity, intensity, intensity, 255 };
            //Draw the rectangles
            DrawRectangle(scrX, scrY, cellSize, cellSize, c);
            //Draw the rectangle outlines
            DrawRectangleLines(scrX, scrY, cellSize, cellSize, GRAY);
        }  
    }
};

void handleGridInput (FluidState& state, int screenWidth, int screenHeight) {
    // Update
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
         Vector2 mousePos = GetMousePosition();
         Vector2 mouseDelta = GetMouseDelta();
         //Determines the size of the individual cell, used in determining whether the mouse is actually inside the fluid grid or in the UI region
         float cellSize = (float)screenHeight / state.height;
    
         int gridX = (int)(mousePos.x / cellSize);
         int gridY = (int)(mousePos.y / cellSize);
    
        if (gridX >= 1 && gridX < state.width - 1 && gridY >= 1 && gridY < state.height - 1) {
         int index = state.getIndex(gridX, gridY);
         //Scalar calculations for input force
         float scalarA = 20.0f / cellSize;
         // Get the delta time (time elapsed since last frame)
         float dt = GetFrameTime();
         //Force x and y calculations based on mouseDeltas, scalarA, and deltaTime
         //scalarA might need to be spilt up into an aX and aY in the future if the fluid grid is sensetive to the directions
         float forceX = mouseDelta.x * scalarA * dt;
         float forceY = mouseDelta.y * scalarA * dt;
         // Add to the current density instead of overwriting it
         // Adjust the multiplier (e.g., 5.0f) to make it fill faster or slower
          state.density[index] += 5.0f * dt; 
                
         // Keep it capped at 1.0f max so it doesn't break the color math later
         if (state.density[index] > 1.0f) {
             state.density[index] = 1.0f;
         }
         //   Velocity Calculations! (yay!)

         //The horizontal velocity (hv) get applied to the current cell face and the right cell face
         state.hv[index] += forceX;
         state.hv[state.getIndex(gridX + 1, gridY)] += forceX;

         //The vertical velocity (vv) get applied to the current cell face and the bottom cell face
         state.vv[index] += forceY;
         state.vv[state.getIndex(gridX, gridY + 1)] += forceY;
         }
      }
   };