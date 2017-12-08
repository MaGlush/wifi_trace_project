#include "geometry.hpp"

Sphere::Sphere (vec3 centre, float r, vec3 col): position (centre), radius(r)
{
    color = col;
}
    
bool Sphere::hit (const SRay& ray, float& point) const {

    vec3 v = ray.origin - position;
    float b = dot(v, ray.direction);
    float c = dot(v,v) - radius*radius;
    float d2 = b*b - c;
    
    if(d2 < 0)
        return false;
    
    float d = sqrtf(d2);
    float t1 = -b + d;
    float t2 = -b - d;

    float min_t  = min(t1, t2);
    float max_t = max(t1, t2);

    point = (min_t >= 0) ? min_t : max_t;
    
    return (point > 0);
}

vec3 Sphere::normalize (const vec3& point) const {
    return glm::normalize(point - position);
}

STriangle::STriangle(vec3 a, vec3 b, vec3 c, vec3 norm, vec3 col):
    A(a), B(b), C(c),
    normal(norm)
{
    color = col;
}

bool STriangle::hit (const SRay& ray, float& point) const {
    vec3 AB = B - A;
	vec3 AC = C - A;
	vec3 c1 = glm::cross(ray.direction, AC);
	float d1 = glm::dot(AB, c1);
	if (fabs(d1) < 0.0001f) // ray parallel to triangle
        return false;
	vec3 v = ray.origin - A;
	float d2 = glm::dot(v, c1) / d1;
    if (d2 < 0 || d2 > 1) 
        return false;
	vec3 c2 = glm::cross(v, AB);
	float d3 = glm::dot(ray.direction, c2) / d1;
	if (d3 < 0 || d2 + d3 > 1) 
        return false;
	point = glm::dot(AC,c2) / d1;

	return true; 
}

vec3 STriangle::normalize (const vec3& point) const {
    return glm::normalize(normal);
}

SWifiRouter::SWifiRouter(vec3 Position, float Radius, vec3 Color, float signal_power)
{
    position = Position;
    radius = Radius;
    color = Color;
    sig_power = signal_power;
}
    
float SWifiRouter::GetPower(vec3 point){
	float length = glm::length(point - position);
	if(length < radius){
		return 0.0f;
    }else{
		return min(sig_power, sig_power/(length*length));
    }
}

void SWifiRouter::FillGrid(vector<Object*> objects, CVoxelGrid& grid, uint num){
    uint i;
    // #pragma omp parallel for private(i)
	for(i = 0; i < num ; i++){
        vec3 vec;
        vec.x = rand() / (RAND_MAX/(2.0f)) - 1;
        vec.y = rand() / (RAND_MAX/(2.0f)) - 1;
        vec.z = rand() / (RAND_MAX/(2.0f)) - 1;
        vec3 direction = glm::normalize(vec);
		SRay random_ray = SRay(position, direction, sig_power);
		RayMarch(random_ray, objects, grid, 0);
	}
}
void SWifiRouter::RayMarch(SRay& ray, vector<Object*> objects, CVoxelGrid& grid, int recursion_step){
    float depth = 0.0f;
	float d_step = 0.02f;
    float end = 100.0f;
    float near = 100.0f;
    size_t index;
    if (CheckIntersect(ray, objects, near, index)){
        end = near;
        vec3 point = ray.origin + ray.direction*near;
        float refl_chance =  float(rand()) / float(RAND_MAX);
        if (refl_chance > 0.5f){ // reflection
            if(recursion_step < RECURSIVE_STEPS){
                point += objects[index]->normalize(point)*0.01f;
                vec3 refl_dir = glm::normalize(glm::reflect(ray.direction, objects[index]->normalize(point)));
                float power = GetPower(point);
                SRay refl_ray(point, refl_dir, power);
                RayMarch(refl_ray, objects, grid, recursion_step+1);
            }
        }else{ // absorption
            vec3 normal =objects[index]->normalize(point);
            point -= normal*0.01f;
            float power = GetPower(point) - ray.reflect*10.0f;
            SRay tray(point, ray.direction, power);
            float t_near = 100.0f;    
            size_t idx;
            if(CheckIntersect(tray, objects, t_near, idx)){
                vec3 p = tray.origin + tray.direction*t_near;
                vec3 n = objects[idx]->normalize(p);
                p += n*0.01f;
                SRay no_refl_ray(p, tray.direction, tray.power, tray.reflect+1);
                RayMarch(no_refl_ray, objects, grid, recursion_step); 
            }
        }
    }
	for(int i = 0; i < MAX_MARCH_STEPS; i++){
		vec3 p = ray.origin + vec3(ray.direction.x*depth, ray.direction.y*depth, ray.direction.z*depth);
		depth += d_step;
        ray.power = GetPower(p) - ray.reflect*10.0f;
		if(depth > end){
			return;
		}
            // set grid voxel with signal power
		int idx = grid.GetVoxel(p);
		if(idx > 0){
			if(grid.voxels[idx].value < ray.power)
			    grid.voxels[idx].value = ray.power;
		}
		else{
			// out of grid
			return;
		}
		
	}
	return;
}
bool SWifiRouter::CheckIntersect(const SRay& ray, vector<Object*> objects, float& near, size_t& index){
    bool intersect = false;
	float t = 100.f;
	for(size_t i = 0; i < objects.size(); ++i)
    {
			if(objects[i]->hit(ray, t))
            {
                if ((t < near) && (t > 0.f))
                {
                    intersect = true;
                    near = t;
                    index = i;
                }
			}
	}
	return intersect;
}