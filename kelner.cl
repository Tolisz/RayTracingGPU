#include "ray.cl"
#include "camera.cl"
#include "sphere.cl"
#include "hit_record.cl"
#include "materials.cl"

// Rundom number generator
#include "mwc64x.cl"


/// ---------------------------------- ///
///         STRUCT FUNCTIONS           ///
/// ---------------------------------- ///


// RAY 
// ---
float3 ray_color(Ray* ray, Spheres_World* spheres_world, mwc64x_state_t* rng, Materials* materials);
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

// CAMERA
// ------
Ray camera_get_ray(Camera* cam, float u, float v);

/// ---------------------------------- ///
///          HELP FUNCTIONS            ///
/// ---------------------------------- ///


// RANDOM
// ------
float random_float(mwc64x_state_t* rng);
float random_float_in(mwc64x_state_t* rng, float min, float max);
float3 random_float3(mwc64x_state_t* rng);
float3 random_float3_in(mwc64x_state_t* rng, float min, float max);
float3 random_in_unit_sphere(mwc64x_state_t* rng);

// SCATTER 
// --------
bool scatter_lambertian(Ray* r_in, Hit_Record* rec, float3* attenuation, Ray* scattered,  Materials* materials, mwc64x_state_t* rng);
bool scatter_metal(Ray* r_in, Hit_Record* rec, float3* attenuation, Ray* scattered,  Materials* materials, mwc64x_state_t* rng);
bool scatter_dielectric(Ray* r_in, Hit_Record* rec, float3* attenuation, Ray* scattered,  Materials* materials, mwc64x_state_t* rng);

// COLOR
// ----- 
void write_color(write_only image2d_t image, float3 color);

// VECTORS
// -------
float3 reflect(float3 v, float3 n);
bool near_zero(float3 v);
float3 refract(float3 uv, float3 n, float etai_over_etat);
float reflectance(float cosine, float ref_dx);


/// ---------------------------------- ///
///            MAIN KELNER             ///
/// ---------------------------------- ///

__kernel void ray_tracer(write_only image2d_t image, Camera cam, Spheres_World spheres_world, Materials materials)
{
    int i = get_global_id(0);   // height
    int j = get_global_id(1);   // width

    int h = get_image_height(image);    // height
    int w = get_image_width(image);     // width

    // Camera 
    //float3 horizontal = (float3)(cam.viewport_width, 0.0f, 0.0f);
    //float3 vertical = (float3)(0.0f, cam.viewport_height, 0.0f);
    //float3 lower_left_corner = cam.origin - horizontal / 2 - vertical / 2 - (float3)(0.0f, 0.0f, cam.focal_length);

    // if (i == 0 && j == 0)
    // {
    //     printf("o = [%f, %f, %f]\n", cam.origin.x,cam.origin.y,cam.origin.z );
    //     printf("l = [%f, %f, %f]\n", cam.lower_left_corner.x,cam.lower_left_corner.y,cam.lower_left_corner.z );
    //     printf("h = [%f, %f, %f]\n", cam.horizontal.x,cam.horizontal.y,cam.horizontal.z );
    //     printf("v = [%f, %f, %f]\n", cam.vertical.x,cam.vertical.y,cam.vertical.z );
    // }
    //ray.origin = cam.origin;
    //ray.direction = lower_left_corner + u * horizontal + v * vertical - cam.origin;

    // Random number generator
    mwc64x_state_t rng;
    MWC64X_SeedStreams(&rng, 2 * j * h * w, 2 * i * w * h);

    // if (i == 0 && j == 0) {
    //     printf("INT_MAX %d\n", INT_MAX);
    //     printf("FLOAT_MAX %f\n", FLT_MAX);
    //     for (int i = 0; i < 1000; i++)
    //         printf("%.14f\n", random_float(&rng));
    // }

    // Color computing
    float3 color = (float3)(0.0f, 0.0f, 0.0f); //= ray_color(&ray, &spheres_world);
    // s - sampler
    for(int s = 0; s < SAMPLES_PER_PIXEL; s++) {
        float u = (float)(j + random_float(&rng)) / (w - 1);
        float v = (float)(i + random_float(&rng)) / (h - 1);

        // Ray
        Ray ray = camera_get_ray(&cam, u, v);
        color += ray_color(&ray, &spheres_world, &rng, &materials);
    }


    // Set pixel color
    // uint4 PixelColor = (uint4)(color.x * 255, color.y * 255 , color.z * 255, 255);
    // int2 PixelPos = (int2)(j, i);
    
    // write_imageui(image, PixelPos, PixelColor);
    write_color(image, color);
}

