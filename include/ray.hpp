#pragma once

#include "includes.hpp"
#include "camera.hpp"

struct SRay
{
    vec3 origin; // Cast point
    vec3 direction; // Cast direction
    float power;
	float reflect; // Reflection coefficient

public:
    void SetPower(float p);
    void SetReflect(float r);
    static SRay BuildRay(float x, float y, CCamera& camera); // #ASU here. remove static. do witho

public:

    SRay(const vec3& orig = vec3(0.0f,0.0f,0.0f), 
         const vec3& dir = vec3(1.0f,0.0f,0.0f), 
         float p = 0.0f, float refl = 0.0f);
};