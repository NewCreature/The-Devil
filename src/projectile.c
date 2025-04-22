#include "t3f/t3f.h"
#include "instance.h"
#include "main.h"
#include "powerup.h"
#include "player.h"
#include "particle.h"

void projectile_logic(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i, j, k;
	char buffer[64] = {0};
	
	/* move player shots */
	for(i = 0; i < GAME_MAX_PLAYER_SHOTS; i++)
	{
		if(instance->player_shot[i].active)
		{
			instance->player_shot[i].x += instance->player_shot[i].vx;
			instance->player_shot[i].y += instance->player_shot[i].vy;
			t3f_move_collision_object_xy(instance->player_shot[i].object, instance->player_shot[i].x, instance->player_shot[i].y);
			for(j = 0; j < GAME_MAX_ENEMIES; j++)
			{
				if(instance->enemy[j].active)
				{
					if(t3f_check_object_collision(instance->player_shot[i].object, instance->enemy[j].object))
					{
						switch(instance->enemy[j].type)
						{
							case ENEMY_TYPE_DEMON:
							{
								instance->player_shot[i].active = false;
								instance->enemy[j].active = false;
								generate_death_particles(PARTICLE_TYPE_DEMON_FLESH, instance->enemy[j].x, instance->enemy[j].y, data);
								instance->score += 25 * instance->multiplier;
								sprintf(buffer, "%d", 25 * instance->multiplier);
								generate_text_particle(buffer, instance->enemy[j].x + 16, instance->enemy[j].y, 30, data);
								generate_powerup(POWERUP_TYPE_SPIRIT, instance->enemy[j].x + 8, instance->enemy[j].y + 8, instance->enemy[j].vx / 2.0, instance->enemy[j].vy / 2.0, data);
								al_play_sample(instance->sample[SAMPLE_HIT], 1.0, t3f_get_sound_position(instance->player.x, instance->player.y, instance->enemy[j].x, instance->enemy[j].y), 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
								break;
							}
							case ENEMY_TYPE_ARCHDEMON:
							{
								instance->player_shot[i].active = false;
								instance->enemy[j].active = false;
								generate_death_particles(PARTICLE_TYPE_ARCHDEMON_FLESH, instance->enemy[j].x, instance->enemy[j].y, data);
								instance->score += 100 * instance->multiplier;
								sprintf(buffer, "%d", 100 * instance->multiplier);
								generate_text_particle(buffer, instance->enemy[j].x + 16, instance->enemy[j].y, 30, data);
								generate_powerup(POWERUP_TYPE_SPIRIT, instance->enemy[j].x + 8, instance->enemy[j].y + 8, instance->enemy[j].vx / 2.0, instance->enemy[j].vy / 2.0, data);
								al_play_sample(instance->sample[SAMPLE_HIT], 1.0, t3f_get_sound_position(instance->player.x, instance->player.y, instance->enemy[j].x, instance->enemy[j].y), 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
								break;
							}
							case ENEMY_TYPE_DARK_ORB:
							{
								instance->player_shot[i].active = false;
								if(instance->enemy[j].var2 > 0)
								{
									instance->enemy[j].var = 30;
									instance->enemy[j].var2--;
									if(instance->enemy[j].var2 <= 0)
									{
										al_play_sample(instance->sample[SAMPLE_ORB_DIE], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
										instance->enemy[j].active = false;
										generate_death_particles(PARTICLE_TYPE_DARK_ORB, instance->enemy[j].x, instance->enemy[j].y, data);
										instance->score += 10000 * instance->multiplier;
										sprintf(buffer, "%d", 10000 * instance->multiplier);
										generate_text_particle(buffer, instance->enemy[j].x + 32, instance->enemy[j].y, 30, data);
										for(k = 1; k < 33; k++)
										{
											instance->enemy[k].active = false;
											generate_death_particles(PARTICLE_TYPE_DARK_SHIELD, instance->enemy[k].x, instance->enemy[k].y, data);
										}
										for(k = 0; k < GAME_MAX_ENEMY_SHOTS; k++)
										{
											instance->enemy_shot[k].active = false;
										}
									}
								}
								break;
							}
							case ENEMY_TYPE_DARK_SHIELD:
							{
								if(instance->enemy[j].var == 0)
								{
									instance->player_shot[i].active = false;
									instance->enemy[j].var = 30;
								}
								else
								{
									instance->enemy[j].var--;
								}
								break;
							}
						}
					}
				}
			}
			if(instance->player_shot[i].x < -16 || instance->player_shot[i].x > 640 || instance->player_shot[i].y < -16 || instance->player_shot[i].y > 480)
			{
				instance->player_shot[i].active = false;
			}
			instance->player_shot[i].tick++;
		}
	}
	
	/* move enemy shots */
	for(i = 0; i < GAME_MAX_ENEMY_SHOTS; i++)
	{
		if(instance->enemy_shot[i].active)
		{
			instance->enemy_shot[i].x += instance->enemy_shot[i].vx;
			instance->enemy_shot[i].y += instance->enemy_shot[i].vy;
			t3f_move_collision_object_xy(instance->enemy_shot[i].object, instance->enemy_shot[i].x, instance->enemy_shot[i].y);
			if(instance->player.active)
			{
				if(t3f_check_object_collision(instance->enemy_shot[i].object, instance->player.object))
				{
					instance->enemy_shot[i].active = false;
					player_lose_life(data);
				}
			}
			if(instance->enemy_shot[i].var == 0)
			{
				if(instance->enemy_shot[i].x < -16 || instance->enemy_shot[i].x > 640 || instance->enemy_shot[i].y < -16 || instance->enemy_shot[i].y > 480)
				{
					instance->enemy_shot[i].active = false;
				}
			}
			else
			{
				if(instance->enemy_shot[i].var2 <= 0)
				{
					if(instance->enemy_shot[i].x < -16 || instance->enemy_shot[i].x > 640 || instance->enemy_shot[i].y < -16 || instance->enemy_shot[i].y > 480)
					{
						instance->enemy_shot[i].active = false;
					}
				}
				else
				{
					if(instance->enemy_shot[i].x < 0 || instance->enemy_shot[i].x >= 624)
					{
						instance->enemy_shot[i].vx = -instance->enemy_shot[i].vx;
						instance->enemy_shot[i].var2--;
					}
					if(instance->enemy_shot[i].y < 0 || instance->enemy_shot[i].y >= 464)
					{
						instance->enemy_shot[i].vy = -instance->enemy_shot[i].vy;
						instance->enemy_shot[i].var2--;
					}
				}
			}
			instance->enemy_shot[i].tick++;
		}
	}
}

void projectile_render(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;
	
	for(i = 0; i < GAME_MAX_PLAYER_SHOTS; i++)
	{
		if(instance->player_shot[i].active)
		{
			t3f_draw_animation(instance->animation[ANIMATION_PLAYER_SHOT], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), instance->player_shot[i].tick, instance->player_shot[i].x, instance->player_shot[i].y, 0, 0);
		}
	}
	for(i = 0; i < GAME_MAX_ENEMY_SHOTS; i++)
	{
		if(instance->enemy_shot[i].active)
		{
			t3f_draw_animation(instance->animation[ANIMATION_ENEMY_SHOT], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), instance->enemy_shot[i].tick, instance->enemy_shot[i].x, instance->enemy_shot[i].y, 0, 0);
		}
	}
}
