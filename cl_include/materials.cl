// Material types value is 
// 0 - lambertian
// 1 - metal 
// 2 - dielectric
typedef struct _Materials
{
    float3 albedo[NUMBER_OF_SPHERES];   // 0, 1 
    float fuzz[NUMBER_OF_SPHERES];      // 1
    float ir[NUMBER_OF_SPHERES];        // 2
} 
Materials;