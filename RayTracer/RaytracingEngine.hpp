#pragma once

#include "Camera.hpp"
#include "Ray.hpp"
#include "IPrimitive.hpp"
#include "PrimitiveList.hpp"
#include "Light/Ambiant.hpp"
#include "Light/Directional.hpp"
#include "Light/PointLight.hpp"
#include "../Math/Color.hpp"
#include "SceneParser.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <random>

namespace RayTracer
{
    class RaytracingEngine
    {
    public:
        RaytracingEngine(const Camera& camera, const IPrimitive& world, int width, int height, int samples);

        void setAmbientLight(Ambiant* ambient);
        void addDirectionalLight(Directional* light);
        void addPointLight(PointLight* light); // NOUVEAU : Déclaration de la méthode

        void setSamplesPerPixel(int samples) { m_samples_per_pixel = samples; }
        void setDiffuseCoeff(double d) { m_diffuse_coeff = d; }
        void setSky(const SkyConfig& sky);

        Math::Color computePixelColor(int x, int y);
        void renderToBuffer(unsigned char* buffer, int width, int height);
        
        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }

        Math::Color adaptiveSample(double x0, double x1, double y0, double y1, int depth);

    private:
        Camera m_camera;
        const IPrimitive& m_world;
        int m_width;
        int m_height;
        int m_samples_per_pixel = 1;
        Ambiant* m_ambient_light;
        std::vector<Directional*> m_directional_lights;
        std::vector<PointLight*> m_point_lights;

        std::mt19937 m_rng;
        std::uniform_real_distribution<double> m_dist;
        int m_adaptive_max_depth;
        double m_adaptive_threshold;
        double m_diffuse_coeff = 0.7;

        SkyConfig m_sky;
        sf::Image m_skybox_image;
        bool m_has_skybox_image = false;

        Math::Color ray_color(const Ray& r, int depth = 0);
    };
}