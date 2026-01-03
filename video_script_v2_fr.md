# Script Vidéo : Présentation Technique de Space Invaders MVC
**Durée Cible :** 10 Minutes
**Audience :** Évaluateurs Techniques / Développeurs
**Ton :** Professionnel / Académique

---

## 1. Introduction (0:00 - 1:00)
**[Visuel : Écran titre - Terminal et SDL côte à côte]**

> "Bienvenue dans cette analyse technique de Space Invaders MVC. Ce projet est bien plus qu'un simple jeu ; c'est une démonstration de programmation C11 haute performance, de séparation architecturale stricte et de portabilité extrême."

**[Visuel : Arborescence du projet - Mise en évidence de src/core, src/views, src/controller]**

> "Aujourd'hui, nous allons explorer comment nous avons conçu un moteur multiplateforme capable de passer d'un champ de distorsion 3D haute vitesse sous SDL3 à une interface ASCII à faible latence dans le terminal, le tout propulsé par le même cœur logique."

---

## 2. Gameplay et Mécaniques de Difficulté (1:00 - 3:30)
**[Visuel : Gameplay SDL - Niveau 1]**

> "La boucle principale s'exécute sur une grille logique de 600x600 unités, indépendante du matériel. Cela garantit que, que vous jouiez à 144 FPS ou via une connexion SSH lente, la physique, la détection de collision et l'IA des ennemis restent cohérentes."

**[Visuel : Passage en mode HARD]**

> "Notre mode de difficulté 'Bullet Hell' met en avant la gestion des entités du moteur. En mode Difficile, les envahisseurs ignorent les limites de tir standard par colonne, obligeant le modèle à traiter des centaines de projectiles actifs sans aucune baisse de performance."

**[Visuel : BOSS NIVEAU 4 - Le Mothership]**

> "Au niveau 4, le Mothership apparaît — un boss multi-phase qui suit la position du joueur et utilise des motifs d'attaque procéduraux. Notez les transitions fluides et les phases de santé avec changement de couleur, gérées via une machine à états modulaire."

---

## 3. Architecture : Les 'Deux Mains' et le MVC (3:30 - 5:30)
**[Visuel : Code - main_sdl.c et main_ncurses.c côte à côte]**

> "Un choix de conception crucial a été l'implémentation des 'Deux Mains'. Contrairement aux moteurs monolithiques, le projet utilise des points d'entrée séparés : `main_sdl.c` et `main_ncurses.c`."

**[Visuel : Code - view_base.h montrant les pointeurs de fonction ou l'interface]**

> "Les deux mains partagent les mêmes instances de Contrôleur et de Modèle, mais initialisent des fournisseurs de Vue différents. Cela illustre le véritable polymorphisme en C. Nous ne nous contentons pas de 'basculer' les graphismes ; nous remplaçons tout le moteur de rendu tandis que l'état du jeu reste intact dans son propre espace mémoire."

**[Visuel : Superposition d'un diagramme de séquence montrant la boucle de mise à jour]**

> "Les entrées passent par le Contrôleur, les commandes sont exécutées sur le Modèle, et la Vue lit l'état pour la frame suivante. Pas de dépendances circulaires, pas d'état global partagé."

---

## 4. Innovation Technique : Audio et Miniaudio (5:30 - 7:30)
**[Visuel : Code - platform_sdl.c ou code de chargement audio]**

> "L'un de nos pivots techniques les plus importants a été notre approche de l'audio. Initialement, nous avions envisagé SDL_mixer, mais nous avons rencontré un obstacle industriel classique : l'incompatibilité de version entre SDL3 et ses bibliothèques auxiliaires."

**[Visuel : Texte en superposition - Pourquoi miniaudio ?]**

> "N'ayant pas trouvé de version stable et compatible de SDL_mixer pour tous les environnements ciblés, nous avons transité vers `miniaudio.h`. Cette bibliothèque en un seul fichier en-tête nous permet de nous affranchir des dépendances externes tout en nous offrant un contrôle bas niveau sur les buffers sonores et l'ajustement du volume en temps réel, visible dans notre menu de paramètres."

---

## 5. Portabilité : La Stratégie 3rdParty (7:30 - 9:00)
**[Visuel : Explorateur - dossier 3rdParty]**

> "Pour garantir que ce projet fonctionne immédiatement sur n'importe quelle machine Linux, nous avons mis en place une stratégie de dépendances autonomes. Le dossier `3rdParty` contient l'intégralité du code source de SDL3, SDL_image et SDL_ttf."

**[Visuel : Makefile - défilement des sections check-sdl-deps et LOCAL_SDL_DIR]**

> "Le Makefile est intelligent. Il vérifie d'abord les bibliothèques installées sur le système. Si elles sont absentes ou incompatibles, il déclenche automatiquement une compilation locale dans le répertoire du projet. Cette isolation garantit que le projet reste fonctionnel, que l'hôte possède SDL ou non."

---

## 6. Vérification : Tests et Sécurité Mémoire (9:00 - 9:50)
**[Visuel : Terminal - exécution de `make test`]**

> "La qualité est assurée par le framework de tests unitaires Check. Chaque composant logique critique — de la collision des projectiles à l'échelle de difficulté — est vérifié avant chaque commit."

**[Visuel : Terminal - sortie de `make valgrind-report`]**

> "Enfin, nous maintenons une politique stricte de 'Zéro Fuite'. En utilisant un fichier de suppression personnalisé pour les fuites au niveau des pilotes système, nous confirmons que 100% de la mémoire de notre logique de jeu est gérée en toute sécurité, assurant une stabilité à long terme sans plantage."

---

## 7. Conclusion (9:50 - 10:00)
**[Visuel : Montage final du gameplay SDL et Ncurses]**

> "Space Invaders MVC témoigne de la puissance d'une architecture propre et de choix techniques proactifs. Des dépendances intégrées à l'audio procédural, tout est conçu pour la performance, la portabilité et les standards professionnels. Merci de votre attention."

---

**[Écran de fin : Lien vers le dépôt du projet]**
