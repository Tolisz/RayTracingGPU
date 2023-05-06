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