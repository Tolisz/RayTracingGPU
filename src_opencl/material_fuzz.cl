#ifndef CL_MATERIAL_FUZZ
#define CL_MATERIAL_FUZZ

typedef struct _Material_Fuzz
{
    float3 albedo[NUM_OF_FUZZ_MATERIALS];
    float fuzz[NUM_OF_FUZZ_MATERIALS];
}
Material_Fuzz;

#endif