#ifndef COMMANDS_H
#define COMMANDS_H

// Commandes disponibles pour le jeu
typedef enum {
    CMD_NONE,
    CMD_MOVE_LEFT,
    CMD_MOVE_RIGHT,
    CMD_SHOOT,
    CMD_PAUSE,
    CMD_START_GAME,
    CMD_QUIT,
    CMD_RESET_GAME,
    CMD_TOGGLE_VIEW,
    CMD_SELECT_OPTION,
    CMD_UP,
    CMD_DOWN,
    CMD_LEFT,
    CMD_RIGHT,
    CMD_CONFIRM,
    CMD_BACK
} Command;

#endif // COMMANDS_H