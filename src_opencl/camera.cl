#ifndef CL_CAMERA
#define CL_CAMERA

#include "random.cl"
#include "ray.cl"

typedef struct _Camera 
{
    float3 origin;
    float3 lower_left_corner;
    float3 horizontal;
    float3 vertical;
    float3 u, v, w;
    float lens_radius;
} 
Camera;

Ray camera_get_ray(__global Camera* cam, mwc64x_state_t* rng, float u, float v)
{
    float3 rd = cam->lens_radius * random_in_unit_disk(rng);
    float3 offset = cam->u * rd.x + cam->v * rd.y;

    Ray r;
    r.origin = cam->origin + offset;
    r.direction = cam->lower_left_corner + u * cam->horizontal + v * cam->vertical - cam->origin - offset;
    return r;
}


#endif // CL_CAMERA