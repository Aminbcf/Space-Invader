#include "model.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>

static void init_player(Player* player) {
    player->hitbox.x = (GAME_AREA_WIDTH / 2) - (PLAYER_WIDTH / 2);
    player->hitbox.y = SCREEN_HEIGHT - (PLAYER_HEIGHT + 10);
    player->hitbox.width = PLAYER_WIDTH;
    player->hitbox.height = PLAYER_HEIGHT;
    player->lives = 3;
    player->score = 0;
    player->level = 1;
    player->shots_fired = 0;
}

static void init_invaders(InvaderGrid* invaders) {
    invaders->direction = DIR_RIGHT;
    invaders->speed = 1;
    invaders->state = 0;
    invaders->killed = 0;
    invaders->state_speed = 1000;
    invaders->state_time = SDL_GetTicks();

    for (int i = 0; i < INVADER_ROWS; i++) {
        for (int j = 0; j < INVADER_COLS; j++) {
            invaders->invaders[i][j].hitbox.x = j * (INVADER_WIDTH + 15);
            invaders->invaders[i][j].hitbox.y = 30 + i * (INVADER_HEIGHT + 15);
            invaders->invaders[i][j].hitbox.width = INVADER_WIDTH;
            invaders->invaders[i][j].hitbox.height = INVADER_HEIGHT;
            invaders->invaders[i][j].alive = true;
            invaders->invaders[i][j].row = i;
            invaders->invaders[i][j].col = j;

            if (i == 0) {
                invaders->invaders[i][j].color = COLOR_PURPLE;
                invaders->invaders[i][j].points = 30;
            } else if (i < 3) {
                invaders->invaders[i][j].color = COLOR_GREEN;
                invaders->invaders[i][j].points = 20;
            } else {
                invaders->invaders[i][j].color = COLOR_RED;
                invaders->invaders[i][j].points = 10;
            }
        }
    }
}

static void init_bases(Base bases[]) {
    int base_total = BASE_WIDTH * BASE_COUNT;
    int space_left = GAME_AREA_WIDTH - base_total;
    int even_space = space_left / (BASE_COUNT + 1);
    int x = even_space;
    int y = 500;

    for (int i = 0; i < BASE_COUNT; i++) {
        bases[i].hitbox.x = x;
        bases[i].hitbox.y = y;
        bases[i].hitbox.width = BASE_WIDTH;
        bases[i].hitbox.height = BASE_HEIGHT;
        bases[i].health = 100;
        bases[i].alive = true;
        x += BASE_WIDTH + even_space;
    }
}

static void init_bullets(Bullet bullets[], int count, bool is_player) {
    for (int i = 0; i < count; i++) {
        bullets[i].hitbox.width = BULLET_WIDTH;
        bullets[i].hitbox.height = BULLET_HEIGHT;
        bullets[i].alive = false;
        bullets[i].is_player_bullet = is_player;
        bullets[i].speed = is_player ? -30 : 15;
    }
}

static void init_saucer(Saucer* saucer) {
    saucer->hitbox.width = 30;
    saucer->hitbox.height = 20;
    saucer->alive = false;
    saucer->direction = DIR_RIGHT;
    saucer->points = 0;
}

void model_init(GameModel* model) {
    memset(model, 0, sizeof(GameModel));
    
    init_player(&model->player);
    init_invaders(&model->invaders);
    init_bases(model->bases);
    init_bullets(model->player_bullets, PLAYER_BULLETS, true);
    init_bullets(model->enemy_bullets, ENEMY_BULLETS, false);
    init_saucer(&model->saucer);
    
    model->state = STATE_MENU;
    model->game_time = SDL_GetTicks();
    model->needs_redraw = true;
    
    model_load_high_score(model);
}

void model_reset_game(GameModel* model) {
    init_player(&model->player);
    init_invaders(&model->invaders);
    init_bases(model->bases);
    init_bullets(model->player_bullets, PLAYER_BULLETS, true);
    init_bullets(model->enemy_bullets, ENEMY_BULLETS, false);
    init_saucer(&model->saucer);
    
    model->state = STATE_PLAYING;
    model->needs_redraw = true;
}

