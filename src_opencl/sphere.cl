typedef struct _Spheres_World
{
    // coordinates
    float x[NUMBER_OF_SPHERES];
    float y[NUMBER_OF_SPHERES];
    float z[NUMBER_OF_SPHERES];

    // radius
    float r[NUMBER_OF_SPHERES];

    // material
    uint mat_id[NUMBER_OF_SPHERES];
    uint mat_num[NUMBER_OF_SPHERES];
} 
Spheres_World;

typedef struct _Sphere 
{
    float3 center;
    float r;
    uint mat_id;
    uint mat_num;
}
Sphere;

