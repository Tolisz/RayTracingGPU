#ifndef CL_MATERIAL_ALBEDO
#define CL_MATERIAL_ALBEDO

#if (NUM_OF_ALBEDO_MATERIALS != 0)
    typedef struct _Material_Albedo 
    {
        int tex_id[NUM_OF_ALBEDO_MATERIALS];
        int tex_num[NUM_OF_ALBEDO_MATERIALS];
    }
    Material_Albedo;
#else 
    typedef struct _Material_Albedo 
    {
        int tex_id[1];
        int tex_num[1];
    }
    Material_Albedo;
#endif



#endif