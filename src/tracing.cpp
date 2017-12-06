#include "tracing.hpp"
#include "postprocessing.hpp"

CTracer::CTracer(const CModel& loaded_model, const CVoxelGrid& vox_grid):
	model (loaded_model),
    vox_grid (vox_grid)
{}

void CTracer::SaveImageToFile(const Image &img, const char *path){
    BMP out;
    out.SetSize(img.n_cols, img.n_rows);

    uint r, g, b;
    RGBApixel p;
    p.Alpha = 255;
    for (uint i = 0; i < img.n_rows; ++i) {
        for (uint j = 0; j < img.n_cols; ++j) {
            std::tie(r, g, b) = img(i, j);
            p.Red = r; p.Green = g; p.Blue = b;
            out.SetPixel(j, i, p);
        }
    }

    if (!out.WriteToFile(path))
        throw string("Error writing file ") + string(path);
}

Image CTracer::LoadImageFromFile(const char *path){
    BMP in;

    if (!in.ReadFromFile(path))
        throw string("Error reading file ") + string(path);

    Image res(in.TellHeight(), in.TellWidth());

    for (uint i = 0; i < res.n_rows; ++i) {
        for (uint j = 0; j < res.n_cols; ++j) {
            RGBApixel *p = in(j, i);
            res(i, j) = std::make_tuple(p->Red, p->Green, p->Blue);
        }
    }

    return res;
}

void CTracer::PlaceRouter(vec3 position){
    SWifiRouter router();
    objects.push_back(&router);
}

SRay CTracer::BuildRay(float x, float y){
    return SRay::BuildRay(x, y);
}

std::tuple<uint,uint,uint> CTracer::vec3_to_color(vec3 vector){
    uint x = uint(vector.x);
    uint y = uint(vector.y);
    uint z = uint(vector.z);
    return std::make_tuple(x, y, z);
}

vec3 CTracer::RayMarcher(const SRay& ray, vec3 color, float t_closest){
    float depth = 0.0f;
    float step = 0.005;
    int idx;
    int p_idx;
    bool first = true;
    float alpha = 0.04f;
    vec3 routerColor = vec3(0.0f,0.0f,0.0f);;
    for(int i = 0; i < MAX_MARCH_STEPS; ++i){
        vec3 point = ray.origin + 
                     vec3(ray.direction.x*depth, \
                          ray.direction.y*depth, \
                          ray.direction.z*depth);
        depth+=step;
        if(first){
            idx = vox_grid.GetVoxel(point);
            first = false;
        }
        else{
            p_idx = idx;
            idx = vox_grid.GetVoxel(point);
            if(idx==p_idx){
                continue;
            }
        }
        if(idx>0){
            float value = vox_grid.voxels[idx].value;
            routerColor += vec3(value/7.f, value/30.f, value/100.f);
            routerColor = min(vec3(255.f,200.f,100.f), routerColor);

            color =  color * (1 - alpha) + routerColor*alpha;
        }
        else{ // out of vox_grid
            return color;
        }
    }

    
    return color;
}

tuple3uint CTracer::RayTracer(const SRay& ray){
    float t0, t1, t_closest = std::numeric_limits<float>::max();
    int i_closest; // index of the closest intersect object
    bool isInter = false;
    vec3 res_color = vec3(0.f, 0.f, 0.f);
    for(size_t i = 0; i < objects.size(); ++i){
            //check intersection
        if(objects[i]->intersect(ray, t0, t1))
        {    
            if (t0 < 0) t0 = t1;
            if (t0 < t_closest) 
            {
                isInter = true;
                t_closest = t0;
                i_closest = i;
            }

        }
    }
    if(!isInter) return vec3_to_color(res_color); // no intersection, returns black

    vec3 pHit = ray.origin + ray.direction*t_closest; // hit point 
    vec3 nHit = objects[i_closest]->normalize(pHit); // normal at hit point
    nHit = glm::dot(ray.direction, nHit) > 0 ? -nHit : nHit; // reverse normal if its inside 
    pHit = pHit + nHit*1e-2f;
   
    SRay shadow_ray;
    for(uint k = 0; k < lights.size(); ++k){
            // processing shadow parts
        vec3 lightdir = lights[k].position - pHit;
        lightdir = glm::normalize(lightdir);
        shadow_ray = SRay (pHit, lightdir); 
        bool isShadow = false;
        for(uint i = 0; i < objects.size(); ++i)
            if ( objects[i]->intersect(shadow_ray, t0, t1))
            {
                if(t0 > 0)
                {
                    isShadow = true; 
                    break;
                }
            }
        if( !isShadow ){
            float dif = glm::dot(lightdir, nHit);
            if(dif < 0.f) 
                continue;
            SRay eye(pHit, ray.origin - pHit); 
            vec3 phong_color = lights[k].PhongShade(vec3(0.1f, 0.1f, 0.1f), // ambient reflection constant
                                                    objects[i_closest]->color*0.7f, // diffuse reflection constant
                                                    vec3(0.1f, 0.1f, 0.1f), // specular reflection constant
                                                    10.f,  // shininess constant for this material
                                                    nHit, 
                                                    pHit,
                                                    current_camera);
            phong_color = min(vec3(255.f, 255.f, 255.f), phong_color);
            res_color += RayMarcher(eye, phong_color, t_closest);
        }
        res_color = min(vec3(255.f, 255.f, 255.f), res_color);
    }
    return vec3_to_color(res_color);
}

void CTracer::RenderScene(int width, int height){

    for(uint k = 0; k < cameras.size(); ++k){
        
        current_camera = cameras[k];
        cerr << current_camera.orientation << " view rendering ...";
        
        Image res_image(height, width);
        current_camera.width = float(width);
        current_camera.height = float(height);

        const int p10 = height*width / 10;
        int part = p10;
        int progress = 0;
        uint i,j;
        #pragma omp parallel for private(i,j)
        for(i = 0; i < res_image.n_rows; ++i)
            for(j = 0; j < res_image.n_cols; ++j){	
                SRay ray = BuildRay(j, i);
                res_image(i, j) = RayTracer(ray);
                    // progress bar 
                if ( (i*height+j)%part == 0){
                    cerr << progress << "%%\n"; 
                    progress += 10;
                    part += p10;
                }
            }
        cerr << " completed" << endl;
        // res_image = postprocessing(res_image);  /* #ASU add postprocessing autocontrast from CG-1 align_project*/
        std::string path = string("./build/bin/") + current_camera.orientation + string(".bmp");
        SaveImageToFile(res_image, path.c_str());
        cerr << "Saved " << path << endl;
    }
}