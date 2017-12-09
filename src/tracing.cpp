#include "tracing.hpp"
#include "postprocessing.hpp"
#include "geometry.hpp"

CTracer::CTracer(const CModel& loaded_model, const CVoxelGrid& grid):
    model (loaded_model),
    vox_grid (grid)
{
    for(uint i = 0; i < model.triangles.size(); i++)
        objects.push_back(&(model.triangles[i]));
}
void CTracer::SaveImageToFile(const Image &img, const char *path){
    BMP out;
    out.SetSize(img.n_cols, img.n_rows);

    uint r, g, b;
    RGBApixel p;
    p.Alpha = 255;
    for (uint i = 0; i < img.n_rows; i++) {
        for (uint j = 0; j < img.n_cols; j++) {
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

    for (uint i = 0; i < res.n_rows; i++) {
        for (uint j = 0; j < res.n_cols; j++) {
            RGBApixel *p = in(j, i);
            res(i, j) = std::make_tuple(p->Red, p->Green, p->Blue);
        }
    }

    return res;
}

void CTracer::PlaceRouter(SWifiRouter& router){
    router.FillGrid(objects, vox_grid, 80000);
    vox_grid.GridFilter();
    objects.push_back(&router);
}

SRay CTracer::BuildRay(float x, float y, CCamera& eye){
    return SRay::BuildRay(x, y, eye);
}

std::tuple<uint,uint,uint> CTracer::vec3_to_color(vec3 vector){
    uint x = uint(vector.x);
    uint y = uint(vector.y);
    uint z = uint(vector.z);
    return std::make_tuple(x, y, z);
}

vec3 ColorScheme(float value){
    if(value < EPS) {
            return glm::vec3(0.0f,0.0f,0.0f);
        }
    float log_value = log10(value);
    log_value = log_value * 10.0f;

    float k2 = (log_value + 80.0f) / 100.0f;
    if(k2 > 1.0f)
        k2 = 1.0f;
    float k1 = 1.0f - k2;
    
    return glm::normalize(glm::vec3(0.0f,0.0f,100.0f) * k1 + glm::vec3(13.0f,0.0f,0.0f) * k2);
}

vec3 CTracer::RayMarcher(const SRay& ray, vec3 color){
    float depth = 0.0f;
    float step = 0.005;
    int idx;
    int p_idx;
    ssize_t march_num = 0;
    bool first = true;
    float alpha = 0.2f;
    vec3 routerColor = vec3(0.0f,0.0f,0.0f);
    for(int i = 0; i < MAX_MARCH_STEPS; i++){
        vec3 p = ray.origin + 
                 vec3(ray.direction.x*depth, \
                      ray.direction.y*depth, \
                      ray.direction.z*depth);
        depth+=step;
        if(first){
            idx = vox_grid.GetVoxel(p);
            first = false;
        }
        else{
            p_idx = idx;
            idx = vox_grid.GetVoxel(p);
            if(idx==p_idx){
                continue;
            }
        }
        if(idx > 0){
            march_num++;
            float value = vox_grid.voxels[idx].value;
            routerColor += ColorScheme(value);
            //routerColor += vec3(value/30.0f, value/7.0f, value/100.0f);
            //routerColor = min(vec3(255.0f,200.0f,100.0f), routerColor);

        }
        else{ // out of vox_grid
            if (march_num)
                return color * (1 - alpha) + routerColor*(255.0f/march_num)*alpha;
            else
                return color;
        }
    }

    
    return color;
}

tuple3uint CTracer::RayTracer(const SRay& ray){
    float t, t_near = std::numeric_limits<float>::max();
    int index_near; // index of the closest intersected object
    bool intersected = false;
    vec3 res_color = vec3(0.0f, 0.0f, 0.0f);
    for(size_t i = 0; i < objects.size(); i++){
            //check intersection
        if(objects[i]->hit(ray, t)){    
            if (t < t_near){
                intersected = true;
                t_near = t;
                index_near = i;
            }
        }
    }
    if(!intersected) return vec3_to_color(res_color); // no intersection, returns black

    vec3 point = ray.origin + ray.direction*t_near; // hit point 
    vec3 normal = objects[index_near]->normalize(point); // normal at hit point
    normal = glm::dot(ray.direction, normal) > 0 ? -normal : normal; // reverse normal if its inside 
    point = point + normal*0.01f;
   
    SRay shadow_ray;
    for(uint k = 0; k < lights.size(); k++){
            // processing shadow parts
        vec3 lightdir = lights[k].position - point;
        lightdir = glm::normalize(lightdir);
        shadow_ray = SRay (point, lightdir); 
        bool shadowed = false;
        for(uint i = 0; i < objects.size(); i++)
            if ( objects[i]->hit(shadow_ray, t)){
                if(t > 0){
                    shadowed = true; 
                    break;
                }
            }
        if( !shadowed ){
            float dif = glm::dot(lightdir, normal);
            if(dif < 0.0f) 
                continue;
            SRay eye(point, ray.origin - point); 
            vec3 phong_color = lights[k].PhongShade(vec3(0.1f, 0.1f, 0.1f), // ambient reflection constant
                                                    objects[index_near]->color*0.7f, // diffuse reflection constant
                                                    vec3(0.1f, 0.1f, 0.1f), // specular reflection constant
                                                    10.0f,  // shininess constant for this material
                                                    normal, 
                                                    point,
                                                    current_camera);
            // phong_color = min(vec3(255.0f, 255.0f, 255.0f), phong_color);
            res_color += RayMarcher(eye, phong_color);
        }
        res_color = min(vec3(255.0f, 255.0f, 255.0f), res_color);
    }
    return vec3_to_color(res_color);
}

void CTracer::RenderScene(int width, int height){

    for(uint k = 0; k < cameras.size(); k++){
        current_camera = cameras[k];
        cerr << current_camera.orientation << " view rendering ...";
        
        Image res_image(height, width);
        current_camera.width = float(width);
        current_camera.height = float(height);

        const int p10 = height*width / 10;
        int part = p10;
        int progress = 0;
        uint i,j;
        // #pragma omp parallel for private(i,j)
        for(i = 0; i < res_image.n_rows; i++)
            for(j = 0; j < res_image.n_cols; j++){	
                SRay ray = BuildRay(j, i, current_camera);
                res_image(i, j) = RayTracer(ray);
                    // progress bar 
                if ( (i*height+j)%part == 0){
                    cerr << progress << "\%\n"; 
                    // #pragma omp critical
                    progress += 10;
                }
            }
        cerr << "completed" << endl;
        res_image = PostProcessing(res_image);  
        std::string path = string("./") + current_camera.orientation + string(".bmp");
        SaveImageToFile(res_image, path.c_str());
        cerr << "Saved " << path << endl;
    }
}