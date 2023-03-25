#include "ray.cl"
#include "camera.cl"
#include "sphere.cl"

float3 ray_color(Ray* ray);

__kernel void ray_tracer(write_only image2d_t image, Camera cam, Sphere sphere)
{
    int i = get_global_id(0);   // height
    int j = get_global_id(1);   // width

    int h = get_image_height(image);    // height
    int w = get_image_width(image);     // width

    float u = (float)(j) / (w - 1);
    float v = (float)(i) / (h - 1);

    // Camera 
    float3 horizontal = (float3)(cam.viewport_width, 0.0f, 0.0f);
    float3 vertical = (float3)(0.0f, cam.viewport_height, 0.0f);
    float3 lower_left_corner = cam.origin - horizontal / 2 - vertical / 2 - (float3)(0.0f, 0.0f, cam.focal_length);

    // Ray
    Ray ray;
    ray.origin = (float3)(0.0f, 0.0f, 0.0f);
    ray.direction = lower_left_corner + u * horizontal + v * vertical - origin;

    // if (i == 224 && j == 399)
    // {
    //     printf("[%f, %f, %f] \n", ray.direction.x, ray.direction.y, ray.direction.z);
    // }

    float3 color = ray_color(&ray);

    //uint4 PixelColor = (uint4)(0, (j * 255) / w , (i * 255) /h, 255);
    uint4 PixelColor = (uint4)(color.x * 255, color.y * 255 , color.z * 255, 255);
    int2 PixelPos = (int2)(j, i);
    
    write_imageui(image, PixelPos, PixelColor);
}

float3 ray_color(Ray* ray)
{
    float3 dir = normalize(ray->direction);
    float t = 0.5f*(dir.y) + 1.0f;
    return (1.0f-t)*(float3)(1.0f, 1.0f, 1.0f) + t*(float3)(0.5f, 0.7f, 1.0f);
}