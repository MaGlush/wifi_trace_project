#pragma once

#include "includes.hpp"
#include "ray.hpp"
#include "voxelgrid.hpp"

class Object // abstract class
{
public:
    vec3 color;
    virtual bool hit ( const SRay&, float&) const = 0; // check on intersections
    virtual vec3 normalize ( const vec3& ) const = 0;

    virtual ~Object(){}
};


struct Sphere : public Object
{   
    vec3 position;
    float radius;   

    Sphere (vec3 centre = vec3(0.0f, 0.0f, 0.0f), float r = 1.0f, vec3 col = vec3(1.0f, 0.0f, 0.0f));
    
    bool hit (const SRay& r, float& point) const; 
    vec3 normalize (const vec3& point) const;
};

struct STriangle : public Object
{
    vec3 A, B, C; // vertices
    vec3 normal; 
    STriangle(vec3 a, vec3 b, vec3 c, vec3 norm=vec3(0,0,0), vec3 col=vec3(1.0f,1.0f,1.0f));

    bool hit (const SRay& r, float& point) const; 
    vec3 normalize (const vec3& point) const;
};

struct SWifiRouter : public Sphere 
{
    float sig_power; // Strength of the signal that router cast

    SWifiRouter(vec3 Position = vec3(0.0f, 0.0f, 0.0f),
                float Radius = 1.0f, 
                vec3 Color = vec3(1.0f, 0.0f, 0.0f),
                float signal_power = 10000);
    
        // Calculate signal power via distance point 
    float GetPower(vec3 point); 
        // Fill voxel grid via marching random rays
    void FillGrid(vector<Object*> objects, CVoxelGrid& grid, uint num);
        // Ray marching algorithm 
    void RayMarch(SRay& ray, vector<Object*> objects, CVoxelGrid& grid, int recursion_step);
        // Check ray and objects intersection
    bool CheckIntersect(const SRay& ray, vector<Object*> objects, float& t, size_t& index);
};