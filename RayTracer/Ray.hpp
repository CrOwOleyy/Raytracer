#pragma once

#include "../Math/Point3D.hpp"
#include "../Math/Vector3D.hpp"

namespace RayTracer
{
    class Ray
    {
        public:
            Math::Point3D origin;
            Math::Vector3D direction;

            Ray() = default;
            Ray(const Math::Point3D& origin, const Math::Vector3D& direction);

            Math::Point3D at(double t) const;
    };
}
