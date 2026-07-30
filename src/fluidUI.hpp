#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include "fluidSolver.hpp"  
#include "raylib.h"

struct UIDiagnostics {
    bool showDivergenceHeatmap = false;
    float currentRMSError = 0.0f;
};

// Helper function to calculate exact divergence for a single cell (i, j)
inline float calculateCellDivergence(const FluidState& state, int i, int j) {
    // Left face is index (i, j), Right face is (i + 1, j)
    float u_right = state.hv[state.getIndex(i + 1, j)];
    float u_left  = state.hv[state.getIndex(i, j)];

    // Top face is index (i, j), Bottom face is (i, j + 1)
    float v_bot   = state.vv[state.getIndex(i, j + 1)];
    float v_top   = state.vv[state.getIndex(i, j)];

    return (u_right - u_left) + (v_bot - v_top);
}

// 1. Compute overall RMS Divergence Error across the grid
inline float computeRMSDivergence(const FluidState& state) {
    float totalError = 0.0f;
    int interiorCells = (state.width - 2) * (state.height - 2);

    for (int j = 1; j < state.height - 1; ++j) {
        for (int i = 1; i < state.width - 1; ++i) {
            float div = calculateCellDivergence(state, i, j);
            totalError += div * div;
        }
    }
    return std::sqrt(totalError / std::max(1, interiorCells));
}

// 2. Main Fluid Render (handles both normal smoke and divergence heatmaps)
inline void drawFluid(const FluidState& state, int screenWidth, int screenHeight, const UIDiagnostics& ui) {
    float cellSize = (float)screenHeight / state.height;

    for (int j = 0; j < state.height; j++) {
        for (int i = 0; i < state.width; i++) {
            float scrX = i * cellSize;
            float scrY = j * cellSize;

            if (ui.showDivergenceHeatmap) {
                // Skip boundary cells for divergence sampling
                if (i == 0 || i == state.width - 1 || j == 0 || j == state.height - 1) continue;

                float div = calculateCellDivergence(state, i, j);

                float errorScale = std::min(1.0f, std::abs(div) * 5.0f);
                unsigned char intensity = static_cast<unsigned char>(errorScale * 255.0f);

                Color c = BLACK;
                if (div > 0.0001f) {
                    c = Color{ intensity, 0, 0, 255 };      // Red = positive divergence
                } else if (div < -0.0001f) {
                    c = Color{ 0, 0, intensity, 255 };      // Blue = negative divergence
                }

                DrawRectangle(scrX, scrY, cellSize, cellSize, c);
            } 
            else {
                // --- Standard Density Rendering ---
                float d = state.density[state.getIndex(i, j)];
                if (d <= 0.001f) continue;

                unsigned char intensity = static_cast<unsigned char>(std::min(1.0f, d) * 255.0f);
                Color c = CLITERAL(Color){ intensity, intensity, intensity, 255 };

                DrawRectangle(scrX, scrY, cellSize, cellSize, c);
            }
        }   
    }
}

// 3. Draw UI Diagnostics Panel on the Unused Right Sidebar
inline void drawSidebarUI(const FluidState& state, UIDiagnostics& ui, int sidebarX) {
    // Update error metric
    ui.currentRMSError = computeRMSDivergence(state);

    // Title Block
    DrawText("FLUID DIAGNOSTICS", sidebarX, 20, 20, RAYWHITE);
    DrawText(TextFormat("Mode: %s (Press TAB)", ui.showDivergenceHeatmap ? "Divergence Map" : "Smoke Density"), 
             sidebarX, 50, 16, GRAY);

    // Error Metric Text
    Color statusColor = (ui.currentRMSError < 0.01f) ? GREEN : 
                        (ui.currentRMSError < 0.05f ? YELLOW : RED);
                        
    DrawText(TextFormat("RMS Div Error: %.5f", ui.currentRMSError), sidebarX, 90, 18, statusColor);

    // Error Meter Bar
    float maxBarWidth = 200.0f;
    float currentBarWidth = std::min(maxBarWidth, ui.currentRMSError * 2000.0f);
    DrawRectangle(sidebarX, 120, static_cast<int>(currentBarWidth), 15, statusColor);
    DrawRectangleLines(sidebarX, 120, static_cast<int>(maxBarWidth), 15, GRAY);
}