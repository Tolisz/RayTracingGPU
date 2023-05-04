#ifndef CL_BVH_TREE
#define CL_BVH_TREE

#include "ray.cl"

typedef struct __attribute__ ((packed)) _BVH_TREE
{
    int left[NUM_OF_BVH];
    int right[NUM_OF_BVH];
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
            bvh->left[i], bvh->right[i], bvh->obj_id[i], bvh->obj_num[i],
            bvh->min[i].x, bvh->min[i].y, bvh->min[i].z,
            bvh->max[i].x, bvh->max[i].y, bvh->max[i].z );
    }

    printf("----------------------------------------\n\n");
}

bool aabb_box_hit(Ray* r, float t_min, float t_max, float3 minimum, float3 maximum)
{
    for (int a = 0; a < 3; a++) {
        float t0 = fmin((minimum[a] - r->origin[a]) / r->direction[a],
                        (maximum[a] - r->origin[a]) / r->direction[a]);
        float t1 = fmax((minimum[a] - r->origin[a]) / r->direction[a],
                        (maximum[a] - r->origin[a]) / r->direction[a]);
        t_min = fmax(t0, t_min);
        t_max = fmin(t1, t_max);
        if (t_max <= t_min)
            return false;
    }
    return true;
}

#endif 