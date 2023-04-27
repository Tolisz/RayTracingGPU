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
    // -------------------------------------------
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

    if (table_size)
        *table_size = lamb_list.size();
    
    *ptr_size = lamb_list.size() * sizeof(cl_float3);

    *ptr = std::calloc(*ptr_size, sizeof(char));
    if (!ptr) {
        WARNING("Can not allocate memory for OpenCL host structure");
        return false;
    }

    auto vec3_to_clfloat3 = [](const vec::vec3& v) -> cl_float3 { return {v[0], v[1], v[2]}; };

    auto it = lamb_list.cbegin();
    for (int i = 0; i < lamb_list.size(); ++i, ++it) {
        *((cl_float3*)*ptr + i) = vec3_to_clfloat3((*it)->color);
    }

    return true;
}


// Metal 
// ----------

size_t Metal::count = 0;

Metal::Metal(const vec::vec3& color, const float& fuzz)
    : color{color}, fuzz{fuzz}, mat_num{count++} 
{
    Metal_List::metal_list.push_back(this);
}

std::list<Metal*> Metal_List::metal_list;

bool Metal_List::get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size) noexcept
{
    // -------------------------------------------
    //               OpenCL struct
    // -------------------------------------------
    // 
    //        typedef struct _Material_Fuzz
    //        {
    //            cl_float3 albedo[table_size];
    //            cl_float fuzz[table_size];
    //        }
    //        Material_Fuzz;
    // 
    // -------------------------------------------

    if(table_size)
        *table_size = metal_list.size();

    *ptr_size = metal_list.size() * (sizeof(cl_float3) + sizeof(cl_float));

    *ptr = std::calloc(*ptr_size, sizeof(char));
    if (!ptr) {
        WARNING("Can not allocate memory for OpenCL host structure");
        return false;
    }

    auto vec3_to_clfloat3 = [](const vec::vec3& v) -> cl_float3 { return {v[0], v[1], v[2]}; };

    auto it = metal_list.cbegin();
    size_t offset = metal_list.size() * sizeof(cl_float3) / 4;

    for (int i = 0; i < metal_list.size(); ++i, ++it) {
        // albedo[i]
        *((cl_float3*)*ptr + i) = vec3_to_clfloat3((*it)->color);
        // fuzz[i]
        *((cl_float*)*ptr + offset + i) = (*it)->fuzz;        
    }

    return true;
}



// Dielektryk
// ----------

size_t Dielectric::count = 0;

Dielectric::Dielectric(const float& index_of_reflection)
    : reflection_index{index_of_reflection}, mat_num{count++}
{
    Dielectric_List::dielec_list.push_back(this);
}

std::list<Dielectric*> Dielectric_List::dielec_list;

bool Dielectric_List::get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size) noexcept
{
    // -------------------------------------------
    //               OpenCL struct
    // -------------------------------------------
    // 
    //        typedef struct _Material_Reflectance
    //        {
    //            cl_float reflection_index[table_size];
    //        }
    //        Material_Reflectance;
    // 
    // -------------------------------------------


    if(table_size)
        *table_size = dielec_list.size();

    *ptr_size = dielec_list.size() * sizeof(cl_float);

    *ptr = std::calloc(*ptr_size, sizeof(char));
    if (!ptr) {
        WARNING("Can not allocate memory for OpenCL host structure");
        return false;
    }

    auto it = dielec_list.cbegin();
    for(int i = 0; i < dielec_list.size(); i++) {
        // reflection_index[i]
        *((cl_float*)*ptr + i) = (*it)->reflection_index;
    }

    return true;
}