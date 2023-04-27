#include "random.hpp"

#include <cstdlib>

float random_float()
{
    return rand() / (RAND_MAX + 1.0f); 
}

float random_float(const float& min, const float& max)
{
    return min + (max - min) * random_float();
}