#pragma once

#include "../Math/Point3D.hpp"
#include "Ray.hpp"
#include <algorithm>

namespace RayTracer
{
    class AABB
    {
    public:
        Math::Point3D minimum;
        Math::Point3D maximum;

        AABB() {}
        AABB(const Math::Point3D& a, const Math::Point3D& b) : minimum(a), maximum(b) {}

        bool hits(const Ray& r, double t_min, double t_max) const
        {
            for (int a = 0; a < 3; a++) {
                double r_dir = (a == 0 ? r.direction.x : (a == 1 ? r.direction.y : r.direction.z));
                double r_orig = (a == 0 ? r.origin.x : (a == 1 ? r.origin.y : r.origin.z));
                double min_val = (a == 0 ? minimum.x : (a == 1 ? minimum.y : minimum.z));
                double max_val = (a == 0 ? maximum.x : (a == 1 ? maximum.y : maximum.z));

                double invD = 1.0 / r_dir;
                double t0 = (min_val - r_orig) * invD;
                double t1 = (max_val - r_orig) * invD;
                if (invD < 0.0)
                    std::swap(t0, t1);
                t_min = t0 > t_min ? t0 : t_min;
                t_max = t1 < t_max ? t1 : t_max;
                if (t_max <= t_min)
                    return false;
            }
            return true;
        }
    };

    inline AABB surrounding_box(const AABB& box0, const AABB& box1)
    {
        Math::Point3D small_pt(
            std::min(box0.minimum.x, box1.minimum.x),
            std::min(box0.minimum.y, box1.minimum.y),
            std::min(box0.minimum.z, box1.minimum.z)
        );
        Math::Point3D big_pt(
            std::max(box0.maximum.x, box1.maximum.x),
            std::max(box0.maximum.y, box1.maximum.y),
            std::max(box0.maximum.z, box1.maximum.z)
        );
        return AABB(small_pt, big_pt);
    }
}
