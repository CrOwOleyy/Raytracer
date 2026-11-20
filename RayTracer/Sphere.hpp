#pragma once

#include "IPrimitive.hpp"

namespace RayTracer
{
    class Sphere : public IPrimitive
    {
        public:
            Math::Point3D center;
            double radius;
            Math::Color color;
            double reflectivity;

            Sphere(const Math::Point3D& center, double radius, const Math::Color& color = Math::Color(0.7, 0.3, 0.3), double reflectivity = 0.2);
            ~Sphere() override = default;

            bool hits(const Ray& ray, double t_min, double t_max, HitRecord& rec) const override;
    };
}
