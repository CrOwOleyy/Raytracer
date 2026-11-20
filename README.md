# Raytracer

Un moteur de rendu 3D par **Raytracing** ultra-performant et modulaire écrit en C++17, doté d'une interface graphique interactive moderne avec **SFML**. Il supporte le multi-threading progressif, des structures d'accélération avancées (BVH), et l'import de modèles 3D complexes (.obj).

---

## ✨ Fonctionnalités Majeures

### 🚀 Optimisation & Performances
- **Multi-threading intensif** : Découpage de l'image en tuiles (tiles) traitées en parallèle par tous les cœurs logiques de votre processeur (via `std::thread::hardware_concurrency`).
- **Rendu progressif temps réel** : La scène se dessine sous vos yeux de manière fluide et progressive directement dans l'interface SFML.
- **Accélération BVH (Bounding Volume Hierarchy)** : Regroupement spatial des objets en arbres binaires de boîtes englobantes AABB, réduisant la complexité des tests d'intersection de $O(N)$ à $O(\log N)$. Indispensable pour afficher des objets complexes.

### 🎨 Rendu & Effets Optiques
- **Anti-aliasing par Échantillonnage Adaptatif (Adaptive Sampling)** : Le moteur détecte automatiquement les zones à forts contrastes (contours) et y concentre les calculs de supersampling selon un seuil perceptuel de couleur, garantissant une image parfaitement lisse sans gaspillage de ressources.
- **Gestion des ombres portées (Shadows)** : Ombres dures et douces réalistes pour toutes les sources de lumière directionnelles et ponctuelles.
- **Matériaux & Réflections** : Réflectivité ajustable par objet (effet miroir réaliste par rebonds de rayons récursifs).
- **Ciel & Skybox** :
  - Dégradé de ciel procédural hautement personnalisable.
  - Chargement de textures d'environnement / Skybox à 360° depuis des images standards.

### 📐 Primitives Géométriques supportées
- **Sphères** et **Plans infinis**.
- **Cylindres** et **Cônes** (avec limitation de hauteur/coiffage).
- **Triangles** et **Rectangles 3D**.
- **Textures de plans procédurales avancées** :
  - Damiers (2 ou 3 couleurs personnalisables).
  - Rayures (stripes) et anneaux concentriques (rings).
  - Motifs animés / ondes complexes (*synthwave*, *gradient*, *wave*).
- **Import de modèles OBJ** : Chargeur de fichiers `.obj` robuste (`OBJParser`) pour intégrer des meshs 3D complexes (ex: Mario, arbres, cubes complexes).

### 🖥️ Interface Graphique Interactive (SFML)
- **Sélecteur de scènes dynamique** : Menu d'accueil premium listant automatiquement tous les fichiers `.cfg` du dossier `scenes/` avec défilement à la souris/clavier et effets de survol de cartes stylés.
- **Redimensionnement dynamique** : La fenêtre s'adapte automatiquement aux dimensions et au ratio de la caméra définis dans le fichier de configuration de la scène.
- **Suivi du Rendu** : Barre de progression fluide en temps réel avec pourcentage et indicateur d'état final.
- **Navigation intuitive** : Bouton de retour interactif permettant de quitter un rendu en cours pour charger instantanément une autre scène.

---

## 🛠️ Compilation

### 🐧 Linux / WSL (Ubuntu)

#### Dépendances requises
Installez les bibliothèques SFML et Libconfig++ :
```bash
sudo apt-get update
sudo apt-get install libsfml-dev libconfig++-dev g++ make cmake
```

#### Option 1 : Via Makefile (Recommandé)
```bash
make          # Compile les exécutables raytracer (PPM) et sfml_window
make re       # Recompile proprement l'ensemble du projet
make clean    # Supprime les fichiers objets temporaires
make fclean   # Supprime les objets et les exécutables
```

#### Option 2 : Via CMake
```bash
mkdir build && cd build
cmake ..
make
```

---

### 🏁 Windows

