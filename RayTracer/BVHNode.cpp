#include "BVHNode.hpp"
#include <iostream>

namespace RayTracer
{
    inline bool box_compare(const std::shared_ptr<IPrimitive>& a, const std::shared_ptr<IPrimitive>& b, int axis)
    {
        AABB box_a;
        AABB box_b;

        if (!get_primitive_aabb(a, box_a) || !get_primitive_aabb(b, box_b)) {
            std::cerr << "No bounding box in BVHNode constructor." << std::endl;
        }

        double val_a = (axis == 0 ? box_a.minimum.x : (axis == 1 ? box_a.minimum.y : box_a.minimum.z));
        double val_b = (axis == 0 ? box_b.minimum.x : (axis == 1 ? box_b.minimum.y : box_b.minimum.z));

        return val_a < val_b;
    }

    inline bool box_x_compare(const std::shared_ptr<IPrimitive>& a, const std::shared_ptr<IPrimitive>& b) {
        return box_compare(a, b, 0);
    }

    inline bool box_y_compare(const std::shared_ptr<IPrimitive>& a, const std::shared_ptr<IPrimitive>& b) {
        return box_compare(a, b, 1);
    }

    inline bool box_z_compare(const std::shared_ptr<IPrimitive>& a, const std::shared_ptr<IPrimitive>& b) {
        return box_compare(a, b, 2);
    }

    BVHNode::BVHNode() {}

    BVHNode::BVHNode(std::vector<std::shared_ptr<IPrimitive>>& objects, size_t start, size_t end)
    {
        // Sélectionner un axe (alternance simple basée sur un choix semi-aléatoire pour équilibrer l'arbre)
        int axis = rand() % 3;
        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare;

        size_t object_span = end - start;

        if (object_span == 1) {
            left = right = objects[start];
        } else if (object_span == 2) {
            if (comparator(objects[start], objects[start+1])) {
                left = objects[start];
                right = objects[start+1];
            } else {
                left = objects[start+1];
                right = objects[start];
            }
        } else {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            auto mid = start + object_span / 2;
            left = std::make_shared<BVHNode>(objects, start, mid);
            right = std::make_shared<BVHNode>(objects, mid, end);
        }

        AABB box_left, box_right;

        if (!get_primitive_aabb(left, box_left) || !get_primitive_aabb(right, box_right)) {
            std::cerr << "No bounding box in BVHNode constructor." << std::endl;
        }

        box = surrounding_box(box_left, box_right);
    }

    bool BVHNode::hits(const Ray& ray, double t_min, double t_max, HitRecord& rec) const
    {
        if (!box.hits(ray, t_min, t_max))
            return false;

        bool hit_left = left->hits(ray, t_min, t_max, rec);
        bool hit_right = right->hits(ray, t_min, hit_left ? rec.distance : t_max, rec);

        return hit_left || hit_right;
    }

    bool BVHNode::get_aabb(AABB& output_box) const
    {
        output_box = box;
        return true;
    }
}
