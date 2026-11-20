#pragma once

#include <string>
#include <vector>
#include "../Math/Point3D.hpp"
#include "../Math/Vector3D.hpp"
#include "../Math/Color.hpp"

namespace RayTracer
{
    struct CameraConfig
    {
        int width = 400;
        int height = 400;
        Math::Point3D position = {0, 0, 0};
        Math::Vector3D rotation = {0, 0, 0};
        double fieldOfView = 72.0;
        int samplesPerPixel = 1;
    };

    struct SphereConfig
    {
        Math::Point3D position;
        double radius;
        Math::Color color;
        double reflectivity = 0.2;
    };

    struct CylinderConfig {
        Math::Point3D position;
        Math::Vector3D axis;
        double radius;
        double height;
        Math::Color color;
        double reflectivity = 0.2;
    };

    struct ConeConfig {
        Math::Point3D position;
        Math::Vector3D axis;
        double angle;
        double height;
        Math::Color color;
        double reflectivity = 0.2;
    };

    struct PlaneConfig
    {
        bool usesAxis = true;
        char axis = 'Y';  // 'X', 'Y', or 'Z'
        double position = 0.0;
        Math::Point3D point = {0, 0, 0};
        Math::Vector3D normal = {0, 1, 0};
        Math::Color color;
        double reflectivity = 0.2;
        Math::Color color2;
        bool hasColor2 = false;
        Math::Color color3;
        bool hasColor3 = false;
        std::string patternType = "checkerboard";
        double patternSize = 2.0;
    };

    struct DirectionalLightConfig
    {
        Math::Vector3D direction;
        double intensity;
        Math::Color color = {1.0, 1.0, 1.0};  // couleur blanche
    };

    struct PointLightConfig
    {
        Math::Point3D position;
        double intensity;
        double radius = 100.0;
        Math::Color color = {1.0, 1.0, 1.0};
    };

    struct MeshConfig
    {
        std::string filePath;
        Math::Point3D position = {0, 0, 0};
        Math::Vector3D rotation = {0, 0, 0};
        double scale = 1.0;
        Math::Color color = {1.0, 1.0, 1.0};
        double reflectivity = 0.2;
    };

    struct LightConfig
    {
        double ambient = 0.3;
        Math::Color ambient_color = {1.0, 1.0, 1.0};  // couleur blanche par défaut
        double diffuse = 0.7;
        // double directional_intensity = 0.7;
        // Math::Vector3D directional_direction = {-1, 1, 1};
        std::vector<DirectionalLightConfig> additional_directional_lights;
        std::vector<PointLightConfig> point_lights;
    };

    struct SkyConfig
    {
        Math::Color color1 = {1.0, 0.7, 0.2};  // couleur bas
        Math::Color color2 = {1.0, 0.5, 0.1};  // couleur haut
        bool hasSkybox = false;
        std::string skyboxPath = "";
    };

    struct SceneConfig
    {
        CameraConfig camera;
        std::vector<SphereConfig> spheres;
        std::vector<PlaneConfig> planes;
        std::vector<MeshConfig> meshes;
        std::vector<CylinderConfig> cylinders;
        std::vector<ConeConfig> cones;
        LightConfig lights;
        SkyConfig sky;
    };

    class SceneParser
    {
    public:
        static SceneConfig parse(const std::string& filename);
    };

}