#include "Cylinder.hpp"
#include <cmath>

namespace RayTracer
{
    Cylinder::Cylinder(const Math::Point3D& center, const Math::Vector3D& axis, double radius, double height, const Math::Color& color, double reflectivity)
        : center(center), axis(Math::Vector3D::unit_vector(axis)), radius(radius), height(height), color(color), reflectivity(reflectivity)
    {
    }

    bool Cylinder::hits(const Ray& ray, double t_min, double t_max, HitRecord& rec) const
    {
        Math::Vector3D oc = ray.origin - center;
        double dir_dot_axis = ray.direction.dot(axis);
        double oc_dot_axis = oc.dot(axis);

        double a = ray.direction.dot(ray.direction) - dir_dot_axis * dir_dot_axis;
        double b = oc.dot(ray.direction) - oc_dot_axis * dir_dot_axis; 
        double c = oc.dot(oc) - oc_dot_axis * oc_dot_axis - radius * radius;

        if (a < 1e-8) return false;

        double discriminant = b * b - a * c;
        if (discriminant < 0) return false;
        
        double sqrtd = std::sqrt(discriminant);

        double t1 = (-b - sqrtd) / a;
        double t2 = (-b + sqrtd) / a;

        double chosen_root = t1;
        bool is_valid = false;

        if (chosen_root >= t_min && chosen_root <= t_max) {
            double projection = (ray.at(chosen_root) - center).dot(axis);
            if (projection >= -height / 2.0 && projection <= height / 2.0) {
                is_valid = true;
            }
        }

        if (!is_valid) {
            chosen_root = t2;
            if (chosen_root >= t_min && chosen_root <= t_max) {
                double projection = (ray.at(chosen_root) - center).dot(axis);
                if (projection >= -height / 2.0 && projection <= height / 2.0) {
                    is_valid = true;
                }
            }
        }

        if (!is_valid) return false;

        rec.distance = chosen_root;
        rec.frappe_rayon = ray.at(rec.distance);

        Math::Vector3D cp = rec.frappe_rayon - center;
        double projection = cp.dot(axis);
        Math::Point3D point_on_axis = center + (axis * projection);
        
        Math::Vector3D outward_normal = (rec.frappe_rayon - point_on_axis) / radius;

        rec.set_face_normal(ray, outward_normal);
        rec.albedo = color;
        rec.reflectivity = reflectivity;

        return true;
    }
}
