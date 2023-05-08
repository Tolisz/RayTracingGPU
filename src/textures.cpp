#include "textures.hpp"

#define CL_TARGET_OPENCL_VERSION 300

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

#include <cstdlib>

#include "error.h"

size_t Solid_Color::count = 0;

Solid_Color::Solid_Color(const vec::vec3& texture_color)
    : color{texture_color}, tex_num{count++}    
{
    Solid_Color_List::sol_colors.push_back(this);
}

int Solid_Color::get_texture_id() const noexcept
{
    return tex_id;
}

int Solid_Color::get_texture_num() const noexcept
{
    return tex_num;
}

std::list<Solid_Color*> Solid_Color_List::sol_colors;

bool Solid_Color_List::get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size) noexcept
{
    // -------------------------------------------
    //               OpenCL struct
    // -------------------------------------------
    // 
    //        typedef struct _texture_solid_color
    //        {
    //            float3 color[table_size];
    //
    //        } texture_solid_color;
    //
    // -------------------------------------------

    if (sol_colors.empty()) {
        *ptr = nullptr;
        *ptr_size = 0;
        *table_size = 0;

        return false;
    }

    if (table_size)
        *table_size = sol_colors.size();
    
    *ptr_size = sol_colors.size() * sizeof(cl_float3);

    *ptr = std::calloc(*ptr_size, sizeof(char));
    if (!ptr) {
        WARNING("Can not allocate memory for OpenCL host structure");
        return false;
    }

    auto vec3_to_clfloat3 = [](const vec::vec3& v) -> cl_float3 { return {v[0], v[1], v[2]}; };

    auto it = sol_colors.cbegin();
    for (int i = 0; i < sol_colors.size(); ++i, ++it) {
        *((cl_float3*)*ptr + i) = vec3_to_clfloat3((*it)->color);
    }

    return true;
}



// -----------------------------



size_t Checker::count = 0;

Checker::Checker(const vec::vec3& _odd, const vec::vec3& _even)
    : odd{_odd}, even{_even}, tex_num{count++}
{
    Checker_List::check_list.push_back(this);
}

int Checker::get_texture_id() const noexcept
{
    return tex_id;
}

int Checker::get_texture_num() const noexcept
{
    return tex_num;
}


std::list<Checker*> Checker_List::check_list;

bool Checker_List::get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size) noexcept
{
    // -------------------------------------------
    //               OpenCL struct
    // -------------------------------------------
    // 
    //      typedef struct _Texture_checker
    //      {
    //          float3 even[table_size];
    //          float3 odd[table_size];
    //      } 
    //      Texture_checker;
    //
    // -------------------------------------------

    if (check_list.empty()) {
        *ptr = nullptr;
        *ptr_size = 0;
        *table_size = 0;

        return false;
    }    

    if (table_size)
        *table_size = check_list.size();
    
    *ptr_size = check_list.size() * (2 * sizeof(cl_float3));

    *ptr = std::calloc(*ptr_size, sizeof(char));
    if (!ptr) {
        WARNING("Can not allocate memory for OpenCL host structure");
        return false;
    }

    auto to_clfloat3 = [](const vec::vec3& v) -> cl_float3 { return {v[0], v[1], v[2]}; };

    std::size_t offset1 = check_list.size() * sizeof(cl_float3);

    auto set_even = [ptr, to_clfloat3](const vec::vec3& even, int i){ *((cl_float3*)*ptr + i) = to_clfloat3(even); };
    auto set_odd  = [ptr, to_clfloat3, offset1](const vec::vec3& odd, int i){ *((cl_float3*)((char*)*ptr + offset1) + i) = to_clfloat3(odd); };

    auto it = check_list.cbegin();
    for (int i = 0; i < check_list.size(); ++i, ++it) {
        set_even((*it)->even, i);
        set_odd((*it)->odd, i);
    }   

    return true;
}