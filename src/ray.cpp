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

void SRay::SetCast(CCamera& camera){
    eye = camera;
}

SRay SRay::BuildRay(float x, float y){
    float width = eye.width;
    float height = eye.height;
	float ratio = width / height;

    vec3 left, right, front;
    std::tie(left, right, front) = eye.GetDirection();

	vec3 ray_dir = front + 
                   right * ((x+0.5f) / width  - 0.5f) * ratio - 
                   left  * ((y+0.5f) / height - 0.5f);

    return SRay( eye.position, glm::normalize(ray_dir) );
}