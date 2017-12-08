#pragma once

#include "includes.hpp"
#include "ray.hpp"

struct SVoxel
{
    vec3 position;
 	float side;
 	vec3 color;
 	float value;

    float CalcSDF(vec3 point); // Signed Distance Function calculating
 	float CalcDepth(SRay& ray, float start, float end); // Get depth via SDF calculating

public:

    SVoxel(vec3 position = vec3(0.0f, 0.0f, 0.0f),
          float side = 5.0f,
          vec3 color = vec3(1.0f, 1.0f, 1.0f),
          float value = 0.0f); 	
};

class CVoxelGrid
{
	vec3 left_top_near; // Size of grid
	vec3 right_bot_far;
	float voxel_side; 
	vec3 size; // Number of voxels at each axis
	float BoxFilter(const vec3&); // Box filter, reduce noises

public:
    void Init(); // Filling voxels vector
	int GetVoxel(vec3 point); // Get voxel via point
	void GridFilter(); // Initialize box filter
	void PrintGrid(); // Print all voxels values

public:
    vector<SVoxel> voxels;
	
	CVoxelGrid(vec3 left_top_near, vec3 right_bot_far);
};