## Avancement actuel
- `make` et `make run` fonctionnent
- Le binaire `raytracer` existe
- `main.cpp` contient une scène codée en dur avec des sphères
- Le rendu PPM est généré en `P3`
- La primitive `Sphere` est implémentée
- La lecture de scène depuis un fichier et le parsing CLI ne sont pas encore présents
- `Plane`, lumière et matériaux manquent encore

### Personne 1 – Interface & configuration
- [ ] Implémenter `./raytracer --help`
- [ ] Supporter `./raytracer <SCENE_FILE>`
- [ ] Lire et parser un fichier de scène `scene.conf`
- [ ] Gérer les erreurs : message sur `stderr` + code de sortie `84`
- [ ] Ajouter un exemple de scène et documenter son format

### Personne 2 – Primitives & scène
- [x] `Sphere` existante
- [ ] Implémenter `Plane`
- [ ] Ajouter les translations pour primitives
- [ ] Permettre l’ajout de plusieurs objets dans la scène
- [ ] Construire la scène à partir du parser avec plusieurs objets

### Personne 3 – Lumière & matériaux
- [ ] Implémenter `Ambient light`
- [ ] Implémenter `Directional light`
- [ ] Ajouter un matériau `flat color` pour les objets
- [ ] Charger la couleur des objets depuis le fichier de scène
- [ ] Intégrer la lumière dans le calcul de couleur de `ray_color`

### Personne 4 – Rendu & sortie
- [x] Le rendu PPM initial est présent
- [ ] Vérifier et améliorer la sortie PPM si besoin
- [ ] Rendre le format de sortie cohérent avec le sujet
- [ ] Gérer la configuration de la caméra depuis le fichier de scène
- [ ] Documenter l’utilisation : compilation, exécution, format du fichier de scène

## Priorités à court terme
1. `./raytracer <SCENE_FILE>` + `--help`
2. `Plane` + `translation`
3. `Ambient light` + `Directional light`
4. `Flat color` material

## Bonus possibles après avoir validé le minimum
- `Cylinder`, `Cone`
- `Rotation`
- `Shadow`
- `Antialiasing`
- `Multithreading`
- `Scene graph`
- `OBJ file` import

## Notes importantes
- Le sujet PDF impose : `Sphere`, `Plane`, `Translation`, `Ambient light`, `Directional light`, `Flat color`, lecture de scène et sortie PPM.
- Les tâches doivent être attribuées à 4 personnes pour avancer en parallèle.
