#include "model.h"
#include "utils/platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// --- Helper Init Functions ---

static void init_player_params(Player *player, int id) {
  player->hitbox.width = PLAYER_WIDTH;
  player->hitbox.height = PLAYER_HEIGHT;
  player->hitbox.x = (GAME_AREA_WIDTH / 2) - (PLAYER_WIDTH / 2) + (id == 1 ? 50 : -50);
  player->hitbox.y = SCREEN_HEIGHT - (PLAYER_HEIGHT + 20);
  player->lives = 3;
  player->score = 0;
  player->level = 1;
  player->shots_fired = 0;
  player->combo_count = 0;
  player->last_kill_time = 0;
  player->is_active = (id == 0); // Player 1 active by default
  player->player_id = id;
  player->active_powerup = PWR_NONE;
  player->powerup_timer = 0;
  player->invincibility_timer = 0.0f;
}

static void reset_player_pos(Player *player, int id) {
  player->hitbox.x = (GAME_AREA_WIDTH / 2) - (PLAYER_WIDTH / 2) + (id == 1 ? 50 : -50);
  player->hitbox.y = SCREEN_HEIGHT - (PLAYER_HEIGHT + 20);
}

static void init_boss(Boss *boss) {
  boss->alive = false;
  boss->hitbox.width = BOSS_WIDTH;
  boss->hitbox.height = BOSS_HEIGHT;
  boss->hitbox.x = (GAME_AREA_WIDTH / 2) - (BOSS_WIDTH / 2);
  boss->hitbox.y = 60;
  boss->max_health = 50;
  boss->health = 50;
  boss->direction = DIR_RIGHT;
  boss->speed_x = 150.0f; // px per second
  boss->speed_y = 0;
  boss->shoot_timer = 0;
  boss->anim_frame = 0;
  boss->anim_counter = 0;
  boss->attack_pattern = 0;
  boss->attack_timer = 0;
}

