# Space Invaders Ultimate - Documentation Complète

## Vue d'ensemble du projet

**Space Invaders Ultimate** est une implémentation multiplateforme du jeu classique Space Invaders écrite en C. Le projet propose deux interfaces graphiques distinctes :
- **Version SDL3** avec graphismes complets, audio et animations
- **Version Ncurses** pour jouer dans le terminal avec des graphismes ASCII

Le jeu suit l'architecture **Modèle-Vue-Contrôleur (MVC)** avec une modularisation étendue et une abstraction de plateforme.

---

## Structure détaillée du projet

### **Organisation racine**

```
.
├── bin/                          # Exécutables compilés et données d'exécution
├── build/                        # Fichiers objets compilés (séparés par interface)
├── fonts/                        # Polices d'écriture pour le rendu SDL
├── Oldassets/                    # Fichiers graphiques hérités
├── src/                          # Code source organisé par fonctionnalité
├── testl/                        # Tests pour ncurses
├── tests/                        # Tests unitaires
└── tools/                        # Outils de génération d'assets
```

### **1. Exécutables et données d'exécution (`./bin/`)**
- `highscore.dat` - Fichier binaire stockant le meilleur score
- `space_invaders_ncurses` - Exécutable pour terminal
- `space_invaders_sdl` - Exécutable graphique SDL3

### **2. Système de compilation (`./build/`)**
Le projet utilise des répertoires de compilation séparés pour chaque interface :

#### **Arborescence de compilation Ncurses :**
```
build/ncurses/
├── controller/           # Objets de gestion des entrées
│   ├── controller.o
│   └── input_handler.o
├── core/                 # Objets de logique de jeu
│   ├── game_state.o
│   └── model.o
├── main_ncurses.o        # Point d'entrée principal pour ncurses
├── utils/                # Fonctions utilitaires
│   └── font_manager.o
└── views/                # Rendu terminal
    └── view_ncurses.o
```

#### **Arborescence de compilation SDL :**
```
build/sdl/
├── controller/           # Objets de gestion des entrées
│   ├── controller.o
│   └── input_handler.o
├── core/                 # Objets de logique de jeu
│   ├── game_state.o
│   └── model.o
├── main_sdl.o           # Point d'entrée principal pour SDL
├── utils/               # Utilitaires spécifiques à la plateforme
│   ├── font_manager.o
│   └── platform_sdl.o
└── views/               # Rendu graphique
    └── view_sdl.o
```

### **3. Architecture du code source (`./src/`)**

#### **Logique de jeu principale (`src/core/`)**

##### **Modèle (`model.h`, `model.c`)**
Le **conteneur central de l'état du jeu** implémentant toute la logique :

```c
// Structures clés :
typedef struct {
    Player player;
    InvaderGrid invaders;      // Grille 5x10 d'ennemis
    Boss boss;                // Boss final pour le niveau 4
    Saucer saucer;            // Soucoupe bonus
    Base bases[BASE_COUNT];   // 4 bases défensives
    Bullet player_bullets[PLAYER_BULLETS];
    Bullet enemy_bullets[ENEMY_BULLETS];
    GameState state;          // État actuel du jeu
    uint32_t game_time;
    bool needs_redraw;
    int high_score;
} GameModel;
```

**Fonctions principales :**
- `model_init()` - Initialise toutes les entités du jeu
- `model_update()` - Logique principale de la boucle de jeu
- `model_move_player()` - Mouvement du joueur avec vérification des limites
- `model_player_shoot()` - Mécanique de tir du joueur
- `model_check_collision()` - Détection de collision AABB
- `model_next_level()` - Système de progression de niveau

**États du jeu :**
```c
typedef enum {
    STATE_MENU,              // Menu principal
    STATE_PLAYING,           // Jeu actif
    STATE_PAUSED,           // Jeu en pause
    STATE_GAME_OVER,        // Défaite du joueur
    STATE_LEVEL_TRANSITION, // Entre les niveaux
    STATE_WIN               // Jeu terminé
} GameState;
```

##### **Gestionnaire d'état de jeu (`game_state.h`, `game_state.c`)**
**Gestionnaire de contexte** qui coordonne modèle, vue et contrôleur :

