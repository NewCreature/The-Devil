#include "t3f/t3f.h"
#include "instance.h"
#include "main.h"
#include "player.h"
#include "particle.h"

void clear_enemies(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;
	
	for(i = 0; i < GAME_MAX_ENEMIES; i++)
	{
		instance->enemy[i].active = false;
	}
}

int count_enemies(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i, c = 0;
	
	if(instance->enemy[0].type == ENEMY_TYPE_DARK_ORB)
	{
		for(i = 0; i < GAME_MAX_ENEMY_SHOTS; i++)
		{
			if(instance->enemy_shot[i].active)
			{
				c++;
			}
		}
		return c;
	}
	for(i = 0; i < GAME_MAX_ENEMIES; i++)
	{
		if(instance->enemy[i].active)
		{
			c++;
		}
	}
	return c;
}

void enemy_fire_shot(float x, float y, float angle, int type, void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;
	
	for(i = 0; i < GAME_MAX_ENEMY_SHOTS; i++)
	{
		if(!instance->enemy_shot[i].active)
		{
			al_play_sample(instance->sample[SAMPLE_ENEMY_SHOOT], 1.0, t3f_get_sound_position(instance->player.x, instance->player.y, x, y), 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			instance->enemy_shot[i].x = x;
			instance->enemy_shot[i].y = y;
			instance->enemy_shot[i].angle = angle;
			instance->enemy_shot[i].vx = cos(angle) * 6.0;
			instance->enemy_shot[i].vy = sin(angle) * 6.0;
			instance->enemy_shot[i].var = type;
			instance->enemy_shot[i].var2 = 3;
			generate_particle(PARTICLE_TYPE_ENEMY_SHOT, x + 4, y + 4, angle + t3f_drand(&instance->rng_state) / 2.0 - 0.25, 8.0 + t3f_drand(&instance->rng_state) / 2.0, 10 + rand() % 10, data);
			generate_particle(PARTICLE_TYPE_ENEMY_SHOT, x + 4, y + 4, angle + t3f_drand(&instance->rng_state) / 2.0 - 0.25, 8.0 + t3f_drand(&instance->rng_state) / 2.0, 10 + rand() % 10, data);
			instance->enemy_shot[i].active = true;
			break;
		}
	}
}

void enemy_spawn_logic_normal(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;
	int r;
	
	if(rand() % 1000 < instance->level.demon_rate)
	{
		for(i = 0; i < GAME_MAX_ENEMIES; i++)
		{
			if(!instance->enemy[i].active)
			{
				r = rand() % instance->enemy_spawners;
				instance->enemy[i].x = instance->enemy_spawner[r].x;
				instance->enemy[i].y = instance->enemy_spawner[r].y;
				instance->enemy[i].type = ENEMY_TYPE_DEMON;
				instance->enemy[i].speed = 2.0 + t3f_drand(&instance->rng_state) * (instance->level.demon_speed - 2.0);
				instance->enemy[i].active = true;
				break;
			}
		}
	}
	if(rand() % 1000 < instance->level.archdemon_rate)
	{
		for(i = 0; i < GAME_MAX_ENEMIES; i++)
		{
			if(!instance->enemy[i].active)
			{
				r = rand() % instance->enemy_spawners;
				instance->enemy[i].x = instance->enemy_spawner[r].x;
				instance->enemy[i].y = instance->enemy_spawner[r].y;
				instance->enemy[i].type = ENEMY_TYPE_ARCHDEMON;
				instance->enemy[i].speed = (2.0 + t3f_drand(&instance->rng_state) * (instance->level.demon_speed - 2.0)) / 2.0;
				instance->enemy[i].var = 60;
				instance->enemy[i].active = true;
				break;
			}
		}
	}
}

void enemy_spawn_logic_easy(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;
	int r;
	
	if(rand() % 3000 < instance->level.demon_rate)
	{
		for(i = 0; i < GAME_MAX_ENEMIES; i++)
		{
			if(!instance->enemy[i].active)
			{
				r = rand() % instance->enemy_spawners;
				instance->enemy[i].x = instance->enemy_spawner[r].x;
				instance->enemy[i].y = instance->enemy_spawner[r].y;
				instance->enemy[i].type = ENEMY_TYPE_DEMON;
				instance->enemy[i].speed = 2.0 + t3f_drand(&instance->rng_state) * (instance->level.demon_speed - 2.0);
				instance->enemy[i].active = true;
				break;
			}
		}
	}
	if(rand() % 3000 < instance->level.archdemon_rate)
	{
		for(i = 0; i < GAME_MAX_ENEMIES; i++)
		{
			if(!instance->enemy[i].active)
			{
				r = rand() % instance->enemy_spawners;
				instance->enemy[i].x = instance->enemy_spawner[r].x;
				instance->enemy[i].y = instance->enemy_spawner[r].y;
				instance->enemy[i].type = ENEMY_TYPE_ARCHDEMON;
				instance->enemy[i].speed = (2.0 + t3f_drand(&instance->rng_state) * (instance->level.demon_speed - 2.0)) / 2.0;
				instance->enemy[i].var = 60;
				instance->enemy[i].active = true;
				break;
			}
		}
	}
}

void enemy_spawn_dark_orb(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;
	
	instance->enemy[0].type = ENEMY_TYPE_DARK_ORB;
	instance->enemy[0].x = 320 - 32;
	instance->enemy[0].y = -192;
	instance->enemy[0].z = 0;
	instance->enemy[0].vx = -1.5;
	instance->enemy[0].angle = -1;
	instance->enemy[0].var = 0;
	instance->enemy[0].var2 = 160;
	instance->enemy[0].tick = 0;
	instance->enemy[0].active = true;
	if(instance->enemy[0].object)
	{
		t3f_destroy_collision_object(instance->enemy[0].object);
	}
	instance->enemy[0].object = t3f_create_collision_object(12, 12, 40, 40, 32, 32, 0);
	
	for(i = 0; i < 16; i++)
	{
		instance->enemy[i + 1].type = ENEMY_TYPE_DARK_SHIELD;
		instance->enemy[i + 1].active = true;
		if(instance->enemy[i + 1].object)
		{
			t3f_destroy_collision_object(instance->enemy[i + 1].object);
		}
		instance->enemy[i + 1].object = t3f_create_collision_object(0, 0, 16, 16, 32, 32, 0);
		
		instance->enemy[i + 1 + 16].type = ENEMY_TYPE_DARK_SHIELD;
		instance->enemy[i + 1 + 16].active = true;
		if(instance->enemy[i + 1 + 16].object)
		{
			t3f_destroy_collision_object(instance->enemy[i + 1 + 16].object);
		}
		instance->enemy[i + 1 + 16].object = t3f_create_collision_object(0, 0, 16, 16, 32, 32, 0);
	}
}

void enemy_logic(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i, j;
	float target_angle;

	for(i = 0; i < GAME_MAX_ENEMIES; i++)
	{
		if(instance->enemy[i].active)
		{
			switch(instance->enemy[i].type)
			{
				case ENEMY_TYPE_DEMON:
				{
					if(instance->player.active)
					{
						target_angle = atan2(instance->enemy[i].y - instance->player.y, instance->enemy[i].x - instance->player.x);
						instance->enemy[i].angle = target_angle + ALLEGRO_PI;
						instance->enemy[i].vx = cos(instance->enemy[i].angle) * instance->enemy[i].speed;
						instance->enemy[i].vy = sin(instance->enemy[i].angle) * instance->enemy[i].speed;
					}
					else
					{
						instance->enemy[i].vx *= 1.01;
						instance->enemy[i].vy *= 1.01;
					}
					instance->enemy[i].x += instance->enemy[i].vx;
					instance->enemy[i].y += instance->enemy[i].vy;
					t3f_move_collision_object_xy(instance->enemy[i].object, instance->enemy[i].x, instance->enemy[i].y);
					if(instance->player.active && t3f_check_object_collision(instance->enemy[i].object, instance->player.object))
					{
						player_lose_life(data);
						instance->enemy[i].active = false;
					}
					if(instance->enemy[i].x < -34 || instance->enemy[i].x > 642 || instance->enemy[i].y < -34 || instance->enemy[i].y > 482)
					{
						instance->enemy[i].active = false;
					}
					break;
				}
				case ENEMY_TYPE_ARCHDEMON:
				{
					if(instance->player.active)
					{
						target_angle = atan2(instance->enemy[i].y - instance->player.y, instance->enemy[i].x - instance->player.x);
						instance->enemy[i].angle = target_angle + ALLEGRO_PI;
						instance->enemy[i].vx = cos(instance->enemy[i].angle) * instance->enemy[i].speed;
						instance->enemy[i].vy = sin(instance->enemy[i].angle) * instance->enemy[i].speed;
					}
					else
					{
						instance->enemy[i].vx *= 1.01;
						instance->enemy[i].vy *= 1.01;
					}
					instance->enemy[i].x += instance->enemy[i].vx;
					instance->enemy[i].y += instance->enemy[i].vy;
					t3f_move_collision_object_xy(instance->enemy[i].object, instance->enemy[i].x, instance->enemy[i].y);
					if(instance->player.active && t3f_check_object_collision(instance->enemy[i].object, instance->player.object))
					{
						player_lose_life(data);
						instance->enemy[i].active = false;
					}
					if(instance->enemy[i].var > 0)
					{
						instance->enemy[i].var--;
					}
					else
					{
						if(instance->player.active && rand() % 1000 < instance->level.archdemon_fire_rate)
						{
							enemy_fire_shot(instance->enemy[i].x + 8, instance->enemy[i].y + 8, instance->enemy[i].angle, 0, data);
							instance->enemy[i].var = 60;
						}
					}
					if(instance->enemy[i].x < -34 || instance->enemy[i].x > 642 || instance->enemy[i].y < -34 || instance->enemy[i].y > 482)
					{
						instance->enemy[i].active = false;
					}
					break;
				}
				case ENEMY_TYPE_DARK_ORB:
				{
					if(instance->enemy[i].y < 48)
					{
						instance->enemy[i].y += 0.5;
					}
					else
					{
						if(instance->enemy[i].angle < 0)
						{
							instance->enemy[i].vx -= 0.1;
							if(instance->enemy[i].vx < -4.0)
							{
								instance->enemy[i].vx = -4.0;
							}
							instance->enemy[i].x += instance->enemy[i].vx;
							if(instance->enemy[i].x < 240)
							{
								instance->enemy[i].angle = -instance->enemy[i].angle;
							}
						}
						else
						{
							instance->enemy[i].vx += 0.1;
							if(instance->enemy[i].vx > 4.0)
							{
								instance->enemy[i].vx = 4.0;
							}
							instance->enemy[i].x += instance->enemy[i].vx;
							if(instance->enemy[i].x > 640 - 240 - 64)
							{
								instance->enemy[i].angle = -instance->enemy[i].angle;
							}
						}
					}
					t3f_move_collision_object_xy(instance->enemy[i].object, instance->enemy[i].x, instance->enemy[i].y);
					if(instance->enemy[i].var > 0)
					{
						instance->enemy[i].var--;
					}
					if(instance->game_mode == GAME_MODE_STORY)
					{
						if(instance->enemy[i].tick % 60 == 0 && instance->player.active && instance->enemy[i].y >= 48)
						{
							enemy_fire_shot(instance->enemy[i].x + 32 - 8, instance->enemy[i].y + 32 - 8, atan2((instance->enemy[i].y + 32) - (instance->player.y + 16), (instance->enemy[i].x + 32) - (instance->player.x + 16)) + ALLEGRO_PI, 1, data);
						}
					}
					else
					{
						if(instance->enemy[i].tick % 90 == 0 && instance->player.active && instance->enemy[i].y >= 48)
						{
							enemy_fire_shot(instance->enemy[i].x + 32 - 8, instance->enemy[i].y + 32 - 8, atan2((instance->enemy[i].y + 32) - (instance->player.y + 16), (instance->enemy[i].x + 32) - (instance->player.x + 16)) + ALLEGRO_PI, 1, data);
						}
					}
					
					/* move shields */
					for(j = 1; j < 17; j++)
					{
						if(instance->enemy[j].var > 0)
						{
							instance->enemy[j].var--;
						}
						instance->enemy[j].angle = (float)instance->enemy[0].tick / 240.0 * ALLEGRO_PI * 2.0 + (j - 1) * ((ALLEGRO_PI * 2.0) / 16.0);
						instance->enemy[j].x = instance->enemy[0].x + 32.0 + cos(instance->enemy[j].angle) * 96.0 - 8.0;
						instance->enemy[j].y = instance->enemy[0].y + 32.0 + sin(instance->enemy[j].angle) * 96.0 - 8.0;
						t3f_move_collision_object_xy(instance->enemy[j].object, instance->enemy[j].x, instance->enemy[j].y);
						if(instance->enemy[j].var == 0 && instance->player.active && t3f_check_object_collision(instance->enemy[j].object, instance->player.object))
						{
							player_lose_life(data);
							instance->enemy[j].var = 30;
						}
					}
					for(j = 17; j < 33; j++)
					{
						if(instance->enemy[j].var > 0)
						{
							instance->enemy[j].var--;
						}
						instance->enemy[j].angle = -(float)instance->enemy[0].tick / 240.0 * ALLEGRO_PI * 2.0 + (j - 1) * ((ALLEGRO_PI * 2.0) / 16.0);
						instance->enemy[j].x = instance->enemy[0].x + 32.0 + cos(instance->enemy[j].angle) * 128.0 - 8.0;
						instance->enemy[j].y = instance->enemy[0].y + 32.0 + sin(instance->enemy[j].angle) * 128.0 - 8.0;
						t3f_move_collision_object_xy(instance->enemy[j].object, instance->enemy[j].x, instance->enemy[j].y);
						if(instance->enemy[j].var == 0 && instance->player.active && t3f_check_object_collision(instance->enemy[j].object, instance->player.object))
						{
							player_lose_life(data);
							instance->enemy[j].var = 30;
						}
					}
					break;
				}
			}
			instance->enemy[i].tick++;
		}
	}
}

void enemy_render_shadow(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;

	for(i = 0; i < GAME_MAX_ENEMIES; i++)
	{
		if(instance->enemy[i].active)
		{
			switch(instance->enemy[i].type)
			{
				case ENEMY_TYPE_DEMON:
				{
					t3f_draw_rotated_animation(instance->animation[ANIMATION_DEMON], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), instance->enemy[i].tick, 16, 16, instance->enemy[i].x + 16 + 4, instance->enemy[i].y + 16 + 4, 0, instance->enemy[i].angle, 0);
					break;
				}
				case ENEMY_TYPE_ARCHDEMON:
				{
					t3f_draw_rotated_animation(instance->animation[ANIMATION_ARCHDEMON], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), instance->enemy[i].tick, 16, 16, instance->enemy[i].x + 16 + 4, instance->enemy[i].y + 16 + 4, 0, instance->enemy[i].angle, 0);
					break;
				}
				case ENEMY_TYPE_DARK_ORB:
				{
					t3f_draw_animation(instance->animation[ANIMATION_DARK_ORB], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), instance->enemy[i].tick, instance->enemy[i].x + 4, instance->enemy[i].y + 4, 0, 0);
					break;
				}
				case ENEMY_TYPE_DARK_SHIELD:
				{
//					t3f_draw_rotated_animation(instance->animation[ANIMATION_DARK_SHIELD], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), instance->enemy[i].tick, 8, 8, instance->enemy[i].x + 8 + 4, instance->enemy[i].y + 8 + 4, 0, instance->enemy[i].angle, 0);
					break;
				}
			}
		}
	}
}