void model_next_level(GameModel* model) {
    model->player.level++;
    init_invaders(&model->invaders);
    init_bases(model->bases);
    init_saucer(&model->saucer);
    
    // Augmenter la difficulté
    model->invaders.speed = 1 + (model->player.level - 1);
    model->invaders.state_speed = 1000 - (model->player.level - 1) * 100;
    if (model->invaders.state_speed < 200) {
        model->invaders.state_speed = 200;
    }
    
    model->state = STATE_LEVEL_TRANSITION;
    model->needs_redraw = true;
}

static void update_invader_speed(InvaderGrid* invaders) {
    switch (invaders->killed) {
        case 10:
            invaders->speed = 2;
            invaders->state_speed = 800;
            break;
        case 20:
            invaders->speed = 4;
            invaders->state_speed = 600;
            break;
        case 30:
            invaders->speed = 8;
            invaders->state_speed = 200;
            break;
        case 40:
            invaders->speed = 16;
            invaders->state_speed = 0;
            break;
    }
}

static void move_invaders_down(InvaderGrid* invaders) {
    for (int i = 0; i < INVADER_ROWS; i++) {
        for (int j = 0; j < INVADER_COLS; j++) {
            if (invaders->invaders[i][j].alive) {
                invaders->invaders[i][j].hitbox.y += 15;
            }
        }
    }
}

void model_update_invaders(GameModel* model, float delta_time) {
    InvaderGrid* invaders = &model->invaders;
    
    update_invader_speed(invaders);
    
    // Animation state
    if (SDL_GetTicks() > invaders->state_time + invaders->state_speed) {
        invaders->state_time = SDL_GetTicks();
        invaders->state = !invaders->state;
        model->needs_redraw = true;
    }
    
    // Movement
    bool changed_direction = false;
    
    if (invaders->direction == DIR_RIGHT) {
        // Find rightmost alive invader
        int max_x = 0;
        for (int i = 0; i < INVADER_COLS; i++) {
            for (int j = 0; j < INVADER_ROWS; j++) {
                if (invaders->invaders[j][i].alive) {
                    int invader_right = invaders->invaders[j][i].hitbox.x + INVADER_WIDTH;
                    if (invader_right > max_x) {
                        max_x = invader_right;
                    }
                    break;
                }
            }
        }
        
        if (max_x + invaders->speed >= GAME_AREA_WIDTH) {
            invaders->direction = DIR_LEFT;
            move_invaders_down(invaders);
            changed_direction = true;
        }
    } else { // DIR_LEFT
        // Find leftmost alive invader
        int min_x = GAME_AREA_WIDTH;
        for (int i = 0; i < INVADER_COLS; i++) {
            for (int j = 0; j < INVADER_ROWS; j++) {
                if (invaders->invaders[j][i].alive) {
                    if (invaders->invaders[j][i].hitbox.x < min_x) {
                        min_x = invaders->invaders[j][i].hitbox.x;
                    }
                    break;
                }
            }
        }
        
        if (min_x - invaders->speed <= 0) {
            invaders->direction = DIR_RIGHT;
            move_invaders_down(invaders);
            changed_direction = true;
        }
    }
    
    // Move invaders if not changing direction
    if (!changed_direction) {
        int move_amount = invaders->speed;
        if (invaders->direction == DIR_LEFT) {
            move_amount = -move_amount;
        }
        
        for (int i = 0; i < INVADER_ROWS; i++) {
            for (int j = 0; j < INVADER_COLS; j++) {
                if (invaders->invaders[i][j].alive) {
                    invaders->invaders[i][j].hitbox.x += move_amount;
                }
            }
        }
    }
}

void model_move_player(GameModel* model, Direction dir) {
    int move_amount = 10;
    
    if (dir == DIR_LEFT) {
        if (model->player.hitbox.x - move_amount >= 0) {
            model->player.hitbox.x -= move_amount;
            model->needs_redraw = true;
        }
    } else if (dir == DIR_RIGHT) {
        if (model->player.hitbox.x + model->player.hitbox.width + move_amount <= GAME_AREA_WIDTH) {
            model->player.hitbox.x += move_amount;
            model->needs_redraw = true;
        }
    }
}

