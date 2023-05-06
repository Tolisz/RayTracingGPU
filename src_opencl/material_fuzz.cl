#ifndef CL_MATERIAL_FUZZ
#define CL_MATERIAL_FUZZ

#if (NUM_OF_FUZZ_MATERIALS != 0)
    typedef struct _Material_Fuzz
    {
        float3 albedo[NUM_OF_FUZZ_MATERIALS];
        float fuzz[NUM_OF_FUZZ_MATERIALS];
    }
    Material_Fuzz;
#else 
    typedef struct _Material_Fuzz
    {
        float3 albedo[1];
        float fuzz[1];
    }
    Material_Fuzz;
#endif 

#endif