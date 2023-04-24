#ifndef CL_CAMERA
#define CL_CAMERA

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

#endif // CL_CAMERA