#include "Vector3D.hpp"

namespace Math {

    Vector3D::Vector3D(double x, double y, double z): x(x), y(y), z(z) {}

    double Vector3D::length() const {
        return std::sqrt(x*x + y*y + z*z);
    }
    double Vector3D::dot(const Vector3D& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3D Vector3D::cross(const Vector3D& other) const
    {
        return Vector3D(y * other.z - z * other.y,
                        z * other.x - x * other.z,
                        x * other.y - y * other.x);
    }
    Vector3D Vector3D::operator+(const Vector3D& other) const
    {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }

    Vector3D Vector3D::operator-(const Vector3D& other) const
    {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }

    Vector3D Vector3D::operator*(const Vector3D& other) const
    {
        return Vector3D(x * other.x, y * other.y, z * other.z);
    }

    Vector3D Vector3D::operator/(const Vector3D& other) const
    {
        return Vector3D(x / other.x, y / other.y, z / other.z);
    }

    Vector3D& Vector3D::operator+=(const Vector3D& other)
    {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }

    Vector3D& Vector3D::operator-=(const Vector3D& other)
    {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }

    Vector3D& Vector3D::operator*=(const Vector3D& other)
    {
        x *= other.x; y *= other.y; z *= other.z;
        return *this;
    }

    Vector3D& Vector3D::operator/=(const Vector3D& other)
    {
        x /= other.x; y /= other.y; z /= other.z;
        return *this;
    }

    Vector3D Vector3D::operator*(double scalar) const
    {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    Vector3D Vector3D::operator/(double scalar) const
    {
        return Vector3D(x / scalar, y / scalar, z / scalar);
    }

    Vector3D& Vector3D::operator*=(double scalar)
    {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }

    Vector3D& Vector3D::operator/=(double scalar)
    {
        x /= scalar; y /= scalar; z /= scalar;
        return *this;
    }

    Vector3D Vector3D::unit_vector(const Vector3D& v)
    {
        return v / v.length();
    }

    Vector3D Vector3D::rotateXYZ(const Vector3D& v, const Vector3D& rotationDegrees)
    {
        constexpr double PI = 3.14159265358979323846;
        const double rx = rotationDegrees.x * PI / 180.0;
        const double ry = rotationDegrees.y * PI / 180.0;
        const double rz = rotationDegrees.z * PI / 180.0;

        Vector3D out = v;

        // Rotation autour de X (pitch)
        {
            const double cx = std::cos(rx);
            const double sx = std::sin(rx);
            out = Vector3D(
                out.x,
                out.y * cx - out.z * sx,
                out.y * sx + out.z * cx
            );
        }

        // Rotation autour de Y (yaw)
        {
            const double cy = std::cos(ry);
            const double sy = std::sin(ry);
            out = Vector3D(
                out.x * cy + out.z * sy,
                out.y,
                -out.x * sy + out.z * cy
            );
        }

        // Rotation autour de Z (roll)
        {
            const double cz = std::cos(rz);
            const double sz = std::sin(rz);
            out = Vector3D(
                out.x * cz - out.y * sz,
                out.x * sz + out.y * cz,
                out.z
            );
        }

        return out;
    }
}
