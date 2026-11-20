#pragma once

#include "IPrimitive.hpp"

namespace RayTracer
{
    class Cone : public IPrimitive
    {
        public:
            Math::Point3D center;
            Math::Vector3D axis;
            double angle;
            double height;
            Math::Color color;
            double reflectivity;

            Cone(const Math::Point3D& center, const Math::Vector3D& axis, double angleDegrees, double height, const Math::Color& color = Math::Color(0.8, 0.6, 0.2), double reflectivity = 0.2);
            ~Cone() override = default;

            bool hits(const Ray& ray, double t_min, double t_max, HitRecord& rec) const override;
    };
}