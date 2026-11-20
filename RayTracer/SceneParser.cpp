#include "SceneParser.hpp"
#include <libconfig.h++>
#include <algorithm>
#include <functional>
#include <set>
#include <stdexcept>

namespace RayTracer
{
    static double readDouble(const libconfig::Setting& setting, const char* name, double defaultValue)
    {
        double doubleValue = 0.0;
        if (setting.lookupValue(name, doubleValue)) {
            return doubleValue;
        }

        int intValue = 0;
        if (setting.lookupValue(name, intValue)) {
            return static_cast<double>(intValue);
        }

        return defaultValue;
    }

    static int readInt(const libconfig::Setting& setting, const char* name, int defaultValue)
    {
        int intValue = 0;
        if (setting.lookupValue(name, intValue)) {
            return intValue;
        }

        return defaultValue;
    }

    static int readSampleCount(const libconfig::Setting& setting, int defaultValue)
    {
        int sampleCount = readInt(setting, "samplesPerPixel", defaultValue);
        if (sampleCount != defaultValue) {
            return sampleCount;
        }

        return readInt(setting, "samples", defaultValue);
    }

    static Math::Point3D parsePoint(const libconfig::Setting& setting)
    {
        return Math::Point3D(
            readDouble(setting, "x", 0.0),
            readDouble(setting, "y", 0.0),
            readDouble(setting, "z", 0.0)
        );
    }

    static Math::Vector3D parseVector(const libconfig::Setting& setting)
    {
        return Math::Vector3D(
            readDouble(setting, "x", 0.0),
            readDouble(setting, "y", 0.0),
            readDouble(setting, "z", 0.0)
        );
    }

    static Math::Color parseColor(const libconfig::Setting& setting)
    {
        return Math::Color(
            readInt(setting, "r", 255) / 255.0,
            readInt(setting, "g", 255) / 255.0,
            readInt(setting, "b", 255) / 255.0
        );
    }

    static double parseReflectivity(const libconfig::Setting& setting, double defaultValue)
    {
        const double value = readDouble(setting, "reflectivity", defaultValue);
        return std::clamp(value, 0.0, 1.0);
    }

    static std::string parentDirectory(const std::string& filePath)
    {
        const std::size_t slash = filePath.find_last_of("/\\");
        if (slash == std::string::npos) {
            return "";
        }
        return filePath.substr(0, slash + 1);
    }

    static std::string resolveRelativePath(const std::string& baseFile, const std::string& relativePath)
    {
        if (relativePath.empty() || relativePath.front() == '/') {
            return relativePath;
        }

        return parentDirectory(baseFile) + relativePath;
    }

    static bool isDefaultColor(const Math::Color& color, const Math::Color& reference)
    {
        const double epsilon = 1e-6;
        return std::abs(color.x - reference.x) < epsilon
            && std::abs(color.y - reference.y) < epsilon
            && std::abs(color.z - reference.z) < epsilon;
    }

    static void mergeScene(SceneConfig& target, const SceneConfig& source)
    {
        target.spheres.insert(target.spheres.end(), source.spheres.begin(), source.spheres.end());
        target.planes.insert(target.planes.end(), source.planes.begin(), source.planes.end());
        target.meshes.insert(target.meshes.end(), source.meshes.begin(), source.meshes.end());
        target.cylinders.insert(target.cylinders.end(), source.cylinders.begin(), source.cylinders.end());
        target.cones.insert(target.cones.end(), source.cones.begin(), source.cones.end());

        target.lights.additional_directional_lights.insert(
            target.lights.additional_directional_lights.end(),
            source.lights.additional_directional_lights.begin(),
            source.lights.additional_directional_lights.end()
        );
        target.lights.point_lights.insert(
            target.lights.point_lights.end(),
            source.lights.point_lights.begin(),
            source.lights.point_lights.end()
        );

        if (target.camera.width == 400 && target.camera.height == 400 && target.camera.fieldOfView == 72.0) {
            target.camera = source.camera;
        }
        if (target.lights.ambient == 0.3) {
            target.lights.ambient = source.lights.ambient;
        }
        if (isDefaultColor(target.lights.ambient_color, Math::Color(1.0, 1.0, 1.0))) {
            target.lights.ambient_color = source.lights.ambient_color;
        }
        if (isDefaultColor(target.sky.color1, Math::Color(1.0, 0.7, 0.2))) {
            target.sky.color1 = source.sky.color1;
        }
        if (isDefaultColor(target.sky.color2, Math::Color(1.0, 0.5, 0.1))) {
            target.sky.color2 = source.sky.color2;
        }
    }

