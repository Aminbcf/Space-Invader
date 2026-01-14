#ifndef MODEL_H
#define MODEL_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

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
#define PLAYER_BULLETS 20
#define ENEMY_BULLETS 10
#define BIG_INVADER_WIDTH 60
#define BIG_INVADER_HEIGHT 50

// Directions
typedef enum {
  DIR_LEFT,
  DIR_RIGHT,
  DIR_UP,
  DIR_DOWN,
  DIR_STATIONARY
} Direction;

// Color types (Legacy support for Ncurses)
typedef enum {
  COLOR_RED,
  COLOR_GREEN,
  COLOR_PURPLE,
  COLOR_YELLOW,
  COLOR_CYAN,
  COLOR_WHITE,
  COLOR_ORANGE,
  COLOR_PINK
} ColorType;

typedef enum {
  PWR_NONE,
  PWR_TRIPLE_SHOT,
  PWR_STRONG_MISSILE,
  PWR_SHIELD,
  PWR_MAX
} PowerUpType;

// Extended Game States
typedef enum {
  STATE_MENU,
  STATE_PLAYING,
  STATE_PAUSED,
  STATE_GAME_OVER,
  STATE_LEVEL_TRANSITION,
  STATE_WIN,
  STATE_QUIT
} GameState;

// Difficulty Levels
typedef enum {
  DIFFICULTY_EASY,
  DIFFICULTY_NORMAL,
  DIFFICULTY_HARD,
  DIFFICULTY_ROGUE
} Difficulty;

// Menu States
typedef enum {
  MENU_MAIN,
  MENU_DIFFICULTY,
  MENU_SETTINGS,
  MENU_CONTROLS
} MenuState;

typedef enum {
  ENTITY_PLAYER,
  ENTITY_INVADER,
  ENTITY_SAUCER,
  ENTITY_BULLET
} EntityType;

typedef struct {
  float x;
  float y;
  float width;
  float height;
} Rect;

// Invader with animation and type support
typedef struct {
  Rect hitbox;
  ColorType color;
  bool alive;
  int points;
  int row;
  int col;
  int type;             // 0=Squid, 1=Crab, 2=Octopus, 3=Big Invader
  int dying_timer;      // >0 means exploding
  int health;           // For big invaders only
  float speed_modifier; // Individual speed for special invaders
} Invader;

// Big Invader - special slow powerful enemy
typedef struct {
  Rect hitbox;
  bool alive;
  int health;
  int max_health;
  int points;
  Direction direction;
  float speed;
  float shoot_timer;
  int attack_type; // 0=Big Slow Shot, 1=Spread Shot
} BigInvader;

typedef struct {
  Invader invaders[INVADER_ROWS][INVADER_COLS];
  Direction direction;
  float speed;
  int killed;
  int state; // 0 or 1 for animation frame
  uint32_t state_time;
  int state_speed;
  int shoot_chance;
  BigInvader big_invader; // Special big slow enemy
  float big_invader_spawn_timer;
} InvaderGrid;

// Boss Structure
typedef struct {
  Rect hitbox;
  bool alive;
  int health;
  int max_health;
  Direction direction;
  float speed_x;
  float speed_y;
  int shoot_timer;
  int anim_frame;     // Animation frame (0 or 1)
  int anim_counter;   // Counter for animation timing
  int attack_pattern; // 0=Horizontal, 1=Circular
  float attack_timer;
} Boss;

typedef struct {
  Rect hitbox;
  bool alive;
  Direction direction;
  int points;
} Saucer;

typedef struct {
  Rect hitbox;
  bool alive;
  bool is_player_bullet;
  int player_id; // 0 or 1
  float speed_x;
  float speed_y;
  bool is_strong;
  int type; // 0=Standard, 1=ZigZag, 2=Laser
} Bullet;

typedef struct {
  Rect hitbox;
  PowerUpType type;
  float speed_y;
  bool alive;
} PowerUp;

typedef struct {
  Rect hitbox;
  int lives;
  int score;
  int level;
  unsigned int shots_fired;
  int combo_count;
  uint32_t last_kill_time;
  bool is_active;
  int player_id;

  // Power-ups
  PowerUpType active_powerup;
  float powerup_timer;

  // Damage immunity
  float invincibility_timer;
  
  // Shooting Cooldown
  float shoot_timer;
} Player;

// The Complete Game Model
typedef struct {
  Player players[2];
  InvaderGrid invaders;
  Boss boss;
  Saucer saucer;
  Bullet player_bullets[2][PLAYER_BULLETS];
  Bullet enemy_bullets[ENEMY_BULLETS];
  PowerUp powerups[10];
  GameState state;
  Difficulty difficulty;
  MenuState menu_state;
  int menu_selection; // Current selected menu item
  float music_volume; // 0.0 to 1.0
  uint32_t game_time;
  bool needs_redraw;
  int high_score;
  bool two_player_mode;
  uint32_t last_update_time;
  float win_timer; // Timer for auto-return to menu after win

  // Keybindings (key codes) - P1: left, right, up, down, shoot; P2: same
  int keybinds_p1[5]; // left, right, up, down, shoot
  int keybinds_p2[5];
  int editing_keybind; // -1 = not editing, 0-4 = P1, 5-9 = P2
  bool waiting_for_key;
} GameModel;

// Initialization
void model_init(GameModel *model);
void model_reset_game(GameModel *model);
void model_next_level(GameModel *model);

// Updates
void model_update(GameModel *model, float delta_time);
void model_process_command(GameModel *model, int command, void *data);

// Entity Management
void model_move_player(GameModel *model, int player_id, Direction dir);
void model_player_shoot(GameModel *model, int player_id);
void model_update_invaders(GameModel *model, float delta_time);
void model_update_bullets(GameModel *model, float delta_time);
void model_update_saucer(GameModel *model, float delta_time);
void model_update_boss(GameModel *model, float delta_time);

// Collisions
bool model_check_collision(Rect a, Rect b);
void model_check_bullet_collisions(GameModel *model);
void model_check_player_invader_collision(GameModel *model);

// State Management
void model_set_state(GameModel *model, GameState state);
void model_toggle_pause(GameModel *model);
void model_save_high_score(GameModel *model);
void model_load_high_score(GameModel *model);

// Getters
int model_get_score(const GameModel *model);
int model_get_lives(const GameModel *model);
int model_get_level(const GameModel *model);
GameState model_get_state(const GameModel *model);

// Menu and Difficulty
void model_process_menu_input(GameModel *model,
                              int direction); // -1=up, 1=down, 0=select
void model_apply_difficulty(GameModel *model);
void model_adjust_music_volume(GameModel *model,
                               int direction); // -1=decrease, 1=increase
int model_get_max_menu_items(const GameModel *model);
void model_set_keybind(GameModel *model, int keycode);

#endif // MODEL_H