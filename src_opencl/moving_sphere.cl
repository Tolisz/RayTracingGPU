#ifndef CL_MOVING_SPHERE
#define CL_MOVING_SPHERE

#include "ray.cl"
#include "hit_record.cl"

typedef struct __attribute__ ((packed)) _Moving_Sphere
{
    float3 center0[NUM_OF_MOVING_SPHERE];
    float3 center1[NUM_OF_MOVING_SPHERE];

    float time0[NUM_OF_MOVING_SPHERE];
    float time1[NUM_OF_MOVING_SPHERE];
    float radius[NUM_OF_MOVING_SPHERE];

    int mat_id[NUM_OF_MOVING_SPHERE];
    int mat_num[NUM_OF_MOVING_SPHERE];
}
Moving_Sphere;

void moving_sphere_debug(__global Moving_Sphere* ms)
{
    printf("----------------------------------------\n");
    printf(" Moving Sphere Debug Info\n");
    printf("----------------------------------------\n");

    for (int i = 0; i < NUM_OF_MOVING_SPHERE; i++) {
        printf("i = %d\n", i);
        printf("c0 = [%f, %f, %f]; c1 = [%f, %f, %f]\n", 
            ms->center0[i].x, ms->center0[i].y, ms->center0[i].z, 
            ms->center1[i].x, ms->center1[i].y, ms->center1[i].z);
        printf("t0 = %f; t1 = %f\n", ms->time0[i], ms->time1[i]);
        printf("mat_id = %4d;  mat_num = %4d\n\n", ms->mat_id[i], ms->mat_num[i]);
    }

    printf("----------------------------------------\n\n");
}

float3 moving_sphere_center(__global Moving_Sphere* ms, int i, float time) {
    return ms->center0[i] + ((time - ms->time0[i]) / (ms->time1[i] - ms->time0[i])) * (ms->center1[i] - ms->center0[i]);
}

bool moving_sphere_hit(__global Moving_Sphere* ms, int i, Ray* ray, float t_min, float t_max, Hit_Record* rec)
{
    float3 oc = ray->origin - moving_sphere_center(ms, i, ray->time);
    float a = dot(ray->direction, ray->direction);
    float half_b = dot(oc, ray->direction);
    float c = dot(oc, oc) - ms->radius[i] * ms->radius[i];
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
    float3 outward_normal = (rec->p - moving_sphere_center(ms, i, ray->time)) / ms->radius[i];
    hit_record_set_face_normal(rec, ray, outward_normal);
    
    return true;
}



#endif