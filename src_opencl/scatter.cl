#ifndef CL_SCATTER
#define CL_SCATTER

#include "material_albedo.cl"
#include "material_fuzz.cl"
#include "material_reflactance.cl"

#include "ray.cl"
#include "hit_record.cl"
#include "helpers.cl"
#include "random.cl"

bool scatter_lambertian(
    __global Material_Albedo* materials,
    Ray* r_in, Hit_Record* rec, float3* attenuation, Ray* scattered, mwc64x_state_t* rng)
{
    float3 scatter_direction = rec->normal + random_in_unit_sphere(rng);
    
    // Catch degenerate scatter direction
    if (near_zero(scatter_direction)) {
        scatter_direction = rec->normal;
    }
    
    scattered->origin = rec->p;
    scattered->direction = scatter_direction;
    scattered->time = r_in->time;

    *attenuation = materials->albedo[rec->mat_num];

    return true;
}


bool scatter_metal( 
    __global Material_Fuzz* materials, 
    Ray* r_in, Hit_Record* rec, float3* attenuation, Ray* scattered, mwc64x_state_t* rng)
{
    float3 reflected = reflect(r_in->direction , rec->normal);

    scattered->origin = rec->p;
    scattered->direction = reflected + materials->fuzz[rec->mat_num] * random_in_unit_sphere(rng);
    scattered->time = r_in->time;

    *attenuation = materials->albedo[rec->mat_num];

    return (dot(scattered->direction, rec->normal) > 0);
}


bool scatter_dielectric(
    __global Material_Reflectance* materials, 
    Ray* r_in, Hit_Record* rec, float3* attenuation, Ray* scattered, mwc64x_state_t* rng)
{
    *attenuation = (float3)(1.0f, 1.0f, 1.0f);
    float refraction_ratio = rec->front_face ? (1.0f / materials->reflection_index[rec->mat_num]) : materials->reflection_index[rec->mat_num];

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
    scattered->time = r_in->time;

    return true;
}

#endif