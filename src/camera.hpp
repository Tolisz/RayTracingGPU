#pragma once 

#include "vec/vec.hpp"

class Camera {
public:

    Camera(
        vec::vec3 lookfrom,
        vec::vec3 lookat,
        vec::vec3 vup,
        double vfov, // vertical field-of-view in degrees
        double aspect_ration
    ) {
        double theta = degree_to_radians(vfov);
    }

private: 

    vec::vec3 origin;
    vec::vec3 lower_left_corner;
    vec::vec3 horizontal;
    vec::vec3 vertical;
};