#### Option 1 : Via CMake et MSVC (Visual Studio 2022)
```bash
mkdir build
cd build
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

#### Option 2 : Via vcpkg (Gestionnaire de paquets)
```bash
vcpkg install sfml libconfig-pp
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=<chemin-vcpkg>/scripts/buildsystems/vcpkg.cmake ..
cmake --build . --config Release
```

---

## 🎮 Utilisation

### 1️⃣ Interface interactive SFML (Recommandé)
Pour lancer l'application avec l'interface graphique :
```bash
./sfml_window
```
*Le sélecteur de scènes s'ouvrira et vous permettra de naviguer parmi toutes les scènes de votre dossier `scenes/`.*

Vous pouvez également charger directement une scène spécifique au démarrage :
```bash
./sfml_window scenes/demo_all.cfg
```

### 2️⃣ Générateur d'images PPM (Standard)
Pour générer une image statique au format PPM standard (sans interface graphique) :
```bash
./raytracer scenes/demo_all.cfg > output.ppm
```

---

## 📁 Structure du Projet

```text
.
├── Math/                    # Vecteurs, points et utilitaires colorimétriques
│   ├── Point3D.hpp / .cpp   # Représentation des points dans l'espace 3D
│   ├── Vector3D.hpp / .cpp  # Algèbre vectorielle 3D de base
│   ├── Color.hpp            # Définition des couleurs et correction gamma
│   └── Random.hpp           # Générateur de nombres aléatoires thread-safe
│
├── RayTracer/               # Moteur de rendu principal
│   ├── Camera.hpp / .cpp    # Caméra avec viewport configurable
│   ├── Ray.hpp / .cpp       # Modélisation des rayons géométriques
│   ├── RaytracingEngine.hpp # Moteur central (Adaptive sampling, rendu, skybox)
│   ├── RaytracingEngine.cpp 
│   ├── IPrimitive.hpp       # Interface polymorphique pour les objets 3D
│   ├── PrimitiveList.hpp    # Liste conteneur d'objets
│   ├── SceneParser.hpp      # Analyseur syntaxique des fichiers de scène .cfg
│   ├── SceneFactory.hpp     # Fabrique d'objets, caméras et lumières
│   ├── OBJParser.hpp / .cpp # Parseur de fichiers géométriques 3D au format .obj
│   ├── BVHNode.hpp / .cpp   # Structure d'arbre BVH pour accélération spatiale
│   ├── AABB.hpp             # Boîte de délimitation alignée sur les axes
│   │
│   ├── Light/               # Sources de lumière
│   │   ├── ILight.hpp       # Interface de base des lumières
│   │   ├── Ambiant.hpp      # Lumière ambiante globale
│   │   ├── Directional.hpp  # Lumière directionnelle (type soleil)
│   │   └── PointLight.hpp   # Lumière ponctuelle avec atténuation quadratique
│   │
│   ├── Material/            # Définitions de matériaux
│   │   └── FlatColor.hpp    # Matériau de couleur uniforme
│   │
│   └── Primitives/          # Primitives géométriques
│       ├── Sphere.cpp       # Sphère
│       ├── Plane.cpp        # Plan avec textures procédurales
│       ├── Cylinder.cpp     # Cylindre
│       ├── Cone.cpp         # Cône coiffé
│       ├── Rectangle3D.cpp  # Rectangle plan orienté
│       └── Triangle.cpp     # Triangle
│
├── scenes/                  # Fichiers de configuration des scènes (.cfg, .obj)
├── main.cpp                 # Point d'entrée de l'application PPM en ligne de commande
├── sfml_window.cpp          # Point d'entrée du sélecteur interactif SFML
├── Makefile                 # Outil de compilation GNU Make (Linux/WSL)
├── CMakeLists.txt           # Fichier de build multiplateforme CMake
└── README.md                # Documentation du projet
```

---

## 🎨 Configuration d'une Scène (`.cfg`)

Les scènes sont rédigées en format structuré Libconfig. En voici un exemple type :

```ini
# Définition de la caméra
camera: {
    width = 800;
    height = 600;
    samplesPerPixel = 64;   # Nombre maximum d'échantillons pour l'anti-aliasing
    origin = { x = 0.0; y = 2.0; z = -5.0; };
    lookAt = { x = 0.0; y = 1.0; z = 0.0; };
    up = { x = 0.0; y = 1.0; z = 0.0; };
    fov = 60.0;
};

# Configuration du ciel
sky: {
    color1 = { x = 0.2; y = 0.3; z = 0.5; }; # Couleur du bas
    color2 = { x = 0.6; y = 0.7; z = 0.9; }; # Couleur du haut
    hasSkybox = false;
};

# Configuration globale des coefficients de lumière
lights: {
    diffuse = 0.8;
};

# Liste des primitives de la scène
primitives: (
    {
        type = "sphere";
        center = { x = 0.0; y = 1.0; z = 0.0; };
        radius = 1.0;
        color = { x = 0.8; y = 0.1; z = 0.1; };
        reflectivity = 0.3;
    },
    {
        type = "plane";
        point = { x = 0.0; y = 0.0; z = 0.0; };
        normal = { x = 0.0; y = 1.0; z = 0.0; };
        color = { x = 0.2; y = 0.2; z = 0.2; };
        color2 = { x = 0.8; y = 0.8; z = 0.8; };
        hasColor2 = true;
        patternType = "checkerboard";
        patternSize = 2.0;
        reflectivity = 0.1;
    }
);

# Sources lumineuses
ambientLight: {
    color = { x = 0.1; y = 0.1; z = 0.1; };
};

directionalLights: (
    {
        direction = { x = -1.0; y = -1.0; z = 1.0; };
        color = { x = 0.8; y = 0.8; z = 0.8; };
    }
);

pointLights: (
    {
        position = { x = 3.0; y = 5.0; z = -2.0; };
        color = { x = 1.0; y = 0.8; z = 0.6; };
        intensity = 1.5;
        radius = 10.0; # Distance d'atténuation maximale
    }
);
```

> [!NOTE!]
> Vous pouvez importer une scène parente dans une autre pour réutiliser sa base via la directive `import = "chemin/vers/parent.cfg";`. Le moteur fusionnera automatiquement les caméras, primitives et sources de lumière de manière intelligente.


---
_Note: the subject PDF, course archives and audio files were left out of this public copy._
