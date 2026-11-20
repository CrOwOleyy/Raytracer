#include "Math/Color.hpp"
#include "RayTracer/Camera.hpp"
#include "RayTracer/Light/Ambiant.hpp"
#include "RayTracer/Light/Directional.hpp"
#include "RayTracer/Light/PointLight.hpp"
#include "RayTracer/PrimitiveList.hpp"
#include "RayTracer/RaytracingEngine.hpp"
#include "RayTracer/SceneFactory.hpp"
#include "RayTracer/SceneParser.hpp"
#include "RayTracer/Sphere.hpp"
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <vector>


RayTracer::Ambiant *global_ambiant = nullptr;
std::vector<RayTracer::Directional *> global_directional_lights;
RayTracer::SkyConfig *global_sky = nullptr;

static void print_help() {
  std::cout
      << "USAGE: ./raytracer <SCENE_FILE>\n"
      << "  SCENE_FILE: fichier de configuration de la scène (libconfig++).\n"
      << "  --help: affiche cette aide\n";
}

Math::Color ray_color(const RayTracer::Ray &r,
                      const RayTracer::IPrimitive &world, int limite = 0) {
  if (limite > 5)
    return Math::Color(0, 0, 0); // noir si on depasse la limite de rebond pour
                                 // eviter les boucle infinie
  RayTracer::HitRecord rec;
  if (world.hits(r, 0, std::numeric_limits<double>::infinity(), rec)) {
    Math::Color ambient_result = global_ambiant->get_light(
        rec.perpendiculaire); // ici on calcule les lumiere en fonction du point
                              // d'impact de la perp (en gros cest la perp qui
                              // dis "oui lumiere" ou "non ombre")
    Math::Color directional_result(0, 0, 0);
    const double EPSILON = 0.001;

    for (const auto &light : global_directional_lights) {
      Math::Color light_contrib = light->get_light(rec.perpendiculaire);

      Math::Vector3D direction_to_light =
          light->get_direction(rec.frappe_rayon);
      Math::Point3D shadow_origin =
          rec.frappe_rayon + rec.perpendiculaire * EPSILON;
      RayTracer::Ray shadow_ray(shadow_origin, direction_to_light);
      double max_distance = light->get_distancemax(rec.frappe_rayon);
      RayTracer::HitRecord shadow_rec;

      if (world.hits(shadow_ray, EPSILON, max_distance, shadow_rec)) {
        light_contrib = Math::Color(0, 0, 0);
      }
      directional_result = directional_result + light_contrib;
    }
    Math::Color total_light = ambient_result + directional_result;
    Math::Color base_color = rec.albedo * total_light;

    Math::Vector3D reflected =
        r.direction -
        rec.perpendiculaire * 2.0 * (r.direction.dot(rec.perpendiculaire));
    Math::Point3D reflect_origin =
        rec.frappe_rayon + rec.perpendiculaire * EPSILON;
    RayTracer::Ray reflect_ray(reflect_origin, reflected);
    Math::Color reflected_color = ray_color(reflect_ray, world, limite + 1);

    const double reflectivity = rec.reflectivity;
    return base_color * (1.0 - reflectivity) + reflected_color * reflectivity;
  }
  Math::Vector3D unit_direction = Math::Vector3D::unit_vector(r.direction);
  auto t =
      0.5 *
      (unit_direction.y +
       1.0); // on converti la direction du rayon en intervalle entre 0 et 1
             // pour faire un mélange lineaire entre les deux couleurs du ciel
  return global_sky->color1 * (1.0 - t) + global_sky->color2 * t;
} // calcule lenear interpolation

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "USAGE: ./raytracer <SCENE_FILE>\n";
    return 84;
  }

  std::string arg = argv[1];
  if (arg == "--help") {
    print_help();
    return 0;
  }

  // lire le fichier de scène
  RayTracer::SceneConfig scene;
  try {
    scene = RayTracer::SceneParser::parse(arg);
  } catch (const std::exception &e) {
    std::cerr << "Erreur : " << e.what() << "\n";
    return 84;
  }

  // créer la scène avec la factory
  std::unique_ptr<RayTracer::Ambiant> ambient_light =
      RayTracer::SceneFactory::createAmbientLight(scene);
  std::vector<std::unique_ptr<RayTracer::Directional>> directional_lights =
      RayTracer::SceneFactory::createDirectionalLight(scene);
  std::vector<std::unique_ptr<RayTracer::PointLight>> point_lights =
      RayTracer::SceneFactory::createPointLight(scene);
  RayTracer::PrimitiveList world = RayTracer::SceneFactory::createWorld(scene);

  global_ambiant = ambient_light.get();
  for (auto &light : directional_lights) {
    global_directional_lights.push_back(light.get());
  }

  // Stocker la config du ciel globalement
  static RayTracer::SkyConfig sky_config = scene.sky;
  global_sky = &sky_config;

  // dimensions de l'image depuis la caméra
  const int image_width = scene.camera.width;
  const int image_height = scene.camera.height;

  // caméra
  RayTracer::Camera cam = RayTracer::SceneFactory::createCamera(scene);

  // créer le moteur de raytracing
  RayTracer::RaytracingEngine engine(cam, world, image_width, image_height,
                                     scene.camera.samplesPerPixel);
  engine.setAmbientLight(ambient_light.get());
  for (auto &light : directional_lights) {
    engine.addDirectionalLight(light.get());
  }
  for (auto &light : point_lights) {
    engine.addPointLight(light.get());
  }
  engine.setSamplesPerPixel(scene.camera.samplesPerPixel);
  engine.setSky(scene.sky);
  engine.setDiffuseCoeff(scene.lights.diffuse);

  // rendu au format PPM
  std::ostream &output_file = std::cout;

  output_file << "P3\n" << image_width << " " << image_height << "\n255\n";

  std::vector<Math::Color> image_buffer(image_width * image_height);
  unsigned int num_threads = std::thread::hardware_concurrency();
  if (num_threads == 0) num_threads = 4;

  std::vector<std::thread> workers;
  std::atomic<int> scanlines_done(0);

  for (unsigned int t = 0; t < num_threads; ++t) {
    workers.emplace_back([&, t]() {
      for (int j = image_height - 1 - t; j >= 0; j -= num_threads) {
        for (int i = 0; i < image_width; ++i) {
          image_buffer[j * image_width + i] = engine.computePixelColor(i, j);
        }
        scanlines_done++;
        if (t == 0) {
          std::cerr << "\rRendu en cours... " << (scanlines_done * 100 / image_height) << "%" << std::flush;
        }
      }
    });
  }

  for (auto &worker : workers) {
    worker.join();
  }
  std::cerr << "\rRendu termine !             \n";

  for (int j = image_height - 1; j >= 0; --j) {
    for (int i = 0; i < image_width; ++i) {
      Math::write_color(output_file, image_buffer[j * image_width + i]);
    }
  }

  std::cerr << "\nDone.\n";
  return 0;
}