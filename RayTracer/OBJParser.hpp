#pragma once

#include <vector>
#include <string>
#include <memory>
#include "IPrimitive.hpp"
#include "Triangle.hpp"

namespace RayTracer
{
    class OBJParser
    {
        public:
            static std::vector<std::shared_ptr<IPrimitive>> parse(const std::string& filePath, const Math::Color& color, double reflectivity, const Math::Point3D& position, const Math::Vector3D& rotation, double scale);
    };
}
