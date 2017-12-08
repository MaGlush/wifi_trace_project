#include "scene.hpp"

vec3 SLight::PhongShade(const vec3& ambi_const, // ambient reflection constant
                        const vec3& diff_const, // diffuse reflection constant
                        const vec3& spec_const, // specular reflection constant
                        float alpha, // shininess constant for this material
                        const vec3& normal, // normal at hit point on the surface
                        const vec3& hit,
                        const CCamera& cam){
        // direction from the hit point to toward light source
    vec3 dir_light = glm::normalize(position - hit);
        // direction pointing towards the viewer
    vec3 dir_view = glm::normalize(cam.position - hit);
        // direction that a perfectly reflected ray of light would take from this point on the surface
        // glm::reflect(a,b) : return a - 2*(a,b)*b
        // dir_reflect = 2*(dir_light,normal)*normal - dir_light
    vec3 dir_reflect = glm::normalize(-glm::reflect(dir_light, normal));  		
    
    float dot_light_normal = glm::dot(dir_light, normal); 
    float dot_reflect_view = glm::dot(dir_reflect, dir_view); 

    float radius = glm::distance(position, hit);
    float area = 4*PI*radius*radius*1e-5f;
    vec3 color = intensity / area;

    if(dot_light_normal < 0.0f) // no light
        return vec3(0.0f, 0.0f, 0.0f);

        // return diffuse 
    if(dot_reflect_view < 0.0f) // no reflections 
        return color * ( ambi_const + diff_const * dot_light_normal );

        // return ambient + diffuse + specular
    float specular = pow(dot_reflect_view, alpha);
    return color * ( ambi_const + diff_const*dot_light_normal + spec_const*specular );
}

CModel::CModel(const char* filename){
    position = vec3(0.0f, 0.0f, 0.0f);
    float max = numeric_limits<float>::max();
 	left = bot = far = max;
 	right = top = near = -max;
    bool loaded = Load(filename);
    if(!loaded){
        cerr << "Error : loading model" << endl;
        exit(0);
    }
}

bool CModel::Load(const char* filename){
	Assimp::Importer import;
    const aiScene *scene = import.ReadFile(filename, aiProcess_Triangulate);	
	
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        cerr << "Error : assimp model loading" << import.GetErrorString() << endl;
        return false;
    }
    CalcNode(scene->mRootNode, scene);
    Init();

    return true;
}

void CModel::CalcNode(aiNode *node, const aiScene *scene){

    for(uint i = 0; i < node->mNumMeshes; i++){
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(ConvertMesh(mesh));			
    }
        // same for children
    for(uint i = 0; i < node->mNumChildren; i++){
        CalcNode(node->mChildren[i], scene);
    }
}  

SMesh CModel::ConvertMesh(aiMesh *mesh){
    vector<vec3> vertices;
    vector<vec3> normals;

    for(uint i = 0; i < mesh->mNumVertices; i++){
        vec3 vertex = ConvertVec(mesh->mVertices[i]) + position;
        vertices.push_back(vertex);
        vec3 normal = ConvertVec(mesh->mNormals[i]);
        normals.push_back(normal);
    }

    return SMesh(vertices, normals);
}  

vec3 CModel::ConvertVec(aiVector3D& aiVec){
    vec3 vec;
    vec.x = aiVec.x;
    vec.y = aiVec.y;
    vec.z = aiVec.z;

    return vec;
}

void CModel::Init(){
    vec3* vec_ptr;
    vec3 normal;

	for(uint i = 0; i < meshes.size(); i++){
		for(uint j = 0; j < meshes[i].vertices.size(); j+=3){
            normal = meshes[i].normals[j];
			vec_ptr = &meshes[i].vertices[j];
            SetBound(vec_ptr);  
			STriangle temp(vec_ptr[0], vec_ptr[1], vec_ptr[2], normal);
			triangles.push_back(temp);
		}
	}
    left_top_near = vec3(left, top, near);
	right_bot_far = vec3(right, bot, far);
}

void CModel::SetBound(vec3* v_ptr){

    for(uint i = 0; i < 3; i++){
        vec3 v = *(v_ptr+i);

        left = v.x < left ? v.x : left;
        bot = v.y < bot ? v.y : bot;
        far = v.z < far ? v.z : far;
        
        right = v.x > right ? v.x : right;
        top = v.y > top ? v.y : top;
        near = v.z > near ? v.z : near;
    }
} 