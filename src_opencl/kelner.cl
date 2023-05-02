#include "ray.cl"
#include "camera.cl"
#include "sphere.cl"
#include "hit_record.cl"

#include "scatter.cl"
#include "moving_sphere.cl"

float3 ray_color
(
    __global Spheres_World*           spheres_world,
    __global Moving_Sphere*           moving_sphere,
    __global Material_Albedo*         mat_albedo,
    __global Material_Fuzz*           mat_fuzz,
    __global Material_Reflectance*    mat_reflectance,
    Ray*                              ray, 
    mwc64x_state_t*                   rng
);

void write_color
(
    write_only image2d_t image, 
    float3 color
);

bool world_hit
(
    __global Spheres_World* spheres_world, 
    __global Moving_Sphere* moving_sphere,
    Ray* ray, 
    float t_min,
    float t_max,
    Hit_Record* rec
);


/// ---------------------------------- ///
///            MAIN KELNER             ///
/// ---------------------------------- ///

__kernel void ray_tracer
(
    write_only image2d_t image, 
    __global Camera*                    cam, 
    __global Spheres_World*             spheres_world, 
    __global Material_Albedo*           mat_albedo,
    __global Material_Fuzz*             mat_fuzz,
    __global Material_Reflectance*      mat_reflectance,
    __global Moving_Sphere*             moving_sphere)
{
    int i = get_global_id(0);   // height
    int j = get_global_id(1);   // width

    int h = get_image_height(image);    // height
    int w = get_image_width(image);     // width

    //To musi pojsc do trybu debagowego


    // if (i == 0 && j == 0) {
    //     printf("-------------- SPHERES --------------");
        
    //     for (int k = 0; k < NUMBER_OF_SPHERES; k++) {
    //         printf("k = %d\n", k);
    //         printf("c = [%f, %f, %f]\n", spheres_world->c[k].x, spheres_world->c[k].y, spheres_world->c[k].z);
    //         printf("r = %f\n", spheres_world->r[k]);
    //         printf("mat_id = %d\n", spheres_world->mat_id[k]);
    //         printf("mat_num = %d\n\n", spheres_world->mat_num[k]);
    //     }
    // }



    // Random number generator
    mwc64x_state_t rng;
    MWC64X_SeedStreams(&rng, 2 * j * h * w, 2 * i * w * h);

    // Color computing
    float3 color = (float3)(0.0f, 0.0f, 0.0f);
    // s - sampler
    for(int s = 0; s < SAMPLES_PER_PIXEL; s++) {
        float u = (float)(j + random_float(&rng)) / (w - 1);
        float v = (float)(i + random_float(&rng)) / (h - 1);

        // Ray
        Ray ray = camera_get_ray(cam, &rng, u, v);
        color += ray_color(spheres_world, moving_sphere, mat_albedo, mat_fuzz, mat_reflectance, &ray, &rng);
    }

    write_color(image, color);
}







float3 ray_color
(
    __global Spheres_World*           spheres_world,
    __global Moving_Sphere*           moving_sphere,
    __global Material_Albedo*         mat_albedo,
    __global Material_Fuzz*           mat_fuzz,
    __global Material_Reflectance*    mat_reflectance,
    Ray*                              ray, 
    mwc64x_state_t*                   rng)
{
    Hit_Record rec;

    float3 end_attenuation = 1.0f;
    for (int recursion = MAX_RECURSION_DEPTH; recursion >= 0; --recursion)
    {
        if (recursion <= 0) {
            return (float3)(0.0f, 0.0f, 0.0f);
        }

        if (world_hit(spheres_world, moving_sphere, ray, 0.001f, FLT_MAX, &rec)) {
            float3 attenuation;
            Ray scattered;

            switch (rec.mat_id)
            { 
                // Lambertian
                case 0:

                    if (scatter_lambertian(mat_albedo, ray, &rec, &attenuation, &scattered, rng)) {
                        ray->origin = scattered.origin;
                        ray->direction = scattered.direction;
                        //ray->time = scattered.time;
                        end_attenuation *= attenuation;
                    }
                    else  {
                        return (float3)(0.0f, 0.0f, 0.0f);
                    }
                    
                    break;

                // Metal
                case 1:

                    if (scatter_metal(mat_fuzz, ray, &rec, &attenuation, &scattered, rng)) {
                        ray->origin = scattered.origin;
                        ray->direction = scattered.direction;
                        //ray->time = scattered.time;
                        end_attenuation *= attenuation;
                    } else {
                        return (float3)(0.0f, 0.0f, 0.0f);
                    }
                    break;

                // Dielectric
                case 2:
                    if (scatter_dielectric(mat_reflectance, ray, &rec, &attenuation, &scattered, rng)) {
                        ray->origin = scattered.origin;
                        ray->direction = scattered.direction;
                        //ray->time = scattered.time;
                        end_attenuation *= attenuation;
                    } else {
                        return (float3)(0.0f, 0.0f, 0.0f);
                    }

                    break;
            }
        }
        else{
            break;
        }
    }

    float3 dir = normalize(ray->direction);
    float t = 0.5f*(dir.y + 1.0f);
    return end_attenuation * ((1.0f-t)*(float3)(1.0f, 1.0f, 1.0f) + t*(float3)(0.5f, 0.7f, 1.0f));
}

void write_color
(
    write_only image2d_t image, 
    float3 color)
{
    int i = get_global_id(0);   // height
    int j = get_global_id(1);   // width

    int2 PixelPos = (int2)(j, i);

    float scale = 1.0f / SAMPLES_PER_PIXEL;
    color = sqrt(scale * color);

    uint4 PixelColor = (uint4)(
        clamp(color.x, 0.0f, 0.9999f) * 256,
        clamp(color.y, 0.0f, 0.9999f) * 256,
        clamp(color.z, 0.0f, 0.9999f) * 256, 256);
    
    write_imageui(image, PixelPos, PixelColor);
}


bool world_hit
(
    __global Spheres_World* spheres_world, 
    __global Moving_Sphere* moving_sphere,
    Ray* ray, 
    float t_min,
    float t_max,
    Hit_Record* rec
)
{   
    Hit_Record temp_rec;
    bool hit_anithing = false;
    float t_nearest = t_max;

    for (int i = 0; i < NUMBER_OF_SPHERES; i++)
    {
        Sphere sphere = sphere_world_get_sphere(spheres_world, i);
        if (sphere_hit(&sphere, ray, t_min, t_nearest, &temp_rec)) {
            hit_anithing = true;
            t_nearest = temp_rec.t;
            temp_rec.mat_id = sphere.mat_id;
            temp_rec.mat_num = sphere.mat_num;
            *rec = temp_rec;
        }
    }

    for (int i = 0; i < NUM_OF_MOVING_SPHERE; i++)
    {
        if (moving_sphere_hit(moving_sphere, i, ray, t_min, t_nearest, &temp_rec)) {
            hit_anithing = true;
            t_nearest = temp_rec.t;
            temp_rec.mat_id = moving_sphere->mat_id[i];
            temp_rec.mat_num = moving_sphere->mat_num[i];
            *rec = temp_rec;
        }
    }

    return hit_anithing;
}