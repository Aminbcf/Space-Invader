#include "model.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils/platform.h"

// --- Helper Init Functions ---

static void init_player(Player* player) {
    player->hitbox.width = PLAYER_WIDTH;
    player->hitbox.height = PLAYER_HEIGHT;
    player->hitbox.x = (GAME_AREA_WIDTH / 2) - (PLAYER_WIDTH / 2);
    player->hitbox.y = SCREEN_HEIGHT - (PLAYER_HEIGHT + 20);
    player->lives = 3;
    player->score = 0;
    player->level = 1;
    player->shots_fired = 0;
}

// Reset position ONLY (keeps score/lives)
static void reset_player_pos(Player* player) {
    player->hitbox.x = (GAME_AREA_WIDTH / 2) - (PLAYER_WIDTH / 2);
    player->hitbox.y = SCREEN_HEIGHT - (PLAYER_HEIGHT + 20);
}

static void init_boss(Boss* boss) {
    boss->alive = false;
    boss->hitbox.width = BOSS_WIDTH;
    boss->hitbox.height = BOSS_HEIGHT;
    boss->hitbox.x = (GAME_AREA_WIDTH / 2) - (BOSS_WIDTH / 2);
    boss->hitbox.y = 60;
    boss->max_health = 50;
    boss->health = 50;
    boss->direction = DIR_RIGHT;
    boss->speed = 3;
    boss->shoot_timer = 0;
    boss->anim_frame = 0;
    boss->anim_counter = 0;
}

static void init_invaders(InvaderGrid* invaders, int level) {
    invaders->direction = DIR_RIGHT;
    invaders->speed = 0.5f + (level * 0.5f);
    invaders->state = 0;
    invaders->killed = 0;
    invaders->state_speed = (level > 3) ? 200 : (1000 - (level * 150));
    invaders->state_time = platform_get_ticks();
    invaders->shoot_chance = (level > 2) ? 40 : (120 - (level * 20));

    for (int i = 0; i < INVADER_ROWS; i++) {
        for (int j = 0; j < INVADER_COLS; j++) {
            invaders->invaders[i][j].hitbox.x = j * (INVADER_WIDTH + 15);
            invaders->invaders[i][j].hitbox.y = 40 + i * (INVADER_HEIGHT + 10);
            invaders->invaders[i][j].hitbox.width = INVADER_WIDTH;
            invaders->invaders[i][j].hitbox.height = INVADER_HEIGHT;
            invaders->invaders[i][j].alive = true;
            invaders->invaders[i][j].dying_timer = 0;
            invaders->invaders[i][j].row = i;
            invaders->invaders[i][j].col = j;
            
            // Types & Points
            if (i == 0) { invaders->invaders[i][j].type = 2; invaders->invaders[i][j].points = 30; }
            else if (i < 3) { invaders->invaders[i][j].type = 1; invaders->invaders[i][j].points = 20; }
            else { invaders->invaders[i][j].type = 0; invaders->invaders[i][j].points = 10; }
        }
    }
}

static void init_bases(Base bases[]) {
    int spacing = GAME_AREA_WIDTH / BASE_COUNT;
    for (int i = 0; i < BASE_COUNT; i++) {
        bases[i].hitbox.width = BASE_WIDTH;
        bases[i].hitbox.height = BASE_HEIGHT;
        bases[i].hitbox.x = (spacing * i) + (spacing / 2) - (BASE_WIDTH / 2);
        bases[i].hitbox.y = SCREEN_HEIGHT - 100;
        bases[i].health = 100;
        bases[i].alive = true;
    }
}

static void init_bullets(Bullet bullets[], int count, bool is_player) {
    for (int i = 0; i < count; i++) {
        bullets[i].hitbox.width = BULLET_WIDTH;
        bullets[i].hitbox.height = BULLET_HEIGHT;
        bullets[i].alive = false;
        bullets[i].is_player_bullet = is_player;
        bullets[i].speed = is_player ? -12 : 6;
    }
}

static void init_saucer(Saucer* saucer) {
    saucer->hitbox.width = 30; saucer->hitbox.height = 20;
    saucer->alive = false; saucer->direction = DIR_RIGHT; saucer->points = 0;
}

// --- Public Init ---

