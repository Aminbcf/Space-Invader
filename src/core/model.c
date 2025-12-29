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
            
            // Assign Types: Top=Octopus(2), Mid=Crab(1), Bot=Squid(0)
            if (i == 0) invaders->invaders[i][j].type = 2;
            else if (i < 3) invaders->invaders[i][j].type = 1;
            else invaders->invaders[i][j].type = 0;

            // Points
            if (i == 0) invaders->invaders[i][j].points = 30;
            else if (i < 3) invaders->invaders[i][j].points = 20;
            else invaders->invaders[i][j].points = 10;
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

// --- Public Initialization ---

void model_init(GameModel* model) {
    memset(model, 0, sizeof(GameModel));
    init_player(&model->player);
    init_invaders(&model->invaders, 1);
    init_boss(&model->boss);
    init_bases(model->bases);
    init_bullets(model->player_bullets, PLAYER_BULLETS, true);
    init_bullets(model->enemy_bullets, ENEMY_BULLETS, false);
    init_saucer(&model->saucer);
    model->state = STATE_MENU;
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
    
    // Clear bullets between levels
    init_bullets(model->player_bullets, PLAYER_BULLETS, true);
    init_bullets(model->enemy_bullets, ENEMY_BULLETS, false);
    
    if (model->player.level == 4) {
        // Level 4 is Boss Fight
        init_boss(&model->boss);
        model->boss.alive = true;
        // Kill invaders so they don't appear
        for(int i=0; i<INVADER_ROWS; i++)
            for(int j=0; j<INVADER_COLS; j++)
                model->invaders.invaders[i][j].alive = false;
    } 
    else if (model->player.level > 4) {
        model->state = STATE_WIN;
        return;
    }
    else {
        // Standard levels
        init_invaders(&model->invaders, model->player.level);
    }
    
    model->state = STATE_LEVEL_TRANSITION;
    model->needs_redraw = true;
}

// --- Update Logic ---

void model_update_boss(GameModel* model) {
    if (!model->boss.alive) return;
    Boss* boss = &model->boss;
    
    // Move Boss
    if (boss->direction == DIR_RIGHT) {
        boss->hitbox.x += boss->speed;
        if (boss->hitbox.x + boss->hitbox.width >= GAME_AREA_WIDTH) boss->direction = DIR_LEFT;
    } else {
        boss->hitbox.x -= boss->speed;
        if (boss->hitbox.x <= 0) boss->direction = DIR_RIGHT;
    }
    
    // Boss Shooting (Rapid)
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
    
    // Animation
    if (platform_get_ticks() > g->state_time + g->state_speed) {
        g->state_time = platform_get_ticks();
        g->state = !g->state; // Toggle animation frame
        model->needs_redraw = true;
    }
    
    // Movement
    bool hit_edge = false;
    int speed = (int)g->speed;
    if (g->direction == DIR_LEFT) speed = -speed;
    
    for (int i=0; i<INVADER_ROWS; i++) {
        for (int j=0; j<INVADER_COLS; j++) {
            if (g->invaders[i][j].alive && g->invaders[i][j].dying_timer == 0) {
                g->invaders[i][j].hitbox.x += speed;
                if (g->direction == DIR_RIGHT && g->invaders[i][j].hitbox.x + INVADER_WIDTH >= GAME_AREA_WIDTH) hit_edge = true;
                if (g->direction == DIR_LEFT && g->invaders[i][j].hitbox.x <= 0) hit_edge = true;
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
    // Player bullets
    for (int i = 0; i < PLAYER_BULLETS; i++) {
        if (model->player_bullets[i].alive) {
            model->player_bullets[i].hitbox.y += model->player_bullets[i].speed;
            if (model->player_bullets[i].hitbox.y < 0) model->player_bullets[i].alive = false;
        }
    }
    // Enemy bullets
    for (int i = 0; i < ENEMY_BULLETS; i++) {
        if (model->enemy_bullets[i].alive) {
            model->enemy_bullets[i].hitbox.y += model->enemy_bullets[i].speed;
            if (model->enemy_bullets[i].hitbox.y > SCREEN_HEIGHT) model->enemy_bullets[i].alive = false;
        }
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
    
    // Standard Enemy Shooting
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
}

void model_move_player(GameModel* model, Direction dir) {
    int speed = 8;
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

void model_check_bullet_collisions(GameModel* model) {
    // Player Bullets
    for (int b = 0; b < PLAYER_BULLETS; b++) {
        if (!model->player_bullets[b].alive) continue;
        
        // 1. Check Boss
        if (model->player.level == 4 && model->boss.alive) {
            if (model_check_collision(model->player_bullets[b].hitbox, model->boss.hitbox)) {
                model->player_bullets[b].alive = false;
                model->boss.health--;
                model->player.score += 5;
                if (model->boss.health <= 0) {
                    model->boss.alive = false;
                    model->player.score += 5000;
                    model_next_level(model); // Win
                }
                continue;
            }
        }
        
        // 2. Check Invaders
        if (model->player.level != 4) {
            for (int i = 0; i < INVADER_ROWS; i++) {
                for (int j = 0; j < INVADER_COLS; j++) {
                    Invader* inv = &model->invaders.invaders[i][j];
                    if (inv->alive && inv->dying_timer == 0) {
                        if (model_check_collision(model->player_bullets[b].hitbox, inv->hitbox)) {
                            model->player_bullets[b].alive = false;
                            inv->dying_timer = 5; // Start explosion
                            model->player.score += inv->points;
                            model->invaders.killed++;
                            if (model->invaders.killed >= INVADER_ROWS * INVADER_COLS) {
                                model_next_level(model);
                            }
                            goto next_bullet;
                        }
                    } else if (inv->alive && inv->dying_timer > 0) {
                        inv->dying_timer--;
                        if (inv->dying_timer == 0) inv->alive = false; // Remove after explosion
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

// Stubs / Setters / Getters
void model_update_saucer(GameModel* model, float delta_time) { (void)model; (void)delta_time; }
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