#ifndef CL_BVH_TREE
#define CL_BVH_TREE

typedef struct __attribute__ ((packed)) _BVH_TREE
{
    int left[NUM_OF_BVH];
    int rigth[NUM_OF_BVH];
    int obj_id[NUM_OF_BVH];
    int obj_num[NUM_OF_BVH];

    float3 min[NUM_OF_BVH];
    float3 max[NUM_OF_BVH];
} 
BVH_tree;

void bvh_tree_debug(__global BVH_tree* bvh)
{
    printf("----------------------------------------\n");
    printf(" BVH Tree Debug Info\n");
    printf("----------------------------------------\n");

    for (int i = 0; i < NUM_OF_BVH; i++) {
        printf("[%-5d] %-5d   %-5d   %-5d   %-5d  {%-10.4f, %-10.4f, %-10.4f}   {%-10.4f, %-10.4f, %-10.4f}\n", i, 
            bvh->left[i], bvh->rigth[i], bvh->obj_id[i], bvh->obj_num[i],
            bvh->min[i].x, bvh->min[i].y, bvh->min[i].z,
            bvh->max[i].x, bvh->max[i].y, bvh->max[i].z );
    }

    printf("----------------------------------------\n\n");
}

#endif 