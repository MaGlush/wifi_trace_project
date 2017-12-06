#include "includes.hpp"
#include "ray.hpp"
#include "voxelgrid.hpp"

// #ASU go to geometry.cpp

class Object // abstract class
{
public:
    vec3 color;
    virtual bool hit ( const SRay&, float&, float&) const = 0;
    virtual vec3 normalize ( const vec3& ) const = 0;

    virtual ~Object(){}
};


struct Sphere : public Object
{   
    vec3 position;
    float radius;   

    Sphere (vec3 cen = vec3(0.f, 0.f, 0.f),
            const float r = 1.f,
            const vec3 col = vec3(1.f, 0.f, 0.f));
    
    bool hit (const SRay& r, float& t0, float& t1) const; 
    vec3 normalize (const vec3& pHit) const;
};

struct STriangle : public Object
{
	vec3 A, B, C;
	vec3 normal;
	STriangle(vec3 a, vec3 b, vec3 c, 
              vec3 norm = vec3(0.f, 0.f, 0.f),
              vec3 col = vec3(1.f, 1.f, 1.f));
	
	bool hit (const SRay& r, float& t0, float& t1) const; 
    vec3 normalize (const vec3& pHit) const;
};

struct SWifiRouter : public Sphere 
{
    float sig_power; // Strength of the signal that router cast

    SWifiRouter(vec3 Position = vec3(0.f, 0.f, 0.f),
                float Radius = 1.f, 
                vec3 Color = vec3(1.f, 0.f, 0.f),
                float signal_power = 10000);
    
        // Calculate signal power via distance point 
	float CalcPowet(vec3 point); 
        // Fill voxel grid via marching random rays
	void FillGrid(std::vector<Object*> figures, CVoxelGrid& grid, uint num);
        // Ray marching algorithm 
	void March(SRay& ray, std::vector<Object*> figures, CVoxelGrid& grid, int recursion_step);
        // Check ray and objects intersection
	bool Intersect(const SRay& ray, std::vector<Object*> figures, float& t, size_t& index);
};