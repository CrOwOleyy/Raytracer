#pragma once

#include "IPrimitive.hpp"
#include <memory>
#include <vector>

namespace RayTracer
{
    class PrimitiveList : public IPrimitive
    {
        public:
            PrimitiveList() = default;
            PrimitiveList(std::shared_ptr<IPrimitive> object) { add(object); }

            void clear() { objects.clear(); }
            void add(std::shared_ptr<IPrimitive> object) { objects.push_back(object); }

            bool hits(const Ray& ray, double t_min, double t_max, HitRecord& rec) const override;

        public:
            std::vector<std::shared_ptr<IPrimitive>> objects;
    };
}
