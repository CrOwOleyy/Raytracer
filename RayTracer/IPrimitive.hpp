#pragma once

#include "../Math/Point3D.hpp"
#include "../Math/Vector3D.hpp"
#include "../Math/Color.hpp"
#include "Ray.hpp"

namespace RayTracer
{
    struct HitRecord
    {
        Math::Point3D frappe_rayon;
        Math::Vector3D perpendiculaire;
        Math::Color albedo;
        double reflectivity = 0.2;
        double distance;
        bool cote;
        double u, v; //c'est les coordonnees de la texture pour le dammier

        inline void set_face_normal(const Ray& r, const Math::Vector3D& outward_normal)
        {
            cote = r.direction.dot(outward_normal) < 0;
            perpendiculaire = cote ? outward_normal : outward_normal * -1.0;
        }
    };

    class IPrimitive
    {
        public:
            virtual ~IPrimitive() = default;
            virtual bool hits(const Ray& ray, double t_min, double t_max, HitRecord& rec) const = 0;
    };
}
