/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Thu, May, 2026                                                     *
 * Title           - G-OOP-400-MAR-4-1-raytracer-2                                      *
 * Description     -                                                                    *
 *     PointLight                                                                       *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *                                                                                      *
 *             ███████╗██████╗ ██╗████████╗███████╗ ██████╗██╗  ██╗                     *
 *             ██╔════╝██╔══██╗██║╚══██╔══╝██╔════╝██╔════╝██║  ██║                     *
 *             █████╗  ██████╔╝██║   ██║   █████╗  ██║     ███████║                     *
 *             ██╔══╝  ██╔═══╝ ██║   ██║   ██╔══╝  ██║     ██╔══██║                     *
 *             ███████╗██║     ██║   ██║   ███████╗╚██████╗██║  ██║                     *
 *             ╚══════╝╚═╝     ╚═╝   ╚═╝   ╚══════╝ ╚═════╝╚═╝  ╚═╝                     *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */


#ifndef INCLUDED_POINTLIGHT_HPP
    #define INCLUDED_POINTLIGHT_HPP

#include <iostream>
#include "ILight.hpp"

namespace RayTracer
{
    class PointLight : public ILight
    {
        private:
            Math::Point3D position;
            double intensity;
            double radius;
            Math::Color color;
        public:
            PointLight(const Math::Point3D& position, double intensity, double radius = 100.0, const Math::Color& color = Math::Color(1.0, 1.0, 1.0));
            ~PointLight() override = default;
            Math::Color get_light(const Math::Vector3D& normal) const override;
            Math::Vector3D get_direction(const Math::Point3D& point) const override;
            double get_distancemax(const Math::Point3D& point) const override;
            Math::Point3D get_position() const { return position; }
            Math::Color get_color() const { return color; }
            double get_intensity() const { return intensity; }
    };
}

#endif
