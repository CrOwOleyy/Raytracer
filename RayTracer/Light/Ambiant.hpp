/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Thu, Apr, 2026                                                     *
 * Title           - G-OOP-400-MAR-4-1-raytracer-2                                      *
 * Description     -                                                                    *
 *     Ambiant                                                                          *
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

#ifndef INCLUDED_AMBIANT_HPP
    #define INCLUDED_AMBIANT_HPP

#include <iostream>
#include "ILight.hpp"

namespace RayTracer
{
    class Ambiant : public ILight
    {
        private:
            double intensity;
            Math::Color color;
        protected:
        public:
            Ambiant(double intensity, const Math::Color& color = Math::Color(1.0, 1.0, 1.0));
            ~Ambiant() override = default;
            Math::Color get_light(const Math::Vector3D& normal) const override;
            Math::Vector3D get_direction(const Math::Point3D& point) const override;
            double get_distancemax (const Math::Point3D& point) const override;
    };
}

#endif