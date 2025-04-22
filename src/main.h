#ifndef MAIN_H
#define MAIN_H

#define THE_DEVIL_VERSION_STRING "The Devil v1.2 (c) 2010 T^3 Software."

#include "t3f/animation.h"
#include "t3f/controller.h"
#include "t3f/gui.h"
#include "t3f/rng.h"
#include "t3net/leaderboard.h"
#include "cinema.h"
#include "game.h"
#include "particle.h"

#define STATE_INTRO        0
//#define STATE_TITLE_MENU   1
#define STATE_GAME         2
#define STATE_LOGO         3
#define STATE_ENDING       4
#define STATE_TITLE_IN     5
#define STATE_TITLE_OUT    6
#define STATE_LEADERBOARDS 7
#define STATE_TITLE_GAME   8
#define STATE_TITLE        9

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
#define TITLE_MENU_NETWORK_ID      14
#define MENU_TITLE                 15
#define MENU_PRIVACY               16

#define MAX_BITMAPS            16
#define BITMAP_ICON            11

#define MAX_ANIMATIONS         16
#define ANIMATION_PLAYER        0
#define ANIMATION_DEMON         1
#define ANIMATION_ARCHDEMON     2
#define ANIMATION_PLAYER_SHOT   3
#define ANIMATION_ENEMY_SHOT    4
#define ANIMATION_SPIRIT        5
#define ANIMATION_TITLE         7
#define ANIMATION_CROSSHAIR     8
#define ANIMATION_CURSOR        9
#define ANIMATION_PLAYER_SHOT_PARTICLE 10
#define ANIMATION_TITLE_EYES   11
#define ANIMATION_DARK_ORB     12
#define ANIMATION_DARK_SHIELD  13
#define ANIMATION_RADAR        14
#define ANIMATION_RADAR_DOT    15

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

#define CONTROLLER_TYPES        6
#define CONTROLLER_TYPE_NORMAL  0
#define CONTROLLER_TYPE_MOUSE   1
#define CONTROLLER_TYPE_ANALOG  2
#define CONTROLLER_TYPE_TOUCH_S 3
#define CONTROLLER_TYPE_TOUCH_M 4
#define CONTROLLER_TYPE_TOUCH_L 5

#define CONTROLLER_UP           0
#define CONTROLLER_DOWN         1
#define CONTROLLER_LEFT         2
#define CONTROLLER_RIGHT        3
#define CONTROLLER_FIRE_UP      4
#define CONTROLLER_FIRE_DOWN    5
#define CONTROLLER_FIRE_LEFT    6
#define CONTROLLER_FIRE_RIGHT   7

typedef struct
{

	int touch_id;
	float pin_x, pin_y;
	float pos_x, pos_y;
	bool active;

} TOUCH_STICK;

void detect_controller(int type, void * data);
void render_mouse(void * data);

#endif
