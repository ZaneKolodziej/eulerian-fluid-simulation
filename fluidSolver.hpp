#pragma once
#include <algorithm> // For std::clamp
#include <vector>


struct FluidState
{
   int width;
   int height;
   int allocatedWidth;
   int allocatedHeight;
   //Horizontal Velocity
   std::vector<float> hv;
   std::vector<float> hvOld;
   //Vertical Velocity
   std::vector<float> vv;
   std::vector<float> vvOld;
   //Density
   std::vector<float> density;
   std::vector<float> densityOld;
   //Pressure
   std::vector<float> pressure;

   //indexing functions 
   //takes a set of 2d coordinates (i, j) and converts it to their 1d array equivolent
   int getIndex (int i, int j) const {
        int index = i + (j * allocatedWidth);
        return index;
   };

   //Constructor
   FluidState (int w, int h): width(w), height(h), allocatedWidth(w+2), allocatedHeight(h+2) {
    int totalSize = allocatedWidth * allocatedHeight;
    hv.resize(totalSize, 0.0f);
    hvOld.resize(totalSize, 0.0f);
    vv.resize(totalSize, 0.0f);
    vvOld.resize(totalSize, 0.0f);
    density.resize(totalSize, 0.0f);
    densityOld.resize(totalSize, 0.0f);
    pressure.resize(totalSize, 0.0f);
   };

   //Bilinear Interpolater 
   float bilerp (float x, float y, const std::vector<float>& array) {
      //Find the four corners of the point
      int x0 = static_cast<int>(x);
      int y0 = static_cast<int>(y);
      int x1 = x0 + 1;
      int y1 = y0 + 1;
      //Find the interpolation weights (clamp just incase)
      float tx = std::clamp(x - x0, 0.0f, 1.0f);
      float ty = std::clamp(y - y0, 0.0f, 1.0f);
      //Clamp the values
      x0 = std::clamp(x0, 0, width - 1);
      x1 = std::clamp(x1, 0, width - 1);
      y0 = std::clamp(y0, 0, height - 1);
      y1 = std::clamp(y1, 0, height - 1);
      //Find the index of the corners and then the value in the index
      float topLeft = array[getIndex(x0, y0)];
      float topRight = array[getIndex(x1, y0)];
      float bottomLeft = array[getIndex(x0, y1)];
      float bottomRight = array[getIndex (x1, y1)];
      //         Interpolation Time!

      //Top blend
      float topBlend = ((topLeft * (1.0f - tx)) + (topRight * tx));
      //Bottom blend
      float bottomBlend = ((bottomLeft * (1.0f - tx)) + (bottomRight * tx));
      //Final blend
      float finalValue = ((topBlend * (1.0f - ty)) + (bottomBlend * ty));

      return finalValue;
   };

   //Advection code
   //It will take an old array and a new array as well as an x offset, a y offset, and a delta time parameter to reduce the amount of spaghetti in the code and for "optimization" or something
   void advect (const std::vector<float>& srcArray, std::vector<float>& destArray, float xOffset, float yOffset, float dt){
      //Loop over the rows (j/y axis)
      for (int j = 0; j < height; ++j){
         //Loop over the columns (i/x axis)
        for (int i = 0; i < width; ++i){
         //Add the staggering offset for velocity arrays (doesn't affect density arrays)
         float xCurrent = static_cast<float>(i) + xOffset;
         float yCurrent = static_cast<float>(j) + yOffset;
         //       Runge-Kutta 2 time
         //Step one, find the current velocities and then multiply by half a time step (dt) to get a midpoint value
         float hvCurrent = bilerp(xCurrent, yCurrent, hv);
         float vvCurrent = bilerp(xCurrent, yCurrent, vv);
         float xMid = xCurrent - (hvCurrent * dt * 0.5f);
         float yMid = yCurrent - (vvCurrent * dt * 0.5f);
         //Step two, repeat step one but with the new midpoint coordinates to find the midpoint velocities
         float hvMid = bilerp(xMid, yMid, hv);
         float vvMid = bilerp(xMid, yMid, vv);
         float xPast = xCurrent - (hvMid * dt);
         float yPast = yCurrent - (vvMid * dt);
         //Step two point five, clamp the calculated past coordinates to ensure no simulation issues
         // Clamp coordinates safely within the grid boundaries
         // Leaves a 0.5 buffer to prevent bilerp from overflowing the top/right edges
         if (xPast < 0.5f) xPast = 0.5f;
         if (xPast > width - 1.5f) xPast = width - 1.5f;

         if (yPast < 0.5f) yPast = 0.5f;
         if (yPast > height - 1.5f) yPast = height - 1.5f;
         //Step three, Interpolate "particle" property using calculated previous coordinates, then apply those values to the destination array
         float finalValue = bilerp(xPast, yPast, srcArray);
         destArray[getIndex(i, j)] = finalValue;
       }
     }
   };
   //checks to set if the cell is at a boundary and depending on the array it will flip copy or exact copy a neighbor cell
   void setBoundary(int b, std::vector<float> &array) {
      //Handles the Vertical Walls (Left and Right)
      for (int j = 1; j < height -1; ++j) {
         //Left wall gets it's value from the cell on the right. If the horizontal velocity is (b == 1) reverse it
         array[getIndex(0, j)] = (b == 1) ? -array[getIndex(1, j)] : array[getIndex(1, j)];
         //Right wall gets it's value from the cell on the left
         array[getIndex(width - 1, j)] = (b == 1) ? -array[getIndex(width - 2, j)] : array[getIndex(width - 2, j)];
      }

      //Handles the Horizontal Walls (Top and Bottom)
      for (int i = 1; i < width-1; ++i) {
         //Top wall gets it's value from the cell below it. If the vertical velocity is (b == 2) reverse it
         array[getIndex(i, 0)] = (b == 2) ? -array[getIndex(i, 1)] : array[getIndex(i, 1)];
         //Bottom wall gets it's value from the cell above it.
         array[getIndex(i, height - 1)] = (b == 2) ? -array[getIndex(i, height - 2)] : array[getIndex(i, height - 2)];
      }
      
      //Handles the corners and their neighbors
      array[getIndex(0, 0)] = 0.5f * (array[getIndex(0, 1)] + array[getIndex(1, 0)]);
      array[getIndex(width - 1, 0)] = 0.5f * (array[getIndex(width - 2, 0)] + array[getIndex(width - 1, 1)]);
      array[getIndex(0, height - 1)] = 0.5f * (array[getIndex(1, height - 1)] + array[getIndex(0, height - 2)]);
      array[getIndex(width - 1, height - 1)] = 0.5f * (array[getIndex(width - 1, height - 2)] + array[getIndex(width - 2, height - 1)]);
      
   };


   void step(float dt) {
    // 1. Double Buffering: Snapshot the current state into the old arrays
    densityOld = density;
    hvOld = hv;
    vvOld = vv;

    // 2. Advection Time!
    // Move horizontal velocity (MAC left faces)
    advect(hvOld, hv, -0.5f, 0.0f, dt);
    setBoundary(1, hv);
    // Move vertical velocity (MAC top faces)
    advect(vvOld, vv, 0.0f, -0.5f, dt);
    setBoundary(2, vv);
    // Move smoke density (Cell centers)
    advect(densityOld, density, 0.0f, 0.0f, dt);
    setBoundary(0, density);

    // 3. Next Up: Add diffuse() and project() physics steps here later...
}

};
