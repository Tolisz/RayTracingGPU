#include "BVH_tree.hpp"

#define CL_TARGET_OPENCL_VERSION 300

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

#include <cstdlib>
#include <algorithm>
#include <stack>

#include "random.hpp"
#include "error.h"


BVH_tree::BVH_node::BVH_node(std::vector<std::shared_ptr<Object>> src_objects, size_t start, size_t end, float time0, float time1, size_t& tree_size)
{
    tree_size++;

    auto objects = src_objects;

    int axis = random_int(0, 2);
    auto comparator = (axis == 0) ? &BVH_node::box_x_compare : 
                      (axis == 1) ? &BVH_node::box_y_compare : 
                                    &BVH_node::box_z_compare ; 

    size_t object_span = end - start;

    if (object_span == 1) {
        right = nullptr;
        left = nullptr;
        obj = objects[start];
        obj->bounding_box(time0, time1, box);
        return;
    }
    else {
        std::sort(objects.begin() + start, objects.begin() + end, 
            [this, comparator](auto& a, auto& b){ return (this->*comparator)(a, b); });

        auto mid = start + object_span/2;
        left = std::make_shared<BVH_node>(objects, start, mid, time0, time1, tree_size);
        right = std::make_shared<BVH_node>(objects, mid, end, time0, time1, tree_size);
    }

    AABB box_left, box_right;

    if ( !left->bounding_box(time0, time1, box_left) ||  !right->bounding_box(time0, time1, box_right) ) {
        WARNING("No bounding box in bvh_node constructor.");
    }

    box = AABB::surrounding_box(box_left, box_right);
}

bool BVH_tree::BVH_node::box_compare(const std::shared_ptr<Object> a, const std::shared_ptr<Object> b, int axis)
{
    AABB box_a;
    AABB box_b;

    if (!a->bounding_box(0,0, box_a) || !b->bounding_box(0,0, box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    return box_a.minimum[axis] < box_b.minimum[axis];
}

bool BVH_tree::BVH_node::box_x_compare (const std::shared_ptr<Object> a, const std::shared_ptr<Object> b)
{
    return box_compare(a, b, 0);
}

bool BVH_tree::BVH_node::box_y_compare (const std::shared_ptr<Object> a, const std::shared_ptr<Object> b)
{
    return box_compare(a, b, 1);
}

bool BVH_tree::BVH_node::box_z_compare (const std::shared_ptr<Object> a, const std::shared_ptr<Object> b)
{
    return box_compare(a, b, 2);
}


bool BVH_tree::BVH_node::bounding_box(double time0, double time1, AABB& output_box) const
{
    output_box = box;
    return true;
}

int BVH_tree::BVH_node::get_object_id() const noexcept
{
    return -1;
};
int BVH_tree::BVH_node::get_object_num() const noexcept
{
    return -1;
};

BVH_tree::BVH_tree(std::vector<std::shared_ptr<Object>> objects, float time0, float time1)
    : BVH_tree(objects, 0, objects.size(), time0, time1) {}

BVH_tree::BVH_tree(std::vector<std::shared_ptr<Object>> objects, size_t start, size_t end, float time0, float time1)
{
    if (objects.size() == 0 || (end - start <= 0))
    {
        root = nullptr;
        return;
    }

    root = std::make_unique<BVH_node>(objects, start, end, time0, time1, tree_size);
}

bool BVH_tree::get_cl_structure(void** ptr, size_t* ptr_size, size_t* table_size)
{
    // -------------------------------------------
    //               OpenCL struct
    // -------------------------------------------
    // 
    //          typedef struct __attribute__ ((packed)) CL_BVH
    //          {
    //              cl_int left;
    //              cl_int rigth;
    //              cl_int obj_id;
    //              cl_int obj_num;
    //          
    //              cl_float3 min;
    //              cl_float3 max;
    //          } 
    //          BVH;
    // 
    // -------------------------------------------

    if (root == nullptr) {
        *ptr = nullptr;
        *ptr_size = 0;
        *table_size = 0;
        
        return false;
    }

    if (table_size)
        *table_size = tree_size;

    *ptr_size = tree_size * (4 * sizeof(cl_int) + 2 * sizeof(cl_float3));
    *ptr = std::calloc(*ptr_size, sizeof(char));
    if (!ptr) {
        WARNING("Can not allocate memory for OpenCL host structure");
        return false;
    }

    auto to_clfloat3 = [](const vec::vec3& v) -> cl_float3 { return {v[0], v[1], v[2]}; };
    
    size_t offset1 /* right     */ = tree_size * (sizeof(cl_int));
    size_t offset2 /* obj_id    */ = tree_size * (2 * sizeof(cl_int));
    size_t offset3 /* obj_num   */ = tree_size * (3 * sizeof(cl_int));
    size_t offset4 /* min       */ = tree_size * (4 * sizeof(cl_int));
    size_t offset5 /* max       */ = tree_size * (4 * sizeof(cl_int) + sizeof(cl_float3));

    auto set_left       = [ptr] (const cl_int& left, int i) { *((cl_int*)*ptr + i) = left; };
    auto set_right      = [ptr, offset1](const cl_int& right, int i) { *((cl_int*)((char*)*ptr + offset1) + i) = right; };
    auto set_obj_id     = [ptr, offset2](const cl_int& obj_id, int i) { *((cl_int*)((char*)*ptr + offset2) + i) = obj_id; };
    auto set_obj_num    = [ptr, offset3](const cl_int& obj_num, int i) { *((cl_int*)((char*)*ptr + offset3) + i) = obj_num; };
    auto set_min        = [ptr, offset4, to_clfloat3](const vec::vec3& min, int i) { *((cl_float3*)((char*)*ptr + offset4) + i) = to_clfloat3(min); };
    auto set_max        = [ptr, offset5, to_clfloat3](const vec::vec3& max, int i) { *((cl_float3*)((char*)*ptr + offset5) + i) = to_clfloat3(max); }; 

    using stack_element = std::pair<std::shared_ptr<BVH_node>, uint>;
    
    uint num = 0;
    std::stack<stack_element> Stack;    

    Stack.push(std::make_pair(root, num++));

    while (!Stack.empty()) {
        stack_element elem = Stack.top();
        Stack.pop();

        uint idx = elem.second;

        set_min(elem.first->box.minimum, idx);
        set_max(elem.first->box.maximum, idx);

        if (elem.first->right && elem.first->left) {
            set_left(num, idx);
            Stack.push(std::make_pair(elem.first->left, num++));
            
            set_right(num, idx);
            Stack.push(std::make_pair(elem.first->right, num++));
            
            set_obj_id(elem.first->get_object_id(), idx);
            set_obj_num(elem.first->get_object_num(), idx);
        }
        else {
            set_left(-1, idx);
            set_right(-1, idx);
            
            set_obj_id(elem.first->obj->get_object_id(), idx);
            set_obj_num(elem.first->obj->get_object_num(), idx);
        }
    }

    return true;
}