/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Thu, Apr, 2026                                                     *
 * Title           - G-OOP-400-MAR-4-1-raytracer-2                                      *
 * Description     -                                                                    *
 *     Directional                                                                      *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *                                                                                      *
 *       _|_|_|_|  _|_|_|    _|_|_|  _|_|_|_|_|  _|_|_|_|    _|_|_|  _|    _|           *
 *       _|        _|    _|    _|        _|      _|        _|        _|    _|           *
 *       _|_|_|    _|_|_|      _|        _|      _|_|_|    _|        _|_|_|_|           *
 *       _|        _|          _|        _|      _|        _|        _|    _|           *
 *       _|_|_|_|  _|        _|_|_|      _|      _|_|_|_|    _|_|_|  _|    _|           *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */

#include "Directional.hpp"

RayTracer::Directional::Directional(const Math::Vector3D& direction, double intensity, const Math::Color& color) : direction(Math::Vector3D::unit_vector(direction)), intensity(intensity), color(color)
{
}

Math::Color RayTracer::Directional::get_light(const Math::Vector3D& normal) const
{
    double diffuse = std::max(0.0, normal.dot(direction));
    return color * (diffuse * intensity);
}
//on calcul la lumiere en fonction de l'angle
//dot ca calcule le "produit scalaire" (cosinus de l'angle entre les deux vecteurs) et ca nous donne une des trois valeur
//-0.5 c'est negatif donc pas de lumiere ------ 0.0 c'est gris -------- 0.5 c'est positif donc lumiere
//on utilise max pour eviter les valeurs negative donc obligatoirement de la couleur grise ou plus claire
//on multiplie par intensity pour regle la limiosité

Math::Vector3D RayTracer::Directional::get_direction(const Math::Point3D&) const
{
    return direction;
}

double RayTracer::Directional::get_distancemax(const Math::Point3D&) const
{
    return std::numeric_limits<double>::infinity();
}