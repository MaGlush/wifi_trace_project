#pragma once

#include "includes.hpp"
#include "geometry.hpp"
#include "scene.hpp"
#include "tracing.hpp"

void default_run()
{
    int xRes = 1920;  // Default resolution
    int yRes = 1080;
    const char* model_path = "../../data/model/flat1.obj";
        
        /* Camera and Light creation */
    CCamera front(vec3(-13.0f,3.0f,5.0f), "leftbot");
    CCamera top(vec3(0.0f,20.0f,0.0f), "top");
    SLight light(vec3(-6.0f, 10.0f, -10.0f), 10.0f);
    SLight light2(vec3(20.0f, 20.0f, 1.0f), 1.0f);
        
        /* Model setting up */
    CModel model(model_path);
    CVoxelGrid grid(model.left_top_near, model.right_bot_far);
    grid.Init();      

        /* Scene init */
    CTracer tracer(model, grid);

    SWifiRouter router(vec3(-6.0f,0.0f,0.0f), 0.2f, vec3(1.0f,0.0f,0.0f), 10000.0f);
    tracer.PlaceRouter(router);

    tracer.lights.push_back(light);
    tracer.lights.push_back(light2);
    tracer.cameras.push_back(front);
    tracer.cameras.push_back(top);
    
    tracer.RenderScene(xRes, yRes);
}