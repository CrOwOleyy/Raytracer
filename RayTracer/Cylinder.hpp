#pragma once

#include "IPrimitive.hpp"

namespace RayTracer
{
    class Cylinder : public IPrimitive
    {
      public:
        Math::Point3D center;
        Math::Vector3D axis;
        double radius;
        double height;
        Math::Color color;
        double reflectivity;

      Cylinder(const Math::Point3D &center, const Math::Vector3D &axis, double radius, double height, const Math::Color &color = Math::Color(0.3, 0.7, 0.3), double reflectivity = 0.2);
      ~Cylinder() override = default;

      bool hits(const Ray &ray, double t_min, double t_max, HitRecord &rec) const override;
    };
} // namespace RayTracer