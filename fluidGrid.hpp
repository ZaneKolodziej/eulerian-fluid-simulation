#pragma once

#include <vector>
#include "raylib.h"

struct fluidGrid 
{
    int width;
    int height;
    std::vector<float> cells;

    // 1. Constructor
    fluidGrid(int w, int h) 
    {
        width = w;
        height = h;
        cells.resize(w * h, 0.0f);
    }

    // 2. Coordinate Indexer
    int getIndex(int x, int y) const 
    {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return -1; // Out of bounds
        }
        else {
            return y * width + x;
        }
    }

    // 3. Draw Function
    void draw(int screenWidth, int screenHeight) const {
        // cellSize calculation using current window size
        int cellSize = screenWidth / width;

        TraceLog(LOG_INFO, "Drawing grid...");
        
        for (int y = 0; y < height; y++){
            for (int x = 0; x < width; x++){

                int posX = x * cellSize;
                int posY = y * cellSize;

                DrawRectangleLines(posX, posY, cellSize, cellSize, LIGHTGRAY);
            }
        }
    }
};