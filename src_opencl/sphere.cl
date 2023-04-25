typedef struct _Spheres_World
{   
    // coordinates
    float3 c[NUMBER_OF_SPHERES];
    //float x[NUMBER_OF_SPHERES];
    //float y[NUMBER_OF_SPHERES];
    //float z[NUMBER_OF_SPHERES];

    // radius
    float r[NUMBER_OF_SPHERES];

    // material
    int mat_id[NUMBER_OF_SPHERES];
    int mat_num[NUMBER_OF_SPHERES];
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
