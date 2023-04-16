#include "camera.hpp"


Camera::Camera(
    vec::vec3 lookfrom,
    vec::vec3 lookat,
    vec::vec3 vup,
    double vfov, // vertical field-of-view in degrees
    double aspect_ratio
) {
    double theta = vec::degree_to_radians(vfov);
    double h = std::tan(theta / 2);
    double viewport_height = 2.0 * h;
    double viewport_width = aspect_ratio * viewport_height;

    auto w = vec::unit_vector(lookfrom - lookat);
    auto u = vec::unit_vector(vec::cross(vup, w));
    auto v = vec::cross(w, u);

    origin = lookfrom;
    horizontal = viewport_height * u;
    vertical = viewport_width * v;
    lower_left_corner = origin - horizontal/2 - vertical/2 - w;
}

void Camera::get_cl_structure(CL_Camera* pointer) 
{    
    auto vec3_to_clfloat3 = [](const vec::vec3& v) -> cl_float3
    {
        return {v[0], v[1], v[2]};
    };

    pointer->origin =  vec3_to_clfloat3(origin);
    pointer->horizontal = vec3_to_clfloat3(horizontal);
    pointer->vertical = vec3_to_clfloat3(vertical);
    pointer->lower_left_corner = vec3_to_clfloat3(lower_left_corner);
}