void model_player_shoot(GameModel* model) {
    // Find available bullet
    for (int i = 0; i < PLAYER_BULLETS; i++) {
        if (!model->player_bullets[i].alive) {
            model->player_bullets[i].hitbox.x = model->player.hitbox.x + (PLAYER_WIDTH / 2) - (BULLET_WIDTH / 2);
            model->player_bullets[i].hitbox.y = model->player.hitbox.y - BULLET_HEIGHT;
            model->player_bullets[i].alive = true;
            model->player.shots_fired++;
            model->needs_redraw = true;
            
            // Check for saucer spawn
            if (model->player.shots_fired % 20 == 0) {
                model->saucer.alive = true;
                if (rand() % 2 == 0) {
                    model->saucer.direction = DIR_RIGHT;
                    model->saucer.hitbox.x = 0;
                } else {
                    model->saucer.direction = DIR_LEFT;
                    model->saucer.hitbox.x = GAME_AREA_WIDTH - model->saucer.hitbox.width;
                }
                model->saucer.hitbox.y = 20;
            }
            break;
        }
    }
}

void model_update_bullets(GameModel* model, float delta_time) {
    // Player bullets
    for (int i = 0; i < PLAYER_BULLETS; i++) {
        if (model->player_bullets[i].alive) {
            model->player_bullets[i].hitbox.y += model->player_bullets[i].speed;
            
            if (model->player_bullets[i].hitbox.y < 0) {
                model->player_bullets[i].alive = false;
            }
        }
    }
    
    // Enemy bullets
    for (int i = 0; i < ENEMY_BULLETS; i++) {
        if (model->enemy_bullets[i].alive) {
            model->enemy_bullets[i].hitbox.y += model->enemy_bullets[i].speed;
            
            if (model->enemy_bullets[i].hitbox.y > SCREEN_HEIGHT) {
                model->enemy_bullets[i].alive = false;
            }
        }
    }
}

void model_update_saucer(GameModel* model, float delta_time) {
    if (!model->saucer.alive) return;
    
    int move_amount = 5;
    if (model->saucer.direction == DIR_LEFT) {
        move_amount = -move_amount;
    }
    
    model->saucer.hitbox.x += move_amount;
    
    // Check bounds
    if (model->saucer.hitbox.x < 0 || 
        model->saucer.hitbox.x + model->saucer.hitbox.width > GAME_AREA_WIDTH) {
        model->saucer.alive = false;
    }
}

bool model_check_collision(Rect a, Rect b) {
    return !(a.x + a.width < b.x ||
             a.x > b.x + b.width ||
             a.y + a.height < b.y ||
             a.y > b.y + b.height);
}

void model_check_bullet_collisions(GameModel* model) {
    // Player bullets vs invaders
    for (int b = 0; b < PLAYER_BULLETS; b++) {
        if (!model->player_bullets[b].alive) continue;
        
        Bullet* bullet = &model->player_bullets[b];
        
        // Check invaders
        for (int i = 0; i < INVADER_ROWS; i++) {
            for (int j = 0; j < INVADER_COLS; j++) {
                if (model->invaders.invaders[i][j].alive) {
                    if (model_check_collision(bullet->hitbox, 
                                            model->invaders.invaders[i][j].hitbox)) {
                        bullet->alive = false;
                        model->invaders.invaders[i][j].alive = false;
                        model->invaders.killed++;
                        model->player.score += model->invaders.invaders[i][j].points;
                        model->needs_redraw = true;
                        
                        // Check level completion
                        if (model->invaders.killed == INVADER_ROWS * INVADER_COLS) {
                            model_next_level(model);
                        }
                        return;
                    }
                }
            }
        }
        
        // Check saucer
        if (model->saucer.alive) {
            if (model_check_collision(bullet->hitbox, model->saucer.hitbox)) {
                bullet->alive = false;
                model->saucer.alive = false;
                
                // Random points for saucer
                int r = rand() % 3;
                switch (r) {
                    case 0: model->player.score += 50; break;
                    case 1: model->player.score += 150; break;
                    case 2: model->player.score += 300; break;
                }
                model->needs_redraw = true;
            }
        }
    }
    
    // Enemy bullets vs player
    for (int b = 0; b < ENEMY_BULLETS; b++) {
        if (!model->enemy_bullets[b].alive) continue;
        
        if (model_check_collision(model->enemy_bullets[b].hitbox, 
                                 model->player.hitbox)) {
            model->enemy_bullets[b].alive = false;
            model->player.lives--;
            model->needs_redraw = true;
            
            if (model->player.lives <= 0) {
                model->state = STATE_GAME_OVER;
                model_save_high_score(model);
            }
        }
    }
}

