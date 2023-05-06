#ifndef CL_TEXTURE_SOLID_COLOR
#define CL_TEXTURE_SOLID_COLOR


#if (NUM_OF_TEX_SOLID_COLOR != 0)
    typedef struct _Texture_Solid_Color
    {
        float3 color[NUM_OF_TEX_SOLID_COLOR];
    } 
    Texture_Solid_Color;
#else 
    typedef struct _Texture_Solid_Color
    {
        float3 color[1];
    } 
    Texture_Solid_Color;
#endif 

#endif 