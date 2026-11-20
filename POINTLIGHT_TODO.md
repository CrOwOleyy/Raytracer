# 💡 Implementation Guide - Point Light

## 🎯 Objectif
Implémenter une lumière ponctuelle (Point Light) qui émet dans toutes les directions avec atténuation.

**Points de bonus:** 0.5 pour Point Light simple + 0.5 pour Multiple Point Lights = 1.0 pt total

---

## 📝 Specification

### Point Light Caractéristiques
```cpp
struct PointLightConfig
{
    Math::Point3D position;      // où se trouve la lumière
    double intensity;            // luminosité (0-1)
    double radius = 100.0;       // distance max avant atténuation = 0
    Math::Color color = {1.0, 1.0, 1.0};  // blanc par défaut
};
```

### Formule Atténuation
```
distance = ||point - light_position||
attenuation = max(0, 1 - (distance / radius)²)
light_color = color * intensity * attenuation * dot(normal, direction)
```

---

## 📂 Fichiers à Créer

### 1. `RayTracer/Light/PointLight.hpp`
```cpp
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
            PointLight(const Math::Point3D& pos, double intensity, double radius = 100.0, const Math::Color& color = Math::Color(1.0, 1.0, 1.0));
            ~PointLight() override = default;
            Math::Color get_light(const Math::Vector3D& normal) const override;
            Math::Vector3D get_direction(const Math::Point3D& point) const override;
            double get_distancemax(const Math::Point3D& point) const override;
    };
}

#endif
```

### 2. `RayTracer/Light/PointLight.cpp`
```cpp
#include "PointLight.hpp"
#include <cmath>

RayTracer::PointLight::PointLight(const Math::Point3D& pos, double intensity, double radius, const Math::Color& color)
    : position(pos), intensity(intensity), radius(radius), color(color)
{
}

Math::Color RayTracer::PointLight::get_light(const Math::Vector3D& normal) const
{
    double diffuse = std::max(0.0, normal.dot(direction));
    
    // Attenuation: 1 - (distance / radius)^2
    double distance = ...;  // À calculer
    double attenuation = std::max(0.0, 1.0 - (distance / radius) * (distance / radius));
    
    return color * (diffuse * intensity * attenuation);
}

Math::Vector3D RayTracer::PointLight::get_direction(const Math::Point3D& point) const
{
    return Math::Vector3D::unit_vector(position - point);
}

double RayTracer::PointLight::get_distancemax(const Math::Point3D& point) const
{
    return radius;
}
```

---

## 📝 Modifications à Fichiers Existants

### 1. Mise à jour `RayTracer/SceneParser.hpp`

```cpp
struct PointLightConfig
{
    Math::Point3D position;
    double intensity;
    double radius = 100.0;
    Math::Color color = {1.0, 1.0, 1.0};
};

struct LightConfig
{
    double ambient = 0.3;
    Math::Color ambient_color = {1.0, 1.0, 1.0};
    std::vector<DirectionalLightConfig> additional_directional_lights;
    std::vector<PointLightConfig> point_lights;  // ADD THIS
};
```

### 2. Mise à jour `RayTracer/SceneParser.cpp`

Dans `SceneParser::parse()`, ajouter après les directional lights :
```cpp
if (lights.exists("point")) {
    const libconfig::Setting& points = lights.lookup("point");
    for (int i = 0; i < points.getLength(); ++i) {
        PointLightConfig plc;
        plc.position = parsePoint3D(points[i].lookup("position"));
        plc.intensity = readDouble(points[i], "intensity", 0.7);
        plc.radius = readDouble(points[i], "radius", 100.0);
        if (points[i].exists("color"))
            plc.color = parseColor(points[i].lookup("color"));
        scene.lights.point_lights.push_back(plc);
    }
}
```

### 3. Mise à jour `RayTracer/SceneFactory.hpp`

```cpp
std::vector<std::unique_ptr<PointLight>> createPointLights(const SceneConfig& scene);
```

### 4. Mise à jour `RayTracer/SceneFactory.cpp`

```cpp
std::vector<std::unique_ptr<PointLight>> SceneFactory::createPointLights(const SceneConfig& scene)
{
    std::vector<std::unique_ptr<PointLight>> lights;
    for (const auto& plc : scene.lights.point_lights) {
        lights.push_back(std::make_unique<PointLight>(plc.position, plc.intensity, plc.radius, plc.color));
    }
    return lights;
}
```

