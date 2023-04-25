#pragma once

#include <memory>
#include <list>

#include "vec/vec.hpp"
#include "materials.hpp"


class Spheres 
{
public:

    Spheres();
    Spheres(const Spheres&) = default;
    Spheres(Spheres&&) = default;

    void add_sphere(vec::vec3 center, float radius, std::shared_ptr<Material> material);


    /**
     * 
     * 
    */
    bool get_cl_structure(void* ptr, size_t* ptr_size, size_t* table_size);

private:

    class Sphere 
    {
    public:

        Sphere(vec::vec3 center, float radius, std::shared_ptr<Material> material);
        Sphere(const Sphere&) = default;
        Sphere(Sphere&&) = default;

    public:

        vec::vec3 center;
        float radius;
        std::shared_ptr<Material> material;
    };

    std::list<Sphere> spheres;
};

