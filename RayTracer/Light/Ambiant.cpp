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

#include "Ambiant.hpp"

RayTracer::Ambiant::Ambiant(double intensity, const Math::Color& color) : intensity(intensity), color(color)
{
}

Math::Color RayTracer::Ambiant::get_light(const Math::Vector3D&) const
{
    return color * intensity;
}
//ici la lumiere est blanche caar 1 = blanc et 0=noit
//on multiplie par intensity pour regle la limiosité

Math::Vector3D RayTracer::Ambiant::get_direction(const Math::Point3D&) const
{
    return Math::Vector3D(0.0, 0.0, 0.0);
}

double RayTracer::Ambiant::get_distancemax(const Math::Point3D&) const
{
    return 0;
}