    static std::vector<std::string> collectReferencePaths(const libconfig::Setting& setting)
    {
        std::vector<std::string> paths;

        if (setting.isScalar()) {
            paths.emplace_back(static_cast<const char*>(setting));
            return paths;
        }

        if (setting.isList()) {
            const int length = setting.getLength();
            for (int i = 0; i < length; ++i) {
                paths.emplace_back(static_cast<const char*>(setting[i]));
            }
        }

        return paths;
    }

    SceneConfig SceneParser::parse(const std::string& filename)
    {
        std::function<SceneConfig(const std::string&, std::set<std::string>&)> parseRec;
        parseRec = [&](const std::string& filePath, std::set<std::string>& visited) -> SceneConfig {
            const std::string normalizedPath = filePath;
            if (visited.find(normalizedPath) != visited.end()) {
                throw std::runtime_error("Recursive include detected: " + normalizedPath);
            }
            visited.insert(normalizedPath);

            libconfig::Config cfg;
            try {
                cfg.readFile(normalizedPath.c_str());
            } catch (const libconfig::FileIOException&) {
                throw std::runtime_error("Impossible d'ouvrir le fichier : " + normalizedPath);
            } catch (const libconfig::ParseException& pex) {
                throw std::runtime_error("Erreur de parsing " + std::string(pex.getFile()) + ":" +
                                         std::to_string(pex.getLine()) + " : " + pex.getError());
            }

            SceneConfig scene;

            if (cfg.exists("include") || cfg.exists("import")) {
                if (cfg.exists("include")) {
                    const libconfig::Setting& includes = cfg.lookup("include");
                    for (const std::string& includePath : collectReferencePaths(includes)) {
                        SceneConfig included = parseRec(resolveRelativePath(normalizedPath, includePath), visited);
                        mergeScene(scene, included);
                    }
                }

                if (cfg.exists("import")) {
                    const libconfig::Setting& imports = cfg.lookup("import");
                    for (const std::string& importPath : collectReferencePaths(imports)) {
                        SceneConfig imported = parseRec(resolveRelativePath(normalizedPath, importPath), visited);
                        mergeScene(scene, imported);
                    }
                }
            }

            if (cfg.exists("camera")) {
                const libconfig::Setting& camera = cfg.lookup("camera");
                scene.camera.width = readInt(camera, "width", scene.camera.width);
                scene.camera.height = readInt(camera, "height", scene.camera.height);
                scene.camera.fieldOfView = readDouble(camera, "fieldOfView", scene.camera.fieldOfView);
                scene.camera.samplesPerPixel = readSampleCount(camera, scene.camera.samplesPerPixel);

                if (camera.exists("position")) {
                    scene.camera.position = parsePoint(camera.lookup("position"));
                }
                if (camera.exists("rotation")) {
                    scene.camera.rotation = parseVector(camera.lookup("rotation"));
                }
            }

            if (cfg.exists("skybox")) {
                const libconfig::Setting& skybox = cfg.lookup("skybox");
                if (skybox.exists("filepath")) {
                    scene.sky.skyboxPath = static_cast<const char*>(skybox.lookup("filepath"));
                    scene.sky.hasSkybox = true;
                }
            }

            if (cfg.exists("spheres")) {
                const libconfig::Setting& spheres = cfg.lookup("spheres");
                const int count = spheres.getLength();
                for (int i = 0; i < count; ++i) {
                    const libconfig::Setting& sphere = spheres[i];
                    SphereConfig s;
                    s.position = parsePoint(sphere.lookup("position"));
                    s.radius = readDouble(sphere, "radius", 1.0);
                    s.color = parseColor(sphere.lookup("color"));
                    s.reflectivity = parseReflectivity(sphere, s.reflectivity);
                    scene.spheres.push_back(s);
                }
            }

            if (cfg.exists("cylinders")) {
                const libconfig::Setting& cylinders = cfg.lookup("cylinders");
                const int count = cylinders.getLength();
                for (int i = 0; i < count; ++i) {
                    const libconfig::Setting& cyl = cylinders[i];
                    CylinderConfig c;
                    c.position = parsePoint(cyl.lookup("position"));
                    c.axis = parseVector(cyl.lookup("axis"));
                    c.height = readDouble(cyl, "height", 2.0);
                    c.radius = readDouble(cyl, "radius", 1.0);
                    c.color = parseColor(cyl.lookup("color"));
                    scene.cylinders.push_back(c);
                }
            }

            if (cfg.exists("cones")) {
                const libconfig::Setting& cones = cfg.lookup("cones");
                const int count = cones.getLength();
                for (int i = 0; i < count; ++i) {
                    const libconfig::Setting& cn = cones[i];
                    ConeConfig c;
                    c.position = parsePoint(cn.lookup("position"));
                    c.axis = parseVector(cn.lookup("axis"));
                    c.angle = readDouble(cn, "angle", 15.0);
                    c.height = readDouble(cn, "height", 2.0);
                    c.color = parseColor(cn.lookup("color"));
                    scene.cones.push_back(c);
                }
            }

            if (cfg.exists("planes")) {
                const libconfig::Setting& planes = cfg.lookup("planes");
                const int count = planes.getLength();
                for (int i = 0; i < count; ++i) {
                    const libconfig::Setting& plane = planes[i];
                    PlaneConfig p;
                    if (plane.exists("axis")) {
                        p.usesAxis = true;
                        const char* axis = static_cast<const char*>(plane.lookup("axis"));
                        p.axis = axis[0];
                        p.position = readDouble(plane, "position", 0.0);
                    } else {
                        p.usesAxis = false;
                        if (plane.exists("point")) {
                            p.point = parsePoint(plane.lookup("point"));
                        }
                        if (plane.exists("normal")) {
                            p.normal = parseVector(plane.lookup("normal"));
                        }
                    }
                    p.color = parseColor(plane.lookup("color"));
                    if (plane.exists("color2")) {
                        p.color2 = parseColor(plane.lookup("color2"));
                        p.hasColor2 = true;
                    }
                    if (plane.exists("color3")) {
                        p.color3 = parseColor(plane.lookup("color3"));
                        p.hasColor3 = true;
                    }
                    if (plane.exists("pattern")) {
                        p.patternType = static_cast<const char*>(plane.lookup("pattern"));
                    }
                    p.patternSize = readDouble(plane, "patternSize", 2.0);
                    
                    if (plane.exists("texture")) {
                        std::string tex = static_cast<const char*>(plane.lookup("texture"));
                        if (tex == "reflection") {
                            p.reflectivity = 0.8;
                        }
                    }
                    p.reflectivity = parseReflectivity(plane, p.reflectivity);
                    scene.planes.push_back(p);
                }
            }

            if (cfg.exists("meshes")) {
                const libconfig::Setting& meshes = cfg.lookup("meshes");
                const int count = meshes.getLength();
                for (int i = 0; i < count; ++i) {
                    const libconfig::Setting& mesh = meshes[i];
                    MeshConfig m;
                    if (mesh.exists("path")) {
                        m.filePath = static_cast<const char*>(mesh.lookup("path"));
                    }
                    if (mesh.exists("position")) {
                        m.position = parsePoint(mesh.lookup("position"));
                    }
                    if (mesh.exists("rotation")) {
                        m.rotation = parseVector(mesh.lookup("rotation"));
                    }
                    m.scale = readDouble(mesh, "scale", 1.0);
                    if (mesh.exists("color")) {
                        m.color = parseColor(mesh.lookup("color"));
                    }
                    m.reflectivity = parseReflectivity(mesh, m.reflectivity);
                    scene.meshes.push_back(m);
                }
            }

            if (cfg.exists("lights")) {
                const libconfig::Setting& lights = cfg.lookup("lights");
                scene.lights.ambient = readDouble(lights, "ambient", scene.lights.ambient);
                scene.lights.diffuse = readDouble(lights, "diffuse", scene.lights.diffuse);
                if (lights.exists("ambient_color")) {
                    scene.lights.ambient_color = parseColor(lights.lookup("ambient_color"));
                }

                if (lights.exists("directional")) {
                    const libconfig::Setting& directionals = lights.lookup("directional");
                    const int count = directionals.getLength();
                    for (int i = 0; i < count; ++i) {
                        const libconfig::Setting& dir = directionals[i];
                        DirectionalLightConfig dlc;
                        dlc.direction = parseVector(dir.lookup("direction"));
                        dlc.intensity = readDouble(dir, "intensity", 0.7);
                        if (dir.exists("color")) {
                            dlc.color = parseColor(dir.lookup("color"));
                        }
                        scene.lights.additional_directional_lights.push_back(dlc);
                    }
                }

                if (lights.exists("point")) {
                    const libconfig::Setting& points = lights.lookup("point");
                    const int count = points.getLength();
                    for (int i = 0; i < count; ++i) {
                        const libconfig::Setting& pl = points[i];
                        PointLightConfig plc;
                        plc.position = parsePoint(pl.lookup("position"));
                        plc.intensity = readDouble(pl, "intensity", 0.7);
                        plc.radius = readDouble(pl, "radius", 100.0);
                        if (pl.exists("color")) {
                            plc.color = parseColor(pl.lookup("color"));
                        }
                        scene.lights.point_lights.push_back(plc);
                    }
                }
            }

            if (cfg.exists("sky")) {
                const libconfig::Setting& sky = cfg.lookup("sky");
                if (sky.exists("color1")) {
                    scene.sky.color1 = parseColor(sky.lookup("color1"));
                }
                if (sky.exists("color2")) {
                    scene.sky.color2 = parseColor(sky.lookup("color2"));
                }
            }

            return scene;
        };

        std::set<std::string> visited;
        return parseRec(filename, visited);
    }
}
