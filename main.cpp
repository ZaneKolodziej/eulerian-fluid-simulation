#include "raylib.h"

int main() {
    // 1. Initialize the window size and title
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Eulerian Fluid Simulation - Phase 1");

    // 2. Target 60 frames per second
    SetTargetFPS(60);

    // 3. The Main Game Loop (runs continuously until you close the window)
    while (!WindowShouldClose()) {
        
        // --- UPDATE LOGIC WILL GO HERE ---

        // 4. Drawing Layer
        BeginDrawing();
        ClearBackground(BLACK); // Clear the screen to black every frame

        // Draw a simple circle at the mouse position to test interactivity
        Vector2 mousePos = GetMousePosition();
        DrawCircleV(mousePos, 20, BLUE);

        DrawText("Raylib is working!", 10, 10, 20, RAYWHITE);

        EndDrawing();
    }

    // 5. Clean up and close OpenGL context
    CloseWindow();
    return 0;
}