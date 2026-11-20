#pragma once

#include "IPrimitive.hpp"

namespace RayTracer
{
    class Triangle : public IPrimitive
    {
        public:
            Triangle(const Math::Point3D& v0, const Math::Point3D& v1, const Math::Point3D& v2, const Math::Color& color, double reflectivity = 0.2);
            ~Triangle() override = default;

            bool hits(const Ray& ray, double t_min, double t_max, HitRecord& rec) const override;

            const Math::Point3D& v0() const { return _v0; }
            const Math::Point3D& v1() const { return _v1; }
            const Math::Point3D& v2() const { return _v2; }

        private:
            Math::Point3D _v0, _v1, _v2;
            Math::Color _color;
            double _reflectivity;
    };
}
