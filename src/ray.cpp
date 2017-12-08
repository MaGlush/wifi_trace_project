#include "ray.hpp"

SRay::SRay(const vec3& orig, const vec3& dir, float p, float refl):
		origin(orig), 
        direction(dir), 
        power(p),
        reflect(refl)
{}


void SRay::SetPower(float p){
	power = p;
}

void SRay::SetReflect(float r){
    reflect = r;
}

SRay SRay::BuildRay(float x, float y, CCamera& eye){
	float ratio = eye.width / eye.height;

    vec3 left, right, front;
    std::tie(left, right, front) = eye.GetDirection();

	vec3 ray_dir = front + 
                   right * ((x+0.5f) / eye.width  - 0.5f) * ratio - 
                   left  * ((y+0.5f) / eye.height - 0.5f);

    return SRay( eye.position, glm::normalize(ray_dir) );
}