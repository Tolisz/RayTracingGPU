#include "ray.cl"
#include "camera.cl"
#include "sphere.cl"
#include "hit_record.cl"

// RAY 
// ---
float3 ray_color(Ray* ray, Spheres_World* spheres_world);
float3 ray_at(Ray* ray, float t);

// SPHERES WORLD
// ------------- 
bool spheres_world_hit(Spheres_World* spheres_world, Ray* ray, float t_min, float t_max, Hit_Record* rec);
Sphere sphere_world_get_sphere(Spheres_World* spheres_world, int i);

// SPHERE
// ------
bool sphere_hit(Sphere* sphere, Ray* ray, float t_min, float t_max, Hit_Record* rec);

// HIT RECORD
// ----------
void hit_record_set_face_normal(Hit_Record* rec ,Ray* r, float3 outward_normal);

/// ---------------------------------- ///
///            MAIN KELNER             ///
/// ---------------------------------- ///

__kernel void ray_tracer(write_only image2d_t image, Camera cam, Spheres_World spheres_world)
{
    int i = get_global_id(0);   // height
    int j = get_global_id(1);   // width

    int h = get_image_height(image);    // height
    int w = get_image_width(image);     // width

    float u = (float)(j) / (w - 1);
    float v = (float)(i) / (h - 1);

    // if (i == 0 && j == 0)
    // {
    //     printf("numb of spheres = %d\n", NUMBER_OF_SPHERES);

    //     for (int k = 0; k < NUMBER_OF_SPHERES; k++)
    //     {
    //         printf("k = %d [%f, %f, %f, %f]\n", k, spheres_world.x[k], spheres_world.y[k], spheres_world.z[k], spheres_world.r[k]);
    //     }
    // }

    // Camera 
    float3 horizontal = (float3)(cam.viewport_width, 0.0f, 0.0f);
    float3 vertical = (float3)(0.0f, cam.viewport_height, 0.0f);
    float3 lower_left_corner = cam.origin - horizontal / 2 - vertical / 2 - (float3)(0.0f, 0.0f, cam.focal_length);

    // Ray
    Ray ray;
    ray.origin = cam.origin;
    ray.direction = lower_left_corner + u * horizontal + v * vertical - cam.origin;

    // Color computing
    float3 color = ray_color(&ray, &spheres_world);

    // Set pixel color
    uint4 PixelColor = (uint4)(color.x * 255, color.y * 255 , color.z * 255, 255);
    int2 PixelPos = (int2)(j, i);
    
    write_imageui(image, PixelPos, PixelColor);
}

/// ------------ ///
///     RAY      ///
/// ------------ ///

float3 ray_color(Ray* ray, Spheres_World* spheres_world)
{
    // float t = spheres_world_hit(spheres_world, ray); 
    // if (t > 0.0f)
    // {
    //     float3 N = normalize(ray_at(ray, t) - (float3)(sphere->x[0], sphere->y[0], sphere->z[0]));
    //     return 0.5f * (N + 1.0f);
    //     //return (float3)(1.0f, 1.0f, 0.5f);
    // }

    Hit_Record rec;
    if (spheres_world_hit(spheres_world, ray, 0.0f, FLT_MAX, &rec)) {
        return 0.5f * (rec.normal + 1.0f);
    }

    float3 dir = normalize(ray->direction);
    float t = 0.5f*(dir.y + 1.0f);
    return (1.0f-t)*(float3)(1.0f, 1.0f, 1.0f) + t*(float3)(0.5f, 0.7f, 1.0f);
}

float3 ray_at(Ray* ray, float t)
{
    return ray->origin + t * ray->direction;
}

/// -------------------- ///
///     SPHERE WORLD     ///
/// -------------------- ///

bool spheres_world_hit(Spheres_World* spheres_world, Ray* ray, float t_min, float t_max, Hit_Record* rec)
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
            *rec = temp_rec;
        }
    }

    return hit_anithing;
}

Sphere sphere_world_get_sphere(Spheres_World* spheres_world, int i)
{
    Sphere sphere;
    sphere.center = (float3)(spheres_world->x[i], spheres_world->y[i], spheres_world->z[i]);
    sphere.r = spheres_world->r[i];

    return sphere;
}

/// -------------- ///
///     SPHERE     ///
/// -------------- ///

bool sphere_hit(Sphere* sphere, Ray* ray, float t_min, float t_max, Hit_Record* rec)
{
    float3 oc = ray->origin - sphere->center;
    float a = dot(ray->direction, ray->direction);
    float half_b = dot(oc, ray->direction);
    float c = dot(oc, oc) - sphere->r * sphere->r;
    float discriminant = half_b * half_b - a*c;

    if (discriminant < 0.0f) {
        return false;
    }

    float sqrt_d = sqrt(discriminant);
    
    // Find the nearest root 
    float root = (-half_b - sqrt_d) / a;
    if (root < t_min || root > t_max) {
        root = (-half_b + sqrt_d) / a;
        if (root < t_min || t_max < root) {
            return false;
        }
    }

    rec->t = root;
    rec->p = ray_at(ray, rec->t);
    float3 outward_normal = (rec->p - sphere->center) / sphere->r;
    hit_record_set_face_normal(rec, ray, outward_normal);
    
    return true;
}

/// ------------------ ///
///     HIT RECORD     ///
/// ------------------ ///

void hit_record_set_face_normal(Hit_Record* rec ,Ray* r, float3 outward_normal) {
    rec->front_face = dot(r->direction, outward_normal) < 0;
    rec->normal = rec->front_face ? outward_normal :-outward_normal;
}