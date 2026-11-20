#include "Rectangle3D.hpp"

namespace RayTracer
{
    Rectangle3D::Rectangle3D()
        : origin(0.0, 0.0, 0.0),
          bottom_side(1.0, 0.0, 0.0),
          left_side(0.0, 1.0, 0.0)
    {
    }

    Rectangle3D::Rectangle3D(
        const Math::Point3D& origin,
        const Math::Vector3D& bottomSide,
        const Math::Vector3D& leftSide
    )
        : origin(origin), bottom_side(bottomSide), left_side(leftSide)
    {
    }

    Math::Point3D Rectangle3D::pointAt(double u, double v) const
    {
        return origin + (bottom_side * u) + (left_side * v);
    }
}
