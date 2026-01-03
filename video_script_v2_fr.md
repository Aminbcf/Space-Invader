# Script Vidéo : Projet Space Invaders MVC (Version Simplifiée)
**Durée prévue :** ~10 Minutes
**Langue :** Français simple et clair

---

## 1. Introduction (0:00 - 1:00)
**[Visuel : Jeu qui tourne en SDL et Terminal côte à côte]**

> "Bonjour à tous. Aujourd'hui, je vais vous présenter mon projet Space Invaders. Ce n'est pas juste un jeu, c'est un projet de programmation solide fait en langage C. Le but était de créer un jeu qui fonctionne aussi bien dans un terminal noir et blanc que dans une fenêtre graphique avec de belles couleurs."

---

## 2. L'Architecture MVC (1:00 - 2:30)
**[Visuel : DIAGRAMME DE CLASSES (Class Diagram)]**

> "Pour organiser le code, j'ai utilisé ce qu'on appelle l'architecture MVC. Regardez ce schéma :
> - **Le Modèle (Model) :** C'est le cerveau du jeu. Il gère les calculs, les positions des ennemis et les collisions. Il ne sait pas s'il y a des images ou du texte.
> - **La Vue (View) :** C'est ce que vous voyez. On a deux vues : une vue graphique avec SDL et une vue texte avec Ncurses.
> - **Le Contrôleur (Controller) :** C'est lui qui écoute vos touches sur le clavier pour faire bouger le vaisseau."

---

## 3. Les Cas d'Utilisation (2:30 - 3:30)
**[Visuel : DIAGRAMME DE CAS D'UTILISATION (Use Case Diagram)]**

> "Ici, on voit comment le joueur interagit avec le programme. On peut lancer une partie, choisir la difficulté, et même changer les touches de commande dans les options. Le système est fait pour répondre simplement à chaque action de l'utilisateur."

---

## 4. Deux Vues, Un Seul Cerveau (3:30 - 5:00)
**[Visuel : Code des fichiers main_sdl.c et main_ncurses.c]**

> "Le point fort du projet, c'est qu'on a 'Deux Mains'. On a deux fichiers principaux différents. Un pour la version graphique et un pour la version terminal. Mais les deux utilisent EXACTEMENT le même 'cerveau' (le Modèle). C'est ce qu'on appelle la séparation du code : on peut changer l'apparence sans casser les règles du jeu."

---

## 5. Le Cycle du Jeu (5:00 - 6:30)
**[Visuel : DIAGRAMME DE SÉQUENCE (Sequence Diagram)]**

> "Voyons comment le jeu tourne en boucle. Ce diagramme montre l'ordre des actions :
> 1. On lit ce que le joueur a pressé (Contrôleur).
> 2. On met à jour les positions et les dégâts (Modèle).
> 3. On redessine tout à l'écran (Vue).
> Cela se répète très vite pour que le jeu soit fluide."

---

## 6. Choix Techniques : Son et Portabilité (6:30 - 8:30)
**[Visuel : Dossier '3rdParty' et code de l'audio]**

> "J'ai fait des choix pour que le projet soit facile à utiliser :
> - **Audio :** J'utilise une petite bibliothèque appelée `miniaudio`. Pourquoi ? Parce qu'elle est simple et qu'elle n'a pas besoin d'installations compliquées.
> - **Portabilité :** J'ai mis toutes les bibliothèques nécessaires dans un dossier appelé `3rdParty`. Si vous téléchargez le projet, vous pouvez le compiler tout de suite sans rien installer d'autre. Tout est déjà là."

---

## 7. Qualité et Sécurité (8:30 - 9:30)
**[Visuel : Terminal avec 'make test' et 'make valgrind-report']**

> "Pour m'assurer que le jeu ne plante pas, j'ai fait deux choses :
> - **Tests :** J'ai écrit des tests automatiques qui vérifient chaque règle du jeu.
> - **Mémoire :** J'ai utilisé un outil appelé Valgrind pour vérifier qu'il n'y a pas de fuites de mémoire. Le jeu est propre et stable."

---

## 8. Conclusion (9:30 - 10:00)
**[Visuel : Montage rapide du gameplay]**

> "En résumé, ce Space Invaders est un exemple de code propre et organisé. C'est robuste, portable et facile à faire évoluer. Merci de m'avoir écouté !"

---
**[Écran de fin : 'Merci !']**