void model_init(GameModel* model) {
    memset(model, 0, sizeof(GameModel));
    
    // Initialize game entities
    init_player(&model->player);
    init_invaders(&model->invaders, 1);
    init_boss(&model->boss);
    init_bases(model->bases);
    init_bullets(model->player_bullets, PLAYER_BULLETS, true);
    init_bullets(model->enemy_bullets, ENEMY_BULLETS, false);
    init_saucer(&model->saucer);
    
    // Game state
    model->state = STATE_MENU;
    model->difficulty = DIFFICULTY_NORMAL;
    model->menu_state = MENU_MAIN;
    model->menu_selection = 0;
    model->music_volume = 0.5f;  // 50% volume by default
    model->game_time = platform_get_ticks();
    model->needs_redraw = true;
    model_load_high_score(model);
}

void model_reset_game(GameModel* model) {
    model_init(model);
    model->state = STATE_PLAYING;
}

void model_next_level(GameModel* model) {
    model->player.level++;
    
    // Check win condition based on difficulty
    int max_level = (model->difficulty == DIFFICULTY_EASY) ? 3 : 4;
    
    // RESET POSITION (User Request)
    reset_player_pos(&model->player);
    
    // Clear bullets
    init_bullets(model->player_bullets, PLAYER_BULLETS, true);
    init_bullets(model->enemy_bullets, ENEMY_BULLETS, false);
    
    // Easy difficulty: Win after level 3 (no boss)
    if (model->difficulty == DIFFICULTY_EASY && model->player.level > 3) {
        model->state = STATE_WIN;
        return;
    }
    
    if (model->player.level == 4 && model->difficulty != DIFFICULTY_EASY) {
        // Level 4: BOSS FIGHT (not in Easy mode)
        init_boss(&model->boss);
        model->boss.alive = true;
        // Kill regular invaders
        for(int i=0; i<INVADER_ROWS; i++)
            for(int j=0; j<INVADER_COLS; j++)
                model->invaders.invaders[i][j].alive = false;
    } 
    else if (model->player.level > max_level) {
        model->state = STATE_WIN;
        return;
    }
    else {
        // Standard Level
        init_invaders(&model->invaders, model->player.level);
        model_apply_difficulty(model);  // CRITICAL: Apply difficulty modifiers
    }
    
    model->state = STATE_LEVEL_TRANSITION;
    model->needs_redraw = true;
}

// --- Updates ---

void model_update_boss(GameModel* model) {
    if (!model->boss.alive) return;
    Boss* boss = &model->boss;
    
    // Animation
    boss->anim_counter++;
    if (boss->anim_counter >= 15) {  // Change frame every 15 updates (~250ms at 60fps)
        boss->anim_counter = 0;
        boss->anim_frame = (boss->anim_frame == 0) ? 1 : 0;
    }
    
    // Movement
    if (boss->direction == DIR_RIGHT) {
        boss->hitbox.x += boss->speed;
        if (boss->hitbox.x + boss->hitbox.width >= GAME_AREA_WIDTH) boss->direction = DIR_LEFT;
    } else {
        boss->hitbox.x -= boss->speed;
        if (boss->hitbox.x <= 0) boss->direction = DIR_RIGHT;
    }
    
    // Shooting
    boss->shoot_timer++;
    if (boss->shoot_timer > 30) {
        boss->shoot_timer = 0;
        for (int i = 0; i < ENEMY_BULLETS; i++) {
            if (!model->enemy_bullets[i].alive) {
                model->enemy_bullets[i].alive = true;
                model->enemy_bullets[i].hitbox.x = boss->hitbox.x + (rand() % boss->hitbox.width);
                model->enemy_bullets[i].hitbox.y = boss->hitbox.y + boss->hitbox.height;
                model->enemy_bullets[i].speed = 9;
                break;
            }
        }
    }
}

void model_update_invaders(GameModel* model, float delta_time) {
    (void)delta_time;
    InvaderGrid* g = &model->invaders;
    
    // 1. Update Animation State (Wiggle)
    if (platform_get_ticks() > g->state_time + g->state_speed) {
        g->state_time = platform_get_ticks();
        g->state = !g->state;
        model->needs_redraw = true;
    }
    
    bool hit_edge = false;
    int speed = (int)g->speed;
    if (g->direction == DIR_LEFT) speed = -speed;
    
    for (int i=0; i<INVADER_ROWS; i++) {
        for (int j=0; j<INVADER_COLS; j++) {
            Invader* inv = &g->invaders[i][j];

            if (inv->alive && inv->dying_timer > 0) {
                inv->dying_timer--;
                if (inv->dying_timer <= 0) {
                    inv->alive = false; // Explosion finished, remove invader
                }
                continue; // Don't move while exploding
            }


            // Move only if alive and NOT exploding
            if (inv->alive && inv->dying_timer == 0) {
                inv->hitbox.x += speed;
                if (g->direction == DIR_RIGHT && inv->hitbox.x + INVADER_WIDTH >= GAME_AREA_WIDTH) hit_edge = true;
                if (g->direction == DIR_LEFT && inv->hitbox.x <= 0) hit_edge = true;
            }
        }
    }
    
    if (hit_edge) {
        g->direction = (g->direction == DIR_RIGHT) ? DIR_LEFT : DIR_RIGHT;
        for (int i=0; i<INVADER_ROWS; i++) {
            for (int j=0; j<INVADER_COLS; j++) {
                if(g->invaders[i][j].alive) g->invaders[i][j].hitbox.y += 10;
            }
        }
    }
}

