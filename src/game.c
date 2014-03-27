#include <stdio.h>
#include "t3f/t3f.h"
#include "t3f/animation.h"
#include "t3f/sound.h"
#include "t3f/music.h"
#include "t3f/resource.h"
#include "t3f/atlas.h"
#include "main.h"
#include "game.h"
#include "title.h"
#include "player.h"
#include "enemy.h"
#include "projectile.h"
#include "powerup.h"
#include "particle.h"
#include "leaderboard.h"

bool game_load_data(void)
{
	animation[ANIMATION_PLAYER] = t3f_load_animation_from_bitmap("data/graphics/player.png");
	if(!animation[ANIMATION_PLAYER])
	{
		return false;
	}
	animation[ANIMATION_DEMON] = t3f_load_animation_from_bitmap("data/graphics/demon.png");
	if(!animation[ANIMATION_DEMON])
	{
		return false;
	}
	animation[ANIMATION_ARCHDEMON] = t3f_load_animation_from_bitmap("data/graphics/archdemon.png");
	if(!animation[ANIMATION_ARCHDEMON])
	{
		return false;
	}
	animation[ANIMATION_PLAYER_SHOT] = t3f_load_animation_from_bitmap("data/graphics/player_shot.png");
	if(!animation[ANIMATION_PLAYER_SHOT])
	{
		return false;
	}
	animation[ANIMATION_ENEMY_SHOT] = t3f_load_animation_from_bitmap("data/graphics/enemy_shot.png");
	if(!animation[ANIMATION_ENEMY_SHOT])
	{
		return false;
	}
	
	/* load spirit animation */
	animation[ANIMATION_SPIRIT] = t3f_create_animation();
	if(!animation[ANIMATION_SPIRIT])
	{
		return false;
	}
	animation[ANIMATION_SPIRIT]->bitmap[0] = t3f_load_resource((void **)(&animation[ANIMATION_SPIRIT]->bitmap[0]), T3F_RESOURCE_TYPE_BITMAP, "data/graphics/spirit0.png", 0, 0, 0);
	animation[ANIMATION_SPIRIT]->bitmap[1] = t3f_load_resource((void **)(&animation[ANIMATION_SPIRIT]->bitmap[1]), T3F_RESOURCE_TYPE_BITMAP, "data/graphics/spirit1.png", 0, 0, 0);
	animation[ANIMATION_SPIRIT]->bitmaps = 2;
	t3f_animation_add_frame(animation[ANIMATION_SPIRIT], 0, 0, 0, 0, al_get_bitmap_width(animation[ANIMATION_SPIRIT]->bitmap[0]), al_get_bitmap_height(animation[ANIMATION_SPIRIT]->bitmap[0]), 0, 5);
	t3f_animation_add_frame(animation[ANIMATION_SPIRIT], 1, 0, 0, 0, al_get_bitmap_width(animation[ANIMATION_SPIRIT]->bitmap[1]), al_get_bitmap_height(animation[ANIMATION_SPIRIT]->bitmap[1]), 0, 5);
	
	/* load dark orb animation */
	animation[ANIMATION_DARK_ORB] = t3f_create_animation();
	if(!animation[ANIMATION_DARK_ORB])
	{
		return false;
	}
	animation[ANIMATION_DARK_ORB]->bitmap[0] = t3f_load_resource((void **)(&animation[ANIMATION_DARK_ORB]->bitmap[0]), T3F_RESOURCE_TYPE_BITMAP, "data/graphics/dark_orb0.png", 0, 0, 0);
	animation[ANIMATION_DARK_ORB]->bitmap[1] = t3f_load_resource((void **)(&animation[ANIMATION_DARK_ORB]->bitmap[1]), T3F_RESOURCE_TYPE_BITMAP, "data/graphics/dark_orb1.png", 0, 0, 0);
	animation[ANIMATION_DARK_ORB]->bitmaps = 2;
	t3f_animation_add_frame(animation[ANIMATION_DARK_ORB], 0, 0, 0, 0, al_get_bitmap_width(animation[ANIMATION_DARK_ORB]->bitmap[0]), al_get_bitmap_height(animation[ANIMATION_DARK_ORB]->bitmap[0]), 0, 5);
	t3f_animation_add_frame(animation[ANIMATION_DARK_ORB], 1, 0, 0, 0, al_get_bitmap_width(animation[ANIMATION_DARK_ORB]->bitmap[1]), al_get_bitmap_height(animation[ANIMATION_DARK_ORB]->bitmap[1]), 0, 5);

	animation[ANIMATION_CROSSHAIR] = t3f_load_animation_from_bitmap("data/graphics/crosshair.png");
	if(!animation[ANIMATION_CROSSHAIR])
	{
		return false;
	}
	
	animation[ANIMATION_PLAYER_SHOT_PARTICLE] = t3f_load_animation_from_bitmap("data/graphics/player_shot_particle.png");
	if(!animation[ANIMATION_PLAYER_SHOT_PARTICLE])
	{
		return false;
	}
	animation[ANIMATION_DARK_SHIELD] = t3f_load_animation_from_bitmap("data/graphics/dark_orb_shield.png");
	if(!animation[ANIMATION_DARK_SHIELD])
	{
		return false;
	}
	atlas = t3f_create_atlas(512, 512);
	if(!atlas)
	{
		return false;
	}
	t3f_add_animation_to_atlas(atlas, animation[ANIMATION_PLAYER], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(atlas, animation[ANIMATION_DEMON], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(atlas, animation[ANIMATION_ARCHDEMON], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(atlas, animation[ANIMATION_PLAYER_SHOT], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(atlas, animation[ANIMATION_ENEMY_SHOT], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(atlas, animation[ANIMATION_SPIRIT], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(atlas, animation[ANIMATION_CROSSHAIR], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(atlas, animation[ANIMATION_PLAYER_SHOT_PARTICLE], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(atlas, animation[ANIMATION_DARK_ORB], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(atlas, animation[ANIMATION_DARK_SHIELD], T3F_ATLAS_SPRITE);

	/* load the first two level backdrops */
	bitmap[0] = t3f_load_resource((void **)(&bitmap[0]), T3F_RESOURCE_TYPE_BITMAP, "data/graphics/bg00.png", 0, 0, 0);
	if(!bitmap[0])
	{
		return false;
	}
	bitmap[1] = t3f_load_resource((void **)(&bitmap[1]), T3F_RESOURCE_TYPE_BITMAP, "data/graphics/bg01.png", 0, 0, 0);
	if(!bitmap[1])
	{
		return false;
	}

	sample[SAMPLE_GAME_OVER] = al_load_sample("data/sounds/game_over.ogg");
	if(!sample[SAMPLE_GAME_OVER])
	{
		return false;
	}
	sample[SAMPLE_LEVEL_UP] = al_load_sample("data/sounds/level_up.ogg");
	if(!sample[SAMPLE_LEVEL_UP])
	{
		return false;
	}
	sample[SAMPLE_MAX_MULTIPLIER] = al_load_sample("data/sounds/max_multiplier.ogg");
	if(!sample[SAMPLE_MAX_MULTIPLIER])
	{
		return false;
	}
	sample[SAMPLE_HIGH_SCORE] = al_load_sample("data/sounds/high_score.ogg");
	if(!sample[SAMPLE_HIGH_SCORE])
	{
		return false;
	}
	sample[SAMPLE_TWIN_SHOT] = al_load_sample("data/sounds/twin_shot.ogg");
	if(!sample[SAMPLE_TWIN_SHOT])
	{
		return false;
	}
	sample[SAMPLE_TRIPLE_SHOT] = al_load_sample("data/sounds/triple_shot.ogg");
	if(!sample[SAMPLE_TRIPLE_SHOT])
	{
		return false;
	}
	sample[SAMPLE_POWERUP] = al_load_sample("data/sounds/powerup.ogg");
	if(!sample[SAMPLE_POWERUP])
	{
		return false;
	}
	sample[SAMPLE_SHOOT] = al_load_sample("data/sounds/shoot.ogg");
	if(!sample[SAMPLE_SHOOT])
	{
		return false;
	}
	sample[SAMPLE_HIT] = al_load_sample("data/sounds/hit.ogg");
	if(!sample[SAMPLE_HIT])
	{
		return false;
	}
	sample[SAMPLE_DIE] = al_load_sample("data/sounds/die.ogg");
	if(!sample[SAMPLE_DIE])
	{
		return false;
	}
	sample[SAMPLE_MULTIPLIER] = al_load_sample("data/sounds/multiplier.ogg");
	if(!sample[SAMPLE_MULTIPLIER])
	{
		return false;
	}
	sample[SAMPLE_ENEMY_SHOOT] = al_load_sample("data/sounds/enemy_shoot.ogg");
	if(!sample[SAMPLE_ENEMY_SHOOT])
	{
		return false;
	}
	sample[SAMPLE_ORB_DIE] = al_load_sample("data/sounds/orb_die.ogg");
	if(!sample[SAMPLE_ORB_DIE])
	{
		return false;
	}
	sample[SAMPLE_REJECTED] = al_load_sample("data/sounds/rejected.ogg");
	if(!sample[SAMPLE_REJECTED])
	{
		return false;
	}
	return true;
}

void game_free_data(void)
{
	int i;
	
	t3f_destroy_animation(animation[ANIMATION_PLAYER]);
	animation[ANIMATION_PLAYER] = NULL;
	t3f_destroy_animation(animation[ANIMATION_DEMON]);
	animation[ANIMATION_DEMON] = NULL;
	t3f_destroy_animation(animation[ANIMATION_ARCHDEMON]);
	animation[ANIMATION_ARCHDEMON] = NULL;
	t3f_destroy_animation(animation[ANIMATION_PLAYER_SHOT]);
	animation[ANIMATION_PLAYER_SHOT] = NULL;
	t3f_destroy_animation(animation[ANIMATION_ENEMY_SHOT]);
	animation[ANIMATION_ENEMY_SHOT] = NULL;
	t3f_destroy_animation(animation[ANIMATION_SPIRIT]);
	animation[ANIMATION_SPIRIT] = NULL;
	t3f_destroy_animation(animation[ANIMATION_DARK_ORB]);
	animation[ANIMATION_DARK_ORB] = NULL;
	t3f_destroy_animation(animation[ANIMATION_CROSSHAIR]);
	animation[ANIMATION_CROSSHAIR] = NULL;
	t3f_destroy_animation(animation[ANIMATION_PLAYER_SHOT_PARTICLE]);
	animation[ANIMATION_PLAYER_SHOT_PARTICLE] = NULL;
	t3f_destroy_animation(animation[ANIMATION_DARK_SHIELD]);
	animation[ANIMATION_DARK_SHIELD] = NULL;
	t3f_destroy_atlas(atlas);
	atlas = NULL;

	for(i = 0; i < MAX_BITMAPS; i++)
	{
		if(bitmap[i])
		{
			t3f_destroy_resource(bitmap[i]);
			bitmap[i] = NULL;
		}
	}

	al_destroy_sample(sample[SAMPLE_GAME_OVER]);
	sample[SAMPLE_GAME_OVER] = NULL;
	al_destroy_sample(sample[SAMPLE_LEVEL_UP]);
	sample[SAMPLE_LEVEL_UP] = NULL;
	al_destroy_sample(sample[SAMPLE_MAX_MULTIPLIER]);
	sample[SAMPLE_MAX_MULTIPLIER] = NULL;
	al_destroy_sample(sample[SAMPLE_HIGH_SCORE]);
	sample[SAMPLE_HIGH_SCORE] = NULL;
	al_destroy_sample(sample[SAMPLE_TWIN_SHOT]);
	sample[SAMPLE_TWIN_SHOT] = NULL;
	al_destroy_sample(sample[SAMPLE_TRIPLE_SHOT]);
	sample[SAMPLE_TRIPLE_SHOT] = NULL;
	al_destroy_sample(sample[SAMPLE_POWERUP]);
	sample[SAMPLE_POWERUP] = NULL;
	al_destroy_sample(sample[SAMPLE_SHOOT]);
	sample[SAMPLE_SHOOT] = NULL;
	al_destroy_sample(sample[SAMPLE_HIT]);
	sample[SAMPLE_HIT] = NULL;
	al_destroy_sample(sample[SAMPLE_DIE]);
	sample[SAMPLE_DIE] = NULL;
	al_destroy_sample(sample[SAMPLE_MULTIPLIER]);
	sample[SAMPLE_MULTIPLIER] = NULL;
	al_destroy_sample(sample[SAMPLE_ENEMY_SHOOT]);
	sample[SAMPLE_ENEMY_SHOOT] = NULL;
	al_destroy_sample(sample[SAMPLE_ORB_DIE]);
	sample[SAMPLE_ORB_DIE] = NULL;
	al_destroy_sample(sample[SAMPLE_REJECTED]);
	sample[SAMPLE_REJECTED] = NULL;
}

bool game_init(int mode)
{
	int i;
	
	game_mode = mode;
	
	al_stop_timer(t3f_timer);
	game_load_data();
	al_start_timer(t3f_timer);

	/* create spawners */
	enemy_spawners = 0;
	for(i = 0; i < (640 + 32 + 32) / 32; i++)
	{
		enemy_spawner[enemy_spawners].x = -32 + i * 32;
		enemy_spawner[enemy_spawners].y = -32;
		enemy_spawners++;
		enemy_spawner[enemy_spawners].x = -32 + i * 32;
		enemy_spawner[enemy_spawners].y = 480;
		enemy_spawners++;
	}
	for(i = 0; i < (480 + 32 + 32) / 32; i++)
	{
		enemy_spawner[enemy_spawners].x = -32;
		enemy_spawner[enemy_spawners].y = -32 + i * 32;
		enemy_spawners++;
		enemy_spawner[enemy_spawners].x = 640;
		enemy_spawner[enemy_spawners].y = -32 + i * 32;
		enemy_spawners++;
	}
	
	/* create game objects */
	player.object = t3f_create_collision_object(8, 8, 16, 16, 32, 32, 0);
	for(i = 0; i < GAME_MAX_ENEMIES; i++)
	{
		enemy[i].object = t3f_create_collision_object(8, 8, 16, 16, 32, 32, 0);
		enemy[i].active = false;
	}
	for(i = 0; i < GAME_MAX_PLAYER_SHOTS; i++)
	{
		player_shot[i].object = t3f_create_collision_object(0, 0, 16, 16, 32, 32, 0);
		player_shot[i].active = false;
	}
	for(i = 0; i < GAME_MAX_ENEMY_SHOTS; i++)
	{
		enemy_shot[i].object = t3f_create_collision_object(0, 0, 16, 16, 32, 32, 0);
		enemy_shot[i].active = false;
	}
	for(i = 0; i < GAME_MAX_POWERUPS; i++)
	{
		powerup[i].object = t3f_create_collision_object(0, 0, 16, 16, 32, 32, 0);
		powerup[i].active = false;
	}
	for(i = 0; i < GAME_MAX_PARTICLES; i++)
	{
		particle[i].active = false;
	}
	
	/* set game up */
	touch_stick[0].active = false;
	touch_stick[1].active = false;
	player.x = 304;
	player.y = 224;
	player.z = 0;
	player.speed = 4.0;
	player.active = true;
	state = STATE_GAME;
	score = 0;
	if(finale_mode)
	{
		current_level = 9;
	}
	else
	{
		current_level = 0;
	}
	game_load_level(&level, current_level);
	multiplier = 1;
	multiplier_tick = 0;
	if(konami_mode)
	{
		lives = 99;
	}
	else
	{
		lives = 3;
	}
	if(fire_power)
	{
		weapon = 2;
	}
	else
	{
		weapon = 0;
	}
	state_ticks = 0;
	if(finale_mode)
	{
		state_ticks = 1700;
	}
	got_high_score = false;
	fade_time = 0;
	game_state = GAME_STATE_PLAY;
	t3f_play_music("data/music/game.xm");
	return true;
}

void game_exit(void)
{
	int i;
	char text[256] = {0};
	
	/* destroy game objects */
	t3f_destroy_collision_object(player.object);
	for(i = 0; i < GAME_MAX_ENEMIES; i++)
	{
		t3f_destroy_collision_object(enemy[i].object);
	}
	for(i = 0; i < GAME_MAX_PLAYER_SHOTS; i++)
	{
		t3f_destroy_collision_object(player_shot[i].object);
	}
	for(i = 0; i < GAME_MAX_ENEMY_SHOTS; i++)
	{
		t3f_destroy_collision_object(enemy_shot[i].object);
	}
	for(i = 0; i < GAME_MAX_POWERUPS; i++)
	{
		t3f_destroy_collision_object(powerup[i].object);
	}
	sprintf(text, "%d", high_score[game_mode]);
	switch(game_mode)
	{
		case GAME_MODE_STORY:
		{
			al_set_config_value(t3f_config, "Save Data", "High Score (S)", text);
			break;
		}
		case GAME_MODE_STORY_EASY:
		{
			al_set_config_value(t3f_config, "Save Data", "High Score (SE)", text);
			break;
		}
		case GAME_MODE_ETERNAL:
		{
			al_set_config_value(t3f_config, "Save Data", "High Score (E)", text);
			break;
		}
	}
	al_stop_timer(t3f_timer);
	game_free_data();
	title_load_data();
	if(upload_scores && !konami_mode && !finale_mode && !fire_power)
	{
		t3net_upload_score("http://www.t3-i.com/leaderboards/poll.php", "devil", "1.1", game_mode_text[game_mode], "0", network_id, score * 2 + 'v' + 'g' + 'o' + 'l' + 'f');
		download_leaderboard();
		if(leaderboard)
		{
			leaderboard_place = -1;
			for(i = 0; i < leaderboard->entries; i++)
			{
				if(score * 2 + 'v' + 'g' + 'o' + 'l' + 'f' == leaderboard->entry[i]->score && !strcmp(network_id, leaderboard->entry[i]->name))
				{
					leaderboard_place = i;
					break;
				}
			}
			leaderboard_mode = game_mode;
			state = STATE_LEADERBOARDS;
		}
		else
		{
			current_menu = TITLE_MENU_MAIN;
			state = STATE_TITLE;
		}
	}
	else
	{
		current_menu = TITLE_MENU_MAIN;
		state = STATE_TITLE;
	}
	t3f_play_music("data/music/title.xm");
	al_start_timer(t3f_timer);
}

bool game_load_level(LEVEL_DATA * lp, int lev)
{
	switch(lev)
	{
		case 0:
		{
			lp->demon_rate = 30;
			lp->demon_speed = 2.0;
			lp->archdemon_rate = 0;
			lp->archdemon_fire_rate = 0;
			break;
		}
		case 1:
		{
			lp->demon_rate = 35;
			lp->demon_speed = 2.1;
			lp->archdemon_rate = 2;
			lp->archdemon_fire_rate = 10;
			break;
		}
		case 2:
		{
			lp->demon_rate = 40;
			lp->demon_speed = 2.2;
			lp->archdemon_rate = 4;
			lp->archdemon_fire_rate = 20;
			break;
		}
		case 3:
		{
			lp->demon_rate = 45;
			lp->demon_speed = 2.3;
			lp->archdemon_rate = 6;
			lp->archdemon_fire_rate = 20;
			break;
		}
		case 4:
		{
			lp->demon_rate = 50;
			lp->demon_speed = 2.4;
			lp->archdemon_rate = 8;
			lp->archdemon_fire_rate = 20;
			break;
		}
		case 5:
		{
			lp->demon_rate = 55;
			lp->demon_speed = 2.5;
			lp->archdemon_rate = 10;
			lp->archdemon_fire_rate = 20;
			break;
		}
		case 6:
		{
			lp->demon_rate = 60;
			lp->demon_speed = 2.6;
			lp->archdemon_rate = 12;
			lp->archdemon_fire_rate = 25;
			break;
		}
		case 7:
		{
			lp->demon_rate = 65;
			lp->demon_speed = 2.7;
			lp->archdemon_rate = 14;
			lp->archdemon_fire_rate = 25;
			break;
		}
		case 8:
		{
			lp->demon_rate = 70;
			lp->demon_speed = 2.8;
			lp->archdemon_rate = 16;
			lp->archdemon_fire_rate = 25;
			break;
		}
		case 9:
		{
			lp->demon_rate = 80;
			lp->demon_speed = 3.0;
			lp->archdemon_rate = 18;
			lp->archdemon_fire_rate = 25;
			break;
		}
		case 10:
		{
			lp->demon_rate = 0;
			lp->demon_speed = 3.0;
			lp->archdemon_rate = 0;
			lp->archdemon_fire_rate = 0;
			break;
		}
	}
	return true;
}

void game_load_next_level_bitmap(void)
{
	char buf[256];
	int last_bitmap, next_bitmap;

	last_bitmap = current_level;
	next_bitmap = (current_level + 2) % 11;
	sprintf(buf, "data/graphics/bg%02d.png", next_bitmap);
	t3f_destroy_resource(bitmap[last_bitmap]);
	bitmap[last_bitmap] = NULL;
	al_stop_timer(t3f_timer);
	bitmap[next_bitmap] = t3f_load_resource((void **)(&bitmap[next_bitmap]), T3F_RESOURCE_TYPE_BITMAP, buf, 0, 0, 0);
	al_start_timer(t3f_timer);
}

void game_level_logic(void)
{
	switch(game_mode)
	{
		case GAME_MODE_ETERNAL:
		{
			if(state_ticks % 300 == 0)
			{
				level.demon_rate++;
				level.demon_speed += 0.015;
			}
			if(state_ticks % 900 == 0)
			{
				level.archdemon_rate++;
				level.archdemon_fire_rate++;
			}
			if(state_ticks % 1800 == 0)
			{
				game_load_next_level_bitmap();
				current_level++;
				if(current_level == 2)
				{
					weapon++;
					al_play_sample(sample[SAMPLE_TWIN_SHOT], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
				if(current_level == 6)
				{
					weapon++;
					al_play_sample(sample[SAMPLE_TRIPLE_SHOT], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
				if(weapon > 2)
				{
					weapon = 2;
				}
			}
			break;
		}
		default:
		{
			if(state_ticks % 1800 == 0)
			{
				game_load_next_level_bitmap();
				current_level++;
				if(current_level == 2)
				{
					weapon++;
					al_play_sample(sample[SAMPLE_TWIN_SHOT], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
				if(current_level == 6)
				{
					weapon++;
					al_play_sample(sample[SAMPLE_TRIPLE_SHOT], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
				if(weapon > 2)
				{
					weapon = 2;
				}
				if(current_level > 10)
				{
					current_level = 10;
				}
				game_load_level(&level, current_level);
			}
			if(current_level >= 10 && count_enemies() <= 0)
			{
				t3f_play_music("data/music/boss.xm");
				enemy_spawn_dark_orb();
				game_state = GAME_STATE_BOSS;
			}
			break;
		}
	}
}

void game_logic(void)
{
	if(t3f_key[ALLEGRO_KEY_ESCAPE] && game_state != GAME_STATE_PAUSED)
	{
		resume_state = game_state;
		game_state = GAME_STATE_PAUSED;
		menu[PAUSE_MENU]->hover_element = 0;
		t3f_select_next_gui_element(menu[PAUSE_MENU]);
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
		return;
	}
	switch(game_state)
	{
		case GAME_STATE_PLAY:
		{
			enemy_spawn_logic();
			player_logic();
			enemy_logic();
			projectile_logic();
			powerup_logic();
			state_ticks++;
			game_level_logic();
			break;
		}
		case GAME_STATE_BOSS:
		{
			player_logic();
			enemy_logic();
			projectile_logic();
			powerup_logic();
			if(!enemy[0].active)
			{
				player.var = 0;
				player.angle = atan2(player.y - 240, player.x - 320) + ALLEGRO_PI;
				player.vx = cos(player.angle) * 2.0;
				player.vy = sin(player.angle) * 2.0;
				game_state = GAME_STATE_END;
			}
			break;
		}
		case GAME_STATE_DIE:
		{
			projectile_logic();
			enemy_logic();
			powerup_logic();
			death_time--;
			if(count_enemies() <= 0 && death_time <= 0)
			{
				player.x = 304;
				player.y = 224;
				player.speed = 4.0;
				player.active = true;
				multiplier = 1;
				game_state = die_state;
			}
			break;
		}
		case GAME_STATE_OVER:
		{
			projectile_logic();
			enemy_logic();
			powerup_logic();
			death_time--;
			if(count_enemies() <= 0 && death_time <= 0)
			{
				game_exit();
			}
			break;
		}
		case GAME_STATE_END:
		{
			projectile_logic();
			enemy_logic();
			powerup_logic();
			player.active = true;
			if(player.var == 0)
			{
				player.x += player.vx;
				player.y += player.vy;
				if(player.x + 16 >= 319 && player.x + 16 <= 321)
				{
					player.angle = ALLEGRO_PI * 1.5;
					player.vx = 0;
					player.vy = -2.0;
					player.var = 1;
				}
			}
			else if(player.var == 1)
			{
				player.x += player.vx;
				player.y += player.vy;
				if(player.y < -128)
				{
					al_play_sample(sample[SAMPLE_REJECTED], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					player.var = 2;
					flash_time = 30;
					fade_time = 0;
				}
			}
			else
			{
				player.angle += 0.2;
				player.y += 8.0;
			}
			if(player.y > 480)
			{
				fade_time++;
				if(fade_time >= 60)
				{
					game_exit();
					ending_cinema->position = 0;
					ending_cinema->tick = 0;
					click = true;
					ending_cinema = load_cinema("data/cinema/ending.cin", 0);
					state = STATE_ENDING;
				}
			}
			flash_time--;
			break;
		}
		case GAME_STATE_PAUSED:
		{
			title_menu_logic(menu[PAUSE_MENU]);
			if(t3f_key[ALLEGRO_KEY_ESCAPE])
			{
				game_state = resume_state;
				t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
			}
			break;
		}
	}
	if(score > high_score[game_mode])
	{
		high_score[game_mode] = score;
		if(!got_high_score)
		{
			al_play_sample(sample[SAMPLE_HIGH_SCORE], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			got_high_score = true;
		}
	}
	particle_logic();
	text_particle_logic();
}

void game_render_bg(void)
{
	int cur, next;
	float alpha;
	
	switch(game_mode)
	{
		case GAME_MODE_ETERNAL:
		{
			cur = current_level % 10;
			next = (current_level + 1) % 10;
			al_draw_bitmap(bitmap[cur], 0, 0, 0);
			alpha = (float)(state_ticks % 1800) / 1800.0;
			al_draw_tinted_bitmap(bitmap[next], al_map_rgba_f(alpha, alpha, alpha, alpha), 0, 0, 0);
			break;
		}
		default:
		{
			if(current_level < 10)
			{
				al_draw_bitmap(bitmap[current_level], 0, 0, 0);
				alpha = (float)(state_ticks % 1800) / 1800.0;
				al_draw_tinted_bitmap(bitmap[current_level + 1], al_map_rgba_f(alpha, alpha, alpha, alpha), 0, 0, 0);
			}
			else
			{
				al_draw_bitmap(bitmap[current_level], 0, 0, 0);
			}
			break;
		}
	}
}

void game_render(void)
{
	float alpha;
	
	game_render_bg();
	powerup_render();
	player_render_shadow();
	enemy_render_shadow();
	particle_render();
	player_render();
	enemy_render();
	projectile_render();
	text_particle_render();
	if(controller_type == CONTROLLER_TYPE_MOUSE && player.active && game_state != GAME_STATE_PAUSED)
	{
		t3f_draw_animation(animation[ANIMATION_CROSSHAIR], t3f_color_white, player.tick, t3f_mouse_x - 8, t3f_mouse_y - 8, 0, 0);
	}
	al_draw_textf(font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.8), GAME_STATS_MARGIN + 2, GAME_STATS_MARGIN + 2, 0, "Score: %06d", score);
	al_draw_textf(font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), GAME_STATS_MARGIN, GAME_STATS_MARGIN, 0, "Score: %06d", score);
	al_draw_textf(font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.8), GAME_STATS_MARGIN + 2, GAME_STATS_MARGIN + al_get_font_line_height(font[FONT_SMALL]) + 2, 0, "Multiplier: %d", multiplier);
	al_draw_textf(font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), GAME_STATS_MARGIN, GAME_STATS_MARGIN + al_get_font_line_height(font[FONT_SMALL]), 0, "Multiplier: %d", multiplier);
	if(current_level < 10)
	{
		al_draw_textf(font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.8), 640 - GAME_STATS_MARGIN + 2, GAME_STATS_MARGIN + 2, ALLEGRO_ALIGN_RIGHT, "Level: %2d", current_level + 1);
		al_draw_textf(font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 640 - GAME_STATS_MARGIN, GAME_STATS_MARGIN, ALLEGRO_ALIGN_RIGHT, "Level: %2d", current_level + 1);
	}
	else
	{
		al_draw_textf(font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.8), 640 - GAME_STATS_MARGIN + 2, GAME_STATS_MARGIN + 2, ALLEGRO_ALIGN_RIGHT, "Level: %2d", current_level);
		al_draw_textf(font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 640 - GAME_STATS_MARGIN, GAME_STATS_MARGIN, ALLEGRO_ALIGN_RIGHT, "Level: %2d", current_level);
	}
	al_draw_textf(font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.8), 640 - GAME_STATS_MARGIN + 2, GAME_STATS_MARGIN + al_get_font_line_height(font[FONT_SMALL]) + 2, ALLEGRO_ALIGN_RIGHT, "Lives: %2d", lives);
	al_draw_textf(font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 640 - GAME_STATS_MARGIN, GAME_STATS_MARGIN + al_get_font_line_height(font[FONT_SMALL]), ALLEGRO_ALIGN_RIGHT, "Lives: %2d", lives);
	al_draw_textf(font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 320 + 2, GAME_STATS_MARGIN + 2, ALLEGRO_ALIGN_CENTRE, "High Score: %06d", high_score[game_mode]);
	al_draw_textf(font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 320, GAME_STATS_MARGIN, ALLEGRO_ALIGN_CENTRE, "High Score: %06d", high_score[game_mode]);
	if(enemy[0].type == ENEMY_TYPE_DARK_ORB && enemy[0].active)
	{
		al_draw_text(font[FONT_TINY], al_map_rgba_f(0.0, 0.0, 0.0, 0.8), 320 + 2, 440 + 2, ALLEGRO_ALIGN_CENTRE, "Dark Power Orb");
		al_draw_text(font[FONT_TINY], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 320, 440, ALLEGRO_ALIGN_CENTRE, "Dark Power Orb");
		al_hold_bitmap_drawing(false);
		al_draw_filled_rectangle(320 - enemy[0].var2 + 2, 460 + 2, 320 + enemy[0].var2 + 2, 468 + 2, al_map_rgba_f(0.0, 0.0, 0.0, 0.8));
		al_draw_filled_rectangle(320 - enemy[0].var2, 460, 320 + enemy[0].var2, 468, al_map_rgba_f(0.0, 1.0, 0.0, 1.0));
		al_hold_bitmap_drawing(true);
	}
	if(flash_time > 0)
	{
		al_hold_bitmap_drawing(false);
		alpha = (float)flash_time / 30.0;
		al_draw_filled_rectangle(0.0, 0.0, 640, 480, al_map_rgba_f(alpha, alpha, alpha, alpha));
		al_hold_bitmap_drawing(true);
	}
	else if(fade_time > 0)
	{
		al_hold_bitmap_drawing(false);
		alpha = (float)fade_time / 60.0;
		al_draw_filled_rectangle(0.0, 0.0, 640, 480, al_map_rgba_f(alpha, alpha, alpha, alpha));
		al_hold_bitmap_drawing(true);
	}
	if(game_state == GAME_STATE_PAUSED)
	{
		al_hold_bitmap_drawing(false);
		al_draw_filled_rectangle(0.0, 0.0, 640, 480, al_map_rgba_f(0.0, 0.0, 0.0, 0.5));
		al_hold_bitmap_drawing(true);
		t3f_render_gui(menu[PAUSE_MENU]);
		render_mouse();
	}
}
