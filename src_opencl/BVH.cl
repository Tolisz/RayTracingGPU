#ifndef CL_BVH
#define CL_BVH

typedef struct __attribute__ ((packed)) CL_BVH
{
    int left;
    int rigth;
    int obj_id;
    int obj_num;
} 
BVH;


#endif 