void model_update_bullets(GameModel* model, float delta_time) {
    (void)delta_time;
    for (int i = 0; i < PLAYER_BULLETS; i++) {
        if (model->player_bullets[i].alive) {
            model->player_bullets[i].hitbox.y += model->player_bullets[i].speed;
            if (model->player_bullets[i].hitbox.y < 0) model->player_bullets[i].alive = false;
        }
    }
    for (int i = 0; i < ENEMY_BULLETS; i++) {
        if (model->enemy_bullets[i].alive) {
            model->enemy_bullets[i].hitbox.y += model->enemy_bullets[i].speed;
            if (model->enemy_bullets[i].hitbox.y > SCREEN_HEIGHT) model->enemy_bullets[i].alive = false;
        }
    }
}

void model_update_saucer(GameModel* model, float delta_time) {
    (void)delta_time;
    if (!model->saucer.alive) return;
    
    int move = 5;
    if (model->saucer.direction == DIR_LEFT) move = -5;
    
    model->saucer.hitbox.x += move;
    if (model->saucer.hitbox.x < -40 || model->saucer.hitbox.x > GAME_AREA_WIDTH + 40) {
        model->saucer.alive = false;
    }
}

void model_update(GameModel* model, float delta_time) {
    if (model->state != STATE_PLAYING) return;
    
    if (model->player.level == 4) model_update_boss(model);
    else model_update_invaders(model, delta_time);
    
    model_update_bullets(model, delta_time);
    model_update_saucer(model, delta_time);
    
    model_check_bullet_collisions(model);
    model_check_player_invader_collision(model);
    model_check_invader_base_collisions(model);
    
    // Enemy Shooting (only levels 1-3)
    if (model->player.level != 4 && (rand() % model->invaders.shoot_chance == 0)) {
        int col = rand() % INVADER_COLS;
        for (int row = INVADER_ROWS - 1; row >= 0; row--) {
            if (model->invaders.invaders[row][col].alive) {
                for (int b = 0; b < ENEMY_BULLETS; b++) {
                    if (!model->enemy_bullets[b].alive) {
                        model->enemy_bullets[b].alive = true;
                        model->enemy_bullets[b].hitbox.x = model->invaders.invaders[row][col].hitbox.x + 10;
                        model->enemy_bullets[b].hitbox.y = model->invaders.invaders[row][col].hitbox.y + 30;
                        model->enemy_bullets[b].speed = 5 + model->player.level;
                        break;
                    }
                }
                break;
            }
        }
    }

    // Saucer
    if (!model->saucer.alive && model->state == STATE_PLAYING) {
        if (rand() % 1200 == 0) {
            model->saucer.alive = true;
            model->saucer.direction = (rand() % 2 == 0) ? DIR_RIGHT : DIR_LEFT;
            model->saucer.hitbox.x = (model->saucer.direction == DIR_RIGHT) ? -30 : GAME_AREA_WIDTH;
            model->saucer.hitbox.y = 40;
        }
    }
}

void model_move_player(GameModel* model, Direction dir) {
    int speed = 15; 
    if (dir == DIR_LEFT && model->player.hitbox.x > 0) 
        model->player.hitbox.x -= speed;
    if (dir == DIR_RIGHT && model->player.hitbox.x < GAME_AREA_WIDTH - PLAYER_WIDTH) 
        model->player.hitbox.x += speed;
    model->needs_redraw = true;
}

void model_player_shoot(GameModel* model) {
    for (int i = 0; i < PLAYER_BULLETS; i++) {
        if (!model->player_bullets[i].alive) {
            model->player_bullets[i].alive = true;
            model->player_bullets[i].hitbox.x = model->player.hitbox.x + (PLAYER_WIDTH/2) - (BULLET_WIDTH/2);
            model->player_bullets[i].hitbox.y = model->player.hitbox.y;
            model->player.shots_fired++;
            break;
        }
    }
}

