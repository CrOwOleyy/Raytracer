#pragma once

#include "../Math/Point3D.hpp"
#include "../Math/Vector3D.hpp"

namespace RayTracer
{
    class Rectangle3D
    {
        public:
            Math::Point3D origin;
            Math::Vector3D bottom_side;
            Math::Vector3D left_side;

            Rectangle3D();
            Rectangle3D(
                const Math::Point3D& origin,
                const Math::Vector3D& bottomSide,
                const Math::Vector3D& leftSide
            );

            Math::Point3D pointAt(double u, double v) const;
    };
}
