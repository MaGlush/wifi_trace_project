#pragma once

#include "includes.hpp"
#include "scene.hpp"

struct SRay
{
    vec3 origin; // Cast point
    vec3 direction; // Cast direction
    float power;
	float reflect; // Reflection coefficient
    static CCamera eye; // Eye casting rays

public:
    void SetPower(float p);
    void SetReflect(float r);
    static void SetCast(CCamera& camera);
    static SRay BuildRay(float x, float y);

public:

    SRay(const vec3& orig = vec3(0.0f,0.0f,0.0f), 
         const vec3& dir = vec3(1.0f,0.0f,0.0f), 
         float p = 0.0f, float refl = 0.0f);
};