static void init_invaders(InvaderGrid *invaders, int level, Difficulty difficulty) {
  invaders->direction = DIR_RIGHT;
  float base_speed = 0.5f + (level * 0.5f);
  if (difficulty == DIFFICULTY_HARD) base_speed *= 2.0f;
  if (difficulty == DIFFICULTY_ROGUE) base_speed *= 1.2f;
  
  invaders->speed = base_speed * 60.0f; // Scale to px/sec
  invaders->state = 0;
  invaders->killed = 0;
  invaders->state_speed = (level > 3) ? 200 : (1000 - (level * 150));
  invaders->state_time = platform_get_ticks();
  invaders->shoot_chance = (level > 2) ? 150 : (300 - (level * 30)); // Extremely conservative firing

  invaders->big_invader_spawn_timer = 0;
  
  // Initialize big invader as inactive
  invaders->big_invader.alive = false;
  invaders->big_invader.hitbox.width = BIG_INVADER_WIDTH;
  invaders->big_invader.hitbox.height = BIG_INVADER_HEIGHT;
  invaders->big_invader.max_health = 40 + level * 5; // More health (was 10)
  invaders->big_invader.health = invaders->big_invader.max_health;
  invaders->big_invader.points = 200;
  invaders->big_invader.speed = 70.0f; // Faster (was 40.0) to avoid sticking
  invaders->big_invader.shoot_timer = 0;
  invaders->big_invader.attack_type = 0;

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
      invaders->invaders[i][j].health = 1; // Normal invaders have 1 HP
      invaders->invaders[i][j].speed_modifier = 1.0f;

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

static void init_bullets(Bullet bullets[], int count, bool is_player, int p_id) {
  for (int i = 0; i < count; i++) {
    bullets[i].hitbox.width = BULLET_WIDTH;
    bullets[i].hitbox.height = BULLET_HEIGHT;
    bullets[i].alive = false;
    bullets[i].is_player_bullet = is_player;
    bullets[i].player_id = p_id;
    bullets[i].speed_x = 0;
    bullets[i].speed_y = is_player ? -600.0f : 300.0f;
    bullets[i].is_strong = false;
  }
}

static void init_powerups(PowerUp powerups[], int count) {
  for (int i = 0; i < count; i++) {
    powerups[i].alive = false;
    powerups[i].hitbox.width = 20;
    powerups[i].hitbox.height = 20;
    powerups[i].speed_y = 150.0f;
  }
}

static void model_drop_powerup(GameModel *model, float x, float y) {
    if ((rand() % 100) > 5) return; // 5% drop rate
    
    for (int i = 0; i < 10; i++) {
        if (!model->powerups[i].alive) {
            model->powerups[i].alive = true;
            model->powerups[i].hitbox.x = x;
            model->powerups[i].hitbox.y = y;
            model->powerups[i].type = (PowerUpType)(1 + (rand() % (PWR_MAX - 1)));
            break;
        }
    }
}

static void init_saucer(Saucer *saucer) {
  saucer->hitbox.width = 30;
  saucer->hitbox.height = 20;
  saucer->alive = false;
  saucer->direction = DIR_RIGHT;
  saucer->points = 0;
}

// --- Public Init ---

void model_init(GameModel *model) {
  memset(model, 0, sizeof(GameModel));
  
  init_player_params(&model->players[0], 0);
  init_player_params(&model->players[1], 1);
  model->players[1].is_active = false; // Player 2 inactive unless chosen

  init_invaders(&model->invaders, 1, DIFFICULTY_NORMAL);
  init_boss(&model->boss);
  init_bases(model->bases);
  
  for(int p=0; p<2; p++)
    init_bullets(model->player_bullets[p], PLAYER_BULLETS, true, p);
  
  init_bullets(model->enemy_bullets, ENEMY_BULLETS, false, -1);
  init_powerups(model->powerups, 10);
  init_saucer(&model->saucer);

  model->state = STATE_MENU;
  model->difficulty = DIFFICULTY_NORMAL;
  model->menu_state = MENU_MAIN;
  model->menu_selection = 0;
  model->music_volume = 0.5f;
  model->game_time = platform_get_ticks();
  model->last_update_time = model->game_time;
  model->needs_redraw = true;
  
  // Default keybindings (SDL keycodes for arrows/WASD)
  // P1: Left=1073741904, Right=1073741903, Up=1073741906, Down=1073741905, Shoot=32 (space)
  // For ncurses: a=97, d=100, w=119, s=115, space=32
  model->keybinds_p1[0] = 1073741904; // Left arrow
  model->keybinds_p1[1] = 1073741903; // Right arrow
  model->keybinds_p1[2] = 1073741906; // Up arrow
  model->keybinds_p1[3] = 1073741905; // Down arrow
  model->keybinds_p1[4] = 32;          // Space
  model->keybinds_p2[0] = 97;  // A
  model->keybinds_p2[1] = 100; // D
  model->keybinds_p2[2] = 119; // W
  model->keybinds_p2[3] = 115; // S
  model->keybinds_p2[4] = 1073742049; // Left Shift
  model->editing_keybind = -1;
  model->waiting_for_key = false;
  
  model_load_high_score(model);
}

void model_reset_game(GameModel *model) {
  Difficulty old_diff = model->difficulty;
  bool old_2p = model->two_player_mode;
  
  // Save keybindings
  int saved_p1[5];
  int saved_p2[5];
  memcpy(saved_p1, model->keybinds_p1, sizeof(int) * 5);
  memcpy(saved_p2, model->keybinds_p2, sizeof(int) * 5);
  
  model_init(model);
  
  // Restore keybindings
  memcpy(model->keybinds_p1, saved_p1, sizeof(int) * 5);
  memcpy(model->keybinds_p2, saved_p2, sizeof(int) * 5);
  
  model->state = STATE_PLAYING;
  model->difficulty = old_diff;
  model->two_player_mode = old_2p;
  model->players[1].is_active = old_2p;
}

void model_next_level(GameModel *model) {
  model->players[0].level++;
  model->players[1].level = model->players[0].level;

  int max_level = (model->difficulty == DIFFICULTY_EASY) ? 3 : 4;
  if (model->difficulty == DIFFICULTY_ROGUE) max_level = 999;

  for(int p=0; p<2; p++) {
    reset_player_pos(&model->players[p], p);
    init_bullets(model->player_bullets[p], PLAYER_BULLETS, true, p);
  }
  init_bullets(model->enemy_bullets, ENEMY_BULLETS, false, -1);

  if (model->difficulty == DIFFICULTY_ROGUE) {
    if (model->players[0].level % 5 == 0) {
      init_boss(&model->boss);
      model->boss.alive = true;
      model->boss.max_health = 50 + (model->players[0].level * 10);
      model->boss.health = model->boss.max_health;
    } else {
      init_invaders(&model->invaders, model->players[0].level, model->difficulty);
    }
  } else {
    if (model->difficulty == DIFFICULTY_EASY && model->players[0].level > 3) {
      model->state = STATE_WIN;
      return;
    }
    if (model->players[0].level == 4 && model->difficulty != DIFFICULTY_EASY) {
      init_boss(&model->boss);
      model->boss.alive = true;
    } else if (model->players[0].level > max_level) {
      model->state = STATE_WIN;
      return;
    } else {
      init_invaders(&model->invaders, model->players[0].level, model->difficulty);
    }
  }

  // Clear all falling power-ups for cleaner transition
  for (int i = 0; i < 10; i++) {
    model->powerups[i].alive = false;
  }
  
  // Clear player power-up effects
  for (int p = 0; p < 2; p++) {
    model->players[p].active_powerup = PWR_NONE;
    model->players[p].powerup_timer = 0;
  }

  model->state = STATE_LEVEL_TRANSITION;
  model->needs_redraw = true;
}

// --- Updates ---

void model_update_boss(GameModel *model, float delta_time) {
  if (!model->boss.alive) return;
  Boss *boss = &model->boss;

  boss->anim_counter++;
  if (boss->anim_counter >= 15) {
    boss->anim_counter = 0;
    boss->anim_frame = !boss->anim_frame;
  }

  boss->attack_timer += delta_time;
  if (boss->attack_timer > 5.0f) {
    boss->attack_timer = 0;
    boss->attack_pattern = rand() % 2;
  }

  // Movement logic
  if (boss->attack_pattern == 0) { // Horizontal bounce
    float move = boss->speed_x * delta_time;
    if (boss->direction == DIR_RIGHT) {
      boss->hitbox.x += move;
      if (boss->hitbox.x + boss->hitbox.width >= GAME_AREA_WIDTH) boss->direction = DIR_LEFT;
    } else {
      boss->hitbox.x -= move;
      if (boss->hitbox.x <= 0) boss->direction = DIR_RIGHT;
    }
  }
  
  // Vertical Movement (Sinewave)
  boss->hitbox.y = 80 + sinf(platform_get_ticks() / 1000.0f) * 30.0f;

  // Shooting
  boss->shoot_timer++;
  
  // Change attack pattern periodically
  if (boss->shoot_timer > 60) {
    boss->attack_pattern = (boss->attack_pattern + 1) % 4; // 4 patterns now
  }
  
  if (boss->shoot_timer > 30) {
    boss->shoot_timer = 0;
    
    if (boss->attack_pattern == 0) { // Circular burst (spinning)
       for (int i = 0; i < 12; i++) {
          for (int b = 0; b < ENEMY_BULLETS; b++) {
            if (!model->enemy_bullets[b].alive) {
              model->enemy_bullets[b].alive = true;
              model->enemy_bullets[b].type = 2; // Laser/Orb
              model->enemy_bullets[b].hitbox.x = boss->hitbox.x + boss->hitbox.width/2;
              model->enemy_bullets[b].hitbox.y = boss->hitbox.y + boss->hitbox.height/2;
              model->enemy_bullets[b].hitbox.width = 8;
              model->enemy_bullets[b].hitbox.height = 8;
              float angle = i * (3.14159f / 6.0f) + (boss->anim_counter * 0.15f); // Spin
              model->enemy_bullets[b].speed_x = cosf(angle) * 250.0f;
              model->enemy_bullets[b].speed_y = sinf(angle) * 250.0f;
              break;
            }
          }
       }
    } else if (boss->attack_pattern == 1) { // Bullet rain
      for (int i = 0; i < 5; i++) {
          for (int b = 0; b < ENEMY_BULLETS; b++) {
            if (!model->enemy_bullets[b].alive) {
              model->enemy_bullets[b].alive = true;
              model->enemy_bullets[b].type = 0;
              model->enemy_bullets[b].hitbox.x = boss->hitbox.x + (rand() % (int)boss->hitbox.width);
              model->enemy_bullets[b].hitbox.y = boss->hitbox.y + boss->hitbox.height;
              model->enemy_bullets[b].hitbox.width = 5;
              model->enemy_bullets[b].hitbox.height = 15;
              model->enemy_bullets[b].speed_x = (rand() % 80) - 40;
              model->enemy_bullets[b].speed_y = 400.0f + (rand() % 100);
              break;
            }
          }
      }
    } else if (boss->attack_pattern == 2) { // BIG SLOW ATTACK - hard to dodge!
      for (int b = 0; b < ENEMY_BULLETS; b++) {
        if (!model->enemy_bullets[b].alive) {
          model->enemy_bullets[b].alive = true;
          model->enemy_bullets[b].type = 2; // Laser type for visual
          model->enemy_bullets[b].hitbox.x = boss->hitbox.x + boss->hitbox.width/2 - 20;
          model->enemy_bullets[b].hitbox.y = boss->hitbox.y + boss->hitbox.height;
          model->enemy_bullets[b].hitbox.width = 40; // BIG!
          model->enemy_bullets[b].hitbox.height = 40;
          model->enemy_bullets[b].speed_x = 0;
          model->enemy_bullets[b].speed_y = 120.0f; // SLOW but deadly
          break;
        }
      }
    } else { // ZigZag spread
      for (int i = 0; i < 3; i++) {
          for (int b = 0; b < ENEMY_BULLETS; b++) {
            if (!model->enemy_bullets[b].alive) {
              model->enemy_bullets[b].alive = true;
              model->enemy_bullets[b].type = 1; // ZigZag
              model->enemy_bullets[b].hitbox.x = boss->hitbox.x + boss->hitbox.width/4 + i * (boss->hitbox.width/4);
              model->enemy_bullets[b].hitbox.y = boss->hitbox.y + boss->hitbox.height;
              model->enemy_bullets[b].hitbox.width = 6;
              model->enemy_bullets[b].hitbox.height = 12;
              model->enemy_bullets[b].speed_x = 0;
              model->enemy_bullets[b].speed_y = 300.0f;
              break;
            }
          }
      }
    }
  }
}

void model_update_invaders(GameModel *model, float delta_time) {
  InvaderGrid *g = &model->invaders;
  bool any_alive = false;
  for(int i=0; i<INVADER_ROWS; i++)
    for(int j=0; j<INVADER_COLS; j++) if(g->invaders[i][j].alive) any_alive = true;
  
  // Include big invader in "any alive" check
  if (g->big_invader.alive) any_alive = true;
  
  if(!any_alive && !g->big_invader.alive) return;

  if (platform_get_ticks() > g->state_time + g->state_speed) {
    g->state_time = platform_get_ticks();
    g->state = !g->state;
    model->needs_redraw = true;
  }

  bool hit_edge = false;
  float shift = g->speed * delta_time;

  for (int i = 0; i < INVADER_ROWS; i++) {
    for (int j = 0; j < INVADER_COLS; j++) {
      Invader *inv = &g->invaders[i][j];
      if (inv->alive && inv->dying_timer > 0) {
        inv->dying_timer--;
        if (inv->dying_timer <= 0) inv->alive = false;
        continue;
      }
      if (inv->alive) {
        float inv_shift = shift * inv->speed_modifier;
        if (g->direction == DIR_RIGHT) {
          inv->hitbox.x += inv_shift;
          if (inv->hitbox.x + inv->hitbox.width >= GAME_AREA_WIDTH) hit_edge = true;
        } else {
          inv->hitbox.x -= inv_shift;
          if (inv->hitbox.x <= 0) hit_edge = true;
        }
      }
    }
  }

  if (hit_edge) {
    g->direction = (g->direction == DIR_RIGHT) ? DIR_LEFT : DIR_RIGHT;
    for (int i = 0; i < INVADER_ROWS; i++)
      for (int j = 0; j < INVADER_COLS; j++)
        if (g->invaders[i][j].alive) g->invaders[i][j].hitbox.y += 15;
  }
  
  // Big Invader spawning (spawn every ~15 seconds if not alive)
  if (!g->big_invader.alive && !model->boss.alive) {
    g->big_invader_spawn_timer += delta_time;
    if (g->big_invader_spawn_timer >= 15.0f) {
      g->big_invader_spawn_timer = 0;
      g->big_invader.alive = true;
      g->big_invader.hitbox.x = (rand() % 2 == 0) ? -BIG_INVADER_WIDTH : GAME_AREA_WIDTH;
      g->big_invader.hitbox.y = 80;
      g->big_invader.direction = (g->big_invader.hitbox.x < 0) ? DIR_RIGHT : DIR_LEFT;
      g->big_invader.health = g->big_invader.max_health;
      g->big_invader.attack_type = rand() % 2;
    }
  }
  
  // Big Invader update
  if (g->big_invader.alive) {
    BigInvader *bi = &g->big_invader;
    
    // Slow horizontal movement
    float bi_move = bi->speed * delta_time;
    if (bi->direction == DIR_RIGHT) {
      bi->hitbox.x += bi_move;
      if (bi->hitbox.x >= GAME_AREA_WIDTH + 20) bi->alive = false; // Exit screen
    } else {
      bi->hitbox.x -= bi_move;
      if (bi->hitbox.x + bi->hitbox.width <= -20) bi->alive = false;
    }
    
    // Big invader shooting - BIG SLOW PROJECTILES
    bi->shoot_timer += delta_time;
    if (bi->shoot_timer >= 2.0f) { // Shoot every 2 seconds
      bi->shoot_timer = 0;
      
      if (bi->attack_type == 0) {
        // Big slow shot - hard to dodge!
        for (int b = 0; b < ENEMY_BULLETS; b++) {
          if (!model->enemy_bullets[b].alive) {
            model->enemy_bullets[b].alive = true;
            model->enemy_bullets[b].type = 2; // Laser type visual
            model->enemy_bullets[b].hitbox.x = bi->hitbox.x + bi->hitbox.width/2 - 25;
            model->enemy_bullets[b].hitbox.y = bi->hitbox.y + bi->hitbox.height;
            model->enemy_bullets[b].hitbox.width = 35; // Smaller (was 50) to allow dodging
            model->enemy_bullets[b].hitbox.height = 50;
            model->enemy_bullets[b].speed_x = 0;
            model->enemy_bullets[b].speed_y = 100.0f; // Faster (was 80) to clear screen
            break;
          }
        }
      } else {
        // Spread shot - 5 shots in a fan pattern
        for (int s = 0; s < 5; s++) {
          for (int b = 0; b < ENEMY_BULLETS; b++) {
            if (!model->enemy_bullets[b].alive) {
              model->enemy_bullets[b].alive = true;
              model->enemy_bullets[b].type = 0;
              model->enemy_bullets[b].hitbox.x = bi->hitbox.x + bi->hitbox.width/2;
              model->enemy_bullets[b].hitbox.y = bi->hitbox.y + bi->hitbox.height;
              model->enemy_bullets[b].hitbox.width = 8;
              model->enemy_bullets[b].hitbox.height = 8;
              float angle = -0.4f + (s * 0.2f); // Fan from -0.4 to 0.4 radians
              model->enemy_bullets[b].speed_x = sinf(angle) * 200.0f;
              model->enemy_bullets[b].speed_y = cosf(angle) * 200.0f;
              break;
            }
          }
        }
      }
    }
  }
}


void model_update_bullets(GameModel *model, float delta_time) {
  for (int p = 0; p < 2; p++) {
    for (int i = 0; i < PLAYER_BULLETS; i++) {
      if (model->player_bullets[p][i].alive) {
        model->player_bullets[p][i].hitbox.x += model->player_bullets[p][i].speed_x * delta_time;
        model->player_bullets[p][i].hitbox.y += model->player_bullets[p][i].speed_y * delta_time;
        if (model->player_bullets[p][i].hitbox.y < 0 || model->player_bullets[p][i].hitbox.y > SCREEN_HEIGHT ||
            model->player_bullets[p][i].hitbox.x < 0 || model->player_bullets[p][i].hitbox.x > GAME_AREA_WIDTH)
          model->player_bullets[p][i].alive = false;
      }
    }
  }
  for (int i = 0; i < ENEMY_BULLETS; i++) {
    if (model->enemy_bullets[i].alive) {
      if (model->enemy_bullets[i].type == 1) { // ZigZag
          model->enemy_bullets[i].speed_x = sinf(model->enemy_bullets[i].hitbox.y * 0.05f) * 150.0f;
      }
      model->enemy_bullets[i].hitbox.x += model->enemy_bullets[i].speed_x * delta_time;
      model->enemy_bullets[i].hitbox.y += model->enemy_bullets[i].speed_y * delta_time;
      if (model->enemy_bullets[i].hitbox.y > SCREEN_HEIGHT || model->enemy_bullets[i].hitbox.y < 0 ||
          model->enemy_bullets[i].hitbox.x < -50 || model->enemy_bullets[i].hitbox.x > GAME_AREA_WIDTH + 50)
        model->enemy_bullets[i].alive = false;
    }
  }
}

void model_update_saucer(GameModel *model, float delta_time) {
  if (!model->saucer.alive) return;
  float speed = 250.0f * delta_time;
  if (model->saucer.direction == DIR_LEFT) speed = -speed;
  model->saucer.hitbox.x += speed;
  if (model->saucer.hitbox.x < -50 || model->saucer.hitbox.x > GAME_AREA_WIDTH + 50)
    model->saucer.alive = false;
}

void model_update(GameModel *model, float delta_time) {
  // Handle WIN state auto-return to menu
  if (model->state == STATE_WIN) {
    model->win_timer += delta_time;
    if (model->win_timer >= 5.0f) { // 5 seconds
      model->state = STATE_MENU;
      model->menu_state = MENU_MAIN;
      model->menu_selection = 0;
      model->win_timer = 0;
    }
    return;
  }
  
  // Handle GAME_OVER state auto-return
  if (model->state == STATE_GAME_OVER) {
    model->win_timer += delta_time;
    if (model->win_timer >= 5.0f) { // 5 seconds
      model->state = STATE_MENU;
      model->menu_state = MENU_MAIN;
      model->menu_selection = 0;
      model->win_timer = 0;
    }
    return;
  }
  
  if (model->state != STATE_PLAYING) return;

  uint32_t now = platform_get_ticks();
  for(int p=0; p<2; p++) {
      if (model->players[p].combo_count > 0 && now - model->players[p].last_kill_time > 2000) {
          model->players[p].combo_count = 0;
      }
  }

  if (model->boss.alive) model_update_boss(model, delta_time);
  else model_update_invaders(model, delta_time);

  model_update_bullets(model, delta_time);
  model_update_saucer(model, delta_time);

  model_check_bullet_collisions(model);
  model_check_player_invader_collision(model);
  model_check_invader_base_collisions(model);

  if (!model->boss.alive && (rand() % model->invaders.shoot_chance == 0)) {
    int col = rand() % INVADER_COLS;
    for (int row = INVADER_ROWS - 1; row >= 0; row--) {
      if (model->invaders.invaders[row][col].alive && model->invaders.invaders[row][col].dying_timer == 0) {
        // Determine attack pattern based on type + randomness
        int attack_rand = rand() % 100;
        int num_shots = 1;
        bool is_angled = false;
        bool is_big_slow = false;
        
        // Top row (type 2) - occasional double shot
        if (row == 0 && attack_rand < 20) num_shots = 2;
        // Middle rows (type 1) - occasional angled shot
        else if (row >= 1 && row < 3 && attack_rand < 15) is_angled = true;
        // Rare big slow shot from any invader
        else if (attack_rand < 5) is_big_slow = true;
        
        for (int shot = 0; shot < num_shots; shot++) {
          for (int b = 0; b < ENEMY_BULLETS; b++) {
            if (!model->enemy_bullets[b].alive) {
              model->enemy_bullets[b].alive = true;
              model->enemy_bullets[b].hitbox.x = model->invaders.invaders[row][col].hitbox.x + 10 + (shot * 10 - 5);
              model->enemy_bullets[b].hitbox.y = model->invaders.invaders[row][col].hitbox.y + 30;
              
              if (is_big_slow) {
                // Big slow projectile - hard to dodge!
                model->enemy_bullets[b].type = 2;
                model->enemy_bullets[b].hitbox.width = 25;
                model->enemy_bullets[b].hitbox.height = 25;
                model->enemy_bullets[b].speed_x = ((rand() % 60) - 30);
                model->enemy_bullets[b].speed_y = 100.0f;
              } else if (is_angled) {
                // Angled shot - aims toward player
                model->enemy_bullets[b].type = 0;
                model->enemy_bullets[b].hitbox.width = 5;
                model->enemy_bullets[b].hitbox.height = 15;
                float target_x = model->players[0].hitbox.x;
                float inv_x = model->invaders.invaders[row][col].hitbox.x;
                model->enemy_bullets[b].speed_x = (target_x - inv_x) * 0.5f;
                model->enemy_bullets[b].speed_x = fmaxf(-200.0f, fminf(200.0f, model->enemy_bullets[b].speed_x));
                model->enemy_bullets[b].speed_y = 300.0f;
              } else if (row >= 4) { // Bottom row - Fast Laser
                model->enemy_bullets[b].type = 2;
                model->enemy_bullets[b].hitbox.width = 5;
                model->enemy_bullets[b].hitbox.height = 15;
                model->enemy_bullets[b].speed_x = 0;
                model->enemy_bullets[b].speed_y = 500.0f;
              } else if (row >= 2) { // Middle rows - ZigZag
                model->enemy_bullets[b].type = 1;
                model->enemy_bullets[b].hitbox.width = 5;
                model->enemy_bullets[b].hitbox.height = 15;
                model->enemy_bullets[b].speed_x = 0;
                model->enemy_bullets[b].speed_y = 350.0f;
              } else { // Top rows - Standard with speed variance
                model->enemy_bullets[b].type = 0;
                model->enemy_bullets[b].hitbox.width = 5;
                model->enemy_bullets[b].hitbox.height = 15;
                model->enemy_bullets[b].speed_x = 0;
                model->enemy_bullets[b].speed_y = 280.0f + (rand() % 60) + (model->players[0].level * 20.0f);
              }
              break;
            }
          }
        }
        break;
      }
    }
  }


  if (!model->saucer.alive && (rand() % 1500 == 0)) {
    model->saucer.alive = true;
    model->saucer.direction = (rand() % 2 == 0) ? DIR_RIGHT : DIR_LEFT;
    model->saucer.hitbox.x = (model->saucer.direction == DIR_RIGHT) ? -40 : GAME_AREA_WIDTH;
    model->saucer.hitbox.y = 40;
  }

  // Update Powerups
  for (int i = 0; i < 10; i++) {
    if (model->powerups[i].alive) {
      model->powerups[i].hitbox.y += model->powerups[i].speed_y * delta_time;
      if (model->powerups[i].hitbox.y > SCREEN_HEIGHT) model->powerups[i].alive = false;
      
      // Check collection
      for (int p=0; p<2; p++) {
        if (model->players[p].is_active && model_check_collision(model->powerups[i].hitbox, model->players[p].hitbox)) {
          model->players[p].active_powerup = model->powerups[i].type;
          model->players[p].powerup_timer = 5.0f; // 5 seconds duration
          model->powerups[i].alive = false;
          model->needs_redraw = true;
          break;
        }
      }
    }
  }
  
  // Powerup timer logic
  for (int p=0; p<2; p++) {
    if (model->players[p].is_active) {
        if (model->players[p].active_powerup != PWR_NONE) {
          model->players[p].powerup_timer -= delta_time;
          if (model->players[p].powerup_timer <= 0) {
            model->players[p].active_powerup = PWR_NONE;
          }
        }
        // Invincibility logic
        if (model->players[p].invincibility_timer > 0) {
            model->players[p].invincibility_timer -= delta_time;
            if (model->players[p].invincibility_timer < 0) model->players[p].invincibility_timer = 0;
            model->needs_redraw = true; // Blink effect relies on this
        }
    }
  }
}

void model_move_player(GameModel *model, int player_id, Direction dir) {
  if (player_id < 0 || player_id > 1 || !model->players[player_id].is_active) return;
  float speed = 6.4f; // Pixels per frame at 60fps = 384 pixels/sec
  Player *p = &model->players[player_id];
  
  if (dir == DIR_LEFT && p->hitbox.x > 0) p->hitbox.x -= speed;
  if (dir == DIR_RIGHT && p->hitbox.x < GAME_AREA_WIDTH - PLAYER_WIDTH) p->hitbox.x += speed;
  if (dir == DIR_UP && p->hitbox.y > SCREEN_HEIGHT / 2) p->hitbox.y -= speed;
  if (dir == DIR_DOWN && p->hitbox.y < SCREEN_HEIGHT - PLAYER_HEIGHT - 10) p->hitbox.y += speed;
  
  model->needs_redraw = true;
}


void model_player_shoot(GameModel *model, int player_id) {
  if (player_id < 0 || player_id > 1 || !model->players[player_id].is_active) return;
  Player *p = &model->players[player_id];
  
  bool is_triple = (p->combo_count >= 5 || p->active_powerup == PWR_TRIPLE_SHOT);
  bool is_strong = (p->active_powerup == PWR_STRONG_MISSILE);
  
  int shots = is_triple ? 3 : 1;
  int fired = 0;

  for (int i = 0; i < PLAYER_BULLETS && fired < shots; i++) {
    if (!model->player_bullets[player_id][i].alive) {
      model->player_bullets[player_id][i].alive = true;
      model->player_bullets[player_id][i].is_strong = is_strong;
      model->player_bullets[player_id][i].hitbox.x = p->hitbox.x + (PLAYER_WIDTH / 2) - (BULLET_WIDTH / 2);
      
      if (is_triple) {
          if (fired == 0) model->player_bullets[player_id][i].speed_x = 0;
          if (fired == 1) { model->player_bullets[player_id][i].speed_x = -150; model->player_bullets[player_id][i].hitbox.x -= 10; }
          if (fired == 2) { model->player_bullets[player_id][i].speed_x = 150; model->player_bullets[player_id][i].hitbox.x += 10; }
      } else {
          model->player_bullets[player_id][i].speed_x = 0;
      }
      
      model->player_bullets[player_id][i].hitbox.y = p->hitbox.y;
      model->player_bullets[player_id][i].speed_y = -700.0f;
      fired++;
      if (fired == 1) p->shots_fired++;
    }
  }
}

bool model_check_collision(Rect a, Rect b) {
  return (a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y);
}

static int apply_difficulty_multiplier(GameModel *model, int base_score) {
  switch (model->difficulty) {
    case DIFFICULTY_EASY: return base_score;
    case DIFFICULTY_NORMAL: return (int)(base_score * 1.5f);
    case DIFFICULTY_HARD: return base_score * 2;
    case DIFFICULTY_ROGUE: return (int)(base_score * (1.0f + model->players[0].level * 0.1f));
    default: return base_score;
  }
}

void model_check_bullet_collisions(GameModel *model) {
  for (int p_idx = 0; p_idx < 2; p_idx++) {
    if (!model->players[p_idx].is_active) continue;
    for (int b = 0; b < PLAYER_BULLETS; b++) {
      Bullet *pb = &model->player_bullets[p_idx][b];
      if (!pb->alive) continue;

      if (model->boss.alive && model_check_collision(pb->hitbox, model->boss.hitbox)) {
        model->boss.health -= (pb->is_strong ? 5 : 1);
        if (!pb->is_strong) pb->alive = false;
        model->players[p_idx].score += apply_difficulty_multiplier(model, 10);
        if (model->boss.health <= 0) {
          model->boss.alive = false;
          model->players[p_idx].score += apply_difficulty_multiplier(model, 5000);
          model_drop_powerup(model, model->boss.hitbox.x, model->boss.hitbox.y);
          model_next_level(model);
        }
        continue;
      }

      if (model->saucer.alive && model_check_collision(pb->hitbox, model->saucer.hitbox)) {
        model->saucer.alive = false;
        if (!pb->is_strong) pb->alive = false;
        model->players[p_idx].score += apply_difficulty_multiplier(model, 300);
        model_drop_powerup(model, model->saucer.hitbox.x, model->saucer.hitbox.y);
        continue;
      }

      // Big Invader collision
      if (model->invaders.big_invader.alive && model_check_collision(pb->hitbox, model->invaders.big_invader.hitbox)) {
        BigInvader *bi = &model->invaders.big_invader;
        bi->health -= (pb->is_strong ? 3 : 1);
        if (!pb->is_strong) pb->alive = false;
        model->players[p_idx].score += apply_difficulty_multiplier(model, 15);
        if (bi->health <= 0) {
          bi->alive = false;
          model->players[p_idx].score += apply_difficulty_multiplier(model, bi->points);
          model_drop_powerup(model, bi->hitbox.x, bi->hitbox.y);
        }
        continue;
      }


      for (int i = 0; i < INVADER_ROWS; i++) {
        for (int j = 0; j < INVADER_COLS; j++) {
          Invader *inv = &model->invaders.invaders[i][j];
          if (inv->alive && inv->dying_timer == 0 && model_check_collision(pb->hitbox, inv->hitbox)) {
            if (!pb->is_strong) pb->alive = false;
            inv->dying_timer = 5;
            model->players[p_idx].score += apply_difficulty_multiplier(model, inv->points);
            model->players[p_idx].combo_count++;
            model->players[p_idx].last_kill_time = platform_get_ticks();
            model->invaders.killed++;
            
            model_drop_powerup(model, inv->hitbox.x, inv->hitbox.y);

            if (model->invaders.killed >= INVADER_ROWS * INVADER_COLS) model_next_level(model);
            if (!pb->is_strong) goto next_bullet;
          }
        }
      }
      next_bullet:;
    }
  }

  for (int b = 0; b < ENEMY_BULLETS; b++) {
    if (!model->enemy_bullets[b].alive) continue;
    for (int p = 0; p < 2; p++) {
      if (model->players[p].is_active && model_check_collision(model->enemy_bullets[b].hitbox, model->players[p].hitbox)) {
        model->enemy_bullets[b].alive = false;
        if (model->players[p].active_powerup == PWR_SHIELD) {
            model->players[p].active_powerup = PWR_NONE;
            model->players[p].powerup_timer = 0;
            model->players[p].invincibility_timer = 2.0f; // Brief immunity after shield break
            model->needs_redraw = true;
        } else if (model->players[p].invincibility_timer <= 0) {
            model->players[p].lives--;
            model->players[p].invincibility_timer = 2.0f; // 2 seconds of immunity
            model->players[p].combo_count = 0;
            
            // Reposition player to center to avoid death loop in corner?
            // Optional: reset_player_pos(&model->players[p], p); 
            
            if (model->players[0].lives <= 0 && (!model->two_player_mode || model->players[1].lives <= 0)) {
              model->state = STATE_GAME_OVER;
              model_save_high_score(model);
            }
        }
      }
    }
    for (int i = 0; i < BASE_COUNT; i++) {
        if (model->bases[i].alive && model_check_collision(model->enemy_bullets[b].hitbox, model->bases[i].hitbox)) {
            model->enemy_bullets[b].alive = false;
            model->bases[i].health -= 10;
            if (model->bases[i].health <= 0) model->bases[i].alive = false;
        }
    }
  }
}

void model_check_player_invader_collision(GameModel *model) {
  if (model->boss.alive) return;
  for (int i = 0; i < INVADER_ROWS; i++) {
    for (int j = 0; j < INVADER_COLS; j++) {
      if (model->invaders.invaders[i][j].alive) {
        for(int p=0; p<2; p++) {
            if (model->players[p].is_active && model->invaders.invaders[i][j].hitbox.y + INVADER_HEIGHT >= model->players[p].hitbox.y) {
              model->players[p].lives = 0;
              if (model->players[0].lives <= 0 && (!model->two_player_mode || model->players[1].lives <= 0)) {
                  model->state = STATE_GAME_OVER;
              }
            }
        }
      }
    }
  }
}

void model_check_invader_base_collisions(GameModel *model) {
  if (model->boss.alive) return;
  for (int i = 0; i < INVADER_ROWS; i++) {
    for (int j = 0; j < INVADER_COLS; j++) {
      if (model->invaders.invaders[i][j].alive) {
        for (int b = 0; b < BASE_COUNT; b++) {
          if (model->bases[b].alive && model_check_collision(model->invaders.invaders[i][j].hitbox, model->bases[b].hitbox))
            model->bases[b].alive = false;
        }
      }
    }
  }
}

void model_set_state(GameModel *model, GameState state) { model->state = state; }
void model_toggle_pause(GameModel *model) {
  if (model->state == STATE_PLAYING) model->state = STATE_PAUSED;
  else if (model->state == STATE_PAUSED) model->state = STATE_PLAYING;
}
void model_save_high_score(GameModel *model) {
  int total = model->players[0].score + model->players[1].score;
  if (total > model->high_score) {
    model->high_score = total;
    FILE *f = fopen("highscore.dat", "wb");
    if (f) { fwrite(&model->high_score, 4, 1, f); fclose(f); }
  }
}
void model_load_high_score(GameModel *model) {
  FILE *f = fopen("highscore.dat", "rb");
  if (f) { fread(&model->high_score, 4, 1, f); fclose(f); }
  else model->high_score = 0;
}
int model_get_score(const GameModel *model) { return model->players[0].score + model->players[1].score; }
int model_get_lives(const GameModel *model) { return model->players[0].lives; }
int model_get_level(const GameModel *model) { return model->players[0].level; }
GameState model_get_state(const GameModel *model) { return model->state; }
void model_process_command(GameModel *model, int command, void *data) { (void)model; (void)command; (void)data; }

int model_get_max_menu_items(const GameModel *model) {
  switch (model->menu_state) {
    case MENU_MAIN: return 4;         // Start 1P, Start 2P, Settings, Quit
    case MENU_DIFFICULTY: return 4;   // Easy, Normal, Hard, Rogue
    case MENU_SETTINGS: return 3;     // Controls, Volume, Back
    case MENU_CONTROLS: return 11;    // 5 P1 + 5 P2 + Back = 11
    default: return 3;
  }
}

void model_process_menu_input(GameModel *model, int direction) {
  // If waiting for key, don't process normal navigation
  if (model->waiting_for_key) return;
  
  int max_items = model_get_max_menu_items(model);
  if (direction == -1) { model->menu_selection--; if (model->menu_selection < 0) model->menu_selection = max_items - 1; }
  else if (direction == 1) { model->menu_selection++; if (model->menu_selection >= max_items) model->menu_selection = 0; }
  else if (direction == 0) {
    switch (model->menu_state) {
      case MENU_MAIN:
        if (model->menu_selection == 0) { model->two_player_mode = false; model->menu_state = MENU_DIFFICULTY; }
        else if (model->menu_selection == 1) { model->two_player_mode = true; model->menu_state = MENU_DIFFICULTY; }
        else if (model->menu_selection == 2) { model->menu_state = MENU_SETTINGS; }
        else if (model->menu_selection == 3) { model->state = STATE_QUIT; }
        break;
      case MENU_DIFFICULTY:
        model->difficulty = (Difficulty)model->menu_selection;
        model_reset_game(model);
        model->state = STATE_PLAYING;
        break;
      case MENU_SETTINGS:
        if (model->menu_selection == 0) { model->menu_state = MENU_CONTROLS; model->menu_selection = 0; }
        else if (model->menu_selection == 1) { /* Volume handled by arrows */ }
        else if (model->menu_selection == 2) { model->menu_state = MENU_MAIN; model->menu_selection = 2; }
        break;
      case MENU_CONTROLS:
        if (model->menu_selection == 10) { 
            model->menu_state = MENU_SETTINGS; 
            model->menu_selection = 0; 
        } else {
            // Start editing this keybind
            model->editing_keybind = model->menu_selection;
            model->waiting_for_key = true;
        }
        break;
      default: break;
    }
  }
  model->needs_redraw = true;
}

void model_apply_difficulty(GameModel *model) { (void)model; }
void model_adjust_music_volume(GameModel *model, int direction) {
  if (direction == -1) model->music_volume = fmaxf(0.0f, model->music_volume - 0.1f);
  else if (direction == 1) model->music_volume = fminf(1.0f, model->music_volume + 0.1f);
  model->needs_redraw = true;
}

void model_set_keybind(GameModel *model, int keycode) {
  if (!model->waiting_for_key || model->editing_keybind < 0) return;
  
  int idx = model->editing_keybind;
  int old_key;
  // Get the old key currently assigned to the action we are editing
  if (idx < 5) old_key = model->keybinds_p1[idx];
  else old_key = model->keybinds_p2[idx - 5];

  // Check for conflicts and SWAP
  // If another action uses the new key, give it our old key
  for (int i = 0; i < 5; i++) {
    if (model->keybinds_p1[i] == keycode) {
      model->keybinds_p1[i] = old_key;
    }
  }
  for (int i = 0; i < 5; i++) {
    if (model->keybinds_p2[i] == keycode) {
      model->keybinds_p2[i] = old_key;
    }
  }
  
  // Assign the new key
  if (idx < 5) {
    model->keybinds_p1[idx] = keycode;
  } else if (idx < 10) {
    model->keybinds_p2[idx - 5] = keycode;
  }
  
  model->waiting_for_key = false;
  model->editing_keybind = -1;
  model->needs_redraw = true;
}
