#include "raylib.h"
#include "fluidSolver.hpp"
#include "fluidGrid.hpp"
#include "fluidUI.hpp"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    // Initialize the FluidGrid (128x128)
    FluidState state(128, 128);

    // Initialize UI diagnostic state (holds heatmap toggle and error readings)
    UIDiagnostics ui;

    InitWindow(screenWidth, screenHeight, "Eulerian Fluid Simulation");

    SetTargetFPS(60);               // Set to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // GetFrameTime() gives the exact time passed since the last frame (~0.016s at 60 FPS)
        float dt = GetFrameTime();

        //----------------------------------------------------------------------------------
        // Input Handling
        //----------------------------------------------------------------------------------
        // Toggle Divergence Heatmap with TAB key
        if (IsKeyPressed(KEY_TAB)) {
            ui.showDivergenceHeatmap = !ui.showDivergenceHeatmap;
        }

        // Add mouse forces & density
        handleGridInput(state, screenWidth, screenHeight);

        // Physics Update Step
        state.step(dt);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);
            
            // 1. Render the fluid grid (passes ui state for density vs heatmap mode)
            drawFluid(state, screenWidth, screenHeight, ui);

            // 2. Draw diagnostics panel on the unused right sidebar space
            int sidebarX = screenHeight + 20; 
            drawSidebarUI(state, ui, sidebarX);

        EndDrawing();
        //----------------------------------------------------------------------------------
    } // <-- This closes the while loop

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
} // <-- This closes the main function