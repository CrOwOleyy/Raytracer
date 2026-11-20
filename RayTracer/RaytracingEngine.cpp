#include "RaytracingEngine.hpp"
#include <limits>
#include <cmath>
#include "../Math/Random.hpp"

namespace RayTracer
{
    static double colorDistance(const Math::Color& a, const Math::Color& b)
    {
        return (a - b).length();
    }

    RaytracingEngine::RaytracingEngine(const Camera& camera, const IPrimitive& world, int width, int height, int samples)
        : m_camera(camera), m_world(world), m_width(width), m_height(height), m_samples_per_pixel(samples), m_ambient_light(nullptr), m_rng(std::random_device{}()), m_dist(0.0, 1.0)
    {
        // Map samples -> adaptive max depth (4^depth ~= samples)
        int s = std::max(1, m_samples_per_pixel);
        m_adaptive_max_depth = 0;
        while ((1 << (2 * m_adaptive_max_depth)) < s) // 4^d = (2^(2d))
            ++m_adaptive_max_depth;
        m_adaptive_threshold = 0.02; // perceptual threshold for color difference
    }

    void RaytracingEngine::setAmbientLight(Ambiant* ambient)
    {
        m_ambient_light = ambient;
    }

    void RaytracingEngine::addDirectionalLight(Directional* light)
    {
        m_directional_lights.push_back(light);
    }

    void RaytracingEngine::addPointLight(PointLight* light)
    {
        m_point_lights.push_back(light);
    }

    void RaytracingEngine::setSky(const SkyConfig& sky)
    {
        m_sky = sky;
        if (m_sky.hasSkybox && !m_sky.skyboxPath.empty()) {
            if (m_skybox_image.loadFromFile(m_sky.skyboxPath)) {
                m_has_skybox_image = true;
            } else {
                std::cerr << "Failed to load skybox image: " << m_sky.skyboxPath << std::endl;
                m_has_skybox_image = false;
            }
        } else {
            m_has_skybox_image = false;
        }
    }

    Math::Color RaytracingEngine::ray_color(const Ray& r, int depth)
    {
        const int MAX_DEPTH = 5;
        if (depth > MAX_DEPTH)
            return Math::Color(0, 0, 0);

        HitRecord rec;
        if (m_world.hits(r, 0, std::numeric_limits<double>::infinity(), rec)) {
            Math::Color ambient_result(0, 0, 0);
            
            if (m_ambient_light)
                ambient_result = m_ambient_light->get_light(rec.perpendiculaire);

            Math::Color directional_result(0, 0, 0);
            const double EPSILON = 0.001;

            for (const auto& light : m_directional_lights) {
                Math::Color light_contrib = light->get_light(rec.perpendiculaire);

                Math::Vector3D direction_to_light = light->get_direction(rec.frappe_rayon);
                Math::Point3D shadow_origin = rec.frappe_rayon + rec.perpendiculaire * EPSILON;
                Ray shadow_ray(shadow_origin, direction_to_light);
                double max_distance = light->get_distancemax(rec.frappe_rayon);
                HitRecord shadow_rec;

                if (m_world.hits(shadow_ray, EPSILON, max_distance, shadow_rec)) {
                    light_contrib = Math::Color(0, 0, 0);
                }
                directional_result = directional_result + light_contrib;
            }

            // Point light
            for (const auto& light : m_point_lights) {
                Math::Vector3D direction_to_light = light->get_direction(rec.frappe_rayon);
                Math::Point3D light_pos = light->get_position();
                double distance = (light_pos - rec.frappe_rayon).length(); //ici on calcule la distance entre le point d'impact et la position de la lumière pour pouvoir faire l'atténuation
                double radius = light->get_distancemax(rec.frappe_rayon); // ici on récupère le radius de la lumière pour faire l'atténuation
                // Calcul de attenuation: 1 - (distance/radius)^2 (si res = 0 alors noir, si =1 alors couleur)
                double attenuation = std::max(0.0, 1.0 - (distance / radius) * (distance / radius));
                if (attenuation <= 0.0)
                    continue;
                // Calcul de la lumière diffuse
                double diffuse = std::max(0.0, rec.perpendiculaire.dot(direction_to_light));
                Math::Color light_color = light->get_color();
                double intensity = light->get_intensity();
                Math::Color light_contrib = light_color * (diffuse * attenuation * intensity * m_diffuse_coeff);
                Math::Point3D shadow_origin = rec.frappe_rayon + rec.perpendiculaire * EPSILON;
                Ray shadow_ray(shadow_origin, direction_to_light);
                HitRecord shadow_rec;
                if (m_world.hits(shadow_ray, EPSILON, radius, shadow_rec)) {
                    light_contrib = Math::Color(0, 0, 0);
                }
                directional_result = directional_result + light_contrib;
            }

            Math::Color total_light = ambient_result + directional_result;
            Math::Color base_color = rec.albedo * total_light;
            
            Math::Vector3D reflected = r.direction - rec.perpendiculaire * 2.0 * (r.direction.dot(rec.perpendiculaire));
            Math::Point3D reflect_origin = rec.frappe_rayon + rec.perpendiculaire * EPSILON;
            Ray reflect_ray(reflect_origin, reflected);
            Math::Color reflected_color = ray_color(reflect_ray, depth + 1);

            const double reflectivity = rec.reflectivity;
            return base_color * (1.0 - reflectivity) + reflected_color * reflectivity;
        }

        Math::Vector3D unit_direction = Math::Vector3D::unit_vector(r.direction);
        if (m_has_skybox_image) {
            double u = 0.5 + std::atan2(unit_direction.z, unit_direction.x) / (2.0 * 3.14159265358979323846);
            double v = 0.5 - std::asin(unit_direction.y) / 3.14159265358979323846;
            sf::Vector2u size = m_skybox_image.getSize();
            int px = std::clamp(static_cast<int>(u * size.x), 0, static_cast<int>(size.x - 1));
            int py = std::clamp(static_cast<int>(v * size.y), 0, static_cast<int>(size.y - 1));
            sf::Color c = m_skybox_image.getPixel(px, py);
            return Math::Color(c.r / 255.0, c.g / 255.0, c.b / 255.0);
        } else {
            auto t = 0.5 * (unit_direction.y + 1.0);
            return m_sky.color1 * (1.0 - t) + m_sky.color2 * t;
        }
    }

