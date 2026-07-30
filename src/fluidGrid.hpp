#pragma once
#include <vector>
#include "fluidSolver.hpp"  
#include "raylib.h"

void drawFluid(const FluidState& state, int screenWidth, int screenHeight) {
    float cellSize = (float)screenHeight / state.height;
    
    for (int j = 0; j < state.height; j++) {
        for (int i = 0; i < state.width; i++) {
            float d = state.density[state.getIndex(i, j)];
            
            // Skip drawing completely dark cells to save GPU work
            if (d <= 0.001f) continue;

            float scrX = i * cellSize;
            float scrY = j * cellSize;

            float scaledD = d * 255.0f;
            if (scaledD > 255.0f) scaledD = 255.0f;

            unsigned char intensity = (unsigned char)scaledD;
            Color c = CLITERAL(Color){ intensity, intensity, intensity, 255 };

            DrawRectangle(scrX, scrY, cellSize, cellSize, c);
            // REMOVED DrawRectangleLines -- this was choking CPU/GPU performance!
        }   
    }
}

void handleGridInput(FluidState& state, int screenWidth, int screenHeight) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        Vector2 mouseDelta = GetMouseDelta();

        float cellSize = (float)screenHeight / state.height;

        // Convert mouse position to floating-point grid coordinates
        float gridX = mousePos.x / cellSize;
        float gridY = mousePos.y / cellSize;

        // Define brush radius in GRID CELLS (e.g., 3.5 cells wide)
        float radius = 3.5f; 
        float radiusSq = radius * radius;

        // Determine bounding box around the brush to avoid checking the whole grid
        int minX = std::max(1, (int)(gridX - radius));
        int maxX = std::min(state.width - 2, (int)(gridX + radius));
        int minY = std::max(1, (int)(gridY - radius));
        int maxY = std::min(state.height - 2, (int)(gridY + radius));

        // Base force scalar from mouse movement
        float forceScalar = 1.5f;
        float baseForceX = mouseDelta.x * forceScalar;
        float baseForceY = mouseDelta.y * forceScalar;

        for (int j = minY; j <= maxY; ++j) {
            for (int i = minX; i <= maxX; ++i) {
                // Calculate squared distance from cell center to mouse position
                float dx = (i + 0.5f) - gridX;
                float dy = (j + 0.5f) - gridY;
                float distSq = dx * dx + dy * dy;

                if (distSq <= radiusSq) {
                    // Smooth falloff factor: 1.0 at center, 0.0 at edge
                    float falloff = 1.0f - (distSq / radiusSq);
                    
                    int index = state.getIndex(i, j);

                    // 1. Add smooth density
                    state.density[index] += 0.8f * falloff;
                    if (state.density[index] > 1.0f) {
                        state.density[index] = 1.0f;
                    }

                    // 2. Add smooth force to velocity faces symmetrically
                    float cellForceX = baseForceX * falloff;
                    float cellForceY = baseForceY * falloff;

                    // Distribute horizontal force evenly across left and right faces of cell (i, j)
                    state.hv[state.getIndex(i, j)]     += cellForceX * 0.5f;
                    state.hv[state.getIndex(i + 1, j)] += cellForceX * 0.5f;

                    // Distribute vertical force evenly across top and bottom faces of cell (i, j)
                    state.vv[state.getIndex(i, j)]     += cellForceY * 0.5f;
                    state.vv[state.getIndex(i, j + 1)] += cellForceY * 0.5f;
                }
            }
        }
    }
}