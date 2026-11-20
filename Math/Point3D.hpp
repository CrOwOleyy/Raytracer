#pragma once
#include "Vector3D.hpp"

namespace Math
{
    class Point3D
    {
        public:
            double x = 0.0;
            double y = 0.0;
            double z = 0.0;

            Point3D() = default;
            Point3D(double x, double y, double z);

            Point3D operator+(const Vector3D& vec) const;
            Point3D& operator+=(const Vector3D& vec);
            Point3D operator-(const Vector3D& vec) const;
            Vector3D operator-(const Point3D& other) const;
    };
}
