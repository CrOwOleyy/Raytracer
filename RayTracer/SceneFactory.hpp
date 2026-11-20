#pragma once

#include <memory>
#include "PrimitiveList.hpp"
#include "SceneParser.hpp"
#include "Camera.hpp"
#include "Light/Ambiant.hpp"
#include "Light/Directional.hpp"
#include "Light/PointLight.hpp"

namespace RayTracer
{
    class SceneFactory
    {
        public:
            static PrimitiveList createWorld(const SceneConfig& scene);
            static std::unique_ptr<Ambiant> createAmbientLight(const SceneConfig& scene);
            static std::vector<std::unique_ptr<Directional>> createDirectionalLight(const SceneConfig& scene);
            static std::vector<std::unique_ptr<PointLight>> createPointLight(const SceneConfig& scene);
            static Camera createCamera(const SceneConfig& scene);
    };
}
