#pragma once

#include "IPrimitive.hpp"

namespace RayTracer
{
    class Plane : public IPrimitive
    {
        public:
            Math::Point3D point;
            Math::Vector3D normal;
            Math::Color color;
            double reflectivity;
            Math::Color color2;
            bool hasColor2 = false;
            Math::Color color3;
            bool hasColor3 = false;
            std::string patternType = "checkerboard";
            double patternSize = 2.0;

            Plane(const Math::Point3D& point, const Math::Vector3D& normal, const Math::Color& color = Math::Color(1, 0, 0), double reflectivity = 0.0,
                  bool hasColor2 = false, const Math::Color& color2 = Math::Color(0, 0, 0), const std::string& patternType = "checkerboard", double patternSize = 2.0,
                  bool hasColor3 = false, const Math::Color& color3 = Math::Color(0, 0, 0));
            ~Plane() override = default;

            bool hits(const Ray& ray, double t_min, double t_max, HitRecord& rec) const override;
    };
}