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
	void PlaceRouter(SWifiRouter& router);
        // Render scene with ray tracing and save it to file
    void RenderScene(int xRes, int yRes);
        // Create ray for specified scene pixel
    SRay BuildRay(float x, float y, CCamera& cam);
        // Check ray intersection points and calculate their color 
    tuple3uint RayTracer (const SRay& ray);
        // TraceRay processing part, ray marching algorithm using voxel grid   
    vec3 RayMarcher (const SRay& ray, vec3 color);

public:
    CModel model;
    CVoxelGrid vox_grid;
    vector<Object*> objects;
	vector<SLight> lights;
	vector<CCamera> cameras;
    CCamera current_camera;

    CTracer(const CModel& loaded_model, const CVoxelGrid& grid);
};