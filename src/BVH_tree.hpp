#pragma once

#include <vector>

#include "spheres_world.hpp"


class BVH_tree 
{
private:

    class BVH_node: public Object
    {
    public:
        BVH_node(std::vector<std::shared_ptr<Object>> objects, float time0, float time1);
        BVH_node(std::vector<std::shared_ptr<Object>> objects, size_t start, size_t end, float time0, float time1);

        bool box_compare(const std::shared_ptr<Object> a, const std::shared_ptr<Object> b, int axis);
        bool box_x_compare (const std::shared_ptr<Object> a, const std::shared_ptr<Object> b);
        bool box_y_compare (const std::shared_ptr<Object> a, const std::shared_ptr<Object> b);
        bool box_z_compare (const std::shared_ptr<Object> a, const std::shared_ptr<Object> b);

        virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;

    public: 

        std::unique_ptr<BVH_node> left;
        std::unique_ptr<BVH_node> right;

        std::shared_ptr<Object> obj = nullptr;
        AABB box;
    };

public:

    BVH_tree(std::vector<std::shared_ptr<Object>> objects, float time0, float time1);
    BVH_tree(std::vector<std::shared_ptr<Object>> objects, size_t start, size_t end, float time0, float time1);

    

private:

    std::unique_ptr<BVH_node> root;
};