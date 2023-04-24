#ifndef CL_RAY
#define CL_RAY

typedef struct _Ray 
{
    float3 origin;
    float3 direction;
} 
Ray;

float3 ray_at(Ray* ray, float t)
{
    return ray->origin + t * ray->direction;
}

#endif 