    Math::Color RaytracingEngine::computePixelColor(int x, int y)
    {
        if (m_samples_per_pixel <= 1) {
            double u = double(x) / (m_width - 1);
            double v = double(y) / (m_height - 1);
            Ray r = m_camera.ray(u, v);
            return ray_color(r);
        }

        return adaptiveSample(static_cast<double>(x), static_cast<double>(x + 1),
                              static_cast<double>(y), static_cast<double>(y + 1), 0);
    }

    Math::Color RaytracingEngine::adaptiveSample(double x0, double x1, double y0, double y1, int depth)
    {
        auto sampleAt = [this](double x, double y) {
            double u = x / (m_width - 1);
            double v = y / (m_height - 1);
            return ray_color(m_camera.ray(u, v));
        };

        const double midX = (x0 + x1) * 0.5;
        const double midY = (y0 + y1) * 0.5;

        const Math::Color c00 = sampleAt(x0, y0);
        const Math::Color c10 = sampleAt(x1, y0);
        const Math::Color c01 = sampleAt(x0, y1);
        const Math::Color c11 = sampleAt(x1, y1);
        const Math::Color cMid = sampleAt(midX, midY);

        const Math::Color average = (c00 + c10 + c01 + c11 + cMid) / 5.0;

        if (depth >= m_adaptive_max_depth) {
            return average;
        }

        const double maxVariation = std::max(
            std::max(colorDistance(c00, cMid), colorDistance(c10, cMid)),
            std::max(colorDistance(c01, cMid), colorDistance(c11, cMid))
        );

        if (maxVariation <= m_adaptive_threshold) {
            return average;
        }

        const Math::Color topLeft = adaptiveSample(x0, midX, y0, midY, depth + 1);
        const Math::Color topRight = adaptiveSample(midX, x1, y0, midY, depth + 1);
        const Math::Color bottomLeft = adaptiveSample(x0, midX, midY, y1, depth + 1);
        const Math::Color bottomRight = adaptiveSample(midX, x1, midY, y1, depth + 1);

        return (topLeft + topRight + bottomLeft + bottomRight) / 4.0;
    }

    void RaytracingEngine::renderToBuffer(unsigned char* buffer, int width, int height)
    {
        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                Math::Color pixel_color = computePixelColor(i, height - 1 - j);
                
                double r = std::clamp(pixel_color.x, 0.0, 0.999);
                double g = std::clamp(pixel_color.y, 0.0, 0.999);
                double b = std::clamp(pixel_color.z, 0.0, 0.999);

                r = std::sqrt(r);
                g = std::sqrt(g);
                b = std::sqrt(b);

                int r_val = static_cast<int>(255.999 * r);
                int g_val = static_cast<int>(255.999 * g);
                int b_val = static_cast<int>(255.999 * b);

                int pixel_idx = (j * width + i) * 4;
                buffer[pixel_idx + 0] = static_cast<unsigned char>(r_val);
                buffer[pixel_idx + 1] = static_cast<unsigned char>(g_val);
                buffer[pixel_idx + 2] = static_cast<unsigned char>(b_val);
                buffer[pixel_idx + 3] = 255;
            }
        }
    }
}
