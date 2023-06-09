#pragma once

#include <list>
#include <memory>

#include "vec/vec.hpp"
#include "textures.hpp"

class Material 
{
public:
    virtual size_t get_material_id() const noexcept = 0;
    virtual size_t get_material_num() const noexcept = 0;
};







class Lambertian: public Material
{
public:

    Lambertian(const vec::vec3& color);
    Lambertian(std::shared_ptr<Texture> a); 

    virtual size_t get_material_id() const noexcept override { return mat_id; }
    virtual size_t get_material_num() const noexcept override { return mat_num; }

private:

    static size_t   count;
    const size_t    mat_id = 0;
    size_t          mat_num; 

public:
    std::shared_ptr<Texture> albedo;
};

class Lambertian_List
{
    friend class Lambertian;

public:
    static bool get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size) noexcept;

private:
    static std::list<Lambertian*> lamb_list;
};








class Metal: public Material
{
public:

    Metal(const vec::vec3& color, const float& fuzz);

    virtual size_t get_material_id() const noexcept override { return mat_id; }
    virtual size_t get_material_num() const noexcept override { return mat_num; }


private:

    static size_t   count;
    const size_t    mat_id = 1;
    size_t          mat_num; 

public: 

    vec::vec3   color;
    float       fuzz;
};

class Metal_List
{
    friend class Metal;

public:
    static bool get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size) noexcept;

private:
    static std::list<Metal*> metal_list;
};








class Dielectric: public Material
{
public:

    Dielectric(const float& index_of_reflection);

    virtual size_t get_material_id() const noexcept override { return mat_id; }
    virtual size_t get_material_num() const noexcept override { return mat_num; }

private:

    static size_t   count;
    const size_t    mat_id = 2;
    size_t          mat_num; 

public:

    float reflection_index;
};

class Dielectric_List
{
    friend class Dielectric;

public:
    static bool get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size) noexcept;

private:
    static std::list<Dielectric*> dielec_list;
};