/// ------------ ///
///     RAY      ///
/// ------------ ///

float3 ray_color(Ray* ray, Spheres_World* spheres_world, mwc64x_state_t* rng, Materials* materials)
{
    Hit_Record rec;

    float3 end_attenuation = 1.0f;
    for (int recursion = MAX_RECURSION_DEPTH; recursion >= 0; --recursion)
    {
        if (recursion <= 0) {
            return (float3)(0.0f, 0.0f, 0.0f);
        }

        if (spheres_world_hit(spheres_world, ray, 0.001f, FLT_MAX, &rec)) {
            //return 0.5f * (rec.normal + 1.0f);

            float3 attenuation;
            Ray scattered;
            // switch (rec.material_type) {
            //     // Lambertian
            //     case 0:
            //         if (scatter_lambertian(ray, &rec, &attenuation, &scattered, materials, rng)) {
            //             ray->origin = scattered.origin;
            //             ray->direction = scattered.direction;
            //             end_attenuation *= 1.0f;
            //         }
            //         else  {
            //             return (float3)(0.0f, 0.0f, 0.0f);
            //         }
            //         break;
            // }

            //printf("%d", rec.material_type);
            switch (rec.material_type)
            { 
                // Lambertian
                case 0:
                    if (scatter_lambertian(ray, &rec, &attenuation, &scattered, materials, rng)) {
                        ray->origin = scattered.origin;
                        ray->direction = scattered.direction;
                        end_attenuation *= attenuation;
                    }
                    else  {
                        return (float3)(0.0f, 0.0f, 0.0f);
                    }
                    
                    break;

                // Metal
                case 1:
                    if (scatter_metal(ray, &rec, &attenuation, &scattered, materials, rng)) {
                        ray->origin = scattered.origin;
                        ray->direction = scattered.direction;
                        end_attenuation *= attenuation;
                    } else {
                        return (float3)(0.0f, 0.0f, 0.0f);
                    }
                    break;

                case 2:
                    if (scatter_dielectric(ray, &rec, &attenuation, &scattered, materials, rng)) {
                        ray->origin = scattered.origin;
                        ray->direction = scattered.direction;
                        end_attenuation *= attenuation;
                    } else {
                        return (float3)(0.0f, 0.0f, 0.0f);
                    }

                    break;
            }

            // printf("material_type = %d\n", rec.material_type);

            // float3 rand = random_in_unit_sphere(rng);
            // float3 target = rec.p + rec.normal + rand;
            // // float3 target = reflect(rec.p - ray->direction, rec.normal);

            // ray->origin = rec.p;
            // ray->direction = target - rec.p;

            // end_attenuation *= 0.5f;
        }
        else{
            break;
        }
    }

    float3 dir = normalize(ray->direction);
    float t = 0.5f*(dir.y + 1.0f);
    return end_attenuation * ((1.0f-t)*(float3)(1.0f, 1.0f, 1.0f) + t*(float3)(0.5f, 0.7f, 1.0f));
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
            temp_rec.sphere_id = i;
            temp_rec.material_type = sphere.material_type;
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
    sphere.material_type = spheres_world->material_type[i];

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

/// ------------------ ///
///      RANDOM        ///
/// ------------------ ///

// Function generates random float number in [0, 1) 
float random_float(mwc64x_state_t* rng)
{
    int n = MWC64X_NextUint(rng);
    return (((float)n / INT_MAX) + 1.0f) / 2.0f;
}

float random_float_in(mwc64x_state_t* rng, float min, float max)
{
    return min + (max - min) * random_float(rng);
}

float3 random_float3(mwc64x_state_t* rng)
{
    return (float3)(random_float(rng), random_float(rng), random_float(rng));
}

float3 random_float3_in(mwc64x_state_t* rng, float min, float max)
{
    return (float3)(random_float_in(rng, min, max), random_float_in(rng, min, max), random_float_in(rng, min, max));
}

float3 random_in_unit_sphere(mwc64x_state_t* rng)
{
    while (true) {
        float3 p = random_float3_in(rng, -1.0f, 1.0f);

        if (dot(p, p) >= 1) 
            continue;

        return p;
    }
}

// ---------
//  SCATTER 
// ---------
bool scatter_lambertian(Ray* r_in, Hit_Record* rec, float3* attenuation, Ray* scattered, Materials* materials, mwc64x_state_t* rng)
{
    float3 scatter_direction = rec->normal + random_in_unit_sphere(rng);
    
    // Catch degenerate scatter direction
    if (near_zero(scatter_direction)) {
        scatter_direction = rec->normal;
    }
    
    scattered->origin = rec->p;
    scattered->direction = scatter_direction;

    *attenuation = materials->albedo[rec->sphere_id];

    return true;
}

bool scatter_metal(Ray* r_in, Hit_Record* rec, float3* attenuation, Ray* scattered,  Materials* materials, mwc64x_state_t* rng)
{
    float3 reflected = reflect(r_in->direction , rec->normal);

    scattered->origin = rec->p;
    scattered->direction = reflected + materials->fuzz[rec->sphere_id] * random_in_unit_sphere(rng);

    *attenuation = materials->albedo[rec->sphere_id];

    return (dot(scattered->direction, rec->normal) > 0);
}

bool scatter_dielectric(Ray* r_in, Hit_Record* rec, float3* attenuation, Ray* scattered,  Materials* materials, mwc64x_state_t* rng)
{
    *attenuation = (float3)(1.0f, 1.0f, 1.0f);
    float refraction_ratio = rec->front_face ? (1.0f / materials->ir[rec->sphere_id]) : materials->ir[rec->sphere_id];

    float3 unit_direction = normalize(r_in->direction);
    float cos_theta = fmin(dot(-unit_direction, rec->normal), 1.0f); 
    float sin_theta = sqrt(1.0f - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0f;
    float3 direction;

    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float(rng)) {
        direction = reflect(unit_direction, rec->normal);
    } else {
        direction = refract(unit_direction, rec->normal, refraction_ratio);
    }

    scattered->origin = rec->p;
    scattered->direction = direction;

    return true;
}




// =--=-=-=-=---=-=-=-=-=---==--=-=-=-=-=-==-

void write_color(write_only image2d_t image, float3 color)
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

float3 reflect(float3 v, float3 n)
{
    return v - 2 * dot(v, n)* n;
}

float3 refract(float3 uv, float3 n, float etai_over_etat)
{
    float cos_theta = fmin(dot(-uv, n), 1.0f);
    float3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    float3 r_out_parallel = -sqrt(fabs(1.0f - dot(r_out_perp, r_out_perp))) * n;
    
    return r_out_parallel + r_out_perp;
}

float reflectance(float cosine, float ref_idx)
{
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0)*pow((1 - cosine), 5);
}

bool near_zero(float3 v)
{
    const float s = 1e-8;
    return (fabs(v.x) < s) && (fabs(v.y) < s) && (fabs(v.z) < s);
}


Ray camera_get_ray(Camera* cam, float u, float v)
{
    Ray r;
    r.origin = cam->origin;
    r.direction = cam->lower_left_corner + u * cam->horizontal + v * cam->vertical - cam->origin;
    return r;
}