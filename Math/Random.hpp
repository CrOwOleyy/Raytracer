#pragma once

#include <random>

namespace Math
{
    inline double random_double()
    {
        thread_local std::uniform_real_distribution<double> distribution(0.0, 1.0);
        thread_local std::mt19937 generator(std::random_device{}());
        return distribution(generator);
    }

    inline double random_double(double min, double max)
    {
        return min + (max - min) * random_double();
    }
}