void model_check_invader_base_collisions(GameModel* model) {
    for (int i = 0; i < INVADER_ROWS; i++) {
        for (int j = 0; j < INVADER_COLS; j++) {
            if (!model->invaders.invaders[i][j].alive) continue;
            
            for (int b = 0; b < BASE_COUNT; b++) {
                if (!model->bases[b].alive) continue;
                
                if (model_check_collision(model->invaders.invaders[i][j].hitbox,
                                        model->bases[b].hitbox)) {
                    // Simple damage to base
                    model->bases[b].health -= 50;
                    if (model->bases[b].health <= 0) {
                        model->bases[b].alive = false;
                    }
                    model->needs_redraw = true;
                }
            }
        }
    }
}

void model_check_player_invader_collision(GameModel* model) {
    for (int i = 0; i < INVADER_ROWS; i++) {
        for (int j = 0; j < INVADER_COLS; j++) {
            if (!model->invaders.invaders[i][j].alive) continue;
            
            if (model_check_collision(model->player.hitbox,
                                    model->invaders.invaders[i][j].hitbox)) {
                model->player.lives--;
                model->needs_redraw = true;
                
                if (model->player.lives <= 0) {
                    model->state = STATE_GAME_OVER;
                    model_save_high_score(model);
                }
                return;
            }
        }
    }
}

void model_update(GameModel* model, float delta_time) {
    if (model->state != STATE_PLAYING) return;
    
    model_update_invaders(model, delta_time);
    model_update_bullets(model, delta_time);
    model_update_saucer(model, delta_time);
    
    model_check_bullet_collisions(model);
    model_check_invader_base_collisions(model);
    model_check_player_invader_collision(model);
    
    // Enemy AI shooting
    if (rand() % 100 == 0) { // 1% chance per frame
        // Find bottom-most alive invader in each column
        for (int col = 0; col < INVADER_COLS; col++) {
            for (int row = INVADER_ROWS - 1; row >= 0; row--) {
                if (model->invaders.invaders[row][col].alive) {
                    // Find available enemy bullet
                    for (int b = 0; b < ENEMY_BULLETS; b++) {
                        if (!model->enemy_bullets[b].alive) {
                            model->enemy_bullets[b].hitbox.x = 
                                model->invaders.invaders[row][col].hitbox.x + 
                                (INVADER_WIDTH / 2) - (BULLET_WIDTH / 2);
                            model->enemy_bullets[b].hitbox.y = 
                                model->invaders.invaders[row][col].hitbox.y + 
                                INVADER_HEIGHT;
                            model->enemy_bullets[b].alive = true;
                            model->needs_redraw = true;
                            goto next_column;
                        }
                    }
                    break;
                }
            }
            next_column:;
        }
    }
}

void model_set_state(GameModel* model, GameState state) {
    model->state = state;
    model->needs_redraw = true;
}

void model_toggle_pause(GameModel* model) {
    if (model->state == STATE_PLAYING) {
        model->state = STATE_PAUSED;
    } else if (model->state == STATE_PAUSED) {
        model->state = STATE_PLAYING;
    }
    model->needs_redraw = true;
}

void model_save_high_score(GameModel* model) {
    if (model->player.score > model->high_score) {
        model->high_score = model->player.score;
        
        FILE* file = fopen("highscore.dat", "wb");
        if (file) {
            fwrite(&model->high_score, sizeof(int), 1, file);
            fclose(file);
        }
    }
}

void model_load_high_score(GameModel* model) {
    FILE* file = fopen("highscore.dat", "rb");
    if (file) {
        fread(&model->high_score, sizeof(int), 1, file);
        fclose(file);
    } else {
        model->high_score = 0;
    }
}

// Getters
int model_get_score(const GameModel* model) { return model->player.score; }
int model_get_lives(const GameModel* model) { return model->player.lives; }
int model_get_level(const GameModel* model) { return model->player.level; }
GameState model_get_state(const GameModel* model) { return model->state; }