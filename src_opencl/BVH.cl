#ifndef CL_BVH
#define CL_BVH

typedef struct __attribute__ ((packed)) _BVH
{
    int left[NUM_OF_BVH];
    int rigth[NUM_OF_BVH];
    int obj_id[NUM_OF_BVH];
    int obj_num[NUM_OF_BVH];

    float3 min[NUM_OF_BVH];
    float3 max[NUM_OF_BVH];
} 
BVH;



#endif 