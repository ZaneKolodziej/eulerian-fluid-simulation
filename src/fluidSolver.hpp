#pragma once
#include <algorithm> // For std::clamp
#include <vector>
#include <cmath>

struct FluidState
{
   int width;
   int height;
   int allocatedWidth;
   int allocatedHeight;
   //Vorticity confinement strength constant
   float epsilon = 0.05f;
   //Size of each cell in units
   float dx = 1.0f;
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
   //divergence
   std::vector<float> divergence;
   //vorticity
   std::vector<float> vorticity;

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
    divergence.resize(totalSize, 0.0f);
    vorticity.resize(totalSize);
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
         float hvCurrent = bilerp(xCurrent - 0.5f, yCurrent, hv);
         float vvCurrent = bilerp(xCurrent, yCurrent - 0.5f, vv);
         float xMid = xCurrent - (hvCurrent * dt * 0.5f);
         float yMid = yCurrent - (vvCurrent * dt * 0.5f);
         //Step two, repeat step one but with the new midpoint coordinates to find the midpoint velocities
         float hvMid = bilerp(xMid - 0.5f, yMid, hv);
         float vvMid = bilerp(xMid, yMid - 0.5f, vv);
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
      if (b == 1) {
         // Horizontal Velocity (hv): Left and Right walls are solid
         for (int j = 0; j < height; ++j) {
               array[getIndex(0, j)] = 0.0f;
               array[getIndex(width - 1, j)] = 0.0f;
         }
      } else if (b == 2) {
         // Vertical Velocity (vv): Top and Bottom walls are solid
         for (int i = 0; i < width; ++i) {
               array[getIndex(i, 0)] = 0.0f;
               array[getIndex(i, height - 1)] = 0.0f;
         }
      } else {
         // Pressure and Density (b == 0): Zero gradient (copy neighbor)
         for (int j = 1; j < height - 1; ++j) {
               array[getIndex(0, j)] = array[getIndex(1, j)];
               array[getIndex(width - 1, j)] = array[getIndex(width - 2, j)];
         }
         for (int i = 1; i < width - 1; ++i) {
               array[getIndex(i, 0)] = array[getIndex(i, 1)];
               array[getIndex(i, height - 1)] = array[getIndex(i, height - 2)];
         }
         // Corners
         array[getIndex(0, 0)] = 0.5f * (array[getIndex(0, 1)] + array[getIndex(1, 0)]);
         array[getIndex(width - 1, 0)] = 0.5f * (array[getIndex(width - 2, 0)] + array[getIndex(width - 1, 1)]);
         array[getIndex(0, height - 1)] = 0.5f * (array[getIndex(1, height - 1)] + array[getIndex(0, height - 2)]);
         array[getIndex(width - 1, height - 1)] = 0.5f * (array[getIndex(width - 1, height - 2)] + array[getIndex(width - 2, height - 1)]);
      }
   }

   void project (){
      for (int j = 1; j < height - 1; ++j) {
         for (int i = 1; i < width - 1; ++i) {
            //Calculate the divergence in each cell
            divergence[getIndex(i, j)] = (hv[getIndex(i + 1, j)] - hv[getIndex(i, j)]) + (vv[getIndex(i, j + 1)] - vv[getIndex(i, j)]);
         }
      }
      //Over-Relaxtion factor calculation
      //float omega = 2.0f / (1.0f + std::sinf(3.14159265f / static_cast<float>(allocatedWidth)));
      float omega = 1.5f;
      //Red-Black Gauss-Siedel Successive Over Relaxtion time! (Rolls off the tongue doesn't it)
      for (int iter = 0; iter < 30; ++iter){
      //Red Cell loop
         for (int j = 1; j < height - 1; ++j) {
            for (int i = 1; i < width - 1; ++i) {
               if ((i + j) % 2 == 0) { //Red Cell check
                  //Calculate Ideal Pressure
                  float p_ideal = 0.25f * (
                     pressure[getIndex(i+1, j)] +
                     pressure[getIndex(i-1, j)] +
                     pressure[getIndex(i, j+1)] +
                     pressure[getIndex(i, j-1)] -
                     divergence[getIndex(i,j)]
                  );
                  //Apply over-relaxation
                  pressure[getIndex(i,j)] += omega * (p_ideal - pressure[getIndex(i,j)]);
               }
            }
         }

         setBoundary(0, pressure);

      //Black Cell loop
         for (int j = 1; j < height - 1; ++j) {
            for (int i = 1; i < width - 1; ++i) {
               if ((i + j) % 2 == 1) { //Black Cell check
                  //Calculate Ideal Pressure
                  float p_ideal = 0.25f * (
                     pressure[getIndex(i+1, j)] +
                     pressure[getIndex(i-1, j)] +
                     pressure[getIndex(i, j+1)] +
                     pressure[getIndex(i, j-1)] -
                     divergence[getIndex(i,j)]
                  );
                  //Apply over-relaxation
                  pressure[getIndex(i,j)] += omega * (p_ideal - pressure[getIndex(i,j)]);
               }
            }
          }
          setBoundary(0, pressure);
      }
      // Subtract pressure gradient from velocities
      for (int j = 1; j < height - 1; ++j) {
         for (int i = 1; i < width - 1; ++i) {
            hv[getIndex(i, j)] -= (pressure[getIndex(i, j)] - pressure[getIndex(i - 1, j)]);
            vv[getIndex(i, j)] -= (pressure[getIndex(i, j)] - pressure[getIndex(i, j - 1)]);
         }
      }

      // Enforce boundary conditions on corrected velocities
      setBoundary(1, hv);
      setBoundary(2, vv);
   };

   //Vorticity calculations (this all went over my head I've only taken calc 1)
   void applyVorticityConfinement(float dt, float dx) {
      //Pass 1
      for (int i = 1; i < width - 1; ++i) {
         for (int j = 1; j < height - 1; ++j) {
            //horizontal change in velocity
            float partialVV_x = (vv[getIndex(i + 1,j)] - vv[getIndex(i,j)]) / dx;
            //vertical change in velocity
            float partialHV_y = (hv[getIndex(i,j + 1)] - hv[getIndex(i,j)]) / dx;
            //Store scalar vorticity (curl) into the array
            vorticity[getIndex(i,j)] = partialVV_x - partialHV_y;
         }
      }

      // Inside applyVorticityConfinement, after Pass 1 loop finishes:
      for (int i = 0; i < width; ++i) {
         vorticity[getIndex(i, 0)] = 0.0f;
         vorticity[getIndex(i, height - 1)] = 0.0f;
      }
      for (int j = 0; j < height; ++j) {
         vorticity[getIndex(0, j)] = 0.0f;
         vorticity[getIndex(width - 1, j)] = 0.0f;
      }
      //Pass 2
      for (int i = 1; i < width - 1; ++i) {
         for (int j = 1; j < height - 1; ++j) {
            //1. Calculate magnitude gradient of the vorticity
            float dw_dx = (std::abs(vorticity[getIndex(i + 1,j)]) - std::abs(vorticity[getIndex(i - 1,j)]))/(2.0f * dx);
            float dw_dy = (std::abs(vorticity[getIndex(i, j + 1)]) - std::abs(vorticity[getIndex(i, j - 1)]))/(2.0f * dx);

            //2. Calculate length of the length of the gradeint vector (1e-5f added to prevent division by zero)
            float length = std::sqrt(dw_dx * dw_dx + dw_dy * dw_dy) + 1e-5f;

            //3. Normalize to get the unit vector
            float Nx = dw_dx / length;
            float Ny = dw_dy / length;

            //4. Rotate N by 90 degrees and scale by vorticity, epsilon, dx, and dt
            float currentW = vorticity[getIndex(i,j)];
            float Fx = Ny * currentW * epsilon * dx * dt;
            float Fy = -Nx * currentW * epsilon * dx * dt;
            
            //5. Apply forces directly to velocity fields
            // Multiply by dt and distribute symmetrically
            hv[getIndex(i,j)]     += Fx * 0.5f;
            hv[getIndex(i + 1,j)] += Fx * 0.5f;

            vv[getIndex(i,j)]     += Fy * 0.5f;
            vv[getIndex(i,j + 1)] += Fy * 0.5f;
         }
      }
      setBoundary(1, hv);
      setBoundary(2, vv);
   };


   void step(float dt) {
    // 1. Double Buffering: Snapshot the current state into the old arrays
    densityOld = density;
    hvOld = hv;
    vvOld = vv;

    //Pre-Projection (ensures input forces are pressure-corrected)
    project();
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
    // Vorticity test
    applyVorticityConfinement(dt, dx);
    // 3. Post advection projection
    //Ensures the fluid stays incompressible after semi-Lagrangian advection
    project();
}

};