```c
typedef struct {
    GameModel* model;        // Données du jeu
    void* view_data;         // Contexte spécifique à la vue
    void* controller_data;   // Contexte du contrôleur
    bool running;            // Drapeau de boucle principale
    uint32_t last_update;    // Timing pour les calculs delta
    float time_scale;        // Multiplicateur de vitesse de jeu
} GameContext;
```

**Fonctions :**
- `game_context_create()` - Initialise le contexte complet du jeu
- `game_context_update()` - Met à jour l'état du jeu avec le temps delta
- `transition_to_state()` - Transitions de la machine à états

#### **Couche contrôleur (`src/controller/`)**

##### **Système de commandes (`commands.h`)**
**Motif de commande basé sur enum** pour l'abstraction des entrées :
```c
typedef enum {
    CMD_NONE, CMD_MOVE_LEFT, CMD_MOVE_RIGHT, CMD_SHOOT,
    CMD_PAUSE, CMD_START_GAME, CMD_QUIT, CMD_RESET_GAME,
    CMD_TOGGLE_VIEW, CMD_SELECT_OPTION, CMD_UP, CMD_DOWN,
    CMD_LEFT, CMD_RIGHT, CMD_CONFIRM, CMD_BACK
} Command;
```

##### **Gestionnaire d'entrées (`input_handler.h`, `input_handler.c`)**
**Système d'entrées abstrait pour la plateforme** avec mappage configurable :

**Caractéristiques clés :**
- **Système d'entrée double** : Support clavier + manette
- **Gestion de la répétition** : Délais initial/répétition configurables
- **Suivi de l'état des entrées** : États pressed/was_pressed
- **Seuil d'axe** : Zones mortes configurables pour la manette

**Machine à états des entrées :**
```c
typedef struct {
    bool left_pressed;      // État actuel pressé
    bool left_was_pressed;  // État de la frame précédente
    uint32_t left_repeat_time;  // Timing pour l'auto-répétition
    uint32_t initial_delay;     // Délai initial de répétition (ms)
    uint32_t repeat_delay;      // Délai de répétition ultérieur (ms)
    // ... (droite, tir, pause, quitter)
} InputState;
```

**Abstraction de plateforme :**
```c
#ifdef USE_SDL_VIEW
void input_handler_process_sdl_event(InputHandler* handler, const SDL_Event* event);
#else
void input_handler_process_ncurses_input(InputHandler* handler, int ch);
#endif
```

##### **Contrôleur (`controller.h`, `controller.c`)**
**Contrôleur principal** qui traduit les entrées en commandes de jeu :

**Architecture :**
```c
typedef struct {
    GameModel* model;               // Référence au modèle de jeu
    void* view_context;             // Données spécifiques à la vue
    InputHandler* input_handler;    // Traitement des entrées
    RenderCallback render_callback; // Callback de mise à jour de la vue
    AudioCallback audio_callback;   // Callback de mise à jour audio
    void* callback_data;            // Contexte des callbacks
    int key_left, key_right, key_shoot, key_pause, key_quit;
    bool quit_requested;            // Drapeau de sortie
    bool paused;                    // État de pause
    uint32_t last_input_time;       // Timing des entrées
} Controller;
```

**Flux des commandes :**
1. Entrée → `controller_translate_input()` → Commande
2. Commande → `controller_execute_command()` → Mise à jour du modèle
3. Callbacks → Mises à jour de la vue/audio

#### **Couche vue (`src/views/`)**

##### **Vue SDL (`view_sdl.h`, `view_sdl.c`)**
**Interface graphique complète** avec SDL3 et Miniaudio :

**Caractéristiques :**
- **Rendu basé sur textures** : Charge les images BMP pour toutes les entités
- **Système d'animation** : Animations de sprites basées sur les frames
- **Système audio** : Intégration Miniaudio avec positionnement 3D du son
- **Effets de particules** : Particules d'explosion avec mélange additif
- **Système HUD** : Barre latérale avec score, vies, niveau, santé du boss
- **Rendu spécifique à l'état** : Visuels différents pour chaque état de jeu

