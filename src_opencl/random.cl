#ifndef CL_RADNOM
#define CL_RADNOM

#include "mwc64x.cl"

/// --------------------------------------------------------------------------------
/// @brief Function returns random floating point number in [0, 1)
/// 
/// @param rng - a pointer to an already initialized random generator
/// @return float number in a range of [0, 1)
/// --------------------------------------------------------------------------------
float random_float(mwc64x_state_t* rng)
{
    int n = MWC64X_NextUint(rng);
    return (((float)n / (float)INT_MAX) + 1.0f) / 2.0f;
}


/// --------------------------------------------------------------------------------
/// @brief Function returns random floating point number in [min, max). 
///
/// @param rng - a pointer to an already initialized random generator
/// @param min - minimum posible value, witch can be ganarated by this function 
/// @param max - maximum posible value, witch can be ganarated by this function
/// 
/// @return float number in a range [min, max)
/// --------------------------------------------------------------------------------
float random_float_in(mwc64x_state_t* rng, float min, float max)
{
    return min + (max - min) * random_float(rng);
}


/// --------------------------------------------------------------------------------
/// @brief Function generates a vector of three floating point numbers, where each
///        float component of the vector is in a range of [0, 1)
/// 
/// @param rng - a pointer to an already initialized random generator
/// @return float3 vector, where each component is in a range of [0, 1)
/// --------------------------------------------------------------------------------
float3 random_float3(mwc64x_state_t* rng)
{
    return (float3)(random_float(rng), random_float(rng), random_float(rng));
}


/// --------------------------------------------------------------------------------
/// @brief Function generates a random vector of three floating point numbers, where each
///        float component of the vector is in a range of [min, max)
/// 
/// @param rng - a pointer to an already initialized random generator
/// @param min - minimum posible value each component may have. 
/// @param max - maximum posible value each component may have. 
/// @return float3 vector, where each component is in a range of [min, max)
/// --------------------------------------------------------------------------------
float3 random_float3_in(mwc64x_state_t* rng, float min, float max)
{
    return (float3)(random_float_in(rng, min, max), random_float_in(rng, min, max), random_float_in(rng, min, max));
}


/// --------------------------------------------------------------------------------
/// @brief Function generates a random wektor inside a unit sphere. 
/// 
/// @param rng - a pointer to an already initialized random generator
/// @return float3 vector inside a unit sphere. 
/// --------------------------------------------------------------------------------
float3 random_in_unit_sphere(mwc64x_state_t* rng)
{
    while (true) {
        float3 p = random_float3_in(rng, -1.0f, 1.0f);

        if (dot(p, p) >= 1) 
            continue;

        return p;
    }
}


/// --------------------------------------------------------------------------------
/// @brief Function generates a random wektor inside a unit disk, where z axis 
///        is perpendicular to disk plane. 
/// 
/// @param rng - a pointer to an already initialized random generator
/// @return float3 vector inside a unit disk.
/// --------------------------------------------------------------------------------
float3 random_in_unit_disk(mwc64x_state_t* rng) {
    while (true) {
        float3 p = (float3)(random_float_in(rng, -1, 1), random_float_in(rng, -1, 1), 0.0f);

        if (dot(p, p) >= 1)
            continue;
        
        return p;
    }
}

#endif 