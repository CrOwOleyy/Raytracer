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


#include "PointLight.hpp"
#include <cmath>
#include <algorithm>

RayTracer::PointLight::PointLight(const Math::Point3D& position, double intensity, double radius, const Math::Color& color) : position(position), intensity(intensity), radius(radius), color(color)
{
}

Math::Color RayTracer::PointLight::get_light(const Math::Vector3D& normal) const
{
    double diffuse = std::max(0.0, normal.dot(Math::Vector3D(0, 0, 1)));
    return color * (diffuse * intensity);
}

Math::Vector3D RayTracer::PointLight::get_direction(const Math::Point3D& point) const
{
    return Math::Vector3D::unit_vector(position - point);
}

double RayTracer::PointLight::get_distancemax(const Math::Point3D&) const
{
    return radius;
}