**Implémentation audio :**
```c
// Intégration Miniaudio
ma_engine audio_engine;
ma_sound sfx_shoot;      // Son de tir
ma_sound sfx_explosion;  // Son d'explosion
ma_sound music_bg;       // Musique de fond (en boucle)
```

**Pipeline de rendu :**
1. Efface l'écran avec l'arrière-plan spatial
2. Dessine les étoiles (arrière-plan procédural)
3. Rendu des entités du jeu (textures ou rectangles de secours)
4. Dessine la barre latérale HUD
5. Applique les superpositions spécifiques à l'état (pause, game over, etc.)
6. Présente à l'écran

##### **Vue Ncurses (`view_ncurses.h`, `view_ncurses.c`)**
**Interface basée sur terminal** utilisant la bibliothèque ncurses :

**Caractéristiques :**
- **Rendu ASCII** : Caractères représentant les entités du jeu
- **Mise à l'échelle des coordonnées** : Positions pixels → cellules terminal
- **Support des couleurs** : Couleurs de terminal basiques
- **Utilisation minimale de ressources** : Aucun asset externe nécessaire

**Représentations des entités :**
- Joueur : `^` avec `<` et `>` pour les ailes
- Envahisseurs : `Y`, `X`, `W` selon le type
- Projectiles : `|` (joueur), `o` (ennemi)
- Boss : Art ASCII multi-lignes
- Soucoupe : `<@>`

**Disposition de l'écran :**
```
┌─────────────────────────────┐
│      SPACE INVADERS         │
├─────────────┬───────────────┤
│ Zone de jeu │ SCORE: 1500   │
│    ^        │ VIES: 3       │
│   < >       │ NIVEAU: 2     │
│    W W W    │               │
│   X X X X   │ PV BOSS:      │
│  Y Y Y Y Y  │ [===     ]    │
└─────────────┴───────────────┘
```

#### **Couche utilitaire (`src/utils/`)**

##### **Abstraction de plateforme (`platform.h`, `platform_sdl.c`)**
**Compilation conditionnelle** pour les implémentations spécifiques à la plateforme :

**Implémentation SDL :**
```c
uint32_t platform_get_ticks(void) {
    return (uint32_t)SDL_GetTicks();
}

bool platform_key_pressed(int key) {
    const bool* keyboard_state = SDL_GetKeyboardState(NULL);
    // ... logique de mappage des touches
}
```

**Implémentation Ncurses (inline) :**
```c
static inline uint32_t platform_get_ticks(void) {
    return (uint32_t)((clock() * 1000) / CLOCKS_PER_SEC);
}

static inline bool platform_key_pressed(int key) {
    (void)key;
    return false; // Non implémenté pour ncurses
}
```

##### **Définitions des codes de touche (`keycodes.h`)**
**Constantes de touches multiplateforme** correspondant aux valeurs SDL3 :
```c
#define SDLK_a 'a'
#define SDLK_SPACE ' '
#define SDLK_LEFT 68
#define SDLK_RIGHT 67
// ... etc.
```

##### **Gestionnaire de polices (`font_manager.h`, `font_manager.c`)**
**Gestion des polices spécifique à SDL** avec chemins de secours :
```c
TTF_Font* load_font(const char* path, int size) {
#ifdef USE_SDL
    return TTF_OpenFont(path, size);
#else
    return NULL; // Ncurses n'utilise pas de polices
#endif
}
```

##### **Enveloppe Miniaudio (`miniaudio.h`)**
**Bibliothèque audio en un seul fichier** pour la version SDL.

##### **Minuteur simple (`simple_timer.h`)**
**Timing multiplateforme** utilisant `gettimeofday()` et `nanosleep()`.

#### **Répertoires d'assets**

##### **Graphismes (`src/pictures/`)**
**Sprites au format BMP** avec conventions de nommage :
- `player.bmp`, `player2.bmp` - Vaisseau du joueur (animation 2 frames)
- `invader{1-3}_{1-2}.bmp` - 3 types d'envahisseurs × 2 frames chacun
- `boss_dreadnought_f{1-2}.bmp` - Frames du boss
- `bonus_saucer_f{1-2}.bmp` - Frames de la soucoupe
- `bullet_player.bmp`, `bullet_enemy.bmp` - Projectiles
- `explosion.bmp` - Effet d'explosion

