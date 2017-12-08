#include "camera.hpp"



CCamera::CCamera(vec3 Pos, string Orient, float FOV){
    SetDirection(Pos, Orient, FOV);
}

void CCamera::SetDirection(const vec3 Pos, const string Orient, float FOV){
    position = Pos;
    orientation = Orient;
    fov = FOV;
    if(orientation == "top"){
        left = vec3(0.0f, 0.0f, -1.0f);
        right = vec3(1.0f, 0.0f, 0.0f);
    }else if(orientation == "leftbot"){
        left = vec3(0.0f, 0.0f, -1.0f);
        right = vec3(1.0f, 1.0f, 0.0f);
        // right = vec3();                                              /*#ASU NEED FILL, GEOMETRY*/
    }else if(orientation == "rightbot"){
        left = vec3(0.0f, 0.0f, -1.0f);
        right = vec3(-1.0f, 1.0f, 0.0f);
    }else if(orientation == "lefttop"){
        left = vec3(1.0f, 1.0f, 0.0f);
        right = vec3(0.0f, 0.0f, 1.0f);
        // right = vec3();
    }else if(orientation == "righttop"){
        left = vec3(0.0f, 0.0f, 1.0f);
        right = vec3(-1.0f, 1.0f, 0.0f);
        // right = vec3();
    }
    front = glm::normalize(glm::cross(left, right)); // vectors multiply, cross product
}

tuple<vec3,vec3,vec3> CCamera::GetDirection() const{
    return std::make_tuple(left,right,front);
}