// --- Collisions ---

bool model_check_collision(Rect a, Rect b) {
    return (a.x < b.x + b.width && a.x + a.width > b.x &&
            a.y < b.y + b.height && a.y + a.height > b.y);
}

// Helper function to calculate score with difficulty multiplier
static int apply_difficulty_multiplier(GameModel* model, int base_score) {
    switch(model->difficulty) {
        case DIFFICULTY_EASY:
            return base_score;  // 1.0x multiplier
        case DIFFICULTY_NORMAL:
            return (int)(base_score * 1.5f);  // 1.5x multiplier
        case DIFFICULTY_HARD:
            return base_score * 2;  // 2.0x multiplier
        default:
            return base_score;
    }
}

void model_check_bullet_collisions(GameModel* model) {
    // Player Bullets
    for (int b = 0; b < PLAYER_BULLETS; b++) {
        if (!model->player_bullets[b].alive) continue;
        
        // Vs Boss
        if (model->player.level == 4 && model->boss.alive) {
            if (model_check_collision(model->player_bullets[b].hitbox, model->boss.hitbox)) {
                model->player_bullets[b].alive = false;
                model->boss.health--;
                model->player.score += apply_difficulty_multiplier(model, 5);
                if (model->boss.health <= 0) {
                    model->boss.alive = false;
                    model->player.score += apply_difficulty_multiplier(model, 5000);
                    model_next_level(model);
                }
                continue;
            }
        }
        
        // Vs Saucer
        if (model->saucer.alive && model_check_collision(model->player_bullets[b].hitbox, model->saucer.hitbox)) {
            model->saucer.alive = false;
            model->player_bullets[b].alive = false;
            model->player.score += apply_difficulty_multiplier(model, 200 + (rand() % 100));
            continue;
        }

        // Vs Invaders
        if (model->player.level != 4) {
            for (int i = 0; i < INVADER_ROWS; i++) {
                for (int j = 0; j < INVADER_COLS; j++) {
                    Invader* inv = &model->invaders.invaders[i][j];
                    if (inv->alive && inv->dying_timer == 0) {
                        if (model_check_collision(model->player_bullets[b].hitbox, inv->hitbox)) {
                            model->player_bullets[b].alive = false;
                            inv->dying_timer = 5;
                            model->player.score += apply_difficulty_multiplier(model, inv->points);
                            model->invaders.killed++;
                            if (model->invaders.killed >= INVADER_ROWS * INVADER_COLS) {
                                model_next_level(model);
                            }
                            goto next_bullet;
                        }
                    } 
                } 
            } 
        } 
        
        next_bullet:;
    }
    
    // Enemy Bullets vs Player
    for (int b = 0; b < ENEMY_BULLETS; b++) {
        if (!model->enemy_bullets[b].alive) continue;
        if (model_check_collision(model->enemy_bullets[b].hitbox, model->player.hitbox)) {
            model->enemy_bullets[b].alive = false;
            model->player.lives--;
            if (model->player.lives <= 0) {
                model->state = STATE_GAME_OVER;
                model_save_high_score(model);
            }
        }
    }
}

void model_check_player_invader_collision(GameModel* model) {
    if (model->player.level == 4) return;
    for (int i=0; i<INVADER_ROWS; i++) {
        for (int j=0; j<INVADER_COLS; j++) {
            if (model->invaders.invaders[i][j].alive) {
                if (model->invaders.invaders[i][j].hitbox.y + INVADER_HEIGHT >= model->player.hitbox.y) {
                    model->player.lives = 0;
                    model->state = STATE_GAME_OVER;
                }
            }
        }
    }
}

void model_check_invader_base_collisions(GameModel* model) {
    if (model->player.level == 4) return;
    for (int i=0; i<INVADER_ROWS; i++) {
        for (int j=0; j<INVADER_COLS; j++) {
            if (model->invaders.invaders[i][j].alive) {
                for(int b=0; b<BASE_COUNT; b++) {
                    if(model->bases[b].alive && model_check_collision(model->invaders.invaders[i][j].hitbox, model->bases[b].hitbox)) {
                        model->bases[b].alive = false;
                    }
                }
            }
        }
    }
}