##### **Audio (`src/assets/`)**
**Sons au format MP3** :
- `shooting.mp3` - Son du laser du joueur
- `explosion.mp3` - Destruction d'ennemi
- `music.mp3` - Musique de fond (en boucle)

##### **Polices (`fonts/`)**
**Polices TTF** pour le rendu du texte SDL avec chaîne de secours.

#### **Outils de génération d'assets (`tools/`)**
**Programmes C pour créer les assets du jeu** :
- `generate_boss.c` - Générateur de sprite de boss
- `generate_invader{1-3}.c` - Générateurs de sprites d'envahisseurs
- `generate_saucer.c` - Générateur de sprite de soucoupe
- `generate_spaceship.c` - Générateur de vaisseau du joueur

### **4. Points d'entrée**

#### **Main SDL (`main_sdl.c`)**
```c
int main() {
    // Initialise les sous-systèmes SDL
    // Crée GameContext avec la vue SDL
    // Boucle de jeu principale :
    while (context->running) {
        // Traite les événements SDL
        // Met à jour le contrôleur
        // Met à jour l'état du jeu
        // Rend avec SDL
        // Limitation du taux de rafraîchissement
    }
    // Nettoyage
}
```

#### **Main Ncurses (`main_ncurses.c`)**
```c
int main() {
    // Initialise ncurses
    // Crée GameContext avec la vue ncurses
    // Boucle de jeu principale :
    while (context->running) {
        // Récupère l'entrée clavier
        // Met à jour le contrôleur
        // Met à jour l'état du jeu
        // Rend avec ncurses
        // Limitation du taux de rafraîchissement
    }
    // Nettoyage ncurses
}
```

---

## Mécaniques de jeu expliquées

### **Progression des niveaux**
1. **Niveaux 1-3** : Envahisseurs standards (5 lignes × 10 colonnes)
   - Vitesse augmentant à chaque niveau
   - Fréquence de tir plus élevée
   - Valeurs de points plus importantes pour les lignes supérieures

2. **Niveau 4** : Combat contre le boss
   - Gros boss avec barre de vie
   - Mouvement en motif (gauche/droite)
   - Tir rapide
   - 5000 points pour la défaite

### **Système de score**
- **Envahisseurs** : 10-30 points (selon le type/ligne)
- **Soucoupe** : 200-300 points aléatoires
- **Boss** : 5 points par coup, 5000 pour la défaite
- **Persistance du meilleur score** dans `highscore.dat`

### **Système de collision**
**Détection Axis-Aligned Bounding Box (AABB)** :
```c
bool model_check_collision(Rect a, Rect b) {
    return (a.x < b.x + b.width && a.x + a.width > b.x &&
            a.y < b.y + b.height && a.y + a.height > b.y);
}
```

**Types de collisions :**
1. Projectiles joueur vs envahisseurs/boss/soucoupe
2. Projectiles ennemis vs joueur
3. Envahisseurs vs bases (détruit les bases)
4. Envahisseurs vs joueur (fin de jeu)

### **Systèmes d'entités**

#### **Grille d'envahisseurs**
- **Grille 5×10** avec mouvement coordonné
- **États d'animation** : Alterne entre deux frames de sprite
- **Inversion de direction** : Lorsqu'ils touchent les bords de l'écran
- **Descente progressive** : Descend après avoir touché un bord
- **Logique de tir** : L'envahisseur le plus bas de la colonne tire

#### **Vaisseau du joueur**
- **Mouvement limité** : Gauche/droite dans les limites de l'écran
- **Muni limité** : Maximum 3 projectiles à l'écran
- **Système de vies** : 3 vies, fin de jeu quand épuisées
- **Suivi du score** : Persistant à travers les niveaux

#### **Boss (Niveau 4)**
- **Barre de vie** : 50 PV, affichée dans le HUD
- **Motif de mouvement** : Balayage horizontal
- **Motif de tir** : Tirs à intervalle régulier
- **Récompense de défaite** : Avance au niveau suivant

