#include "camera.hpp"

#include "random.hpp"

Camera::Camera(
    vec::vec3 lookfrom,
    vec::vec3 lookat,
    vec::vec3 vup,
    double vfov, // vertical field-of-view in degrees
    double aspect_ratio,
    double aperture,
    double focus_dist,
    double _time0,
    double _time1
)  
    : _time0{_time0}, _time1{_time1}
{
    double theta = vec::degree_to_radians(vfov);
    double h = std::tan(theta / 2);
    double viewport_height = 2.0 * h;
    double viewport_width = aspect_ratio * viewport_height;

    w = vec::unit_vector(lookfrom - lookat);
    u = vec::unit_vector(vec::cross(vup, w));
    v = vec::cross(w, u);

    origin = lookfrom;
    horizontal = focus_dist * viewport_height * u;
    vertical = focus_dist * viewport_width * v;
    lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist * w;

    lens_radius = aperture / 2;

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

    pointer->u = vec3_to_clfloat3(u);
    pointer->v = vec3_to_clfloat3(v);
    pointer->w = vec3_to_clfloat3(w);
    
    pointer->lens_radius = static_cast<cl_float>(lens_radius);

    pointer->time0 = _time0;
    pointer->time1 = _time1;
}
