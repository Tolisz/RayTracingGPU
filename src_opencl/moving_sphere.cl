#ifndef CL_MOVING_SPHERE
#define CL_MOVING_SPHERE

typedef struct __attribute__ ((packed)) _Moving_Sphere
{
    float3 center0[NUM_OF_MOVING_SPHERE];
    float3 center1[NUM_OF_MOVING_SPHERE];

    float time0[NUM_OF_MOVING_SPHERE];
    float time1[NUM_OF_MOVING_SPHERE];
    float radius[NUM_OF_MOVING_SPHERE];

    int mat_id[NUM_OF_MOVING_SPHERE];
    int mat_num[NUM_OF_MOVING_SPHERE];
}
Moving_Sphere;


void Moving_Sphere_Debug(Moving_Sphere* ms)
{
    printf("----------------------------------------\n");
    printf(" Moving Sphere Debug Info\n")
    printf("----------------------------------------\n");

    for (int i = 0; i < NUM_OF_MOVING_SPHERE; i++) {
        printf("i = %d", i);
        printf("c0 = [%f, %f, %f]; c1 = [%f, %f, %f]", 
            ms->center0[i].x, ms->center0[i].y, ms->center0[i].z, 
            ms->center1[i].x, ms->center1[i].y, ms->center1[i].z);
        printf("t0 = %f; t1 = %f", ms->time0[i], ms->time1[i]);
        printf("mat_id = %4d;  mat_num = %4d\n\n", ms->mat_id[i], ms->mat_num[i]);
    }

    printf("----------------------------------------\n\n");
}


#endif