#include "spheres_world.hpp"

#define CL_TARGET_OPENCL_VERSION 300

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

#include <cstdlib>

#include "error.h"

Spheres_World::Spheres_World() {};

void Spheres_World::add_sphere(vec::vec3 center, float radius, std::shared_ptr<Material> material)
{
    Sphere s(center, radius, material);
    spheres.push_back(std::move(s));
}

bool Spheres_World::get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size)
{

    // -------------------------------------------
    //               OpenCL struct
    // -------------------------------------------
    // 
    //        typedef struct __attribute__ ((packed)) _Spheres_World
    //        {   
    //            cl_float3 center[table_size];
    //            cl_float radius[table_size];
    //
    //            cl_int mat_id[table_size];
    //            cl_int mat_num[table_size];
    //        } 
    //        Spheres_World;
    // 
    // -------------------------------------------


    if (table_size)
        *table_size = spheres.size();

    *ptr_size = spheres.size() * (sizeof(cl_float3) + sizeof(cl_float) + 2 * sizeof(cl_int));
    *ptr = std::calloc(*ptr_size, sizeof(char));
    if (!ptr) {
        WARNING("Can not allocate memory for OpenCL host structure");
        return false;
    }

    auto vec3_to_clfloat3 = [](const vec::vec3& v) -> cl_float3 { return {v[0], v[1], v[2]}; };

    auto it = spheres.cbegin();
    size_t offset1 = spheres.size() * (sizeof(cl_float3));
    size_t offset2 = spheres.size() * (sizeof(cl_float3) + sizeof(cl_float));
    size_t offset3 = spheres.size() * (sizeof(cl_float3) + sizeof(cl_float) + sizeof(cl_int));

    for (int i = 0; i < spheres.size(); ++i, ++it) {
        *((cl_float3*)*ptr + i) = vec3_to_clfloat3((*it).center);
        // *((cl_float*)*ptr + offset1 + i) = (*it).radius;
        // *((cl_int*)*ptr + offset2 + i) = (*it).material->get_material_id();
        // *((cl_int*)*ptr + offset3 + i) = (*it).material->get_material_num();
        *((cl_float*)((char*)*ptr + offset1) + i) = (*it).radius;
        *((cl_int*)((char*)*ptr + offset2) + i) = (*it).material->get_material_id();
        *((cl_int*)((char*)*ptr + offset3) + i) = (*it).material->get_material_num();
    }

    return true;
}

Spheres_World::Sphere::Sphere(vec::vec3 center, float radius, std::shared_ptr<Material> material)
    : center{center}, radius{radius}, material{material}  {}