#include "Triangle.hpp"

namespace RayTracer
{
    Triangle::Triangle(const Math::Point3D& v0, const Math::Point3D& v1, const Math::Point3D& v2, const Math::Color& color, double reflectivity)
        : _v0(v0), _v1(v1), _v2(v2), _color(color), _reflectivity(reflectivity)
    {
    }

    bool Triangle::hits(const Ray& ray, double t_min, double t_max, HitRecord& rec) const
    {
        Math::Vector3D edge1 = _v1 - _v0;
        Math::Vector3D edge2 = _v2 - _v0;
        Math::Vector3D h = ray.direction.cross(edge2);
        double a = edge1.dot(h);

        if (a > -0.000001 && a < 0.000001)
            return false;

        double f = 1.0 / a;
        Math::Vector3D s = ray.origin - _v0;
        double u = f * s.dot(h);

        if (u < 0.0 || u > 1.0)
            return false;

        Math::Vector3D q = s.cross(edge1);
        double v = f * ray.direction.dot(q);

        if (v < 0.0 || u + v > 1.0)
            return false;

        double t = f * edge2.dot(q);

        if (t > t_min && t < t_max) {
            rec.distance = t;
            rec.frappe_rayon = ray.at(t);
            Math::Vector3D normal = Math::Vector3D::unit_vector(edge1.cross(edge2));
            rec.set_face_normal(ray, normal);
            rec.albedo = _color;
            rec.reflectivity = _reflectivity;
            return true;
        }

        return false;
    }
}
