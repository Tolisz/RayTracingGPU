#ifndef CL_SPHERES_WORLD
#define CL_SPHERES_WORLD

#include "hit_record.cl"
#include "ray.cl"

typedef struct _Spheres_World
{   
    // coordinates
    float3 c[NUMBER_OF_SPHERES];
    //float x[NUMBER_OF_SPHERES];
    //float y[NUMBER_OF_SPHERES];
    //float z[NUMBER_OF_SPHERES];

    // radius
    float r[NUMBER_OF_SPHERES];

    // material
    int mat_id[NUMBER_OF_SPHERES];
    int mat_num[NUMBER_OF_SPHERES];
} 
Spheres_World;

typedef struct _Sphere 
{
    float3 center;
    float r;
    uint mat_id;
    uint mat_num;
}
Sphere;

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


Sphere sphere_world_get_sphere(__global Spheres_World* spheres_world, int i)
{
    Sphere sphere;
    sphere.center = spheres_world->c[i].xyz;
    sphere.r = spheres_world->r[i];
    sphere.mat_id = spheres_world->mat_id[i];
    sphere.mat_num = spheres_world->mat_num[i];

    return sphere;
}

bool spheres_world_hit(__global Spheres_World* spheres_world, Ray* ray, float t_min, float t_max, Hit_Record* rec)
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

    return hit_anithing;
}




#endif