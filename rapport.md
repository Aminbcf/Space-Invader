---
title: "Space Invaders MVC - Rapport de Projet"
author: "Amine Boucif"
date: "Janvier 2026"
lang: fr-FR
geometry: margin=2.5cm
---

# 1. Introduction & Objectifs

Ce projet présente une réimplémentation moderne du jeu d'arcade *Space Invaders* en langage C (C11), conçue autour de l'architecture **Modèle-Vue-Contrôleur (MVC)**. L'objectif principal était de démontrer une séparation stricte des responsabilités, permettant au même code logique (Modèle) de piloter deux interfaces radicalement différentes : une interface graphique riche via **SDL3** et une interface textuelle via **ncurses**.

Les objectifs pédagogiques incluent la maîtrise de la gestion mémoire (vérifiée par Valgrind), la compilation modulaire (Makefile), et l'intégration de systèmes complexes comme l'audio procédural et la gestion d'états.

# 2. Architecture Logicielle (MVC)

L'architecture est le cœur du projet, garantissant modularité et extensibilité.

## 2.1 Schéma Structurel
Le Modèle est totalement indépendant de la Vue et du Contrôleur. La Vue observe le Modèle en lecture seule, et le Contrôleur modifie le Modèle via des actions abstraites.

```
[ UTILISATEUR ]
      | (Inputs)
      v
[ CONTRÔLEUR ] ----------------> [ MODÈLE ] <---------------- [ VUE ]
(Translate Inputs -> Commands)   (Logique/État)          (SDL3 / Ncurses)
```

## 2.2 Composants
*   **Modèle (`src/core/`)**: Contient toute la logique de jeu, la physique, les collisions, et l'état des entités (Joueur, Ennemis, Boss). Il ne contient **aucun code spécifique à l'affichage**.
*   **Vue (`src/views/`)**: Utilise une interface abstraite (`View` struct) permettre le polymorphisme.
    *   *SDL3*: Rendu matériel, sprites, effets de fond "Warp Speed", audio.
    *   *Ncurses*: Rendu ASCII fluide, optimisé pour terminaux.
*   **Contrôleur (`src/controller/`)**: Intercepte les événements clavier et les traduit en commandes sémantiques (`CMD_SHOOT`, `CMD_MOVE_LEFT`), découplant les touches physiques de la logique.

# 3. Fonctionnalités Avancées

## 3.1 Gameplay & Boss System
Le jeu propose trois niveaux de difficulté (Easy, Normal, Hard) influençant la vitesse, le score et les ennemis.
*   **Boss Dreadnought** (Niveau 4): Une entité complexe avec patterns de tir multiples et changements visuels dynamiques selon ses points de vie.
*   **Système de Score**: Bonus multiplicateurs selon la difficulté (x2 en Hard).

## 3.2 Moteur Audio Procédural
L'audio n'est pas statique ; les pistes musicales (Menu, Jeu, Boss, Victoire) sont générées algorithmiquement via Python et intégrées via `miniaudio`, assurant des transitions fluides sans interruption du gameplay.

## 3.3 Graphismes & Effets
*   **SDL**: Implémentation d'un effet "Warp Speed" radial 3D pour le fond, donnant une impression de vitesse, et gestion des sprites avec transparence.
*   **Ncurses**: Algorithme de lissage des entrées pour compenser le délai de répétition des touches terminal.

## 3.4 Personnalisation
Un menu de paramètres complet permet la **reconfiguration totale des touches** (Keybindings) pour les deux vues, sauvegardées en mémoire.

# 4. Défis Techniques & Solutions

## 4.1 Gestion Mémoire (Zero Leaks)
Une attention particulière a été portée à la sécurité mémoire. Chaque allocation est suivie, et l'utilisation de **Valgrind** est intégrée au pipeline de test (`make valgrind-sdl`). Le projet atteint le standard "Zero Leaks" même lors de crashs simulés ou de fermetures abruptes.

## 4.2 Synchronisation Audio/Vidéo
Le défi majeur fut de synchroniser les événements sonores (tirs, impacts) sans que le Modèle n'appelle de fonctions audio.
*   *Solution*: La Vue surveille les changements de variables d'état (ex: `shots_fired` increment) à chaque frame pour déclencher les sons, préservant l'isolation du Modèle.

## 4.3 Double Rendu (SDL/Ncurses)
Faire cohabiter SDL et Ncurses dans le même binaire a nécessité une abstraction rigoureuse de la boucle principale et une gestion fine des signaux système pour l'interface terminal.

# 5. Conclusion

*Space Invaders MVC* démontre qu'une architecture rigoureuse permet de créer des logiciels robustes et portables. Le projet réussit à combiner des concepts bas niveau (gestion mémoire C) avec des abstractions haut niveau (MVC, Audio Procédural), aboutissant à un jeu complet, performant et bug-free.
