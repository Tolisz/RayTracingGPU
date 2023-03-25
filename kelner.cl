#include "ray.cl"
#include "camera.cl"
#include "sphere.cl"

float3 ray_color(Sphere* sphere, Ray* ray);
float3 ray_at(Ray* ray, float t);
float sphere_hit(Sphere* sphere, Ray* ray);


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
    ray.origin = cam.origin;
    ray.direction = lower_left_corner + u * horizontal + v * vertical - cam.origin;

    // if (i == 224 && j == 399)
    // {
    //     printf("[%f, %f, %f] \n", ray.direction.x, ray.direction.y, ray.direction.z);
    // }

    float3 color = ray_color(&sphere, &ray);

    //uint4 PixelColor = (uint4)(0, (j * 255) / w , (i * 255) /h, 255);
    uint4 PixelColor = (uint4)(color.x * 255, color.y * 255 , color.z * 255, 255);
    int2 PixelPos = (int2)(j, i);
    
    write_imageui(image, PixelPos, PixelColor);
}

float3 ray_color(Sphere* sphere, Ray* ray)
{
    float t = sphere_hit(sphere, ray); 
    if (t > 0.0f)
    {
        float3 N = normalize(ray_at(ray, t) - (float3)(sphere->x, sphere->y, sphere->z));
        return 0.5f * (N + 1.0f);
        //return (float3)(1.0f, 1.0f, 0.5f);
    }

    float3 dir = normalize(ray->direction);
    t = 0.5f*(dir.y + 1.0f);
    return (1.0f-t)*(float3)(1.0f, 1.0f, 1.0f) + t*(float3)(0.5f, 0.7f, 1.0f);
}

float3 ray_at(Ray* ray, float t)
{
    return ray->origin + t * ray->direction;
}

// bool hit_sphere(const point3& center, double radius, const ray& r) {
//     vec3 oc = r.origin() - center;
//     auto a = dot(r.direction(), r.direction());
//     auto b = 2.0 * dot(oc, r.direction());
//     auto c = dot(oc, oc) - radius*radius;
//     auto discriminant = b*b - 4*a*c;
//     return (discriminant > 0);
// }


float sphere_hit(Sphere* sphere, Ray* ray)
{
    float3 center = (float3)(sphere->x, sphere->y, sphere->z);
    float3 oc = ray->origin - center;
    float a = dot(ray->direction, ray->direction);
    float b = 2.0f * dot(oc, ray->direction);
    float c = dot(oc, oc) - sphere->r * sphere->r;
    float discriminant = b*b - 4.0f*a*c;
    // //printf("%f\n", discriminant);
    
    // if (get_global_id(1) == 49 && get_global_id(0) == 28)
    // {
    //     printf("center = [%f, %f, %f] \n", center.x, center.y, center.z);
    //     printf("direct = [%f, %f, %f] \n", ray->direction.x, ray->direction.y, ray->direction.z);
    //     printf("oc     = [%f, %f, %f]\n", oc.x, oc.y, oc.z);
    //     printf("abc    = [%f, %f, %f]\n", a, b, c);
    //     printf("discr  = [%f] \n", discriminant);
    // }
    
    if (discriminant < 0.0f) {
        return -1.0f;
    }
    else {
       return (-b - sqrt(discriminant) ) / (2.0f*a);
    }
}