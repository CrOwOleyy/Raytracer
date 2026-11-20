#include "Cone.hpp"
#include <cmath>

namespace RayTracer
{
    constexpr double PI = 3.14159265358979323846;

    Cone::Cone(const Math::Point3D& center, const Math::Vector3D& axis, double angleDegrees, double height, const Math::Color& color, double reflectivity)
        : center(center), axis(Math::Vector3D::unit_vector(axis)), height(height), color(color), reflectivity(reflectivity)
    {
        this->angle = angleDegrees * (PI / 180.0);
    }

    bool Cone::hits(const Ray& ray, double t_min, double t_max, HitRecord& rec) const
    {
        Math::Vector3D oc = ray.origin - center;
        double dir_dot_axis = ray.direction.dot(axis);
        double oc_dot_axis = oc.dot(axis);

        double tan_theta = std::tan(angle);
        double k = 1.0 + tan_theta * tan_theta;

        double a = ray.direction.dot(ray.direction) - k * dir_dot_axis * dir_dot_axis;
        double b = oc.dot(ray.direction) - k * oc_dot_axis * dir_dot_axis; 
        double c = oc.dot(oc) - k * oc_dot_axis * oc_dot_axis;

        if (std::abs(a) < 1e-8) return false;

        double discriminant = b * b - a * c;
        if (discriminant < 0) return false;
        
        double sqrtd = std::sqrt(discriminant);
        double t1 = (-b - sqrtd) / a;
        double t2 = (-b + sqrtd) / a;

        double chosen_root = t1;
        bool is_valid = false;

        if (chosen_root >= t_min && chosen_root <= t_max) {
            double projection = (ray.at(chosen_root) - center).dot(axis);
            if (projection > 0 && projection <= height) {
                is_valid = true;
            }
        }

        if (!is_valid) {
            chosen_root = t2;
            if (chosen_root >= t_min && chosen_root <= t_max) {
                double projection = (ray.at(chosen_root) - center).dot(axis);
                if (projection > 0 && projection <= height) {
                    is_valid = true;
                }
            }
        }

        if (!is_valid) return false;
        rec.distance = chosen_root;
        rec.frappe_rayon = ray.at(rec.distance);
        Math::Vector3D cp = rec.frappe_rayon - center;
        double m = cp.dot(axis);
        
        Math::Vector3D normal = cp - (axis * (k * m));
        Math::Vector3D outward_normal = Math::Vector3D::unit_vector(normal);
        rec.set_face_normal(ray, outward_normal);
        rec.albedo = color;
        rec.reflectivity = reflectivity;

        return true;
    }
}
