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



#endif