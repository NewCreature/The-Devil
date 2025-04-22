#include "t3f/t3f.h"
#include "instance.h"
#include "main.h"
#include "game.h"
#include "title.h"
#include "player.h"
#include "enemy.h"
#include "projectile.h"
#include "powerup.h"
#include "particle.h"
#include "leaderboard.h"

bool game_load_data(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->animation[ANIMATION_PLAYER] = t3f_load_animation("data/graphics/player.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!instance->animation[ANIMATION_PLAYER])
	{
		return false;
	}
	instance->animation[ANIMATION_DEMON] = t3f_load_animation("data/graphics/demon.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!instance->animation[ANIMATION_DEMON])
	{
		return false;
	}
	instance->animation[ANIMATION_ARCHDEMON] = t3f_load_animation("data/graphics/archdemon.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!instance->animation[ANIMATION_ARCHDEMON])
	{
		return false;
	}
	instance->animation[ANIMATION_PLAYER_SHOT] = t3f_load_animation("data/graphics/player_shot.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!instance->animation[ANIMATION_PLAYER_SHOT])
	{
		return false;
	}
	instance->animation[ANIMATION_ENEMY_SHOT] = t3f_load_animation("data/graphics/enemy_shot.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!instance->animation[ANIMATION_ENEMY_SHOT])
	{
		return false;
	}

	/* load spirit animation */
	instance->animation[ANIMATION_SPIRIT] = t3f_create_animation();
	if(!instance->animation[ANIMATION_SPIRIT])
	{
		return false;
	}
	instance->animation[ANIMATION_SPIRIT]->data->bitmaps->bitmap[0] = t3f_load_bitmap("data/graphics/spirit0.png", T3F_BITMAP_FLAG_PADDED, false);
	instance->animation[ANIMATION_SPIRIT]->data->bitmaps->bitmap[1] = t3f_load_bitmap("data/graphics/spirit1.png", T3F_BITMAP_FLAG_PADDED, false);
	instance->animation[ANIMATION_SPIRIT]->data->bitmaps->count = 2;
	t3f_animation_add_frame(instance->animation[ANIMATION_SPIRIT], 0, 0, 0, 0, instance->animation[ANIMATION_SPIRIT]->data->bitmaps->bitmap[0]->target_width, instance->animation[ANIMATION_SPIRIT]->data->bitmaps->bitmap[0]->target_height, 0, 5, 0);
	t3f_animation_add_frame(instance->animation[ANIMATION_SPIRIT], 1, 0, 0, 0, instance->animation[ANIMATION_SPIRIT]->data->bitmaps->bitmap[1]->target_width, instance->animation[ANIMATION_SPIRIT]->data->bitmaps->bitmap[1]->target_height, 0, 5, 0);

	/* load dark orb animation */
	instance->animation[ANIMATION_DARK_ORB] = t3f_create_animation();
	if(!instance->animation[ANIMATION_DARK_ORB])
	{
		return false;
	}
	instance->animation[ANIMATION_DARK_ORB]->data->bitmaps->bitmap[0] = t3f_load_bitmap("data/graphics/dark_orb0.png", T3F_BITMAP_FLAG_PADDED, false);
	instance->animation[ANIMATION_DARK_ORB]->data->bitmaps->bitmap[1] = t3f_load_bitmap("data/graphics/dark_orb1.png", T3F_BITMAP_FLAG_PADDED, false);
	instance->animation[ANIMATION_DARK_ORB]->data->bitmaps->count = 2;
	t3f_animation_add_frame(instance->animation[ANIMATION_DARK_ORB], 0, 0, 0, 0, instance->animation[ANIMATION_DARK_ORB]->data->bitmaps->bitmap[0]->target_width, instance->animation[ANIMATION_DARK_ORB]->data->bitmaps->bitmap[0]->target_height, 0, 5, 0);
	t3f_animation_add_frame(instance->animation[ANIMATION_DARK_ORB], 1, 0, 0, 0, instance->animation[ANIMATION_DARK_ORB]->data->bitmaps->bitmap[1]->target_width, instance->animation[ANIMATION_DARK_ORB]->data->bitmaps->bitmap[1]->target_height, 0, 5, 0);

	instance->animation[ANIMATION_CROSSHAIR] = t3f_load_animation("data/graphics/crosshair.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!instance->animation[ANIMATION_CROSSHAIR])
	{
		return false;
	}

	instance->animation[ANIMATION_PLAYER_SHOT_PARTICLE] = t3f_load_animation("data/graphics/player_shot_particle.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!instance->animation[ANIMATION_PLAYER_SHOT_PARTICLE])
	{
		return false;
	}
	instance->animation[ANIMATION_DARK_SHIELD] = t3f_load_animation("data/graphics/dark_orb_shield.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!instance->animation[ANIMATION_DARK_SHIELD])
	{
		return false;
	}
	instance->animation[ANIMATION_RADAR] = t3f_load_animation("data/graphics/radar.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!instance->animation[ANIMATION_RADAR])
	{
		return false;
	}
	instance->animation[ANIMATION_RADAR_DOT] = t3f_load_animation("data/graphics/radar_dot.png", T3F_BITMAP_FLAG_PADDED, false);
	if(!instance->animation[ANIMATION_RADAR_DOT])
	{
		return false;
	}
	instance->atlas = t3f_create_atlas(512, 512);
	if(!instance->atlas)
	{
		return false;
	}
	t3f_add_animation_to_atlas(instance->atlas, instance->animation[ANIMATION_PLAYER], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(instance->atlas, instance->animation[ANIMATION_DEMON], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(instance->atlas, instance->animation[ANIMATION_ARCHDEMON], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(instance->atlas, instance->animation[ANIMATION_PLAYER_SHOT], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(instance->atlas, instance->animation[ANIMATION_ENEMY_SHOT], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(instance->atlas, instance->animation[ANIMATION_SPIRIT], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(instance->atlas, instance->animation[ANIMATION_CROSSHAIR], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(instance->atlas, instance->animation[ANIMATION_PLAYER_SHOT_PARTICLE], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(instance->atlas, instance->animation[ANIMATION_DARK_ORB], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(instance->atlas, instance->animation[ANIMATION_DARK_SHIELD], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(instance->atlas, instance->animation[ANIMATION_RADAR], T3F_ATLAS_SPRITE);
	t3f_add_animation_to_atlas(instance->atlas, instance->animation[ANIMATION_RADAR_DOT], T3F_ATLAS_SPRITE);

	/* load the first two level backdrops */
	instance->bitmap[0] = t3f_load_bitmap("data/graphics/bg00.png", 0, false);
	if(!instance->bitmap[0])
	{
		return false;
	}
	instance->bitmap[1] = t3f_load_bitmap("data/graphics/bg01.png", 0, false);
	if(!instance->bitmap[1])
	{
		return false;
	}

	instance->sample[SAMPLE_GAME_OVER] = al_load_sample("data/sounds/game_over.ogg");
	if(!instance->sample[SAMPLE_GAME_OVER])
	{
		return false;
	}
	instance->sample[SAMPLE_LEVEL_UP] = al_load_sample("data/sounds/level_up.ogg");
	if(!instance->sample[SAMPLE_LEVEL_UP])
	{
		return false;
	}
	instance->sample[SAMPLE_MAX_MULTIPLIER] = al_load_sample("data/sounds/max_multiplier.ogg");
	if(!instance->sample[SAMPLE_MAX_MULTIPLIER])
	{
		return false;
	}
	instance->sample[SAMPLE_HIGH_SCORE] = al_load_sample("data/sounds/high_score.ogg");
	if(!instance->sample[SAMPLE_HIGH_SCORE])
	{
		return false;
	}
	instance->sample[SAMPLE_TWIN_SHOT] = al_load_sample("data/sounds/twin_shot.ogg");
	if(!instance->sample[SAMPLE_TWIN_SHOT])
	{
		return false;
	}
	instance->sample[SAMPLE_TRIPLE_SHOT] = al_load_sample("data/sounds/triple_shot.ogg");
	if(!instance->sample[SAMPLE_TRIPLE_SHOT])
	{
		return false;
	}
	instance->sample[SAMPLE_POWERUP] = al_load_sample("data/sounds/powerup.ogg");
	if(!instance->sample[SAMPLE_POWERUP])
	{
		return false;
	}
	instance->sample[SAMPLE_SHOOT] = al_load_sample("data/sounds/shoot.ogg");
	if(!instance->sample[SAMPLE_SHOOT])
	{
		return false;
	}
	instance->sample[SAMPLE_HIT] = al_load_sample("data/sounds/hit.ogg");
	if(!instance->sample[SAMPLE_HIT])
	{
		return false;
	}
	instance->sample[SAMPLE_DIE] = al_load_sample("data/sounds/die.ogg");
	if(!instance->sample[SAMPLE_DIE])
	{
		return false;
	}
	instance->sample[SAMPLE_MULTIPLIER] = al_load_sample("data/sounds/multiplier.ogg");
	if(!instance->sample[SAMPLE_MULTIPLIER])
	{
		return false;
	}
	instance->sample[SAMPLE_ENEMY_SHOOT] = al_load_sample("data/sounds/enemy_shoot.ogg");
	if(!instance->sample[SAMPLE_ENEMY_SHOOT])
	{
		return false;
	}
	instance->sample[SAMPLE_ORB_DIE] = al_load_sample("data/sounds/orb_die.ogg");
	if(!instance->sample[SAMPLE_ORB_DIE])
	{
		return false;
	}
	instance->sample[SAMPLE_REJECTED] = al_load_sample("data/sounds/rejected.ogg");
	if(!instance->sample[SAMPLE_REJECTED])
	{
		return false;
	}
	return true;
}

void game_free_data(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;

	t3f_destroy_animation(instance->animation[ANIMATION_PLAYER]);
	instance->animation[ANIMATION_PLAYER] = NULL;
	t3f_destroy_animation(instance->animation[ANIMATION_DEMON]);
	instance->animation[ANIMATION_DEMON] = NULL;
	t3f_destroy_animation(instance->animation[ANIMATION_ARCHDEMON]);
	instance->animation[ANIMATION_ARCHDEMON] = NULL;
	t3f_destroy_animation(instance->animation[ANIMATION_PLAYER_SHOT]);
	instance->animation[ANIMATION_PLAYER_SHOT] = NULL;
	t3f_destroy_animation(instance->animation[ANIMATION_ENEMY_SHOT]);
	instance->animation[ANIMATION_ENEMY_SHOT] = NULL;
	t3f_destroy_animation(instance->animation[ANIMATION_SPIRIT]);
	instance->animation[ANIMATION_SPIRIT] = NULL;
	t3f_destroy_animation(instance->animation[ANIMATION_DARK_ORB]);
	instance->animation[ANIMATION_DARK_ORB] = NULL;
	t3f_destroy_animation(instance->animation[ANIMATION_CROSSHAIR]);
	instance->animation[ANIMATION_CROSSHAIR] = NULL;
	t3f_destroy_animation(instance->animation[ANIMATION_PLAYER_SHOT_PARTICLE]);
	instance->animation[ANIMATION_PLAYER_SHOT_PARTICLE] = NULL;
	t3f_destroy_animation(instance->animation[ANIMATION_DARK_SHIELD]);
	instance->animation[ANIMATION_DARK_SHIELD] = NULL;
	t3f_destroy_animation(instance->animation[ANIMATION_RADAR]);
	instance->animation[ANIMATION_RADAR] = NULL;
	t3f_destroy_animation(instance->animation[ANIMATION_RADAR_DOT]);
	instance->animation[ANIMATION_RADAR_DOT] = NULL;
	t3f_destroy_atlas(instance->atlas);
	instance->atlas = NULL;

	for(i = 0; i < MAX_BITMAPS; i++)
	{
		if(instance->bitmap[i])
		{
			t3f_destroy_bitmap(instance->bitmap[i]);
			instance->bitmap[i] = NULL;
		}
	}

	al_destroy_sample(instance->sample[SAMPLE_GAME_OVER]);
	instance->sample[SAMPLE_GAME_OVER] = NULL;
	al_destroy_sample(instance->sample[SAMPLE_LEVEL_UP]);
	instance->sample[SAMPLE_LEVEL_UP] = NULL;
	al_destroy_sample(instance->sample[SAMPLE_MAX_MULTIPLIER]);
	instance->sample[SAMPLE_MAX_MULTIPLIER] = NULL;
	al_destroy_sample(instance->sample[SAMPLE_HIGH_SCORE]);
	instance->sample[SAMPLE_HIGH_SCORE] = NULL;
	al_destroy_sample(instance->sample[SAMPLE_TWIN_SHOT]);
	instance->sample[SAMPLE_TWIN_SHOT] = NULL;
	al_destroy_sample(instance->sample[SAMPLE_TRIPLE_SHOT]);
	instance->sample[SAMPLE_TRIPLE_SHOT] = NULL;
	al_destroy_sample(instance->sample[SAMPLE_POWERUP]);
	instance->sample[SAMPLE_POWERUP] = NULL;
	al_destroy_sample(instance->sample[SAMPLE_SHOOT]);
	instance->sample[SAMPLE_SHOOT] = NULL;
	al_destroy_sample(instance->sample[SAMPLE_HIT]);
	instance->sample[SAMPLE_HIT] = NULL;
	al_destroy_sample(instance->sample[SAMPLE_DIE]);
	instance->sample[SAMPLE_DIE] = NULL;
	al_destroy_sample(instance->sample[SAMPLE_MULTIPLIER]);
	instance->sample[SAMPLE_MULTIPLIER] = NULL;
	al_destroy_sample(instance->sample[SAMPLE_ENEMY_SHOOT]);
	instance->sample[SAMPLE_ENEMY_SHOOT] = NULL;
	al_destroy_sample(instance->sample[SAMPLE_ORB_DIE]);
	instance->sample[SAMPLE_ORB_DIE] = NULL;
	al_destroy_sample(instance->sample[SAMPLE_REJECTED]);
	instance->sample[SAMPLE_REJECTED] = NULL;
}

bool game_init(int mode, void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;

	instance->game_mode = mode;

	al_stop_timer(t3f_timer);
	if(!game_load_data(data))
	{
		return false;
	}
	al_start_timer(t3f_timer);

	/* create spawners */
	instance->enemy_spawners = 0;
	for(i = 0; i < (640 + 32 + 32) / 32; i++)
	{
		instance->enemy_spawner[instance->enemy_spawners].x = -32 + i * 32;
		instance->enemy_spawner[instance->enemy_spawners].y = -32;
		instance->enemy_spawners++;
		instance->enemy_spawner[instance->enemy_spawners].x = -32 + i * 32;
		instance->enemy_spawner[instance->enemy_spawners].y = 480;
		instance->enemy_spawners++;
	}
	for(i = 0; i < (480 + 32 + 32) / 32; i++)
	{
		instance->enemy_spawner[instance->enemy_spawners].x = -32;
		instance->enemy_spawner[instance->enemy_spawners].y = -32 + i * 32;
		instance->enemy_spawners++;
		instance->enemy_spawner[instance->enemy_spawners].x = 640;
		instance->enemy_spawner[instance->enemy_spawners].y = -32 + i * 32;
		instance->enemy_spawners++;
	}

	/* create game objects */
	instance->player.object = t3f_create_collision_object(8, 8, 16, 16, 32, 32, 0);
	for(i = 0; i < GAME_MAX_ENEMIES; i++)
	{
		instance->enemy[i].object = t3f_create_collision_object(8, 8, 16, 16, 32, 32, 0);
		instance->enemy[i].active = false;
	}
	for(i = 0; i < GAME_MAX_PLAYER_SHOTS; i++)
	{
		instance->player_shot[i].object = t3f_create_collision_object(0, 0, 16, 16, 32, 32, 0);
		instance->player_shot[i].active = false;
	}
	for(i = 0; i < GAME_MAX_ENEMY_SHOTS; i++)
	{
		instance->enemy_shot[i].object = t3f_create_collision_object(0, 0, 16, 16, 32, 32, 0);
		instance->enemy_shot[i].active = false;
	}
	for(i = 0; i < GAME_MAX_POWERUPS; i++)
	{
		instance->powerup[i].object = t3f_create_collision_object(0, 0, 16, 16, 32, 32, 0);
		instance->powerup[i].active = false;
	}
	for(i = 0; i < GAME_MAX_PARTICLES; i++)
	{
		instance->particle[i].active = false;
	}

	/* set game up */
	instance->touch_stick[0].active = false;
	instance->touch_stick[1].active = false;
	instance->player.x = 304;
	instance->player.y = 224;
	instance->player.z = 0;
	instance->player.speed = 4.0;
	instance->player.active = true;
	instance->state = STATE_GAME;
	instance->score = 0;
	if(instance->finale_mode)
	{
		instance->current_level = 9;
	}
	else
	{
		instance->current_level = 0;
	}
	game_load_level(&instance->level, instance->current_level);
	instance->multiplier = 1;
	instance->multiplier_tick = 0;
	if(instance->konami_mode)
	{
		instance->lives = 99;
	}
	else
	{
		instance->lives = 3;
	}
	if(instance->fire_power)
	{
		instance->weapon = 2;
	}
	else
	{
		instance->weapon = 0;
	}
	instance->state_ticks = 0;
	if(instance->finale_mode)
	{
		instance->state_ticks = 1700;
	}
	instance->got_high_score = false;
	instance->fade_time = 0;
	instance->game_state = GAME_STATE_PLAY;
	t3f_play_music("data/music/game.xm");
	return true;
}

void game_exit(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;
	char text[256] = {0};

	/* destroy game objects */
	t3f_destroy_collision_object(instance->player.object);
	for(i = 0; i < GAME_MAX_ENEMIES; i++)
	{
		t3f_destroy_collision_object(instance->enemy[i].object);
	}
	for(i = 0; i < GAME_MAX_PLAYER_SHOTS; i++)
	{
		t3f_destroy_collision_object(instance->player_shot[i].object);
	}
	for(i = 0; i < GAME_MAX_ENEMY_SHOTS; i++)
	{
		t3f_destroy_collision_object(instance->enemy_shot[i].object);
	}
	for(i = 0; i < GAME_MAX_POWERUPS; i++)
	{
		t3f_destroy_collision_object(instance->powerup[i].object);
	}
	sprintf(text, "%d", instance->high_score[instance->game_mode]);
	switch(instance->game_mode)
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
	game_free_data(data);
	title_load_data(data);
	if(instance->upload_scores && instance->score > 0 && !instance->konami_mode && !instance->finale_mode && !instance->fire_power)
	{
		t3net_upload_score("http://www.t3-i.com/t3net2/leaderboards/insert.php", "devil", "1.1", instance->game_mode_text[instance->game_mode], "0", instance->network_id, instance->score * 2 + 'v' + 'g' + 'o' + 'l' + 'f', NULL);
		download_leaderboard(data);
		if(instance->leaderboard)
		{
			instance->leaderboard_place = -1;
			for(i = 0; i < instance->leaderboard->entries; i++)
			{
				if((unsigned int)instance->score * 2 + 'v' + 'g' + 'o' + 'l' + 'f' == instance->leaderboard->entry[i]->score && !strcmp(instance->network_id, instance->leaderboard->entry[i]->name))
				{
					instance->leaderboard_place = i;
					break;
				}
			}
			instance->leaderboard_mode = instance->game_mode;
			instance->state = STATE_LEADERBOARDS;
		}
		else
		{
			select_menu(TITLE_MENU_MAIN, data);
			instance->state = STATE_TITLE;
		}
	}
	else
	{
		select_menu(TITLE_MENU_MAIN, data);
		instance->state = STATE_TITLE;
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

void game_load_next_level_bitmap(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	char buf[256];
	int last_bitmap, next_bitmap;

	last_bitmap = instance->current_level % 11;
	next_bitmap = (instance->current_level + 2) % 11;
	sprintf(buf, "data/graphics/bg%02d.png", next_bitmap);
	t3f_destroy_bitmap(instance->bitmap[last_bitmap]);
	instance->bitmap[last_bitmap] = NULL;
	al_stop_timer(t3f_timer);
	instance->bitmap[next_bitmap] = t3f_load_bitmap(buf, 0, false);
	al_start_timer(t3f_timer);
}

void game_level_logic(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	switch(instance->game_mode)
	{
		case GAME_MODE_ETERNAL:
		{
			if(instance->state_ticks % 300 == 0)
			{
				instance->level.demon_rate++;
				instance->level.demon_speed += 0.015;
			}
			if(instance->state_ticks % 900 == 0)
			{
				instance->level.archdemon_rate++;
				instance->level.archdemon_fire_rate++;
			}
			if(instance->state_ticks % 1800 == 0)
			{
				game_load_next_level_bitmap(data);
				instance->current_level++;
				if(instance->current_level == 2)
				{
					instance->weapon++;
					al_play_sample(instance->sample[SAMPLE_TWIN_SHOT], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
				if(instance->current_level == 6)
				{
					instance->weapon++;
					al_play_sample(instance->sample[SAMPLE_TRIPLE_SHOT], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
				if(instance->weapon > 2)
				{
					instance->weapon = 2;
				}
			}
			break;
		}
		default:
		{
			if(instance->state_ticks % 1800 == 0)
			{
				game_load_next_level_bitmap(data);
				instance->current_level++;
				if(instance->current_level == 2)
				{
					instance->weapon++;
					al_play_sample(instance->sample[SAMPLE_TWIN_SHOT], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
				if(instance->current_level == 6)
				{
					instance->weapon++;
					al_play_sample(instance->sample[SAMPLE_TRIPLE_SHOT], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
				if(instance->weapon > 2)
				{
					instance->weapon = 2;
				}
				if(instance->current_level > 10)
				{
					instance->current_level = 10;
				}
				game_load_level(&instance->level, instance->current_level);
			}
			if(instance->current_level >= 10 && count_enemies(data) <= 0)
			{
				t3f_play_music("data/music/boss.xm");
				enemy_spawn_dark_orb(data);
				instance->game_state = GAME_STATE_BOSS;
			}
			break;
		}
	}
}

void read_touch_controls(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int touch_used[2] = {-1, -1};
	int i;

	/* see which touches are already in use */
	for(i = 0; i < 2; i++)
	{
		if(instance->touch_stick[i].active)
		{
			touch_used[i] = instance->touch_stick[i].touch_id;
		}
	}

	/* player movement */
	if(!instance->touch_stick[0].active)
	{
		for(i = 0; i < T3F_MAX_TOUCHES; i++)
		{
			if(i != touch_used[0] && i != touch_used[1] && t3f_touch_active(i) && t3f_get_touch_x(i) < 320.0)
			{
				instance->touch_stick[0].touch_id = i;
				instance->touch_stick[0].pin_x = t3f_get_touch_x(i);
				instance->touch_stick[0].pin_y = t3f_get_touch_y(i);
				instance->touch_stick[0].pos_x = t3f_get_touch_x(i);
				instance->touch_stick[0].pos_y = t3f_get_touch_y(i);
				instance->touch_stick[0].active = true;
				touch_used[0] = i;
				break;
			}
		}
	}
	else
	{
		if(!t3f_touch_active(instance->touch_stick[0].touch_id))
		{
			instance->touch_stick[0].active = false;
		}
		else
		{
			instance->touch_stick[0].pos_x = t3f_get_touch_x(instance->touch_stick[0].touch_id);
			instance->touch_stick[0].pos_y = t3f_get_touch_y(instance->touch_stick[0].touch_id);
		}
	}

	/* player attack */
	if(!instance->touch_stick[1].active)
	{
		for(i = 0; i < T3F_MAX_TOUCHES; i++)
		{
			if(i != touch_used[0] && i != touch_used[1] && t3f_touch_active(i) && t3f_get_touch_x(i) >= 320.0)
			{
				instance->touch_stick[1].touch_id = i;
				instance->touch_stick[1].pin_x = t3f_get_touch_x(i);
				instance->touch_stick[1].pin_y = t3f_get_touch_y(i);
				instance->touch_stick[1].pos_x = t3f_get_touch_x(i);
				instance->touch_stick[1].pos_y = t3f_get_touch_y(i);
				instance->touch_stick[1].active = true;
				touch_used[1] = i;
				break;
			}
		}
	}
	else
	{
		if(!t3f_touch_active(instance->touch_stick[1].touch_id))
		{
			instance->touch_stick[1].active = false;
		}
		else
		{
			instance->touch_stick[1].pos_x = t3f_get_touch_x(instance->touch_stick[1].touch_id);
			instance->touch_stick[1].pos_y = t3f_get_touch_y(instance->touch_stick[1].touch_id);
		}
	}
}

void game_logic(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	if((t3f_key_pressed(ALLEGRO_KEY_ESCAPE) || t3f_key_pressed(ALLEGRO_KEY_BACK)) && instance->game_state != GAME_STATE_PAUSED)
	{
		instance->resume_state = instance->game_state;
		instance->game_state = GAME_STATE_PAUSED;
		instance->menu[PAUSE_MENU]->hover_element = 0;
		t3f_select_next_gui_element(instance->menu[PAUSE_MENU]);
		t3f_use_key_press(ALLEGRO_KEY_ESCAPE);
		t3f_use_key_press(ALLEGRO_KEY_BACK);
		return;
	}
	if(instance->controller_type >= CONTROLLER_TYPE_TOUCH_S)
	{
		read_touch_controls(data);
	}
	switch(instance->game_state)
	{
		case GAME_STATE_PLAY:
		{
			instance->enemy_spawn_logic();
			player_logic(data);
			enemy_logic(data);
			projectile_logic(data);
			powerup_logic(data);
			instance->state_ticks++;
			game_level_logic(data);
			break;
		}
		case GAME_STATE_BOSS:
		{
			player_logic(data);
			enemy_logic(data);
			projectile_logic(data);
			powerup_logic(data);
			if(!instance->enemy[0].active)
			{
				instance->player.var = 0;
				instance->player.angle = atan2(instance->player.y - 240, instance->player.x - 320) + ALLEGRO_PI;
				instance->player.vx = cos(instance->player.angle) * 2.0;
				instance->player.vy = sin(instance->player.angle) * 2.0;
				instance->game_state = GAME_STATE_END;
			}
			break;
		}
		case GAME_STATE_DIE:
		{
			projectile_logic(data);
			enemy_logic(data);
			powerup_logic(data);
			instance->death_time--;
			if(count_enemies(data) <= 0 && instance->death_time <= 0)
			{
				instance->player.x = 304;
				instance->player.y = 224;
				instance->player.speed = 4.0;
				instance->player.active = true;
				instance->multiplier = 1;
				instance->game_state = instance->die_state;
			}
			break;
		}
		case GAME_STATE_OVER:
		{
			projectile_logic(data);
			enemy_logic(data);
			powerup_logic(data);
			instance->death_time--;
			if(count_enemies(data) <= 0 && instance->death_time <= 0)
			{
				game_exit(data);
			}
			break;
		}
		case GAME_STATE_END:
		{
			projectile_logic(data);
			enemy_logic(data);
			powerup_logic(data);
			instance->player.active = true;
			if(instance->player.var == 0)
			{
				instance->player.x += instance->player.vx;
				instance->player.y += instance->player.vy;
				if(instance->player.x + 16 >= 319 && instance->player.x + 16 <= 321)
				{
					instance->player.angle = ALLEGRO_PI * 1.5;
					instance->player.vx = 0;
					instance->player.vy = -2.0;
					instance->player.var = 1;
				}
			}
			else if(instance->player.var == 1)
			{
				instance->player.x += instance->player.vx;
				instance->player.y += instance->player.vy;
				if(instance->player.y < -128)
				{
					al_play_sample(instance->sample[SAMPLE_REJECTED], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					instance->player.var = 2;
					instance->flash_time = 30;
					instance->fade_time = 0;
				}
			}
			else
			{
				instance->player.angle += 0.2;
				instance->player.y += 8.0;
			}
			if(instance->player.y > 480)
			{
				instance->fade_time++;
				if(instance->fade_time >= 60)
				{
					game_exit(data);
//					click = true;
					instance->ending_cinema = load_cinema("data/cinema/ending.cin", 0);
					instance->ending_cinema->position = 0;
					instance->ending_cinema->tick = 0;
					instance->state = STATE_ENDING;
				}
			}
			instance->flash_time--;
			break;
		}
		case GAME_STATE_PAUSED:
		{
			title_process_menu(instance->menu[PAUSE_MENU], data);
			if(t3f_key_pressed(ALLEGRO_KEY_ESCAPE) || t3f_key_pressed(ALLEGRO_KEY_BACK))
			{
				instance->game_state = instance->resume_state;
				t3f_use_key_press(ALLEGRO_KEY_ESCAPE);
				t3f_use_key_press(ALLEGRO_KEY_BACK);
			}
			break;
		}
	}
	if(instance->score > instance->high_score[instance->game_mode])
	{
		instance->high_score[instance->game_mode] = instance->score;
		if(!instance->got_high_score)
		{
			al_play_sample(instance->sample[SAMPLE_HIGH_SCORE], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			instance->got_high_score = true;
		}
	}
	particle_logic(data);
	text_particle_logic(data);
}

void game_render_bg(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int cur, next;
	float alpha;

	switch(instance->game_mode)
	{
		case GAME_MODE_ETERNAL:
		{
			cur = instance->current_level % 11;
			next = (instance->current_level + 1) % 11;
			t3f_draw_bitmap(instance->bitmap[cur], t3f_color_white, 0, 0, 0, 0);
			alpha = (float)(instance->state_ticks % 1800) / 1800.0;
			t3f_draw_bitmap(instance->bitmap[next], al_map_rgba_f(alpha, alpha, alpha, alpha), 0, 0, 0, 0);
			break;
		}
		default:
		{
			if(instance->current_level < 10)
			{
				t3f_draw_bitmap(instance->bitmap[instance->current_level], t3f_color_white, 0, 0, 0, 0);
				alpha = (float)(instance->state_ticks % 1800) / 1800.0;
				t3f_draw_bitmap(instance->bitmap[instance->current_level + 1], al_map_rgba_f(alpha, alpha, alpha, alpha), 0, 0, 0, 0);
			}
			else
			{
				t3f_draw_bitmap(instance->bitmap[instance->current_level], t3f_color_white, 0, 0, 0, 0);
			}
			break;
		}
	}
}

void game_render_radar(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	double a;
	int i;

	if(instance->player.active)
	{
		t3f_draw_animation(instance->animation[ANIMATION_RADAR], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), 0, instance->player.x + 16 - instance->animation[ANIMATION_RADAR]->data->bitmaps->bitmap[0]->target_width / 2, instance->player.y + 16 - instance->animation[ANIMATION_RADAR]->data->bitmaps->bitmap[0]->target_height / 2, 0.0, 0);
		for(i = 0; i < GAME_MAX_ENEMIES; i++)
		{
			if(instance->enemy[i].active)
			{
				a = atan2(instance->enemy[i].y - instance->player.y, instance->enemy[i].x - instance->player.x);
				t3f_draw_animation(instance->animation[ANIMATION_RADAR_DOT], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), 0, instance->player.x + cos(a) * 40.0 - 2.0 + 16.0, instance->player.y + sin(a) * 40.0 - 2.0 + 16.0, 0.0, 0);
			}
		}
		t3f_draw_animation(instance->animation[ANIMATION_RADAR_DOT], al_map_rgba_f(0.75, 0.75, 0.0, 0.75), 0, instance->player.x + cos(fire_angle) * 37.0 - 2.0 + 16.0, instance->player.y + sin(fire_angle) * 37.0 - 2.0 + 16.0, 0.0, 0);
		t3f_draw_animation(instance->animation[ANIMATION_RADAR_DOT], al_map_rgba_f(0.75, 0.75, 0.0, 0.75), 0, instance->player.x + cos(instance->player.angle) * 25.0 - 2.0 + 16.0, instance->player.y + sin(instance->player.angle) * 25.0 - 2.0 + 16.0, 0.0, 0);
	}
}

void game_render_touch_helpers(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;
	float scale;

	scale = (instance->touch_size * 2.0) / instance->animation[ANIMATION_RADAR]->data->bitmaps->bitmap[0]->target_width;

	al_hold_bitmap_drawing(false);
	al_set_clipping_rectangle(0, 0, al_get_display_width(t3f_display), al_get_display_height(t3f_display));
	al_hold_bitmap_drawing(true);
	for(i = 0; i < 2; i++)
	{
		if(instance->touch_stick[i].active)
		{
			t3f_draw_scaled_animation(instance->animation[ANIMATION_RADAR], al_map_rgba_f(0.0, 0.25, 0.0, 0.25), 0, instance->touch_stick[i].pin_x - instance->touch_size, instance->touch_stick[i].pin_y - instance->touch_size, 0.0, scale, 0);
			t3f_draw_animation(instance->animation[ANIMATION_RADAR_DOT], al_map_rgba_f(0.0, 0.25, 0.0, 0.25), 0, instance->touch_stick[i].pos_x - 2.0, instance->touch_stick[i].pos_y - 2.0, 0.0, 0);
		}
	}
	al_hold_bitmap_drawing(false);
	t3f_set_clipping_rectangle(0, 0, 0, 0);
	al_hold_bitmap_drawing(true);
}

void game_render(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	float alpha;

	game_render_bg(data);
	powerup_render(data);
	player_render_shadow(data);
	enemy_render_shadow(data);
	particle_render(data);
	player_render(data);
	enemy_render(data);
	projectile_render(data);
	text_particle_render(data);
	if(instance->controller_type == CONTROLLER_TYPE_MOUSE && instance->player.active && instance->game_state != GAME_STATE_PAUSED)
	{
		t3f_draw_animation(instance->animation[ANIMATION_CROSSHAIR], t3f_color_white, instance->player.tick, t3f_get_mouse_x() - 8, t3f_get_mouse_y() - 8, 0, 0);
	}
	game_render_radar(data);
	t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.8), t3f_default_view->left + GAME_STATS_MARGIN + 2, t3f_default_view->top + GAME_STATS_MARGIN + 2, 0, 0, "Score: %06d", instance->score);
	t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), t3f_default_view->left + GAME_STATS_MARGIN, t3f_default_view->top + GAME_STATS_MARGIN, 0, 0, "Score: %06d", instance->score);
	t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.8), t3f_default_view->left + GAME_STATS_MARGIN + 2, t3f_default_view->top + GAME_STATS_MARGIN + t3f_get_font_line_height(instance->font[FONT_SMALL]) + 2, 0, 0, "Multiplier: %d", instance->multiplier);
	t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), t3f_default_view->left + GAME_STATS_MARGIN, t3f_default_view->top + GAME_STATS_MARGIN + t3f_get_font_line_height(instance->font[FONT_SMALL]), 0, 0, "Multiplier: %d", instance->multiplier);
	if(instance->game_mode == GAME_MODE_ETERNAL || instance->current_level < 10)
	{
		t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.8), t3f_default_view->right - GAME_STATS_MARGIN + 2, t3f_default_view->top + GAME_STATS_MARGIN + 2, T3F_FONT_ALIGN_RIGHT, 0, "Level: %2d", instance->current_level + 1);
		t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), t3f_default_view->right - GAME_STATS_MARGIN, t3f_default_view->top + GAME_STATS_MARGIN, T3F_FONT_ALIGN_RIGHT, 0, "Level: %2d", instance->current_level + 1);
	}
	else
	{
		t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.8), t3f_default_view->right - GAME_STATS_MARGIN + 2, t3f_default_view->top + GAME_STATS_MARGIN + 2, T3F_FONT_ALIGN_RIGHT, 0, "Level: %2d", instance->current_level);
		t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), t3f_default_view->right - GAME_STATS_MARGIN, t3f_default_view->top + GAME_STATS_MARGIN, T3F_FONT_ALIGN_RIGHT, 0, "Level: %2d", instance->current_level);
	}
	t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.8), t3f_default_view->right - GAME_STATS_MARGIN + 2, t3f_default_view->top + GAME_STATS_MARGIN + t3f_get_font_line_height(instance->font[FONT_SMALL]) + 2, T3F_FONT_ALIGN_RIGHT, 0, "Lives: %2d", instance->lives);
	t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), t3f_default_view->right - GAME_STATS_MARGIN, t3f_default_view->top + GAME_STATS_MARGIN + t3f_get_font_line_height(instance->font[FONT_SMALL]), T3F_FONT_ALIGN_RIGHT, 0, "Lives: %2d", instance->lives);
	t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 320 + 2, t3f_default_view->top + GAME_STATS_MARGIN + 2, T3F_FONT_ALIGN_CENTER, 0, "High Score: %06d", instance->high_score[instance->game_mode]);
	t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 320, t3f_default_view->top + GAME_STATS_MARGIN, T3F_FONT_ALIGN_CENTER, 0, "High Score: %06d", instance->high_score[instance->game_mode]);
	if(instance->enemy[0].type == ENEMY_TYPE_DARK_ORB && instance->enemy[0].active)
	{
		t3f_draw_text(instance->font[FONT_TINY], al_map_rgba_f(0.0, 0.0, 0.0, 0.8), 320 + 2, t3f_default_view->bottom - 40 + 2, T3F_FONT_ALIGN_CENTER, 0, "Dark Power Orb");
		t3f_draw_text(instance->font[FONT_TINY], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 320, t3f_default_view->bottom - 40, T3F_FONT_ALIGN_CENTER, 0, "Dark Power Orb");
		al_hold_bitmap_drawing(false);
		al_draw_filled_rectangle(320 - instance->enemy[0].var2 + 2, t3f_default_view->bottom - 20 + 2, 320 + instance->enemy[0].var2 + 2, t3f_default_view->bottom - 12 + 2, al_map_rgba_f(0.0, 0.0, 0.0, 0.8));
		al_draw_filled_rectangle(320 - instance->enemy[0].var2, t3f_default_view->bottom - 20, 320 + instance->enemy[0].var2, t3f_default_view->bottom - 12, al_map_rgba_f(0.0, 1.0, 0.0, 1.0));
		al_hold_bitmap_drawing(true);
	}
	if(instance->controller_type >= CONTROLLER_TYPE_TOUCH_S)
	{
		game_render_touch_helpers(data);
	}
	if(instance->flash_time > 0)
	{
		al_hold_bitmap_drawing(false);
		alpha = (float)instance->flash_time / 30.0;
		al_draw_filled_rectangle(0.0, 0.0, 640, 480, al_map_rgba_f(alpha, alpha, alpha, alpha));
		al_hold_bitmap_drawing(true);
	}
	else if(instance->fade_time > 0)
	{
		al_hold_bitmap_drawing(false);
		alpha = (float)instance->fade_time / 60.0;
		al_draw_filled_rectangle(0.0, 0.0, 640, 480, al_map_rgba_f(alpha, alpha, alpha, alpha));
		al_hold_bitmap_drawing(true);
	}
	if(instance->game_state == GAME_STATE_PAUSED)
	{
		al_hold_bitmap_drawing(false);
		al_draw_filled_rectangle(0.0, 0.0, 640, 480, al_map_rgba_f(0.0, 0.0, 0.0, 0.5));
		al_hold_bitmap_drawing(true);
		t3f_render_gui(instance->menu[PAUSE_MENU], 0);
		render_mouse(data);
	}
}
