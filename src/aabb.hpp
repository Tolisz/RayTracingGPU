#pragma once

#include "vec/vec.hpp"

class AABB 
{
public:

    AABB();
    AABB(const vec::vec3& min, const vec::vec3& max);

    static AABB surrounding_box(const AABB& box0, const AABB& box1);

    vec::vec3 minimum;
    vec::vec3 maximum;
};

