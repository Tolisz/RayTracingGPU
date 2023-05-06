#ifndef CL_MATERIAL_REFLACTANCE
#define CL_MATERIAL_REFLACTANCE


#if (NUM_OF_REFLECTANCE_MATERIALS != 0)
    typedef struct _Material_Reflectance
    {
        float reflection_index[NUM_OF_REFLECTANCE_MATERIALS];
    }
    Material_Reflectance;
#else 
    typedef struct _Material_Reflectance
    {
        float reflection_index[1];
    }
    Material_Reflectance;
#endif 

#endif