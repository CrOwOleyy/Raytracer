#pragma once
#include <cmath>

namespace Math
{
    class Vector3D
    {
        public:
            double x = 0.0;
            double y = 0.0;
            double z = 0.0;

            Vector3D() = default;
            Vector3D(double x, double y, double z);

            double length() const;
            double dot(const Vector3D& other) const;
            Vector3D cross(const Vector3D& other) const;

            Vector3D operator+(const Vector3D& other) const;
            Vector3D operator-(const Vector3D& other) const;
            Vector3D operator*(const Vector3D& other) const;
            Vector3D operator/(const Vector3D& other) const;

            Vector3D& operator+=(const Vector3D& other);
            Vector3D& operator-=(const Vector3D& other);
            Vector3D& operator*=(const Vector3D& other);
            Vector3D& operator/=(const Vector3D& other);

            Vector3D operator*(double scalar) const;
            Vector3D operator/(double scalar) const;
            Vector3D& operator*=(double scalar);
            Vector3D& operator/=(double scalar);

            static Vector3D unit_vector(const Vector3D& v);
            static Vector3D rotateXYZ(const Vector3D& v, const Vector3D& rotationDegrees);
    };
}
