#include <math.h>
#include "t3f/t3f.h"
#include "t3f/animation.h"
#include "t3f/sound.h"
#include "main.h"
#include "player.h"
#include "particle.h"

void clear_enemies(void)
{
	int i;
	
	for(i = 0; i < GAME_MAX_ENEMIES; i++)
	{
		enemy[i].active = false;
	}
}

int count_enemies(void)
{
	int i, c = 0;
	
	if(enemy[0].type == ENEMY_TYPE_DARK_ORB)
	{
		for(i = 0; i < GAME_MAX_ENEMY_SHOTS; i++)
		{
			if(enemy_shot[i].active)
			{
				c++;
			}
		}
		return c;
	}
	for(i = 0; i < GAME_MAX_ENEMIES; i++)
	{
		if(enemy[i].active)
		{
			c++;
		}
	}
	return c;
}

void enemy_fire_shot(float x, float y, float angle, int type)
{
	int i;
	
	for(i = 0; i < GAME_MAX_ENEMY_SHOTS; i++)
	{
		if(!enemy_shot[i].active)
		{
			al_play_sample(sample[SAMPLE_ENEMY_SHOOT], 1.0, t3f_get_sound_position(player.x, player.y, x, y), 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			enemy_shot[i].x = x;
			enemy_shot[i].y = y;
			enemy_shot[i].angle = angle;
			enemy_shot[i].vx = cos(angle) * 6.0;
			enemy_shot[i].vy = sin(angle) * 6.0;
			enemy_shot[i].var = type;
			enemy_shot[i].var2 = 3;
			generate_particle(PARTICLE_TYPE_ENEMY_SHOT, x + 4, y + 4, angle + t3f_drand(&rng_state) / 2.0 - 0.25, 8.0 + t3f_drand(&rng_state) / 2.0, 10 + rand() % 10);
			generate_particle(PARTICLE_TYPE_ENEMY_SHOT, x + 4, y + 4, angle + t3f_drand(&rng_state) / 2.0 - 0.25, 8.0 + t3f_drand(&rng_state) / 2.0, 10 + rand() % 10);
			enemy_shot[i].active = true;
			break;
		}
	}
}

void enemy_spawn_logic_normal(void)
{
	int i;
	int r;
	
	if(rand() % 1000 < level.demon_rate)
	{
		for(i = 0; i < GAME_MAX_ENEMIES; i++)
		{
			if(!enemy[i].active)
			{
				r = rand() % enemy_spawners;
				enemy[i].x = enemy_spawner[r].x;
				enemy[i].y = enemy_spawner[r].y;
				enemy[i].type = ENEMY_TYPE_DEMON;
				enemy[i].speed = 2.0 + t3f_drand(&rng_state) * (level.demon_speed - 2.0);
				enemy[i].active = true;
				break;
			}
		}
	}
	if(rand() % 1000 < level.archdemon_rate)
	{
		for(i = 0; i < GAME_MAX_ENEMIES; i++)
		{
			if(!enemy[i].active)
			{
				r = rand() % enemy_spawners;
				enemy[i].x = enemy_spawner[r].x;
				enemy[i].y = enemy_spawner[r].y;
				enemy[i].type = ENEMY_TYPE_ARCHDEMON;
				enemy[i].speed = (2.0 + t3f_drand(&rng_state) * (level.demon_speed - 2.0)) / 2.0;
				enemy[i].var = 60;
				enemy[i].active = true;
				break;
			}
		}
	}
}

void enemy_spawn_logic_easy(void)
{
	int i;
	int r;
	
	if(rand() % 3000 < level.demon_rate)
	{
		for(i = 0; i < GAME_MAX_ENEMIES; i++)
		{
			if(!enemy[i].active)
			{
				r = rand() % enemy_spawners;
				enemy[i].x = enemy_spawner[r].x;
				enemy[i].y = enemy_spawner[r].y;
				enemy[i].type = ENEMY_TYPE_DEMON;
				enemy[i].speed = 2.0 + t3f_drand(&rng_state) * (level.demon_speed - 2.0);
				enemy[i].active = true;
				break;
			}
		}
	}
	if(rand() % 3000 < level.archdemon_rate)
	{
		for(i = 0; i < GAME_MAX_ENEMIES; i++)
		{
			if(!enemy[i].active)
			{
				r = rand() % enemy_spawners;
				enemy[i].x = enemy_spawner[r].x;
				enemy[i].y = enemy_spawner[r].y;
				enemy[i].type = ENEMY_TYPE_ARCHDEMON;
				enemy[i].speed = (2.0 + t3f_drand(&rng_state) * (level.demon_speed - 2.0)) / 2.0;
				enemy[i].var = 60;
				enemy[i].active = true;
				break;
			}
		}
	}
}

void enemy_spawn_dark_orb(void)
{
	int i;
	
	enemy[0].type = ENEMY_TYPE_DARK_ORB;
	enemy[0].x = 320 - 32;
	enemy[0].y = -192;
	enemy[0].z = 0;
	enemy[0].vx = -1.5;
	enemy[0].angle = -1;
	enemy[0].var = 0;
	enemy[0].var2 = 160;
	enemy[0].tick = 0;
	enemy[0].active = true;
	if(enemy[0].object)
	{
		t3f_destroy_collision_object(enemy[0].object);
	}
	enemy[0].object = t3f_create_collision_object(12, 12, 40, 40, 32, 32, 0);
	
	for(i = 0; i < 16; i++)
	{
		enemy[i + 1].type = ENEMY_TYPE_DARK_SHIELD;
		enemy[i + 1].active = true;
		if(enemy[i + 1].object)
		{
			t3f_destroy_collision_object(enemy[i + 1].object);
		}
		enemy[i + 1].object = t3f_create_collision_object(0, 0, 16, 16, 32, 32, 0);
		
		enemy[i + 1 + 16].type = ENEMY_TYPE_DARK_SHIELD;
		enemy[i + 1 + 16].active = true;
		if(enemy[i + 1 + 16].object)
		{
			t3f_destroy_collision_object(enemy[i + 1 + 16].object);
		}
		enemy[i + 1 + 16].object = t3f_create_collision_object(0, 0, 16, 16, 32, 32, 0);
	}
}

void enemy_logic(void)
{
	int i, j;
	float target_angle;

	for(i = 0; i < GAME_MAX_ENEMIES; i++)
	{
		if(enemy[i].active)
		{
			switch(enemy[i].type)
			{
				case ENEMY_TYPE_DEMON:
				{
					if(player.active)
					{
						target_angle = atan2(enemy[i].y - player.y, enemy[i].x - player.x);
						enemy[i].angle = target_angle + ALLEGRO_PI;
						enemy[i].vx = cos(enemy[i].angle) * enemy[i].speed;
						enemy[i].vy = sin(enemy[i].angle) * enemy[i].speed;
					}
					else
					{
						enemy[i].vx *= 1.01;
						enemy[i].vy *= 1.01;
					}
					enemy[i].x += enemy[i].vx;
					enemy[i].y += enemy[i].vy;
					t3f_move_collision_object_xy(enemy[i].object, enemy[i].x, enemy[i].y);
					if(player.active && t3f_check_object_collision(enemy[i].object, player.object))
					{
						player_lose_life();
						enemy[i].active = false;
					}
					if(enemy[i].x < -34 || enemy[i].x > 642 || enemy[i].y < -34 || enemy[i].y > 482)
					{
						enemy[i].active = false;
					}
					break;
				}
				case ENEMY_TYPE_ARCHDEMON:
				{
					if(player.active)
					{
						target_angle = atan2(enemy[i].y - player.y, enemy[i].x - player.x);
						enemy[i].angle = target_angle + ALLEGRO_PI;
						enemy[i].vx = cos(enemy[i].angle) * enemy[i].speed;
						enemy[i].vy = sin(enemy[i].angle) * enemy[i].speed;
					}
					else
					{
						enemy[i].vx *= 1.01;
						enemy[i].vy *= 1.01;
					}
					enemy[i].x += enemy[i].vx;
					enemy[i].y += enemy[i].vy;
					t3f_move_collision_object_xy(enemy[i].object, enemy[i].x, enemy[i].y);
					if(player.active && t3f_check_object_collision(enemy[i].object, player.object))
					{
						player_lose_life();
						enemy[i].active = false;
					}
					if(enemy[i].var > 0)
					{
						enemy[i].var--;
					}
					else
					{
						if(player.active && rand() % 1000 < level.archdemon_fire_rate)
						{
							enemy_fire_shot(enemy[i].x + 8, enemy[i].y + 8, enemy[i].angle, 0);
							enemy[i].var = 60;
						}
					}
					if(enemy[i].x < -34 || enemy[i].x > 642 || enemy[i].y < -34 || enemy[i].y > 482)
					{
						enemy[i].active = false;
					}
					break;
				}
				case ENEMY_TYPE_DARK_ORB:
				{
					if(enemy[i].y < 48)
					{
						enemy[i].y += 0.5;
					}
					else
					{
						if(enemy[i].angle < 0)
						{
							enemy[i].vx -= 0.1;
							if(enemy[i].vx < -4.0)
							{
								enemy[i].vx = -4.0;
							}
							enemy[i].x += enemy[i].vx;
							if(enemy[i].x < 240)
							{
								enemy[i].angle = -enemy[i].angle;
							}
						}
						else
						{
							enemy[i].vx += 0.1;
							if(enemy[i].vx > 4.0)
							{
								enemy[i].vx = 4.0;
							}
							enemy[i].x += enemy[i].vx;
							if(enemy[i].x > 640 - 240 - 64)
							{
								enemy[i].angle = -enemy[i].angle;
							}
						}
					}
					t3f_move_collision_object_xy(enemy[i].object, enemy[i].x, enemy[i].y);
					if(enemy[i].var > 0)
					{
						enemy[i].var--;
					}
					if(game_mode == GAME_MODE_STORY)
					{
						if(enemy[i].tick % 60 == 0 && player.active && enemy[i].y >= 48)
						{
							enemy_fire_shot(enemy[i].x + 32 - 8, enemy[i].y + 32 - 8, atan2((enemy[i].y + 32) - (player.y + 16), (enemy[i].x + 32) - (player.x + 16)) + ALLEGRO_PI, 1);
						}
					}
					else
					{
						if(enemy[i].tick % 90 == 0 && player.active && enemy[i].y >= 48)
						{
							enemy_fire_shot(enemy[i].x + 32 - 8, enemy[i].y + 32 - 8, atan2((enemy[i].y + 32) - (player.y + 16), (enemy[i].x + 32) - (player.x + 16)) + ALLEGRO_PI, 1);
						}
					}
					
					/* move shields */
					for(j = 1; j < 17; j++)
					{
						if(enemy[j].var > 0)
						{
							enemy[j].var--;
						}
						enemy[j].angle = (float)enemy[0].tick / 240.0 * ALLEGRO_PI * 2.0 + (j - 1) * ((ALLEGRO_PI * 2.0) / 16.0);
						enemy[j].x = enemy[0].x + 32.0 + cos(enemy[j].angle) * 96.0 - 8.0;
						enemy[j].y = enemy[0].y + 32.0 + sin(enemy[j].angle) * 96.0 - 8.0;
						t3f_move_collision_object_xy(enemy[j].object, enemy[j].x, enemy[j].y);
						if(enemy[j].var == 0 && player.active && t3f_check_object_collision(enemy[j].object, player.object))
						{
							player_lose_life();
							enemy[j].var = 30;
						}
					}
					for(j = 17; j < 33; j++)
					{
						if(enemy[j].var > 0)
						{
							enemy[j].var--;
						}
						enemy[j].angle = -(float)enemy[0].tick / 240.0 * ALLEGRO_PI * 2.0 + (j - 1) * ((ALLEGRO_PI * 2.0) / 16.0);
						enemy[j].x = enemy[0].x + 32.0 + cos(enemy[j].angle) * 128.0 - 8.0;
						enemy[j].y = enemy[0].y + 32.0 + sin(enemy[j].angle) * 128.0 - 8.0;
						t3f_move_collision_object_xy(enemy[j].object, enemy[j].x, enemy[j].y);
						if(enemy[j].var == 0 && player.active && t3f_check_object_collision(enemy[j].object, player.object))
						{
							player_lose_life();
							enemy[j].var = 30;
						}
					}
					break;
				}
			}
			enemy[i].tick++;
		}
	}
}

void enemy_render_shadow(void)
{
	int i;

	for(i = 0; i < GAME_MAX_ENEMIES; i++)
	{
		if(enemy[i].active)
		{
			switch(enemy[i].type)
			{
				case ENEMY_TYPE_DEMON:
				{
					t3f_draw_rotated_animation(animation[ANIMATION_DEMON], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), enemy[i].tick, 16, 16, enemy[i].x + 16 + 4, enemy[i].y + 16 + 4, 0, enemy[i].angle, 0);
					break;
				}
				case ENEMY_TYPE_ARCHDEMON:
				{
					t3f_draw_rotated_animation(animation[ANIMATION_ARCHDEMON], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), enemy[i].tick, 16, 16, enemy[i].x + 16 + 4, enemy[i].y + 16 + 4, 0, enemy[i].angle, 0);
					break;
				}
				case ENEMY_TYPE_DARK_ORB:
				{
					t3f_draw_animation(animation[ANIMATION_DARK_ORB], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), enemy[i].tick, enemy[i].x + 4, enemy[i].y + 4, 0, 0);
					break;
				}
				case ENEMY_TYPE_DARK_SHIELD:
				{
//					t3f_draw_rotated_animation(animation[ANIMATION_DARK_SHIELD], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), enemy[i].tick, 8, 8, enemy[i].x + 8 + 4, enemy[i].y + 8 + 4, 0, enemy[i].angle, 0);
					break;
				}
			}
		}
	}
}

