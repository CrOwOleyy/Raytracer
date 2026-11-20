#include "Point3D.hpp"

namespace Math
{
    Point3D::Point3D(double x, double y, double z) : x(x), y(y), z(z) {}

    Point3D Point3D::operator+(const Vector3D& vec) const
    {
        return Point3D(x + vec.x, y + vec.y, z + vec.z);
    }

    Point3D& Point3D::operator+=(const Vector3D& vec)
    {
        x += vec.x; y += vec.y; z += vec.z;
        return *this;
    }

    Point3D Point3D::operator-(const Vector3D& vec) const
    {
        return Point3D(x - vec.x, y - vec.y, z - vec.z);
    }

    Vector3D Point3D::operator-(const Point3D& other) const
    {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }
}
