#include "Sphere.hpp"
#include <cmath>

namespace RayTracer
{
    Sphere::Sphere(const Math::Point3D& center, double radius, const Math::Color& color, double reflectivity)
        : center(center), radius(radius), color(color), reflectivity(reflectivity)
    {
    }

    bool Sphere::hits(const Ray& ray, double t_min, double t_max, HitRecord& rec) const
    {
        Math::Vector3D oc = ray.origin - center;
        double a = ray.direction.dot(ray.direction);
        double b = oc.dot(ray.direction);
        double c = oc.dot(oc) - radius * radius;
        double discriminant = b * b - a * c;

        if (discriminant < 0)
            return false;
        
        double sqrtd = std::sqrt(discriminant);

        double root = (-b - sqrtd) / a;
        if (root < t_min || t_max < root) {
            root = (-b + sqrtd) / a;
            if (root < t_min || t_max < root)
                return false;
        }

        rec.distance = root;
        rec.frappe_rayon = ray.at(rec.distance);
        Math::Vector3D outward_normal = (rec.frappe_rayon - center) / radius;
        rec.set_face_normal(ray, outward_normal);
        rec.albedo = color; // couleur propre à la sphère
        rec.reflectivity = reflectivity;

        return true;
    }
}
