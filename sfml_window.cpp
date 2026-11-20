#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <vector>
#include <string>
#include "RayTracer/Camera.hpp"
#include "RayTracer/PrimitiveList.hpp"
#include "RayTracer/SceneParser.hpp"
#include "RayTracer/SceneFactory.hpp"
#include "RayTracer/RaytracingEngine.hpp"

namespace fs = std::filesystem;

enum AppState {
    STATE_MENU,
    STATE_RENDERING
};

int main(int argc, char* argv[])
{
    // Ignorer les arguments en entrée
    (void)argc;
    (void)argv;

    // Charger les scènes disponibles dans le dossier scenes
    std::vector<std::string> scene_paths;
    try {
        for (const auto& entry : fs::directory_iterator("scenes")) {
            if (entry.is_regular_file() && entry.path().extension() == ".cfg") {
                scene_paths.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Erreur lors de la lecture du dossier scenes : " << e.what() << std::endl;
    }
    std::sort(scene_paths.begin(), scene_paths.end());

    // Charger la police de caractères avec gestion robuste des chemins système Linux
    sf::Font font;
    std::vector<std::string> font_paths = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "DejaVuSans.ttf",
        "Arial.ttf"
    };
    bool font_loaded = false;
    for (const auto& path : font_paths) {
        if (font.loadFromFile(path)) {
            font_loaded = true;
            break;
        }
    }

    // Créer la fenêtre SFML initiale en 800x600 avec style propre (non redimensionnable manuellement)
    sf::RenderWindow window(sf::VideoMode(800, 600), "Raytracer - Scene Selector", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    AppState current_state = STATE_MENU;
    int scroll_offset = 0;
    const int items_per_page = 16; // 8 lignes x 2 colonnes
    const int row_height = 55;

    // Gestionnaires de mémoire pour la scène active
    std::unique_ptr<RayTracer::Ambiant> ambient_light;
    std::vector<std::unique_ptr<RayTracer::Directional>> directional_lights;
    std::vector<std::unique_ptr<RayTracer::PointLight>> point_lights;
    RayTracer::PrimitiveList world;
    std::unique_ptr<RayTracer::RaytracingEngine> engine;

    std::vector<unsigned char> pixel_buffer;
    sf::Texture texture;
    sf::Sprite sprite;

    std::thread render_thread;
    std::atomic<bool> stop_rendering(false);
    std::atomic<bool> rendering_finished(false);
    std::atomic<int> completed_tiles(0);
    std::atomic<int> total_tiles(1);

    std::string current_scene_name = "";
    int image_width = 800;
    int image_height = 600;

    auto stop_active_render = [&]() {
        stop_rendering = true;
        if (render_thread.joinable()) {
            render_thread.join();
        }
    };

    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                stop_active_render();
                window.close();
            }

            if (current_state == STATE_MENU) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mouse_pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    int start_idx = scroll_offset * 2;
                    int end_idx = std::min(static_cast<int>(scene_paths.size()), start_idx + items_per_page);

                    for (int i = start_idx; i < end_idx; ++i) {
                        int col = (i - start_idx) % 2;
                        int row = (i - start_idx) / 2;
                        float x = 60.f + col * 360.f;
                        float y = 115.f + row * row_height;

                        if (mouse_pos.x >= x && mouse_pos.x <= x + 320.f && mouse_pos.y >= y && mouse_pos.y <= y + 45.f) {
                            try {
                                stop_active_render();
                                std::string filepath = scene_paths[i];
                                RayTracer::SceneConfig scene = RayTracer::SceneParser::parse(filepath);
                                
                                ambient_light = RayTracer::SceneFactory::createAmbientLight(scene);
                                directional_lights = RayTracer::SceneFactory::createDirectionalLight(scene);
                                point_lights = RayTracer::SceneFactory::createPointLight(scene);
                                world = RayTracer::SceneFactory::createWorld(scene);
                                image_width = scene.camera.width;
                                image_height = scene.camera.height;

                                RayTracer::Camera cam = RayTracer::SceneFactory::createCamera(scene);
                                engine = std::make_unique<RayTracer::RaytracingEngine>(cam, world, image_width, image_height, scene.camera.samplesPerPixel);
                                engine->setAmbientLight(ambient_light.get());
                                for (auto& l : directional_lights) engine->addDirectionalLight(l.get());
                                for (auto& l : point_lights) engine->addPointLight(l.get());
                                engine->setSamplesPerPixel(scene.camera.samplesPerPixel);
                                engine->setSky(scene.sky);
                                engine->setDiffuseCoeff(scene.lights.diffuse);

                                window.setSize(sf::Vector2u(image_width, image_height + 60));
                                window.setView(sf::View(sf::FloatRect(0.f, 0.f, (float)image_width, (float)image_height + 60)));
                                texture.create(image_width, image_height);
                                sprite.setTexture(texture, true);
                                
                                pixel_buffer.assign(image_width * image_height * 4, 0);
                                for (int idx = 3; idx < (int)pixel_buffer.size(); idx += 4) pixel_buffer[idx] = 255;

                                rendering_finished = false;
                                stop_rendering = false;
                                completed_tiles = 0;
                                total_tiles = ((image_width + 15) / 16) * ((image_height + 15) / 16);

                                std::string filename = fs::path(filepath).filename().string();
                                if (filename.length() > 4 && filename.substr(filename.length() - 4) == ".cfg")
                                    filename = filename.substr(0, filename.length() - 4);
                                current_scene_name = filename;
                                current_state = STATE_RENDERING;

                                render_thread = std::thread([&, image_width, image_height]() {
                                    const int tileSize = 16;
                                    unsigned int num_threads = std::thread::hardware_concurrency();
                                    if (num_threads == 0) num_threads = 4;
                                    std::atomic<int> next_tile_index(0);
                                    std::vector<std::thread> workers;
                                    for (unsigned int t = 0; t < num_threads; ++t) {
                                        workers.emplace_back([&, tiles_x = (image_width + tileSize - 1) / tileSize]() {
                                            while (true) {
                                                if (stop_rendering) return;
                                                int tile_idx = next_tile_index++;
                                                if (tile_idx >= total_tiles) break;
                                                int tx = tile_idx % tiles_x;
                                                int ty = tile_idx / tiles_x;
                                                for (int y_tile = ty * tileSize; y_tile < std::min((ty + 1) * tileSize, image_height); ++y_tile) {
                                                    for (int x_tile = tx * tileSize; x_tile < std::min((tx + 1) * tileSize, image_width); ++x_tile) {
                                                        if (stop_rendering) return;
                                                        Math::Color p_col = engine->computePixelColor(x_tile, image_height - 1 - y_tile);
                                                        int p_idx = (y_tile * image_width + x_tile) * 4;
                                                        double r = std::clamp(p_col.x, 0.0, 0.999);
                                                        double g = std::clamp(p_col.y, 0.0, 0.999);
                                                        double b = std::clamp(p_col.z, 0.0, 0.999);
                                                        pixel_buffer[p_idx + 0] = (unsigned char)(255.999 * std::sqrt(r));
                                                        pixel_buffer[p_idx + 1] = (unsigned char)(255.999 * std::sqrt(g));
                                                        pixel_buffer[p_idx + 2] = (unsigned char)(255.999 * std::sqrt(b));
                                                    }
                                                }
                                                completed_tiles++;
                                            }
                                        });
                                    }
                                    for (auto& worker : workers) worker.join();
                                    if (!stop_rendering) rendering_finished = true;
                                });
                                break;
                            } catch (const std::exception& e) {
                                std::cerr << "Erreur de chargement : " << e.what() << std::endl;
                            }
                        }
                    }
                }
                // Défilement des scènes avec la molette de la souris
                if (event.type == sf::Event::MouseWheelScrolled) {
                    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                        int delta = -static_cast<int>(event.mouseWheelScroll.delta);
                        scroll_offset += delta;
                        int total_rows = (scene_paths.size() + 1) / 2;
                        if (scroll_offset < 0) scroll_offset = 0;
                        if (scroll_offset > std::max(0, total_rows - 8)) {
                            scroll_offset = std::max(0, total_rows - 8);
                        }
                    }
                }
                // Défilement des scènes avec les flèches du clavier
                if (event.type == sf::Event::KeyPressed) {
                    int total_rows = (scene_paths.size() + 1) / 2;
                    if (event.key.code == sf::Keyboard::Up) {
                        scroll_offset = std::max(0, scroll_offset - 1);
                    } else if (event.key.code == sf::Keyboard::Down) {
                        scroll_offset = std::min(std::max(0, total_rows - 8), scroll_offset + 1);
                    }
                }
            } else if (current_state == STATE_RENDERING) {
                // Intercepter le clic sur le bouton Retour
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
                    
                    // Coordonnées du bouton Retour : x=[15, 135], y=[hauteur + 12, hauteur + 48]
                    if (mouse_pos.x >= 15 && mouse_pos.x <= 135 &&
                        mouse_pos.y >= image_height + 12 && mouse_pos.y <= image_height + 48) {
                        
                        stop_active_render();
                        
                        // Rétablir les dimensions de la fenêtre pour le menu de sélection
                        window.setSize(sf::Vector2u(800, 600));
                        window.setView(sf::View(sf::FloatRect(0.f, 0.f, 800.f, 600.f)));
                        current_state = STATE_MENU;
                    }
                }
            }
        }

        // Fond d'écran sombre premium
        window.clear(sf::Color(18, 18, 24));

        if (current_state == STATE_MENU) {
            // Dessiner le Header et Titre
            if (font_loaded) {
                sf::Text title("RAYTRACER", font, 36);
                title.setFillColor(sf::Color::White);
                title.setStyle(sf::Text::Bold);
                title.setPosition(60.f, 25.f);
                window.draw(title);

                sf::Text subtitle("Selectionnez une scene a charger :", font, 16);
                subtitle.setFillColor(sf::Color(180, 180, 180));
                subtitle.setPosition(60.f, 75.f);
                window.draw(subtitle);

                // Indication de navigation
                if (scene_paths.size() > 16) {
                    sf::Text hint("[Utilisez la molette de la souris ou les flèches pour faire défiler]", font, 12);
                    hint.setFillColor(sf::Color(100, 100, 120));
                    hint.setPosition(400.f - hint.getGlobalBounds().width / 2.f, 565.f);
                    window.draw(hint);
                }
            }

            // Dessiner la grille de fichiers de scènes
            sf::Vector2f mouse_pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            int start_idx = scroll_offset * 2;
            int end_idx = std::min(static_cast<int>(scene_paths.size()), start_idx + items_per_page);

            for (int i = start_idx; i < end_idx; ++i) {
                int col = (i - start_idx) % 2;
                int row = (i - start_idx) / 2;

                float x = 60.f + col * 360.f;
                float y = 115.f + row * row_height;

                sf::RectangleShape card(sf::Vector2f(320.f, 45.f));
                card.setPosition(x, y);
                card.setFillColor(sf::Color(30, 30, 40));
                card.setOutlineColor(sf::Color(60, 60, 80));
                card.setOutlineThickness(1.f);

                // Extraire le nom de fichier sans dossier et sans extension
                std::string filepath = scene_paths[i];
                std::string filename = fs::path(filepath).filename().string();
                if (filename.length() > 4 && filename.substr(filename.length() - 4) == ".cfg") {
                    filename = filename.substr(0, filename.length() - 4);
                }

                sf::Text name(filename, font, 15);
                name.setFillColor(sf::Color::White);
                name.setPosition(x + 15.f, y + 12.f);

                // Détecter le survol (Hover effect)
                bool is_hovered = mouse_pos.x >= x && mouse_pos.x <= x + 320.f &&
                                  mouse_pos.y >= y && mouse_pos.y <= y + 45.f;

                if (is_hovered) {
                    card.setFillColor(sf::Color(45, 45, 60));
                    card.setOutlineColor(sf::Color(0, 180, 255));
                    name.setFillColor(sf::Color(0, 180, 255));
                }

                window.draw(card);
                if (font_loaded) {
                    window.draw(name);
                }
            }
        } else if (current_state == STATE_RENDERING) {
            // Mettre à jour la texture dynamique avec le buffer partagé par le thread
            texture.update(pixel_buffer.data());
            window.draw(sprite);

            // Dessiner le bandeau de contrôle inférieur
            sf::RectangleShape bottom_bar(sf::Vector2f(static_cast<float>(image_width), 60.f));
            bottom_bar.setPosition(0.f, static_cast<float>(image_height));
            bottom_bar.setFillColor(sf::Color(25, 25, 30));
            bottom_bar.setOutlineColor(sf::Color(45, 45, 55));
            bottom_bar.setOutlineThickness(1.f);
            window.draw(bottom_bar);

            // Dessiner le bouton Retour
            sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
            bool is_back_hovered = mouse_pos.x >= 15 && mouse_pos.x <= 135 &&
                                  mouse_pos.y >= image_height + 12 && mouse_pos.y <= image_height + 48;

            sf::RectangleShape back_btn(sf::Vector2f(120.f, 36.f));
            back_btn.setPosition(15.f, static_cast<float>(image_height + 12));
            back_btn.setFillColor(is_back_hovered ? sf::Color(100, 40, 40) : sf::Color(65, 30, 30));
            back_btn.setOutlineColor(is_back_hovered ? sf::Color(200, 80, 80) : sf::Color(110, 50, 50));
            back_btn.setOutlineThickness(1.f);
            window.draw(back_btn);

            if (font_loaded) {
                sf::Text back_txt("< Retour", font, 14);
                back_txt.setFillColor(sf::Color::White);
                sf::FloatRect text_bounds = back_txt.getLocalBounds();
                back_txt.setPosition(15.f + (120.f - text_bounds.width) / 2.f, static_cast<float>(image_height + 12) + (36.f - text_bounds.height) / 2.f - 3.f);
                window.draw(back_txt);
            }

            // Calculer le pourcentage d'avancement
            double progress = static_cast<double>(completed_tiles) / total_tiles;
            if (progress > 1.0) progress = 1.0;

            // Dessiner la barre de progression adaptative
            float progress_bar_start_x = 150.f;
            float progress_bar_end_x = static_cast<float>(image_width - 180);
            float progress_bar_width = std::max(50.f, progress_bar_end_x - progress_bar_start_x);

            sf::RectangleShape prog_bg(sf::Vector2f(progress_bar_width, 10.f));
            prog_bg.setPosition(progress_bar_start_x, static_cast<float>(image_height + 25));
            prog_bg.setFillColor(sf::Color(40, 40, 50));
            window.draw(prog_bg);

            sf::RectangleShape prog_fill(sf::Vector2f(progress_bar_width * static_cast<float>(progress), 10.f));
            prog_fill.setPosition(progress_bar_start_x, static_cast<float>(image_height + 25));
            prog_fill.setFillColor(sf::Color(0, 150, 255));
            window.draw(prog_fill);

            // Texte de progression ou état de fin
            if (font_loaded) {
                std::string status_str = "Rendu : " + std::to_string(static_cast<int>(progress * 100)) + "%";
                if (rendering_finished) {
                    status_str = "Termine !";
                }
                sf::Text status_txt(status_str, font, 14);
                status_txt.setFillColor(sf::Color::White);
                status_txt.setPosition(static_cast<float>(image_width - 160), static_cast<float>(image_height + 20));
                window.draw(status_txt);
            }
        }

        window.display();
    }

    return 0;
}