void enemy_render(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;
	float alpha;

	for(i = 0; i < GAME_MAX_ENEMIES; i++)
	{
		if(instance->enemy[i].active)
		{
			switch(instance->enemy[i].type)
			{
				case ENEMY_TYPE_DEMON:
				{
					t3f_draw_rotated_animation(instance->animation[ANIMATION_DEMON], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), instance->enemy[i].tick, 16, 16, instance->enemy[i].x + 16, instance->enemy[i].y + 16, 0, instance->enemy[i].angle, 0);
					break;
				}
				case ENEMY_TYPE_ARCHDEMON:
				{
					t3f_draw_rotated_animation(instance->animation[ANIMATION_ARCHDEMON], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), instance->enemy[i].tick, 16, 16, instance->enemy[i].x + 16, instance->enemy[i].y + 16, 0, instance->enemy[i].angle, 0);
					break;
				}
				case ENEMY_TYPE_DARK_ORB:
				{
					t3f_draw_animation(instance->animation[ANIMATION_DARK_ORB], al_map_rgba_f((float)instance->enemy[i].var / 30.0, 0.0, 0.0, 1.0), instance->enemy[i].tick, instance->enemy[i].x, instance->enemy[i].y, 0, 0);
					break;
				}
				case ENEMY_TYPE_DARK_SHIELD:
				{
					alpha = instance->enemy[i].var > 0 ? 0.5 : 1.0;
					t3f_draw_rotated_animation(instance->animation[ANIMATION_DARK_SHIELD], al_map_rgba_f(alpha, alpha, alpha, alpha), instance->enemy[i].tick, 8, 8, instance->enemy[i].x + 8 + 4, instance->enemy[i].y + 8 + 4, 0, instance->enemy[i].angle, 0);
					break;
				}
			}
		}
	}
}
