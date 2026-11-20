#include "Ray.hpp"

namespace RayTracer
{
    Ray::Ray(const Math::Point3D& origin, const Math::Vector3D& direction)
        : origin(origin), direction(direction)
    {
    }

    Math::Point3D Ray::at(double t) const
    {
        return origin + direction * t;
    }
}

//at donne le point le long du rayon a une distance t