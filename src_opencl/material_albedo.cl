#ifndef CL_MATERIAL_ALBEDO
#define CL_MATERIAL_ALBEDO

typedef struct _Material_Albedo 
{
    //float3 albedo[NUM_OF_ALBEDO_MATERIALS];
    int tex_id[NUM_OF_ALBEDO_MATERIALS];
    int tex_num[NUM_OF_ALBEDO_MATERIALS];
}
Material_Albedo;

#endif