### 5. Mise à jour `RayTracer/RaytracingEngine.hpp`

```cpp
#include "Light/PointLight.hpp"

class RaytracingEngine
{
    // ... existing code ...
    
    void addPointLight(PointLight* light);
    
private:
    // ... existing code ...
    std::vector<PointLight*> m_point_lights;
};
```

### 6. Mise à jour `RayTracer/RaytracingEngine.cpp`

```cpp
void RaytracingEngine::addPointLight(PointLight* light)
{
    m_point_lights.push_back(light);
}

// Dans ray_color(), après la boucle directional lights:
for (const auto& light : m_point_lights) {
    Math::Color light_contrib = light->get_light(rec.perpendiculaire);
    
    Math::Vector3D direction_to_light = light->get_direction(rec.frappe_rayon);
    Math::Point3D shadow_origin = rec.frappe_rayon + rec.perpendiculaire * EPSILON;
    Ray shadow_ray(shadow_origin, direction_to_light);
    double max_distance = light->get_distancemax(rec.frappe_rayon);
    HitRecord shadow_rec;
    
    if (m_world.hits(shadow_ray, EPSILON, max_distance, shadow_rec)) {
        light_contrib = light_contrib * 0.1;
    }
    directional_result = directional_result + light_contrib;
}
```

### 7. Mise à jour `main.cpp`

```cpp
#include "RayTracer/Light/PointLight.hpp"

// Dans main():
std::vector<std::unique_ptr<RayTracer::PointLight>> point_lights = RayTracer::SceneFactory::createPointLights(scene);
engine.addPointLight(point_lights.get());  // À adapter selon votre pattern

for (auto& light : point_lights) {
    engine.addPointLight(light.get());
}
```

---

## 📋 Exemple Configuration

### `demo_point_light.cfg`
```libconfig
camera = {
  width = 400;
  height = 400;
  position = { x = 0; y = 1; z = 2; };
  rotation = { x = 0; y = 0; z = 0; };
  fieldOfView = 72.0;
};

spheres = (
  { position = { x = 0; y = 0; z = -1; }; radius = 0.5; color = { r = 200; g = 100; b = 100; }; },
  { position = { x = 0; y = -100.5; z = -1; }; radius = 100; color = { r = 100; g = 100; b = 100; }; }
);

planes = ();

lights = {
  ambient = 0.05;
  directional = ();
  point = (
    { position = { x = 2; y = 1; z = 1; }; intensity = 1.0; radius = 20.0; color = { r = 255; g = 255; b = 255; }; },
    { position = { x = -2; y = 1; z = 1; }; intensity = 0.8; radius = 20.0; color = { r = 255; g = 0; b = 0; }; }
  );
};

sky = {
  color1 = { r = 50; g = 50; b = 50; };
  color2 = { r = 100; g = 100; b = 100; };
};
```

---

## ✅ Checklist Implémentation

- [ ] Créer `PointLight.hpp`
- [ ] Créer `PointLight.cpp`
- [ ] Mettre à jour `SceneParser.hpp` - ajouter struct
- [ ] Mettre à jour `SceneParser.cpp` - parser point lights depuis config
- [ ] Mettre à jour `SceneFactory.hpp` - déclarer fonction
- [ ] Mettre à jour `SceneFactory.cpp` - implémenter fonction
- [ ] Mettre à jour `RaytracingEngine.hpp` - ajouter member + method
- [ ] Mettre à jour `RaytracingEngine.cpp` - implémenter addPointLight() et appel dans ray_color()
- [ ] Mettre à jour `main.cpp` - créer et assigner point lights
- [ ] Mettre à jour `Makefile` - compiler new .cpp
- [ ] Créer `demo_point_light.cfg` de test
- [ ] Tester et vérifier rendu

---

## 🎨 Résultat Attendu

Les Point Lights devraient :
- Émettre depuis leur position
- S'affaiblir avec la distance (atténuation)
- Supporter plusieurs instances simultanément
- Supporter les ombres (shadow casting)
- Supporter colorage
- Combiner correctement avec ambient et directional lights

**Avant/Après :** Avant = surfaces plates. Après = éclairage localisé avec falloff réaliste.
