#include "sphere.hpp"

Spheres::Spheres() {};

void Spheres::add_sphere(vec::vec3 center, float radius, std::shared_ptr<Material> material)
{
    Sphere s(center, radius, material);
    spheres.push_back(std::move(s));
}

bool Spheres::get_cl_structure(void* ptr, size_t* ptr_size, size_t* table_size)
{
    return true;
}

Spheres::Sphere::Sphere(vec::vec3 center, float radius, std::shared_ptr<Material> material)
    : center{center}, radius{radius}, material{material}  {}