// Getters/Setters
void model_set_state(GameModel* model, GameState state) { model->state = state; }
void model_toggle_pause(GameModel* model) { 
    if(model->state == STATE_PLAYING) model->state = STATE_PAUSED;
    else if(model->state == STATE_PAUSED) model->state = STATE_PLAYING;
}
void model_save_high_score(GameModel* model) {
    if(model->player.score > model->high_score) {
        model->high_score = model->player.score;
        FILE* f = fopen("highscore.dat", "wb");
        if(f) { fwrite(&model->high_score, 4, 1, f); fclose(f); }
    }
}
void model_load_high_score(GameModel* model) {
    FILE* f = fopen("highscore.dat", "rb");
    if(f) { fread(&model->high_score, 4, 1, f); fclose(f); }
    else model->high_score = 0;
}
int model_get_score(const GameModel* model) { return model->player.score; }
int model_get_lives(const GameModel* model) { return model->player.lives; }
int model_get_level(const GameModel* model) { return model->player.level; }
GameState model_get_state(const GameModel* model) { return model->state; }
void model_process_command(GameModel* model, int command, void* data) { (void)model; (void)command; (void)data; }
// --- Menu and Difficulty Functions ---

int model_get_max_menu_items(const GameModel* model) {
    switch(model->menu_state) {
        case MENU_MAIN: return 3;        // Start, Settings, Quit
        case MENU_DIFFICULTY: return 3;  // Easy, Normal, Hard
        case MENU_SETTINGS: return 3;    // Controls, Music Volume, Back
        case MENU_CONTROLS: return 1;    // Back
        default: return 1;
    }
}

void model_process_menu_input(GameModel* model, int direction) {
    int max_items = model_get_max_menu_items(model);
    
    if (direction == -1) {  // Up
        model->menu_selection--;
        if (model->menu_selection < 0) model->menu_selection = max_items - 1;
    }
    else if (direction == 1) {  // Down
        model->menu_selection++;
        if (model->menu_selection >= max_items) model->menu_selection = 0;
    }
    else if (direction == 0) {  // Select
        switch(model->menu_state) {
            case MENU_MAIN:
                if (model->menu_selection == 0) {  // Start
                    model->menu_state = MENU_DIFFICULTY;
                    model->menu_selection = 1;  // Default to Normal
                }
                else if (model->menu_selection == 1) {  // Settings
                    model->menu_state = MENU_SETTINGS;
                    model->menu_selection = 0;
                }
                else if (model->menu_selection == 2) {  // Quit
                    model->state = STATE_QUIT;
                }
                break;
                
            case MENU_DIFFICULTY:
                model->difficulty = (Difficulty)model->menu_selection;
                model_reset_game(model);
                model->state = STATE_PLAYING;
                model_apply_difficulty(model);  // Apply after reset
                break;
                
            case MENU_SETTINGS:
                if (model->menu_selection == 0) {  // Controls
                    model->menu_state = MENU_CONTROLS;
                    model->menu_selection = 0;
                }
                else if (model->menu_selection == 2) {  // Back
                    model->menu_state = MENU_MAIN;
                    model->menu_selection = 0;
                }
                break;
                
            case MENU_CONTROLS:
                // Back to settings
                model->menu_state = MENU_SETTINGS;
                model->menu_selection = 0;
                break;
        }
    }
    model->needs_redraw = true;
}

void model_apply_difficulty(GameModel* model) {
    int level = model->player.level;
    
    switch(model->difficulty) {
        case DIFFICULTY_EASY:
            // Easy: 3 levels (no boss), normal speeds
            model->invaders.speed = 0.5f + (level * 0.5f);
            model->invaders.shoot_chance = (level > 2) ? 40 : (120 - (level * 20));
            break;
            
        case DIFFICULTY_NORMAL:
            // Normal: 4 levels with boss, default speeds
            model->invaders.speed = 0.5f + (level * 0.5f);
            model->invaders.shoot_chance = (level > 2) ? 40 : (120 - (level * 20));
            break;
            
        case DIFFICULTY_HARD:
            // Hard: 4 levels with boss, 1.5x faster, more shots
            model->invaders.speed = (0.5f + (level * 0.5f)) * 1.5f;
            int base_chance = (level > 2) ? 40 : (120 - (level * 20));
            model->invaders.shoot_chance = (int)(base_chance * 0.67f);
            break;
    }
}

void model_adjust_music_volume(GameModel* model, int direction) {
    if (direction == -1) {  // Decrease
        model->music_volume -= 0.1f;
        if (model->music_volume < 0.0f) model->music_volume = 0.0f;
    }
    else if (direction == 1) {  // Increase
        model->music_volume += 0.1f;
        if (model->music_volume > 1.0f) model->music_volume = 1.0f;
    }
    model->needs_redraw = true;
}
