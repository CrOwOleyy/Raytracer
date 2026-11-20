#include "Plane.hpp"
#include <cmath>

namespace RayTracer
{
    Plane::Plane(const Math::Point3D& point, const Math::Vector3D& normal, const Math::Color& color, double reflectivity,
                 bool hasColor2, const Math::Color& color2, const std::string& patternType, double patternSize,
                 bool hasColor3, const Math::Color& color3)
        : point(point), normal(Math::Vector3D::unit_vector(normal)), color(color), reflectivity(reflectivity),
          color2(color2), hasColor2(hasColor2), color3(color3), hasColor3(hasColor3), patternType(patternType), patternSize(patternSize)
    {
    }

    bool Plane::hits(const Ray& ray, double t_min, double t_max, HitRecord& rec) const
    {
        double denom = normal.dot(ray.direction);
        if (std::abs(denom) < 1e-6) {
            return false;
        }

        double t = (point - ray.origin).dot(normal) / denom;

        if (t < t_min || t > t_max) {
            return false;
        }

        rec.distance = t;
        rec.frappe_rayon = ray.at(t);
        rec.set_face_normal(ray, normal);
        rec.reflectivity = reflectivity;

        if (hasColor2) {
            // Find two orthonormal basis vectors on the plane.
            Math::Vector3D u;
            if (std::abs(normal.x) < 0.9) {
                u = Math::Vector3D::unit_vector(normal.cross(Math::Vector3D(1, 0, 0)));
            } else {
                u = Math::Vector3D::unit_vector(normal.cross(Math::Vector3D(0, 1, 0)));
            }
            Math::Vector3D v = normal.cross(u);

            Math::Vector3D d = rec.frappe_rayon - point;
            double xp = d.dot(u);
            double yp = d.dot(v);

            double size = patternSize > 0.0 ? patternSize : 2.0;

            if (hasColor3) {
                if (patternType == "synthwave" || patternType == "gradient" || patternType == "wave") {
                    double factor = 0.5 + 0.5 * std::sin((xp + yp) / size);
                    if (factor < 0.33) {
                        double step = factor / 0.33;
                        rec.albedo = color * (1.0 - step) + color2 * step;
                    } else if (factor < 0.66) {
                        double step = (factor - 0.33) / 0.33;
                        rec.albedo = color2 * (1.0 - step) + color3 * step;
                    } else {
                        double step = (factor - 0.66) / 0.34;
                        rec.albedo = color3 * (1.0 - step) + color * step;
                    }
                } else if (patternType == "stripes") {
                    int ix = std::floor(xp / size);
                    int idx = ((ix % 3) + 3) % 3;
                    if (idx == 0) rec.albedo = color;
                    else if (idx == 1) rec.albedo = color2;
                    else rec.albedo = color3;
                } else if (patternType == "rings") {
                    double dist = std::sqrt(xp * xp + yp * yp);
                    int ir = std::floor(dist / size);
                    int idx = ((ir % 3) + 3) % 3;
                    if (idx == 0) rec.albedo = color;
                    else if (idx == 1) rec.albedo = color2;
                    else rec.albedo = color3;
                } else { // default to 3-color checkerboard
                    int ix = std::floor(xp / size);
                    int iy = std::floor(yp / size);
                    int idx = (((ix + iy) % 3) + 3) % 3;
                    if (idx == 0) rec.albedo = color;
                    else if (idx == 1) rec.albedo = color2;
                    else rec.albedo = color3;
                }
            } else {
                if (patternType == "stripes") {
                    int ix = std::floor(xp / size);
                    if (ix % 2 == 0) {
                        rec.albedo = color;
                    } else {
                        rec.albedo = color2;
                    }
                } else if (patternType == "rings") {
                    double dist = std::sqrt(xp * xp + yp * yp);
                    int ir = std::floor(dist / size);
                    if (ir % 2 == 0) {
                        rec.albedo = color;
                    } else {
                        rec.albedo = color2;
                    }
                } else { // default to checkerboard
                    int ix = std::floor(xp / size);
                    int iy = std::floor(yp / size);
                    if ((ix + iy) % 2 == 0) {
                        rec.albedo = color;
                    } else {
                        rec.albedo = color2;
                    }
                }
            }
        } else {
            rec.albedo = color;
        }

        return true;
    }
}