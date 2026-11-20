/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Thu, Apr, 2026                                                     *
 * Title           - G-OOP-400-MAR-4-1-raytracer-2                                      *
 * Description     -                                                                    *
 *     FlatColor                                                                        *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *                                                                                      *
 *         ░        ░       ░░        ░        ░        ░░      ░░  ░░░░  ░             *
 *         ▒  ▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒▒▒  ▒▒▒▒  ▒▒▒▒▒▒▒  ▒▒▒▒  ▒  ▒▒▒▒  ▒             *
 *         ▓      ▓▓▓       ▓▓▓▓▓  ▓▓▓▓▓▓▓  ▓▓▓▓      ▓▓▓  ▓▓▓▓▓▓▓        ▓             *
 *         █  ███████  ██████████  ███████  ████  ███████  ████  █  ████  █             *
 *         █        █  ███████        ████  ████        ██      ██  ████  █             *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */

#include "FlatColor.hpp"

RayTracer::FlatColor::FlatColor(const Math::Color& color, double reflectivity) : _color(color), _reflectivity(reflectivity)
{
}

RayTracer::FlatColor::FlatColor(double rouge, double vert, double bleu, double reflectivity) : _color(rouge, vert, bleu), _reflectivity(reflectivity)
{
}

Math::Color RayTracer::FlatColor::get_color() const
{
    return _color;
}

double RayTracer::FlatColor::get_reflectivity() const
{
    return _reflectivity;
}