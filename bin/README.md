# Space Invaders MVC - Documentation du Projet

## Table des Matières

1. [Vue d'Ensemble](#vue-densemble)
2. [Architecture](#architecture)
3. [Installation](#installation)
4. [Compilation](#compilation)
5. [Utilisation](#utilisation)
6. [Structure du Projet](#structure-du-projet)
7. [Dépendances](#dépendances)
8. [Tests et Validation](#tests-et-validation)
9. [Contrôles](#contrôles)
10. [Développement](#développement)
11. [Licence](#licence)

---

## Vue d'Ensemble

**Space Invaders MVC** est une implémentation moderne du jeu d'arcade classique Space Invaders, développée en langage C (norme C11) selon l'architecture Modèle-Vue-Contrôleur (MVC). Le projet offre deux interfaces utilisateur distinctes :

- **Interface graphique SDL3** : Version complète avec graphismes, textures, polices TTF et support d'images
- **Interface texte ncurses** : Version terminal légère pour une compatibilité maximale

### Caractéristiques Principales

- Architecture MVC stricte avec séparation claire des responsabilités
- Double implémentation de vue (SDL3 et ncurses) partageant le même modèle
- Code modulaire et réutilisable
- Gestion robuste de la mémoire (vérifiée avec Valgrind)
- Compilation séparée pour chaque interface
- Support complet sur Linux (Debian/Ubuntu et distributions dérivées)

---

## Architecture

Le projet suit rigoureusement le modèle MVC :

### Modèle (Model)

Le modèle contient toute la logique métier du jeu et est totalement indépendant de l'interface utilisateur :

- **`src/core/model.h` / `model.c`** : Logique centrale du jeu
  - Gestion de l'état du jeu (positions, collisions, score)
  - Déplacement des ennemis et du joueur
  - Système de tir et détection de collisions
  - Progression des niveaux

- **`src/core/game_state.h` / `game_state.c`** : Gestion des états
  - États du jeu (menu, en jeu, pause, game over, victoire)
  - Transitions entre les états
  - Contexte global du jeu

### Vue (View)

Deux implémentations distinctes de la vue, interchangeables au lancement :

#### Vue SDL

- **`src/views/view_sdl.h` / `view_sdl.c`** : Rendu graphique
  - Utilisation de SDL3 pour le rendu
  - Support des textures via SDL3_image
  - Rendu de texte avec SDL3_ttf
  - Interface graphique complète

#### Vue ncurses

- **`src/views/view_ncurses.h` / `view_ncurses.c`** : Rendu terminal
  - Affichage texte ASCII
  - Interface légère pour terminal
  - Compatibilité étendue

### Contrôleur (Controller)

Le contrôleur fait le lien entre les entrées utilisateur et le modèle :

- **`src/controller/controller.h` / `controller.c`** : Contrôleur principal
  - Orchestration des interactions
  - Communication entre modèle et vue

- **`src/controller/input_handler.h` / `input_handler.c`** : Gestion des entrées
  - Abstraction des entrées clavier
  - Support SDL et ncurses

- **`src/controller/commands.h`** : Définition des commandes
  - Commandes abstraites du jeu
  - Indépendantes de l'implémentation des entrées

### Utilitaires

- **`src/utils/platform.h` / `platform_sdl.c`** : Abstraction de plateforme
  - Fonctions spécifiques à la plateforme
  - API unifiée pour SDL et ncurses

- **`src/utils/font_manager.h` / `font_manager.c`** : Gestion des polices
  - Chargement et gestion des polices TTF pour SDL

---

## Installation

### Prérequis Système

- Distribution Linux (Debian, Ubuntu, Fedora, Arch, ou dérivées)
- GCC (version 7.0 ou supérieure)
- Make
- pkg-config

### Installation des Dépendances

#### Méthode Automatique

Le Makefile inclut une cible pour installer automatiquement toutes les dépendances :

```bash
make install-deps
```

Cette commande détecte automatiquement votre gestionnaire de paquets (apt, dnf, pacman, zypper) et installe les dépendances nécessaires.

#### Installation Manuelle

##### Debian/Ubuntu

```bash
sudo apt-get update
sudo apt-get install -y libsdl3-dev libsdl3-ttf-dev libsdl3-image-dev \
                        libncurses-dev check valgrind doxygen graphviz \
                        pkg-config make gcc git clang-format
```

##### Fedora

```bash
sudo dnf install -y SDL3-devel SDL3_ttf-devel SDL3_image-devel \
                    ncurses-devel check-devel valgrind doxygen graphviz \
                    pkg-config make gcc git clang-tools-extra
```

##### Arch Linux

```bash
sudo pacman -S --noconfirm sdl3 sdl3_ttf sdl3_image ncurses check \
                           valgrind doxygen graphviz pkg-config make gcc git clang
```

### Vérification des Dépendances

Vérifiez que les dépendances SDL3 sont correctement installées :

```bash
make check-sdl-deps
make check-ncurses-deps
```

---

## Compilation

Le projet utilise un Makefile unique avec plusieurs cibles pour faciliter la compilation.

### Compilation Complète

Pour compiler les deux versions (SDL et ncurses) ainsi que les outils :

```bash
make all
```

### Compilation Sélective

#### Version SDL uniquement

```bash
make sdl
```

#### Version ncurses uniquement

```bash
make ncurses
```

#### Outils auxiliaires

```bash
make tools
```

### Modes de Compilation Spéciaux

#### Mode Debug

Compilation avec symboles de débogage et sanitizers :

```bash
make debug
```

#### Mode Release

Compilation optimisée pour la production (niveau d'optimisation O3) :

```bash
make release
```

#### Mode Profiling

Compilation avec support du profiling (gprof) :

```bash
make profile
```

### Recompilation Complète

Pour nettoyer et recompiler depuis zéro :

```bash
make rebuild
```

### Informations de Compilation

Afficher les informations sur la configuration de compilation :

```bash
make info
```

---

## Utilisation

### Lancement du Jeu

#### Version SDL

```bash
make run-sdl
```

Ou directement après compilation :

```bash
./bin/space_invaders_sdl
```

#### Version ncurses

```bash
make run-ncurses
```

Ou directement après compilation :

```bash
./bin/space_invaders_ncurses
```

### Préparation des Ressources

Avant la première exécution, il est recommandé de préparer les ressources (polices, fichiers de score) :

```bash
make prepare-assets
```

---

## Structure du Projet

```
.
├── Makefile                    # Système de compilation principal
├── README.md                   # Cette documentation
├── checkproject.sh             # Script de validation de la structure
├── highscore.dat              # Fichier de sauvegarde des scores
│
├── bin/                        # Exécutables compilés
│   ├── space_invaders_sdl
│   ├── space_invaders_ncurses
│   └── fonts/                  # Polices copiées pour l'exécution
│
├── build/                      # Fichiers objets de compilation
│   ├── sdl/                    # Objets pour la version SDL
│   │   ├── controller/
│   │   ├── core/
│   │   ├── utils/
│   │   └── views/
│   └── ncurses/                # Objets pour la version ncurses
│       ├── controller/
│       ├── core/
│       └── views/
│
├── src/                        # Code source
│   ├── controller/             # Contrôleur (MVC)
│   │   ├── commands.h
│   │   ├── controller.h
│   │   ├── controller.c
│   │   ├── input_handler.h
│   │   └── input_handler.c
│   │
│   ├── core/                   # Modèle (MVC)
│   │   ├── model.h
│   │   ├── model.c
│   │   ├── game_state.h
│   │   └── game_state.c
│   │
│   ├── views/                  # Vues (MVC)
│   │   ├── view_base.h
│   │   ├── view_sdl.h
│   │   ├── view_sdl.c
│   │   ├── view_ncurses.h
│   │   ├── view_ncurses.c
│   │   └── rect_utils.h
│   │
│   ├── utils/                  # Utilitaires
│   │   ├── platform.h
│   │   ├── platform_sdl.c
│   │   ├── font_manager.h
│   │   └── font_manager.c
│   │
│   ├── main_sdl.c             # Point d'entrée SDL
│   └── main_ncurses.c         # Point d'entrée ncurses
│
├── tests/                      # Tests unitaires
│   └── test_model.c
│
├── tools/                      # Outils de génération d'assets
│
├── fonts/                      # Polices de caractères
│
├── docs/                       # Documentation générée (Doxygen)
│
├── reports/                    # Rapports Valgrind
│   ├── valgrind-sdl-rapport.txt
│   ├── valgrind-ncurses-rapport.txt
│   ├── valgrind-tests-rapport.txt
│   └── valgrind-rapport-complet.txt
│
└── dist/                       # Packages de distribution
```

---

## Dépendances

### Bibliothèques Principales

#### Pour la Version SDL

- **SDL3** (libsdl3-dev) : Bibliothèque graphique principale
- **SDL3_ttf** (libsdl3-ttf-dev) : Support des polices TrueType
- **SDL3_image** (libsdl3-image-dev) : Chargement d'images (BMP, PNG)

#### Pour la Version ncurses

- **ncurses** (libncurses-dev) : Bibliothèque pour interface terminal

#### Pour les Tests

- **Check** (check ou check-devel) : Framework de tests unitaires en C

#### Pour le Développement

- **Valgrind** : Détection de fuites mémoire
- **Doxygen** : Génération de documentation
- **Graphviz** : Diagrammes pour la documentation
- **clang-format** : Formatage automatique du code

### Versions Recommandées

- SDL3 : version 3.0.0 ou supérieure
- ncurses : version 6.0 ou supérieure
- GCC : version 7.0 ou supérieure (support C11)
- Check : version 0.10.0 ou supérieure

---

## Tests et Validation

Le projet inclut plusieurs mécanismes de test et de validation pour assurer la qualité du code.

### Tests Unitaires

Exécuter les tests unitaires :

```bash
make run-tests
```

Ou simplement :

```bash
make test
```

### Analyse Mémoire avec Valgrind

#### Analyse de la Version SDL

```bash
make valgrind-sdl
```

Le rapport détaillé sera sauvegardé dans `reports/valgrind-sdl-rapport.txt`.

#### Analyse de la Version ncurses

```bash
make valgrind-ncurses
```

Le rapport détaillé sera sauvegardé dans `reports/valgrind-ncurses-rapport.txt`.

#### Analyse des Tests

```bash
make valgrind-tests
```

Le rapport détaillé sera sauvegardé dans `reports/valgrind-tests-rapport.txt`.

#### Rapport Valgrind Complet

Pour générer un rapport consolidé de toutes les analyses Valgrind :

```bash
make valgrind-report
```

Ce rapport combiné sera sauvegardé dans `reports/valgrind-rapport-complet.txt` et inclut :
- Rapport SDL
- Rapport ncurses
- Rapport des tests
- Horodatage de génération

#### Vérification Mémoire Complète

Pour exécuter toutes les analyses mémoire :

```bash
make check-memory
```

Ou :

```bash
make memcheck
```

#### Vérification Rapide des Fuites

Pour une vérification rapide (résumé uniquement) :

```bash
make leak-check
```

### Vérification du Style de Code

Vérifier la conformité du code avec les standards de style :

```bash
make check-style
```

### Formatage Automatique du Code

Formater automatiquement le code selon les standards :

```bash
make format
```

### Vérification Complète du Projet

Pour une vérification complète (compilation, tests, mémoire, style) :

```bash
make fullcheck
```

### Validation de la Structure du Projet

Vérifier que tous les fichiers requis sont présents :

```bash
make check-project
```

Ou directement :

```bash
./checkproject.sh
```

---

## Contrôles

### Version SDL

- **Déplacement** : Flèches gauche/droite ou touches A/D
- **Tir** : Barre d'espace
- **Pause** : Touche P
- **Quitter** : Touche Échap ou Q
- **Redémarrer** : Touche R (après game over)

### Version ncurses

- **Déplacement** : Touches A (gauche) et D (droite)
- **Tir** : Barre d'espace
- **Pause** : Touche P
- **Quitter** : Touche Q
- **Redémarrer** : Touche R (après game over)

Les contrôles peuvent être personnalisés en modifiant les constantes dans les fichiers du contrôleur.

---

## Développement

### Génération de la Documentation

Générer la documentation Doxygen :

```bash
make doc
```

ou

```bash
make generate-docs
```

La documentation sera générée dans `docs/html/`. Ouvrez `docs/html/index.html` dans un navigateur.

### Système de Couverture de Code

Pour générer un rapport de couverture de code (à implémenter) :

```bash
make coverage
```

### Benchmarks de Performance

Pour exécuter des tests de performance (à implémenter) :

```bash
make benchmark
```

### Nettoyage

#### Nettoyage Standard

Supprimer tous les fichiers générés (objets, exécutables, rapports) :

```bash
make clean
```

#### Fichiers Supprimés

- Répertoire `build/`
- Répertoire `bin/`
- Fichiers objets (*.o)
- Fichiers de profiling (gmon.out, callgrind.out.*)
- Documentation générée (`docs/html`, `docs/latex`)
- Rapports Valgrind (`reports/`)
- Packages de distribution (`dist/*.tar.gz`, `dist/*.zip`)

### Installation Système

#### Installer les Exécutables

Installer les exécutables dans `/usr/local/bin/` :

```bash
sudo make install
```

#### Désinstaller

Supprimer les exécutables installés :

```bash
sudo make uninstall
```

### Création de Packages de Distribution

Créer un package de distribution (archive tar.gz) :

```bash
make dist
```

ou

```bash
make package
```

Le package sera créé dans le répertoire `dist/` avec un nom incluant la date et l'heure de création.

---

## Standards de Code

### Conventions de Nommage

- **Variables et fonctions** : `snake_case` (ex: `player_position`, `update_game_state()`)
- **Constantes et macros** : `UPPER_CASE` (ex: `MAX_ENEMIES`, `SCREEN_WIDTH`)
- **Types (structures, enums)** : `PascalCase` avec typedef (ex: `GameState`, `PlayerData`)

### Style de Code

- **Indentation** : 4 espaces (pas de tabulations)
- **Accolades** : Style K&R (accolade ouvrante sur la même ligne)
- **Longueur de ligne** : Maximum 100 caractères
- **Commentaires** : Style C99 (`//`) pour les commentaires courts, `/* */` pour les blocs

### Organisation des Fichiers

Chaque module doit contenir :

1. **Fichier d'en-tête (.h)** :
   - Protection contre les inclusions multiples (`#ifndef`, `#define`, `#endif`)
   - Déclarations des structures publiques
   - Prototypes des fonctions publiques
   - Commentaires de documentation

2. **Fichier d'implémentation (.c)** :
   - Inclusion du fichier d'en-tête correspondant
   - Structures et fonctions privées (static)
   - Implémentation des fonctions publiques

### Gestion de la Mémoire

- Toute allocation (`malloc`, `calloc`) doit avoir une libération (`free`) correspondante
- Vérifier systématiquement le retour des allocations (NULL)
- Utiliser `valgrind` pour détecter les fuites mémoire
- Préférer l'allocation sur la pile quand c'est possible

### Documentation du Code

- Documenter toutes les fonctions publiques (style Doxygen recommandé)
- Expliquer les algorithmes complexes
- Inclure des exemples d'utilisation pour les APIs publiques

---

## Exigences du Projet

Ce projet respecte les exigences suivantes conformément au cahier des charges :

### Fonctionnalités Requises

- [x] Modèle MVC strict avec séparation claire
- [x] Deux vues interchangeables (SDL et ncurses)
- [x] Gestion complète de l'état du jeu
- [x] Logique de déplacement des ennemis
- [x] Système de tir et collisions
- [x] Gestion des niveaux et progression
- [x] Système de score et vies
- [x] Makefile avec cibles requises
- [x] README détaillé
- [x] Architecture propre et modulaire

### Tests et Validation

- [x] Tests unitaires pour le modèle
- [x] Validation Valgrind (pas de fuites mémoire)
- [x] Support de compilation sur Linux
- [x] Documentation du code

### Livrables

- [x] Code source complet et organisé
- [x] Makefile avec toutes les cibles requises
- [x] README détaillé
- [x] Script de validation du projet
- [x] Rapports Valgrind

---

## Cibles Makefile Disponibles

### Compilation

- `make all` : Compile SDL, ncurses et les outils
- `make sdl` : Compile uniquement la version SDL
- `make ncurses` : Compile uniquement la version ncurses
- `make tools` : Compile les outils auxiliaires
- `make rebuild` : Nettoie et recompile tout

### Exécution

- `make run-sdl` : Compile et lance la version SDL
- `make run-ncurses` : Compile et lance la version ncurses
- `make run-tests` : Compile et exécute les tests

### Tests et Vérifications

- `make test` : Exécute les tests unitaires
- `make valgrind-sdl` : Analyse mémoire (SDL)
- `make valgrind-ncurses` : Analyse mémoire (ncurses)
- `make valgrind-tests` : Analyse mémoire (tests)
- `make valgrind-report` : Génère un rapport Valgrind complet
- `make check-memory` : Analyse mémoire complète
- `make leak-check` : Vérification rapide des fuites
- `make check-style` : Vérifie le style du code
- `make format` : Formate automatiquement le code
- `make fullcheck` : Vérification complète du projet

### Documentation

- `make doc` : Génère la documentation Doxygen
- `make info` : Affiche les informations de build

### Dépendances

- `make install-deps` : Installe les dépendances nécessaires
- `make check-sdl-deps` : Vérifie les dépendances SDL3
- `make check-ncurses-deps` : Vérifie les dépendances ncurses

### Nettoyage

- `make clean` : Supprime tous les fichiers générés

### Installation

- `make install` : Installe dans /usr/local/bin
- `make uninstall` : Désinstalle du système
- `make dist` : Crée un package de distribution
- `make package` : Alias pour dist

### Modes de Compilation Spéciaux

- `make debug` : Compile en mode débogage (sanitizers)
- `make release` : Compile en mode optimisé (production)
- `make profile` : Compile avec support du profiling

### Utilitaires

- `make prepare-assets` : Prépare les ressources (polices, etc.)
- `make check-project` : Vérifie la structure du projet
- `make help` : Affiche l'aide complète

---

## Dépannage

### Problèmes Courants

#### SDL3 non trouvé

**Erreur** : `pkg-config: command not found` ou `SDL3 not found`

**Solution** :
1. Vérifiez que pkg-config est installé : `which pkg-config`
2. Vérifiez que SDL3 est détectable : `pkg-config --modversion sdl3`
3. Vérifiez la variable PKG_CONFIG_PATH : `echo $PKG_CONFIG_PATH`
4. Si nécessaire, ajoutez le chemin vers les fichiers .pc de SDL3

#### Erreurs de compilation avec ncurses

**Erreur** : `ncurses.h: No such file or directory`

**Solution** :
```bash
sudo apt-get install libncurses-dev  # Debian/Ubuntu
sudo dnf install ncurses-devel       # Fedora
sudo pacman -S ncurses              # Arch Linux
```

#### Fuites mémoire détectées

**Problème** : Valgrind signale des fuites mémoire

**Solution** :
1. Exécutez `make valgrind-report` pour obtenir un rapport détaillé
2. Consultez le fichier `reports/valgrind-rapport-complet.txt`
3. Vérifiez chaque allocation avec son `free()` correspondant
4. Utilisez le débogueur gdb si nécessaire

#### Erreurs de permissions lors de l'installation

**Erreur** : `Permission denied` lors de `make install`

**Solution** :
```bash
sudo make install
```

---

## Contribution

Les contributions sont les bienvenues. Veuillez suivre ces directives :

1. Respecter l'architecture MVC
2. Suivre les standards de code définis
3. Ajouter des tests pour les nouvelles fonctionnalités
4. Vérifier l'absence de fuites mémoire avec Valgrind
5. Documenter le code avec des commentaires clairs
6. Tester sur les deux interfaces (SDL et ncurses)

---

## Licence

Ce projet est sous licence MIT. Voir le fichier LICENSE pour plus de détails.

### Attributions

- **Concept original** : Space Invaders (Taito, 1978)
- **Architecture** : Basée sur les design patterns classiques (MVC)
- **Bibliothèques utilisées** :
  - SDL3 : Licence Zlib
  - SDL3_ttf : Licence Zlib
  - SDL3_image : Licence Zlib
  - ncurses : Licence X11
  - Check : Licence LGPL

---

## Contact et Support

Pour toute question ou problème :

1. Consultez d'abord cette documentation
2. Vérifiez les problèmes courants dans la section Dépannage
3. Exécutez `make check-project` pour valider votre installation
4. Consultez les rapports Valgrind pour les problèmes de mémoire

---

## Références

- [SDL3 Documentation](https://wiki.libsdl.org/SDL3/FrontPage)
- [ncurses Programming Guide](https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/)
- [Design Patterns in C](https://www.adamtornhill.com/Patterns%20in%20C.pdf)
- [Valgrind User Manual](https://valgrind.org/docs/manual/manual.html)

---

**Version du Document** : 1.0  
**Dernière Mise à Jour** : 2025-12-31  
**Auteur** : Projet Space Invaders MVC
