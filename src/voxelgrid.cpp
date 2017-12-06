#include "voxelgrid.hpp"

SVoxel::SVoxel(vec3 Position, float Side, vec3 Color, float Value):
	position (Position),
	side (Side),
	color (Color),
	value (Value)
{}

float SVoxel::CalcSDF(vec3 point)
{
	vec3 pp = glm::abs(point - position) - side/2.f;
    return max(pp.x, max(pp.y, pp.z));
}

float SVoxel::CalcDepth(SRay& ray, float start, float end)
{
	float depth = start;
	for(int i = 0; i < MAX_MARCH_STEPS; ++i){
		vec3 point = ray.origin + 
                     vec3(ray.direction.x * depth, \
                          ray.direction.y * depth, \
                          ray.direction.z * depth);
			
		float dist = CalcSDF(point);
		if(dist < EPS)
			return depth;

		depth += dist;
		if(depth >= end)
			break;
	}
	return end;
}

CVoxelGrid::CVoxelGrid(vec3 ltn, vec3 rbf)
{
	left_top_near = ltn;
	right_bot_far = rbf;
	vec3 substract = glm::abs(right_bot_far - left_top_near);
	voxel_side = min(substract.x, min(substract.y, substract.z)) / 20;
	size = substract / voxel_side;
}


void CVoxelGrid::Init()
{
	for(uint k = 0; k < size.z; ++k)
		for(uint j = 0; j < size.y; ++j)
			for(uint i = 0; i < size.x; ++i){
				vec3 offset = left_top_near + vec3(voxel_side/2, voxel_side/2, voxel_side/2);
				voxels.push_back(SVoxel(offset + vec3(voxel_side*i, -voxel_side*j, -voxel_side*k),
						   		 		voxel_side,
						   		 		vec3(1.f, 0.f, 0.f)));
			}
}

int CVoxelGrid::GetVoxel(vec3 point){
	if(point.x > right_bot_far.x || point.y < right_bot_far.y || point.z < right_bot_far.z || 
 	   point.x < left_top_near.x  || point.y > left_top_near.y  || point.z > left_top_near.z)
			return -1;
	
	vec3 substract = glm::abs(point - left_top_near);

	size_t x = substract.x/voxel_side;
	size_t y = substract.y/voxel_side;
	size_t z = substract.z/voxel_side;

	return x + y*size.x + z*size.x*size.y;  
}

void CVoxelGrid::GridFilter()
{
	for(ssize_t k = 1; k < size.z-1; ++k){
 		for(ssize_t j = 1; j < size.y-1; ++j){
 			for(ssize_t i = 1; i < size.x-1; ++i)
			{	
				vec3 idx = vec3(i, j, k);

				float value = BoxFilter(idx);
				voxels[i+j*size.x+k*size.x*size.y].value = value;
 			}
 		}
 	}
}

float CVoxelGrid::BoxFilter(const vec3& v)
{	 
	float val = 0;
	for(size_t k = 0; k < 3; ++k)
		for(size_t j = 0; j < 3; ++j)
			for(size_t i = 0; i < 3; ++i)
				val += voxels[(v.x-1+i)+(v.y-1+j)*size.x+(v.z-1+k)*size.x*size.y].value;
				
	return (1.0f/27.0f)*val;
}

void CVoxelGrid::PrintGrid()
{
	uint size3 = size.x*size.y*size.z;
	for(uint i = 0; i < size3; ++i)
		if(voxels[i].value > 0)
			std::cout << voxels[i].value << endl;
}