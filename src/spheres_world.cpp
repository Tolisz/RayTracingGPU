#include "spheres_world.hpp"

#define CL_TARGET_OPENCL_VERSION 300

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

#include <cstdlib>

#include "error.h"


void World::add(std::shared_ptr<Object> obj)
{
    objects.push_back(obj);
}

bool World::bounding_box(double time0, double time1, AABB& output_box) const
{
    if (objects.empty()) 
        return false;

    AABB temp_box;
    bool is_first_box = true;

    for(const auto& obj : objects) {
        if (!obj->bounding_box(time0, time1, temp_box))
            return false;
        
        output_box = is_first_box ? temp_box : AABB::surrounding_box(output_box, temp_box);
        is_first_box = false;
    }

    return true;
}

int World::get_object_id() const noexcept
{
    return -1;
}
int World::get_object_num() const noexcept
{
    return -1;
}



size_t Sphere::count = 0;

Sphere::Sphere(vec::vec3 center, float radius, std::shared_ptr<Material> material)
    : center{center}, radius{radius}, material{material}, obj_num{count++}
{    
    Sphere_List::spheres.push_back(this);
}

bool Sphere::bounding_box(double time0, double time1, AABB& output_box) const
{
    output_box = AABB(center - vec::vec3{radius, radius, radius}, center + vec::vec3{radius, radius, radius});
    return true;
}

int Sphere::get_object_id() const noexcept
{
    return obj_id;
}

int Sphere::get_object_num() const noexcept
{
    return obj_num;
}

std::list<Sphere*> Sphere_List::spheres;

bool Sphere_List::get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size)
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
        *((cl_float3*)*ptr + i) = vec3_to_clfloat3((*it)->center);
        *((cl_float*)((char*)*ptr + offset1) + i) = (*it)->radius;
        *((cl_int*)((char*)*ptr + offset2) + i) = (*it)->material->get_material_id();
        *((cl_int*)((char*)*ptr + offset3) + i) = (*it)->material->get_material_num();
    }

    return true;
}





size_t Moving_Sphere::count = 0;

Moving_Sphere::Moving_Sphere(
    vec::vec3 center0,
    vec::vec3 center1, 
    float time0, 
    float time1, 
    float radius, 
    std::shared_ptr<Material> material)
: 
    center0{center0}, 
    center1{center1}, 
    time0{time0}, 
    time1{time1}, 
    radius{radius}, 
    material{material},
    obj_num{count++}
{
    Moving_Sphere_List::moving_spheres.push_back(this);
}

bool Moving_Sphere::bounding_box(double time0, double time1, AABB& output_box) const 
{
    AABB box0(center0 - vec::vec3{radius, radius, radius}, center0 + vec::vec3{radius, radius, radius});
    AABB box1(center1 - vec::vec3{radius, radius, radius}, center1 + vec::vec3{radius, radius, radius});

    output_box = AABB::surrounding_box(box0, box1);
    return true;
}

int Moving_Sphere::get_object_id() const noexcept
{
    return obj_id;
}

int Moving_Sphere::get_object_num() const noexcept
{
    return obj_num;
}

std::list<Moving_Sphere*> Moving_Sphere_List::moving_spheres;

bool Moving_Sphere_List::get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size)
{
    // -------------------------------------------
    //               OpenCL struct
    // -------------------------------------------
    // 
    //      typedef struct __attribute__ ((packed)) _Moving_Sphere
    //      {
    //          cl_float3 center0[table_size];
    //          cl_float3 center1[table_size];
    //
    //          cl_float time0[table_size];
    //          cl_float time1[table_size];
    //          cl_float radius[table_size];
    //
    //          cl_int mat_id[table_size];
    //          cl_int mat_num[table_size];
    //      }
    //      Moving_Sphere;
    //
    // -------------------------------------------


    if (table_size)
        *table_size = moving_spheres.size();

    *ptr_size = moving_spheres.size() * 
        (2 * sizeof(cl_float3) + 3 * sizeof(cl_float) + 2 * sizeof(cl_int));
    
    *ptr = std::calloc(*ptr_size, sizeof(char));
    if (!ptr) {
        WARNING("Can not allocate memory for OpenCL host structure");
        return false;
    }

    auto vec3_to_clfloat3 = [](const vec::vec3& v) -> cl_float3 { return {v[0], v[1], v[2]}; };

    size_t offset1 = moving_spheres.size() * (sizeof(cl_float3));
    size_t offset2 = moving_spheres.size() * (2 * sizeof(cl_float3));
    size_t offset3 = moving_spheres.size() * (2 * sizeof(cl_float3) + 1 * sizeof(cl_float));
    size_t offset4 = moving_spheres.size() * (2 * sizeof(cl_float3) + 2 * sizeof(cl_float));
    size_t offset5 = moving_spheres.size() * (2 * sizeof(cl_float3) + 3 * sizeof(cl_float));
    size_t offset6 = moving_spheres.size() * (2 * sizeof(cl_float3) + 3 * sizeof(cl_float) + sizeof(cl_int));

    auto it = moving_spheres.cbegin();
    for (int i = 0; i < moving_spheres.size(); ++i, ++it) {
        *((cl_float3*)*ptr + i) = vec3_to_clfloat3((*it)->center0);
        *((cl_float3*)((char*)*ptr + offset1) + i) = vec3_to_clfloat3((*it)->center1);
        *((cl_float*)((char*)*ptr + offset2) + i) = (*it)->time0;
        *((cl_float*)((char*)*ptr + offset3) + i) = (*it)->time1;
        *((cl_float*)((char*)*ptr + offset4) + i) = (*it)->radius;
        *((cl_int*)((char*)*ptr + offset5) + i) = (*it)->material->get_material_id();
        *((cl_int*)((char*)*ptr + offset6) + i) = (*it)->material->get_material_num();
    }

    return true;
}