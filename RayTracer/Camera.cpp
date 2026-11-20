#include "Camera.hpp"
#include <cmath>

namespace RayTracer
{
    Camera::Camera()
        : origin(0.0, 0.0, 0.0),
          lower_left_corner(-1.0, -1.0, -1.0),
          horizontal(2.0, 0.0, 0.0),
          vertical(0.0, 2.0, 0.0)
    {
    }

    Camera::Camera(const Math::Point3D& origin,
                   const Math::Point3D& look_at,
                   const Math::Vector3D& up,
                   double field_of_view_degrees,
                   double aspect_ratio)
        : origin(origin)
    {
        const double theta = field_of_view_degrees * M_PI / 180.0;
        const double h = std::tan(theta / 2.0);
        const double viewport_height = 2.0 * h;
        const double viewport_width = aspect_ratio * viewport_height;

        const Math::Vector3D w = Math::Vector3D::unit_vector(origin - look_at);
        const Math::Vector3D u = Math::Vector3D::unit_vector(up.cross(w));
        const Math::Vector3D v = w.cross(u);

        horizontal = u * viewport_width;
        vertical = v * viewport_height;
        lower_left_corner = origin - horizontal / 2.0 - vertical / 2.0 - w;
    }

    Ray Camera::ray(double u, double v) const
    {
        const Math::Point3D target = lower_left_corner + (horizontal * u) + (vertical * v);
        const Math::Vector3D direction(
            target.x - origin.x,
            target.y - origin.y,
            target.z - origin.z
        );
        return Ray(origin, direction);
    }
}
