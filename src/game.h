#ifndef GAME_H
#define GAME_H

#include "t3f/collision.h"

#define GAME_MAX_ENEMIES         256
#define GAME_MAX_PLAYER_SHOTS    256
#define GAME_MAX_ENEMY_SHOTS     256
#define GAME_MAX_ENTITY_SPAWNERS 128
#define GAME_MAX_POWERUPS         64
#define GAME_MAX_PARTICLES       256
#define GAME_MAX_TEXT_PARTICLES   32

#define GAME_STATE_PLAY            0
#define GAME_STATE_DIE             1
#define GAME_STATE_OVER            2
#define GAME_STATE_END             3
#define GAME_STATE_PAUSED          4
#define GAME_STATE_BOSS            5

#define GAME_MODE_STORY            0
#define GAME_MODE_STORY_EASY       1
#define GAME_MODE_ETERNAL          2

#define GAME_STATS_MARGIN          4

typedef struct
{
	
	int demon_rate;
	float demon_speed;
	int demon_fire_rate;
	int archdemon_rate;
	int archdemon_fire_rate;
	int fireball_rate;
	int nuke_rate;
	
} LEVEL_DATA;

typedef struct
{
	
	T3F_COLLISION_OBJECT * object;
	
	int type;
	float x, y, z, vx, vy, speed, angle;
	int tick;
	int var, var2;
	bool active;
	
} GAME_ENTITY;

typedef struct
{
	
	float x, y;
	
} GAME_ENTITY_SPAWNER;

void define_game_entity(GAME_ENTITY * ep, int type);

bool game_init(int mode);
void game_exit(void);
bool game_load_level(LEVEL_DATA * lp, int level);
void game_logic(void);
void game_render(void);

#endif