void enemy_render(void)
{
	int i;
	float alpha;

	for(i = 0; i < GAME_MAX_ENEMIES; i++)
	{
		if(enemy[i].active)
		{
			switch(enemy[i].type)
			{
				case ENEMY_TYPE_DEMON:
				{
					t3f_draw_rotated_animation(animation[ANIMATION_DEMON], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), enemy[i].tick, 16, 16, enemy[i].x + 16, enemy[i].y + 16, 0, enemy[i].angle, 0);
					break;
				}
				case ENEMY_TYPE_ARCHDEMON:
				{
					t3f_draw_rotated_animation(animation[ANIMATION_ARCHDEMON], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), enemy[i].tick, 16, 16, enemy[i].x + 16, enemy[i].y + 16, 0, enemy[i].angle, 0);
					break;
				}
				case ENEMY_TYPE_DARK_ORB:
				{
					t3f_draw_animation(animation[ANIMATION_DARK_ORB], al_map_rgba_f((float)enemy[i].var / 30.0, 0.0, 0.0, 1.0), enemy[i].tick, enemy[i].x, enemy[i].y, 0, 0);
					break;
				}
				case ENEMY_TYPE_DARK_SHIELD:
				{
					alpha = enemy[i].var > 0 ? 0.5 : 1.0;
					t3f_draw_rotated_animation(animation[ANIMATION_DARK_SHIELD], al_map_rgba_f(alpha, alpha, alpha, alpha), enemy[i].tick, 8, 8, enemy[i].x + 8 + 4, enemy[i].y + 8 + 4, 0, enemy[i].angle, 0);
					break;
				}
			}
		}
	}
}
