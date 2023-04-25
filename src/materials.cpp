#include "materials.hpp"

#define CL_TARGET_OPENCL_VERSION 300

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

#include <cstdlib>

#include "error.h"

// Lambertian 
// ----------

size_t Lambertian::count = 0;
Lambertian::Lambertian(const vec::vec3& color): color{color}, mat_num{count++} 
{
    Lambertian_List::lamb_list.push_back(this);
}

std::list<Lambertian*> Lambertian_List::lamb_list;

bool Lambertian_List::get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size) noexcept
{
    //               OpenCL struct
    // -------------------------------------------
    // 
    //      typedef struct _Material_Albedo 
    //      {
    //          cl_float3 albedo[table_size];
    //      }
    //      Material_Albedo;
    // 
    // -------------------------------------------

    *table_size = lamb_list.size();
    *ptr_size = lamb_list.size() * sizeof(cl_float3);

    *ptr = std::calloc(*ptr_size, sizeof(char));
    if (!ptr) {
        WARNING("Can not allocate memory for OpenCL host structure");
        return false;
    }

    auto vec3_to_clfloat3 = [](const vec::vec3& v) -> cl_float3 { return {v[0], v[1], v[2], 0.0f}; };

    // cl_float3 albedo[table_size];
    auto it = lamb_list.cbegin();
    for (int i = 0; i < *table_size; ++i, ++it) {
        *((cl_float3*)*ptr + i) = vec3_to_clfloat3((*it)->color);
    }

    return true;
}


// Metal 
// ----------

size_t Metal::count = 0;



// Dielektryk
// ----------

