#pragma once

#include "Ray.hpp"
#include "../Math/Point3D.hpp"
#include "../Math/Vector3D.hpp"

namespace RayTracer
{
    class Camera
    {
        public:
            Math::Point3D origin;
            Math::Point3D lower_left_corner;
            Math::Vector3D horizontal;
            Math::Vector3D vertical;

            Camera();
            Camera(const Math::Point3D& origin,
                   const Math::Point3D& look_at,
                   const Math::Vector3D& up,
                   double field_of_view_degrees,
                   double aspect_ratio);
            Camera(const Camera&) = default;
            Camera& operator=(const Camera&) = default;
            ~Camera() = default;

            Ray ray(double u, double v) const;
    };
}