#### **Soucoupe (Bonus)**
- **Apparition aléatoire** : ~1/1200 chance par frame
- **Mouvement horizontal** : Direction gauche ou droite
- **Points variables** : 200-300 score aléatoire

#### **Bases défensives**
- **4 bases** positionnées au-dessus du joueur
- **Protection** : Bloquer les projectiles ennemis
- **Destructible** : Peut être détruite par les envahisseurs

---

## Système de compilation et compilation

### **Structure des Makefiles**
Deux Makefiles séparés :
- `Makefile_sdl` - Compile la version SDL avec audio/graphismes
- `Makefile_ncurses` - Compile la version terminal

### **Drapeaux de compilation**
**Version SDL :**
```makefile
CFLAGS = -Wall -Wextra -std=c11 -DUSE_SDL_VIEW -I/usr/include/SDL3
LDFLAGS = -lSDL3 -lSDL3_image -lSDL3_ttf -lm
```

**Version Ncurses :**
```makefile
CFLAGS = -Wall -Wextra -std=c11 -I/usr/include/ncursesw
LDFLAGS = -lncursesw -lm
```

### **Processus de compilation**
```bash
# Version SDL
make -f Makefile_sdl clean
make -f Makefile_sdl
./bin/space_invaders_sdl

# Version Ncurses
make -f Makefile_ncurses clean
make -f Makefile_ncurses
./bin/space_invaders_ncurses
```

---

## Configuration et personnalisation

### **Mappage des touches**
**Mappage SDL par défaut :**
- Déplacement : Flèches ou WASD
- Tir : Barre d'espace
- Pause : P
- Quitter : Échap ou Q
- Réinitialiser : R

**Mappage Ncurses par défaut :**
- Déplacement : A (gauche), D (droite)
- Tir : Barre d'espace
- Pause : P
- Quitter : Q
- Réinitialiser : R

### **Constantes de jeu**
Modifiables dans `model.h` :
```c
#define INVADER_ROWS 5      // Nombre de lignes d'envahisseurs
#define INVADER_COLS 10     // Nombre de colonnes d'envahisseurs
#define PLAYER_BULLETS 3    // Nombre maximum de projectiles du joueur à l'écran
#define PLAYER_LIVES 3      // Vies initiales
#define BOSS_HEALTH 50      // Points de vie du boss
```

### **Ajustement des performances**
**SDL :**
- Taux de rafraîchissement : Cible 60 FPS
- Mise à l'échelle des textures : Nearest-neighbor pour le pixel art
- Mixage audio : Mixage en temps réel avec Miniaudio

**Ncurses :**
- Polling des entrées : getch() non bloquant
- Mises à jour de l'écran : Redessin minimal via le drapeau `needs_redraw`
- Mise à l'échelle des coordonnées : Division entière pour les performances

---

## Tests et développement

### **Structure des tests (`tests/`)**
- `test_model.c` - Tests unitaires pour la logique de jeu
- `test_runner.sh` - Exécution automatisée des tests
- `testl/` - Tests spécifiques à ncurses

### **Fonctionnalités de débogage**
**Débogage du modèle :**
- Visualisation des collisions (SDL seulement)
- Compteur de FPS (SDL seulement)
- Journalisation des transitions d'état (les deux versions)

**Gestion de la mémoire :**
- Toutes les allocations sont associées à des libérations
- Vérifications des pointeurs NULL partout
- Nettoyage des ressources à la sortie

---

## Caractéristiques de performance

### **Version SDL**
- **Utilisation CPU** : ~5-10% (60 FPS)
- **Mémoire** : ~50MB (textures + audio)
- **Rendu** : Accéléré matériellement via SDL
- **Audio** : Mixage en temps réel avec Miniaudio

