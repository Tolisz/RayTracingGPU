#include "BVH_tree.hpp"

#include <algorithm>

#include "random.hpp"
#include "error.h"

BVH_tree::BVH_node::BVH_node(std::vector<std::shared_ptr<Object>> objects, float time0, float time1)
    : BVH_node(objects, 0, objects.size(), time0, time1){}

BVH_tree::BVH_node::BVH_node(std::vector<std::shared_ptr<Object>> src_objects, size_t start, size_t end, float time0, float time1)
{
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
        left = std::make_unique<BVH_node>(objects, start, mid, time0, time1);
        right = std::make_unique<BVH_node>(objects, mid, end, time0, time1);
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


BVH_tree::BVH_tree(std::vector<std::shared_ptr<Object>> objects, float time0, float time1)
    : BVH_tree(objects, 0, objects.size(), time0, time1) {}

BVH_tree::BVH_tree(std::vector<std::shared_ptr<Object>> objects, size_t start, size_t end, float time0, float time1)
{
    root = std::make_unique<BVH_node>(objects, start, end, time0, time1);
}