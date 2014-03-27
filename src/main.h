#ifndef MAIN_H
#define MAIN_H

#define THE_DEVIL_VERSION_STRING "The Devil v1.2 (c) 2010 T^3 Software."

#include "t3f/animation.h"
#include "t3f/controller.h"
#include "t3f/gui.h"
#include "t3f/rng.h"
#include "t3net/t3net.h"
#include "cinema.h"
#include "game.h"
#include "particle.h"

#define STATE_INTRO  0
#define STATE_TITLE  1
#define STATE_GAME   2
#define STATE_LOGO   3
#define STATE_ENDING 4
#define STATE_TITLE_IN 5
#define STATE_TITLE_OUT 6
#define STATE_LEADERBOARDS 7
#define STATE_TITLE_GAME 8

#define TITLE_MAX_MENUS            32
#define TITLE_MENU_MAIN             0
#define TITLE_MENU_DIFFICULTY       1
#define TITLE_MENU_SETTINGS         2
#define TITLE_MENU_CONTROL_NORMAL   3
#define TITLE_MENU_CONTROL_MOUSE    4
#define TITLE_MENU_CONTROL_ANALOG   5
#define TITLE_MENU_CONTROL          6
#define TITLE_MENU_INTERNET         7
#define LEADERBOARD_MENU            8
#define TITLE_MENU_MODE             9
#define PAUSE_MENU                 10
#define TITLE_MENU_FIRST           11
#define TITLE_MENU_DISPLAY         12
#define TITLE_MENU_ANALOG          13

#define MAX_BITMAPS            16
#define BITMAP_ICON            11

#define MAX_ANIMATIONS        256
#define ANIMATION_PLAYER        0
#define ANIMATION_DEMON         1
#define ANIMATION_ARCHDEMON     2
#define ANIMATION_PLAYER_SHOT   3
#define ANIMATION_ENEMY_SHOT    4
#define ANIMATION_SPIRIT        5
#define ANIMATION_LOGO          6
#define ANIMATION_TITLE         7
#define ANIMATION_CROSSHAIR     8
#define ANIMATION_CURSOR        9
#define ANIMATION_PLAYER_SHOT_PARTICLE 10
#define ANIMATION_TITLE_EYES   11
#define ANIMATION_DARK_ORB     12
#define ANIMATION_DARK_SHIELD  13

#define MAX_FONTS               8
#define FONT_LARGE              0
#define FONT_SMALL              1
#define FONT_TINY               2

#define MAX_SAMPLES            16
#define SAMPLE_LEVEL_UP         0
#define SAMPLE_GAME_OVER        1
#define SAMPLE_HIGH_SCORE       2
#define SAMPLE_MAX_MULTIPLIER   3
#define SAMPLE_TWIN_SHOT        4
#define SAMPLE_TRIPLE_SHOT      5
#define SAMPLE_POWERUP          6
#define SAMPLE_SHOOT            7
#define SAMPLE_DIE              8
#define SAMPLE_HIT              9
#define SAMPLE_MULTIPLIER      10
#define SAMPLE_ENEMY_SHOOT     11
#define SAMPLE_ORB_DIE         12
#define SAMPLE_REJECTED        13
#define SAMPLE_LOGO            14

#define ENEMY_TYPE_DEMON        0
#define ENEMY_TYPE_ARCHDEMON    1
#define ENEMY_TYPE_DARK_ORB     2
#define ENEMY_TYPE_DARK_SHIELD  3

#define PARTICLE_TYPE_PLAYER_SHOT       0
#define PARTICLE_TYPE_ENEMY_SHOT        1
#define PARTICLE_TYPE_PLAYER_FLESH      2
#define PARTICLE_TYPE_PLAYER_CLOTHES    3
#define PARTICLE_TYPE_DEMON_FLESH       4
#define PARTICLE_TYPE_DEMON_CLOTHES     5
#define PARTICLE_TYPE_ARCHDEMON_FLESH   6
#define PARTICLE_TYPE_ARCHDEMON_CLOTHES 7
#define PARTICLE_TYPE_REMAINS           8
#define PARTICLE_TYPE_DARK_ORB          9
#define PARTICLE_TYPE_DARK_SHIELD      10

#define POWERUP_TYPE_SPIRIT     0
#define POWERUP_TYPE_UPGRADE    1

#define CONTROLLER_TYPES        3
#define CONTROLLER_TYPE_NORMAL  0
#define CONTROLLER_TYPE_MOUSE   1
#define CONTROLLER_TYPE_ANALOG  2

#define CONTROLLER_UP           0
#define CONTROLLER_DOWN         1
#define CONTROLLER_LEFT         2
#define CONTROLLER_RIGHT        3
#define CONTROLLER_FIRE_UP      4
#define CONTROLLER_FIRE_DOWN    5
#define CONTROLLER_FIRE_LEFT    6
#define CONTROLLER_FIRE_RIGHT   7

extern int state;
extern int state_ticks;
extern T3F_ANIMATION * animation[MAX_ANIMATIONS];
extern ALLEGRO_FONT * font[MAX_FONTS];
extern ALLEGRO_SAMPLE * sample[MAX_SAMPLES];
extern ALLEGRO_BITMAP * bitmap[MAX_BITMAPS];
extern T3F_ATLAS * atlas;
extern CINEMA * cinema;
extern CINEMA * ending_cinema;
extern int current_menu;
extern T3F_CONTROLLER * controller;
extern int controller_type;
extern bool click;
extern bool mouse_disabled;
extern T3F_RNG_STATE rng_state;

/* menu data */
extern T3F_GUI * menu[TITLE_MAX_MENUS];
extern int current_menu;
extern char menu_text[32][64];

/* game objects */
extern LEVEL_DATA  level;
extern GAME_ENTITY player;
extern GAME_ENTITY enemy[GAME_MAX_ENEMIES];
extern GAME_ENTITY player_shot[GAME_MAX_PLAYER_SHOTS];
extern GAME_ENTITY enemy_shot[GAME_MAX_ENEMY_SHOTS];
extern GAME_ENTITY powerup[GAME_MAX_POWERUPS];
extern GAME_ENTITY particle[GAME_MAX_PARTICLES];
extern GAME_ENTITY_SPAWNER enemy_spawner[GAME_MAX_ENTITY_SPAWNERS];
extern TEXT_PARTICLE text_particle[GAME_MAX_TEXT_PARTICLES];
extern int enemy_spawners;
extern int current_level;
extern int score;
extern int high_score[16];
extern bool got_high_score;
extern int multiplier;
extern int multiplier_tick;
extern int weapon;
extern int game_state;
extern int resume_state;
extern int die_state;
extern int die_timer;
extern int lives;
void (*enemy_spawn_logic)();
extern bool konami_mode;
extern bool fire_power;
extern bool finale_mode;
extern int game_mode;
extern char * game_mode_text[16];
extern int death_time;
extern int flash_time;
extern int fade_time;

/* network data */
extern int network_id_pos;
extern bool network_id_entry;
extern T3NET_LEADERBOARD * leaderboard;
extern int leaderboard_place;
extern int leaderboard_mode;

/* config */
extern char network_id[256];
extern bool upload_scores;
extern int display_type;
extern bool force_aspect;

void detect_controller(int type);
void render_mouse(void);

#endif
