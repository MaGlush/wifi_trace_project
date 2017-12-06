#pragma once

#include "includes.hpp"

class CCamera
{
	vec3 left; // Camera orientation
	vec3 right;
	vec3 front;  // Orthonormal basis = cross [up, right]
	float fov; // Field of view, angle

public:
		// Setting up camera at the scene
	void SetDirection(vec3 Pos, string Orient, float FOV);
		// Getting position and orientation of the camera
	tuple<vec3,vec3,vec3> GetDirection() const;

public:
	vec3 position;  // Camera position
	string orientation;
	float width, height; // Image resolution

	CCamera(vec3 position = vec3(0.0f, 10.0f, 0.0f), string orientation = "top", float fov = PI/2);
};

struct SLight 
{
    vec3 position; 
    vec3 intensity; 

	vec3 PhongShade(const vec3& ambi_const, // ambient reflection constant
					const vec3& diff_const, // diffuse reflection constant
					const vec3& spec_const, // specular reflection constant
					float alpha, // shininess constant for this material
					const vec3& normal, // normal at hit point on the surface
					const vec3& hit_point,
					const CCamera& camera);
					    
    SLight(const vec3& p, const float i = 1.0f): position(p), intensity(vec3(i,i,i))
	{}
};

struct SMesh
{
	vector<vec3> vertices;
	vector<vec3> normals;

	SMesh(vector<vec3> _vertices, vector<vec3> _normals): vertices(_vertices), normals(_normals)
    {}
};


class CModel
{
	float left, bot, far;
	float right, top, near;

public: 
		// Import model with assimp  	
 	bool Load(const char* filename);
	 	// Calculate assimp node meshes
 	void CalcNode(aiNode *node, const aiScene *scene);
		// Set up model filling meshes and triangles
 	void Init();
		// Convert assimp mesh to CMesh
	SMesh ConvertMesh(aiMesh *mesh);
        // Get vec3 from aiVector3D aiVec
    vec3 ConvertVec(aiVector3D& aiVec);
	    // Correction bounds values
	void SetBound(vec3* vec_ptr);

public:
	// vector<STriangle> triangles;
	vector<SMesh> meshes;
	vec3 left_top_near;
	vec3 right_bot_far;
	vec3 position;

	CModel(const char* filename = "no path");
};