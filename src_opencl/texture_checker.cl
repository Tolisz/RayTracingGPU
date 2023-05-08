#ifndef CL_TEXTURE_CHECKER
#define CL_TEXTURE_CHECKER

#if (NUM_OF_TEX_CHECKER != 0)
    typedef struct _Texture_Checker
    {
        float3 even[NUM_OF_TEX_CHECKER];
        float3 odd[NUM_OF_TEX_CHECKER];
    } 
    Texture_Checker;
#else 
    typedef struct _Texture_Checker
    {
        float3 even[1];
        float3 odd[1];
    } 
    Texture_Checker;
#endif 

float3 texture_checker_value(__global Texture_Checker* texture, int i, float u, float v, float3 p)
{
    float sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
    if (sines < 0)
        return texture->even[i];
    else 
        return texture->odd[i];
}

#endif