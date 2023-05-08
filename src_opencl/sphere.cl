#ifndef CL_SPHERES_WORLD
#define CL_SPHERES_WORLD


#include "hit_record.cl"
#include "ray.cl"

#if (NUMBER_OF_SPHERES != 0)
    typedef struct __attribute__ ((packed)) _Spheres_World
    {   
        // center
        float3 c[NUMBER_OF_SPHERES];

        // radius
        float r[NUMBER_OF_SPHERES];

        // material
        int mat_id[NUMBER_OF_SPHERES];
        int mat_num[NUMBER_OF_SPHERES];
    } 
    Spheres_World;
#else 
    typedef struct __attribute__ ((packed)) _Spheres_World
    {   
        // center
        float3 c[1];

        // radius
        float r[1];

        // material
        int mat_id[1];
        int mat_num[1];
    } 
    Spheres_World;
#endif

typedef struct _Sphere 
{
    float3 center;
    float r;
    uint mat_id;
    uint mat_num;
}
Sphere;

void sphere_get_uv(float3* p, float* u, float* v)
{
    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

    float theta = acos(-p->y);
    float phi = atan2(-p->z, p->x) + M_PI_F;

    *u = phi / (2 * M_PI_F);
    *v = theta / M_PI_F;
}

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
    sphere_get_uv(&outward_normal, &rec->u, &rec->v);
    
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

#endif