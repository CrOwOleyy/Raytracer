#include "SceneFactory.hpp"
#include "Sphere.hpp"
#include "Plane.hpp"
#include "Cone.hpp"
#include "Cylinder.hpp"
#include "OBJParser.hpp"
#include "BVHNode.hpp"
#include <cmath>


namespace RayTracer
{
    PrimitiveList SceneFactory::createWorld(const SceneConfig& scene)
    {
        PrimitiveList world;

        for (const auto& sphereConfig : scene.spheres) {
            world.add(std::make_shared<Sphere>(
                sphereConfig.position,
                sphereConfig.radius,
                sphereConfig.color,
                sphereConfig.reflectivity
            ));
        }

        for (const auto& cylConfig : scene.cylinders) {
            world.add(std::make_shared<Cylinder>(
                cylConfig.position,
                cylConfig.axis,
                cylConfig.radius,
                cylConfig.height,
                cylConfig.color,
                cylConfig.reflectivity
            ));
        }

        for (const auto& coneConfig : scene.cones) {
            world.add(std::make_shared<Cone>(
                coneConfig.position,
                coneConfig.axis,
                coneConfig.angle,
                coneConfig.height,
                coneConfig.color,
                coneConfig.reflectivity
            ));
        }

        for (const auto& planeConfig : scene.planes) {
            Math::Point3D point = planeConfig.point;
            Math::Vector3D normal = planeConfig.normal;

            if (planeConfig.usesAxis) {
                const char axis = static_cast<char>(std::toupper(static_cast<unsigned char>(planeConfig.axis)));

                if (axis == 'X') {
                    point = Math::Point3D(planeConfig.position, 0, 0);
                    normal = Math::Vector3D(1, 0, 0);
                } else if (axis == 'Y') {
                    point = Math::Point3D(0, planeConfig.position, 0);
                    normal = Math::Vector3D(0, 1, 0);
                } else if (axis == 'Z') {
                    point = Math::Point3D(0, 0, planeConfig.position);
                    normal = Math::Vector3D(0, 0, 1);
                }
            }

            world.add(std::make_shared<Plane>(
                point,
                normal,
                planeConfig.color,
                planeConfig.reflectivity,
                planeConfig.hasColor2,
                planeConfig.color2,
                planeConfig.patternType,
                planeConfig.patternSize,
                planeConfig.hasColor3,
                planeConfig.color3
            ));
        }

        for (const auto& meshConfig : scene.meshes) {
            auto meshTriangles = OBJParser::parse(
                meshConfig.filePath,
                meshConfig.color,
                meshConfig.reflectivity,
                meshConfig.position,
                meshConfig.rotation,
                meshConfig.scale
            );
            if (!meshTriangles.empty()) {
                auto bvh_root = std::make_shared<BVHNode>(meshTriangles, 0, meshTriangles.size());
                world.add(bvh_root);
            }
        }

        return world;
    }

    std::unique_ptr<Ambiant> SceneFactory::createAmbientLight(const SceneConfig& scene)
    {
        return std::make_unique<Ambiant>(scene.lights.ambient, scene.lights.ambient_color);
    }

    std::vector<std::unique_ptr<Directional>> SceneFactory::createDirectionalLight(const SceneConfig& scene)
    {
        std::vector<std::unique_ptr<Directional>> lights;

        for (const auto& dlc : scene.lights.additional_directional_lights) {
            lights.push_back(std::make_unique<Directional>(dlc.direction, dlc.intensity, dlc.color));
        }
        return lights;
    }

    std::vector<std::unique_ptr<PointLight>> SceneFactory::createPointLight(const SceneConfig& scene)
    {
        std::vector<std::unique_ptr<PointLight>> lights;

        for (const auto& plc : scene.lights.point_lights) {
            lights.push_back(std::make_unique<PointLight>(plc.position, plc.intensity, plc.radius, plc.color));
        }
        return lights;
    }

    Camera SceneFactory::createCamera(const SceneConfig& scene)
    {
        const double aspectRatio = static_cast<double>(scene.camera.width) / static_cast<double>(scene.camera.height);
        const Math::Vector3D defaultForward(0.0, 0.0, -1.0);
        const Math::Vector3D defaultUp(0.0, 1.0, 0.0);

        const Math::Vector3D forward = Math::Vector3D::unit_vector(
            Math::Vector3D::rotateXYZ(defaultForward, scene.camera.rotation)
        );
        const Math::Vector3D up = Math::Vector3D::unit_vector(
            Math::Vector3D::rotateXYZ(defaultUp, scene.camera.rotation)
        );
        const Math::Point3D lookAt = scene.camera.position + forward;

        return Camera(
            scene.camera.position,
            lookAt,
            up,
            scene.camera.fieldOfView,
            aspectRatio
        );
    }
}
