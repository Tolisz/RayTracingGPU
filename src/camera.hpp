#pragma once 

#include "vec/vec.hpp"
#include "cl_structs/camera.h"
#include "error.h"

class Camera {
public:

    Camera(
        vec::vec3 lookfrom,
        vec::vec3 lookat,
        vec::vec3 vup,
        double vfov, // vertical field-of-view in degrees
        double aspect_ratio,
        double aperture,
        double focus_dist
    );

    void get_cl_structure(CL_Camera* pointer);

private: 

    vec::vec3 origin;
    vec::vec3 lower_left_corner;
    vec::vec3 horizontal;
    vec::vec3 vertical;
    vec::vec3 u, v, w;
    double lens_radius;
};