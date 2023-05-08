#pragma once

#include "vec/vec.hpp"

#include <list>

class Texture 
{
public: 
    
    virtual int get_texture_id() const noexcept = 0;
    virtual int get_texture_num() const noexcept = 0;
};


class Solid_Color: public Texture
{
public:

    Solid_Color(const vec::vec3& texture_color);

    int get_texture_id() const noexcept override;
    int get_texture_num() const noexcept override;

private:

    static size_t   count;
    const size_t    tex_id = 0;
    size_t          tex_num; 

public: 

    vec::vec3 color;
};


class Solid_Color_List
{
    friend class Solid_Color;

public:

    static bool get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size) noexcept;

private:

    static std::list<Solid_Color*> sol_colors;
};





// ----------- 


class Checker: public Texture
{
public:

    Checker(const vec::vec3& _odd, const vec::vec3& _even);

    int get_texture_id() const noexcept override;
    int get_texture_num() const noexcept override;

private:

    static size_t   count;
    const size_t    tex_id = 1;
    size_t          tex_num; 

public: 

    vec::vec3 even;
    vec::vec3 odd;
};

class Checker_List
{
    friend class Checker;

public:

    static bool get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size) noexcept;

private:

    static std::list<Checker*> check_list;
};


