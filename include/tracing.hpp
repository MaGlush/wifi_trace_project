#pragma once

#include "includes.hpp"
#include "scene.hpp"
#include "voxelgrid.hpp"

class CTracer
{
        // Convert vec3 to tuple
    tuple3uint vec3_to_color(vec3 vector);
public:
		// Load image from file	
    Image LoadImageFromFile(const char* filename);
		// Save result image to .bmp file	
    void SaveImageToFile(const Image &img, const char *path);
		// Place wifi router at scene
	void PlaceRouter(vec3 position);
        // Render scene with ray tracing and save it to file
    void RenderScene(int xRes, int yRes);
        // Create ray for specified scene pixel
    SRay BuildRay(float x, float y);
        // Check ray intersection points and calculate their color 
    tuple3uint RayTracer (const SRay& ray);
        // TraceRay processing part, ray marching algorithm using voxel grid   
    vec3 RayMarcher (const SRay& ray, vec3 color, float t_closest);

public:
    CVoxelGrid vox_grid;
    CModel model;
    vector<SObject*> objects;
	vector<SLight> lights;
	vector<CCamera> cameras;
    CCamera current_camera;

    CTracer(const CModel& model, const CVoxelGrid& grid);
};