### **Version Ncurses**
- **Utilisation CPU** : ~1-3% (cible 30 FPS)
- **Mémoire** : <5MB (pas d'assets externes)
- **Rendu** : Buffer de caractères du terminal
- **Entrées** : Basé sur le polling avec timeout

---

## Flux de données et architecture

### **Flux de la boucle de jeu**
```
[Entrée] → [Contrôleur] → [Commandes] → [Modèle] → [Changements d'état]
    ↑                                      ↓
[Événements]                          [Rendu de la vue]
    ↑                                      ↓
[Plateforme]                         [Sortie écran]
```

### **Hiérarchie de gestion de la mémoire**
```
GameContext
├── GameModel (alloué)
│   ├── Player (inline)
│   ├── InvaderGrid (tableau inline)
│   ├── Tableaux de projectiles (inline)
│   └── Tableau de bases (inline)
├── Contexte de la vue (spécifique à la plateforme)
│   ├── SDL : Textures, audio, polices
│   └── Ncurses : Buffer du terminal
└── Contexte du contrôleur
    └── InputHandler (alloué)
```

### **Diagramme de transition d'état**
```
      [MENU]
        ↓ (ESPACE)
   [JEU] ←→ [PAUSE] (P)
        ↓ (Niveau terminé)
[TRANSITION_NIVEAU]
        ↓ (ESPACE)
   [JEU] (Niveau suivant)
        ↓ (Condition de victoire)
       [VICTOIRE]
        ↓ (R)
      [MENU]
        ↓ (Défaite)
   [GAME_OVER]
        ↓ (R)
      [MENU]
```

---

## Outils de développement

### **Pipeline d'assets**
```
Éditeurs Bitmap → generate_*.c → Fichiers BMP → Textures du jeu
                 (Programmes C)
```

### **Outils de débogage**
- **SDL** : Minuterie de frames intégrée et visualisation audio
- **Ncurses** : Sortie de débogage des coordonnées de caractères
- **Les deux** : Journalisation des transitions d'état vers stderr

### **Drapeaux de profil**
Compiler avec `-pg` pour le profilage gprof :
```makefile
CFLAGS += -pg
LDFLAGS += -pg
```

---

## Évolutivité et extensibilité

### **Ajout de nouvelles fonctionnalités**
1. **Nouveaux types d'entités** :
   - Ajouter à l'enum `EntityType` dans `model.h`
   - Implémenter les fonctions de mise à jour/rendu
   - Ajouter à la détection de collision

2. **Nouveaux états de jeu** :
   - Ajouter à l'enum `GameState`
   - Implémenter la logique d'état dans `model.c`
   - Ajouter le rendu dans les deux implémentations de vue

3. **Nouveaux types d'entrées** :
   - Étendre l'enum `InputType`
   - Ajouter la gestion dans `input_handler.c`
   - Mapper aux commandes dans `controller.c`

### **Portabilité des plateformes**
**Support actuel :**
- Linux (SDL3, ncurses)
- Ports potentiels : Windows (SDL), macOS (SDL)

**Couches d'abstraction :**
1. Timing de plateforme (`platform.h`)
2. Gestion des entrées (`input_handler.h`)
3. Rendu (`view_base.h` interface)

---

## Patrons de conception utilisés

### **1. Modèle-Vue-Contrôleur (MVC)**
- **Modèle** : `GameModel` - État pur du jeu
- **Vue** : `SDLView`/`NcursesView` - Rendu spécifique à la plateforme
- **Contrôleur** : `Controller` - Mappage entrée → action

### **2. Patron Commande**
- L'enum `Command` représente toutes les actions possibles
- Découple la gestion des entrées de la logique de jeu
- Permet l'enregistrement/rejeu des entrées

### **3. Patron État**
- Enum `GameState` pour les états du jeu
- Comportement spécifique à l'état dans `model_update()`
- Transitions d'état propres

### **4. Patron Stratégie**
- Différentes implémentations de `view_render()`
- Stratégies spécifiques à la plateforme pour les entrées/rendu
- Échangeables à la compilation

### **5. Patron Observateur**
- Callbacks pour les mises à jour de rendu/audio
- Les changements de modèle déclenchent les mises à jour de la vue
- Couplage faible entre les composants

---

## Caractéristiques de qualité du code

### **Gestion des erreurs**
```c
// Modèle d'erreur cohérent
TTF_Font* font = TTF_OpenFont(path, size);
if (!font) {
    fprintf(stderr, "Échec du chargement de la police %s: %s\n", path, SDL_GetError());
    return NULL;
}
```

### **Sécurité mémoire**
- Toutes les allocations vérifiées pour NULL
- Fonctions d'allocation/libération appariées
- Allocation sur la pile quand possible
- Pas de variables globales

### **Sécurité des types**
- Types enum explicites pour les états/directions
- Typedef structs pour toutes les structures de données
- Exactitude const là où applicable

### **Documentation**
- Commentaires de style Doxygen pour les APIs publiques
- En-têtes de fonction expliquant le but
- Algorithmes complexes commentés

---

## Améliorations futures

### **Fonctionnalités prévues**
1. **Multijoueur en réseau** - Jeu coopératif/compétitif
2. **Système de power-ups** - Améliorations temporaires du joueur
3. **Éditeur de niveaux** - Création de niveaux personnalisés
4. **Système de rejeu** - Enregistrement/rejeu du jeu
5. **Système de succès** - Suivi des jalons

### **Améliorations techniques**
1. **Architecture ECS** - Refactorisation vers Entity Component System
2. **Support de script** - Intégration Lua pour la logique de jeu
3. **Contenu procédural** - Génération algorithmique de niveaux
4. **Effets de shader** - Shaders GLSL pour la version SDL
5. **États de sauvegarde** - Plusieurs emplacements de sauvegarde

---

## Références et dépendances

### **Bibliothèques principales**
- **SDL3** : Graphismes, gestion de fenêtre, entrées
- **SDL3_image** : Chargement de textures (support BMP)
- **SDL3_ttf** : Rendu de polices
- **Miniaudio** : Lecture et mixage audio
- **Ncursesw** : Interface terminal avec support des caractères larges

### **Sources des assets**
- **Graphismes** : BMPs générés sur mesure via tools/
- **Polices** : Venite Adoremus (police gratuite)
- **Audio** : Effets sonores et musique personnalisés

### **Outils de développement**
- **GCC/Clang** : Compilateurs C11
- **Make** : Automatisation de la construction
- **Valgrind** : Débogage mémoire
- **GDB** : Débogage

---

## Lignes directrices pour les contributions

### **Normes de code**
- **Nommage** : snake_case pour variables/fonctions, UPPER_CASE pour constantes
- **Indentation** : 4 espaces (pas de tabulations)
- **Style d'accolades** : K&R avec accolades ouvrantes sur la même ligne
- **Commentaires** : Minimum 20% de ratio commentaire/code

### **Organisation des modules**
```
Nouvelle fonctionnalité doit inclure :
1. Fichier d'en-tête (.h) avec interface publique
2. Fichier d'implémentation (.c)
3. Implémentations spécifiques à la plateforme si nécessaire
4. Mettre à jour les deux versions SDL et ncurses
5. Ajouter au Makefile approprié
```

### **Exigences de test**
- Tests unitaires pour la nouvelle logique de jeu
- Tests multiplateforme (SDL + ncurses)
- Vérification des fuites mémoire
- Vérification du mappage des entrées

---

## Licence et attribution

### **Licence du code**
- Code du projet : Licence MIT
- Assets du jeu : CC BY-NC 4.0 (utilisation non commerciale)
- Bibliothèques tierces : Licences open-source respectives

### **Attributions**
- **Conception du jeu** : Inspiré du Space Invaders original (Taito, 1978)
- **Architecture** : Basée sur les patrons de programmation de jeu classiques
- **Outils** : Construit avec des outils de développement open-source

---

## Conclusion

Space Invaders Ultimate démontre une **architecture de jeu de niveau professionnel** avec :
- **Séparation claire des préoccupations** via MVC
- **Support d'interface double** sans duplication de code
- **Gestion complète des erreurs** et sécurité mémoire
- **Conception extensible** pour les améliorations futures
- **Compatibilité multiplateforme** avec sélection à la compilation

Le projet sert à la fois de **jeu jouable** et de **ressource éducative** pour le développement de jeux en C, montrant des techniques modernes tout en maintenant la compatibilité avec les systèmes minimaux.

**Bon jeu !**