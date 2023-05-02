#pragma once

#include <memory>
#include <list>

#include "vec/vec.hpp"
#include "materials.hpp"

#include "aabb.hpp"

class Object 
{
public:

    virtual bool bounding_box(double time0, double time1, AABB& output_box) const = 0;
};

class World: public Object 
{
public:

    void add(std::shared_ptr<Object> obj);

    virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;

    std::list<std::shared_ptr<Object>> objects;
};


class Sphere: public Object
{
public:

    Sphere(vec::vec3 center, float radius, std::shared_ptr<Material> material);
    Sphere(const Sphere&) = default;
    Sphere(Sphere&&) = default;

    virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;

public:

    vec::vec3 center;
    float radius;
    std::shared_ptr<Material> material;
};


class Sphere_List 
{
    friend Sphere;

public:

    static bool get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size);

private:

    static std::list<Sphere*> spheres;
};





class Moving_Sphere: public Object 
{
public:

    Moving_Sphere(
        vec::vec3 center0, 
        vec::vec3 center1, 
        float time0, 
        float time1, 
        float radius, 
        std::shared_ptr<Material> material);

    virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;

public:

    vec::vec3 center0, center1;
    float time0, time1;
    float radius;
    std::shared_ptr<Material> material;
};


class Moving_Sphere_List
{
    friend Moving_Sphere;

public:

    static bool get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size);

private:

    static std::list<Moving_Sphere*> moving_spheres;
};