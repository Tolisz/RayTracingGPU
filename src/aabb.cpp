#include "aabb.hpp"

AABB::AABB() {}  

AABB::AABB(const vec::vec3& min, const vec::vec3& max)
    : maximum{max}, minimum{min}    { }

AABB AABB::surrounding_box(const AABB& box0, const AABB& box1)
{
    vec::vec3 min{
        fmin(box0.minimum[0], box1.minimum[0]),
        fmin(box0.minimum[1], box1.minimum[1]),
        fmin(box0.minimum[2], box1.minimum[2]) };

    vec::vec3 max{
        fmin(box0.maximum[0], box1.maximum[0]),
        fmin(box0.maximum[1], box1.maximum[1]),
        fmin(box0.maximum[2], box1.maximum[2]) };

    return AABB(min, max);
}

