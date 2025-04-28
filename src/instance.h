#ifndef _TD_INSTANCE_H
#define _TD_INSTANCE_H

#include "t3f/t3f.h"
#include "t3f/extra/leaderboard.h"
#include "main.h"

typedef struct
{

  /* main data */
  int state;
  int state_ticks;
  T3F_ANIMATION * animation[MAX_ANIMATIONS];
  T3F_FONT * font[MAX_FONTS];
  ALLEGRO_SAMPLE * sample[MAX_SAMPLES];
  T3F_BITMAP * bitmap[MAX_BITMAPS];
  T3F_ATLAS * atlas;
  CINEMA * cinema;
  CINEMA * ending_cinema;
  T3F_CONTROLLER * controller;
  int controller_type;
  bool click;
  bool first_run;
  bool mouse_disabled;
  T3F_RNG_STATE rng_state;
  char copyright_message[256];
  ALLEGRO_USTR * copyright_message_ustr;
  ALLEGRO_USTR * copyright_message_uchar;
  float copyright_message_width;

  /* menu data */
  float logo_pos_x, logo_pos_y;
  T3F_GUI * menu[TITLE_MAX_MENUS];
  int current_menu;
  char menu_text[32][64];

  /* game data */
  LEVEL_DATA level;
  GAME_ENTITY player;
  GAME_ENTITY enemy[GAME_MAX_ENEMIES];
  GAME_ENTITY player_shot[GAME_MAX_PLAYER_SHOTS];
  GAME_ENTITY enemy_shot[GAME_MAX_ENEMY_SHOTS];
  GAME_ENTITY powerup[GAME_MAX_POWERUPS];
  GAME_ENTITY particle[GAME_MAX_PARTICLES];
  GAME_ENTITY_SPAWNER enemy_spawner[GAME_MAX_ENTITY_SPAWNERS];
  TEXT_PARTICLE text_particle[GAME_MAX_TEXT_PARTICLES];
  int enemy_spawners;
  int current_level;
  int score;
  int high_score[16];
  bool got_high_score;
  int multiplier;
  int multiplier_tick;
  int weapon;
  int game_state;
  int resume_state;
  int die_state;
  int die_timer;
  int lives;
  void (*enemy_spawn_logic)(void * data);
  bool konami_mode;
  bool fire_power;
  bool finale_mode;
  int game_mode;
  char * game_mode_text[16];
  int death_time;
  int flash_time;
  int fade_time;
  TOUCH_STICK touch_stick[2];
  float touch_size;

  /* network data */
  int network_id_pos;
  bool network_id_entry;
  T3F_LEADERBOARD * leaderboard;
  int leaderboard_place;
  int leaderboard_mode;

  /* config */
  char network_id[256];
  bool upload_scores;
  int display_type;
  bool force_aspect;

} APP_INSTANCE;

APP_INSTANCE * td_create_instance(void);
void td_destroy_instance(APP_INSTANCE * instance);

#endif
