#include "OBJParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace RayTracer
{
    std::vector<std::shared_ptr<IPrimitive>> OBJParser::parse(const std::string& filePath, const Math::Color& color, double reflectivity, const Math::Point3D& position, const Math::Vector3D& rotation, double scale)
    {
        std::vector<std::shared_ptr<IPrimitive>> triangles;
        std::vector<Math::Point3D> vertices;
        std::ifstream file(filePath);

        if (!file.is_open()) {
            std::cerr << "Warning: Could not open OBJ file: " << filePath << std::endl;
            return triangles;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string type;
            iss >> type;

            if (type == "v") {
                double x, y, z;
                iss >> x >> y >> z;
                
                // Apply transformations
                Math::Vector3D v(x, y, z);
                v = v * scale;
                v = Math::Vector3D::rotateXYZ(v, rotation);
                
                vertices.push_back(Math::Point3D(v.x + position.x, v.y + position.y, v.z + position.z));
            } else if (type == "f") {
                std::vector<int> faceIndices;
                std::string vertexData;
                while (iss >> vertexData) {
                    std::istringstream viss(vertexData);
                    std::string indexStr;
                    std::getline(viss, indexStr, '/');
                    if (!indexStr.empty()) {
                        faceIndices.push_back(std::stoi(indexStr));
                    }
                }

                // Triangulate if more than 3 vertices (e.g., quads)
                for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
                    int i0 = faceIndices[0];
                    int i1 = faceIndices[i];
                    int i2 = faceIndices[i + 1];

                    // OBJ indices are 1-based. Also support negative indices if needed, but let's stick to positive for now.
                    if (i0 > 0 && i1 > 0 && i2 > 0 &&
                        static_cast<size_t>(i0) <= vertices.size() &&
                        static_cast<size_t>(i1) <= vertices.size() &&
                        static_cast<size_t>(i2) <= vertices.size()) {
                        triangles.push_back(std::make_shared<Triangle>(
                            vertices[i0 - 1],
                            vertices[i1 - 1],
                            vertices[i2 - 1],
                            color,
                            reflectivity
                        ));
                    }
                }
            }
        }

        std::cerr << "Loaded " << triangles.size() << " triangles from " << filePath << std::endl;
        return triangles;
    }
}
