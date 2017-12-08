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