#ifndef MODEL_H
#define MODEL_H

#include <stdbool.h>
#include <time.h>
#include <stdint.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GAME_AREA_WIDTH 600
#define INVADER_ROWS 5
#define INVADER_COLS 10
#define INVADER_WIDTH 30
#define INVADER_HEIGHT 30
#define BOSS_WIDTH 90
#define BOSS_HEIGHT 45
#define PLAYER_WIDTH 30
#define PLAYER_HEIGHT 20
#define BULLET_WIDTH 5
#define BULLET_HEIGHT 15
#define PLAYER_BULLETS 3
#define ENEMY_BULLETS 10
#define BASE_COUNT 4
#define BASE_WIDTH 60
#define BASE_HEIGHT 40

// Directions
typedef enum {
    DIR_LEFT,
    DIR_RIGHT,
    DIR_STATIONARY
} Direction;

// Color types (Legacy support for Ncurses)
typedef enum {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_PURPLE
} ColorType;

// Extended Game States
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER,
    STATE_LEVEL_TRANSITION,
    STATE_WIN
} GameState;

typedef enum {
    ENTITY_PLAYER,
    ENTITY_INVADER,
    ENTITY_SAUCER,
    ENTITY_BULLET,
    ENTITY_BASE
} EntityType;

typedef struct {
    int x;
    int y;
    int width;
    int height;
} Rect;

// Invader with animation and type support
typedef struct {
    Rect hitbox;
    ColorType color;
    bool alive;
    int points;
    int row;
    int col;
    int type;        // 0=Squid, 1=Crab, 2=Octopus
    int dying_timer; // >0 means exploding
} Invader;

typedef struct {
    Invader invaders[INVADER_ROWS][INVADER_COLS];
    Direction direction;
    float speed;
    int killed;
    int state;             // 0 or 1 for animation frame
    uint32_t state_time;
    int state_speed;
    int shoot_chance;
} InvaderGrid;

// Boss Structure
typedef struct {
    Rect hitbox;
    bool alive;
    int health;
    int max_health;
    Direction direction;
    int speed;
    int shoot_timer;
} Boss;

typedef struct {
    Rect hitbox;
    bool alive;
    Direction direction;
    int points;
} Saucer;

typedef struct {
    Rect hitbox;
    int health;
    bool alive;
} Base;

typedef struct {
    Rect hitbox;
    bool alive;
    bool is_player_bullet;
    int speed;
} Bullet;

typedef struct {
    Rect hitbox;
    int lives;
    int score;
    int level;
    unsigned int shots_fired;
} Player;

// The Complete Game Model
typedef struct {
    Player player;
    InvaderGrid invaders;
    Boss boss;
    Saucer saucer;
    Base bases[BASE_COUNT];
    Bullet player_bullets[PLAYER_BULLETS];
    Bullet enemy_bullets[ENEMY_BULLETS];
    GameState state;
    uint32_t game_time;
    bool needs_redraw;
    int high_score;
} GameModel;

// Initialization
void model_init(GameModel* model);
void model_reset_game(GameModel* model);
void model_next_level(GameModel* model);

// Updates
void model_update(GameModel* model, float delta_time);
void model_process_command(GameModel* model, int command, void* data);

// Entity Management
void model_move_player(GameModel* model, Direction dir);
void model_player_shoot(GameModel* model);
void model_update_invaders(GameModel* model, float delta_time);
void model_update_bullets(GameModel* model, float delta_time);
void model_update_saucer(GameModel* model, float delta_time);
void model_update_boss(GameModel* model);

// Collisions
bool model_check_collision(Rect a, Rect b);
void model_check_bullet_collisions(GameModel* model);
void model_check_invader_base_collisions(GameModel* model);
void model_check_player_invader_collision(GameModel* model);

// State Management
void model_set_state(GameModel* model, GameState state);
void model_toggle_pause(GameModel* model);
void model_save_high_score(GameModel* model);
void model_load_high_score(GameModel* model);

// Getters
int model_get_score(const GameModel* model);
int model_get_lives(const GameModel* model);
int model_get_level(const GameModel* model);
GameState model_get_state(const GameModel* model);

#endif // MODEL_H