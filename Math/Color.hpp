#pragma once

#include "Vector3D.hpp"
#include <iostream>

namespace Math
{
    using Color = Vector3D;

    inline void write_color(std::ostream& out, const Color& pixel_color)
    {
        auto clamp = [](double v, double lo, double hi) {
            if (v < lo) return lo;
            if (v > hi) return hi;
            return v;
        };

        // Apply gamma correction (gamma = 2.0) and clamp to [0,0.999]
        double r = clamp(pixel_color.x, 0.0, 0.999);
        double g = clamp(pixel_color.y, 0.0, 0.999);
        double b = clamp(pixel_color.z, 0.0, 0.999);

        r = sqrt(r);
        g = sqrt(g);
        b = sqrt(b);

        out << static_cast<int>(255.999 * r) << ' '
            << static_cast<int>(255.999 * g) << ' '
            << static_cast<int>(255.999 * b) << '\n';
    }
}
