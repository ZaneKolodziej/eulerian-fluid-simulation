#include "raylib.h"
#include "fluidSolver.hpp"
#include "fluidGrid.hpp"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    // Initialize the FluidGrid (64x64)
    FluidState state(64, 64);

    InitWindow(screenWidth, screenHeight, "Eulerian Fluid Simulation");

    SetTargetFPS(60);               // Set to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // GetFrameTime() gives the exact time passed since the last frame (~0.016s at 60 FPS)
        float dt = GetFrameTime();

        //----------------------------------------------------------------------------------
        // TODO: Add mouse forces / user input here before updating the physics
        //----------------------------------------------------------------------------------
        handleGridInput(state, screenWidth, screenHeight);
        // TODO: Call the master step function here (e.g., state.step(dt);)
        state.step(dt);
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);
            
            // Render the fluid grid scaled to the screen size
            drawFluid(state, screenWidth, screenHeight);

        EndDrawing();
        //----------------------------------------------------------------------------------
    } // <-- This closes the while loop

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
} // <-- This closes the main function