#pragma once

#include "IPrimitive.hpp"
#include "AABB.hpp"
#include "Triangle.hpp"
#include <memory>
#include <vector>
#include <algorithm>

namespace RayTracer
{
    class BVHNode : public IPrimitive
    {
    public:
        BVHNode();
        BVHNode(std::vector<std::shared_ptr<IPrimitive>>& src_objects, size_t start, size_t end);
        ~BVHNode() override = default;

        bool hits(const Ray& ray, double t_min, double t_max, HitRecord& rec) const override;
        bool get_aabb(AABB& output_box) const;

    private:
        std::shared_ptr<IPrimitive> left;
        std::shared_ptr<IPrimitive> right;
        AABB box;
    };

    inline AABB get_triangle_aabb(const std::shared_ptr<Triangle>& tri) {
        Math::Point3D v0 = tri->v0();
        Math::Point3D v1 = tri->v1();
        Math::Point3D v2 = tri->v2();

        double min_x = std::min({v0.x, v1.x, v2.x});
        double min_y = std::min({v0.y, v1.y, v2.y});
        double min_z = std::min({v0.z, v1.z, v2.z});

        double max_x = std::max({v0.x, v1.x, v2.x});
        double max_y = std::max({v0.y, v1.y, v2.y});
        double max_z = std::max({v0.z, v1.z, v2.z});

        return AABB(
            Math::Point3D(min_x - 0.0001, min_y - 0.0001, min_z - 0.0001),
            Math::Point3D(max_x + 0.0001, max_y + 0.0001, max_z + 0.0001)
        );
    }

    inline bool get_primitive_aabb(const std::shared_ptr<IPrimitive>& prim, AABB& output_box) {
        if (auto tri = std::dynamic_pointer_cast<Triangle>(prim)) {
            output_box = get_triangle_aabb(tri);
            return true;
        }
        if (auto node = std::dynamic_pointer_cast<BVHNode>(prim)) {
            return node->get_aabb(output_box);
        }
        return false;
    }
}
