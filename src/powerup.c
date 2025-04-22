#include "t3f/t3f.h"
#include "instance.h"
#include "main.h"

void generate_powerup(int type, float x, float y, float vx, float vy, void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;
	
	for(i = 0; i < GAME_MAX_POWERUPS; i++)
	{
		if(!instance->powerup[i].active)
		{
			instance->powerup[i].type = type;
			instance->powerup[i].x = x;
			instance->powerup[i].y = y;
			instance->powerup[i].vx = vx;
			instance->powerup[i].vy = vy;
			instance->powerup[i].var = 120;
			instance->powerup[i].active = true;
			break;
		}
	}
}

void powerup_logic(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	char buffer[64] = {0};
	int i, j;
	
	for(i = 0; i < GAME_MAX_POWERUPS; i++)
	{
		if(instance->powerup[i].active)
		{
			switch(instance->powerup[i].type)
			{
				case POWERUP_TYPE_SPIRIT:
				{
					instance->powerup[i].var--;
					instance->powerup[i].x += instance->powerup[i].vx;
					instance->powerup[i].y += instance->powerup[i].vy;
					if(instance->powerup[i].var <= 0)
					{
						instance->powerup[i].active = false;
					}
					else
					{
						t3f_move_collision_object_xy(instance->powerup[i].object, instance->powerup[i].x, instance->powerup[i].y);
						if(instance->player.active && t3f_check_object_collision(instance->powerup[i].object, instance->player.object))
						{
							al_play_sample(instance->sample[SAMPLE_POWERUP], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
							for(j = 0; j < 6; j++)
							{
								generate_particle(PARTICLE_TYPE_REMAINS, instance->powerup[i].x, instance->powerup[i].y, t3f_drand(&instance->rng_state) * ALLEGRO_PI * 2.0, 2.0 + t3f_drand(&instance->rng_state) * 4.0, 15 + rand() % 25, data);
							}
							instance->score += 10 * instance->multiplier;
							sprintf(buffer, "%d", 10 * instance->multiplier);
							generate_text_particle(buffer, instance->player.x + 16, instance->player.y, 30, data);
							instance->multiplier_tick++;
							if(instance->multiplier_tick >= 10)
							{
								if(instance->multiplier < 10)
								{
									al_play_sample(instance->sample[SAMPLE_MULTIPLIER], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
									instance->multiplier++;
									if(instance->multiplier == 10)
									{
										al_play_sample(instance->sample[SAMPLE_MAX_MULTIPLIER], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
										sprintf(buffer, "%d", 25 * instance->multiplier);
										generate_text_particle("Max", instance->player.x + 16, instance->player.y - 16, 30, data);
									}
									else
									{
										sprintf(buffer, "x%d", instance->multiplier);
										generate_text_particle(buffer, instance->player.x + 16, instance->player.y - 16, 30, data);
									}
								}
								instance->multiplier_tick = 0;
							}
							instance->powerup[i].active = 0;
						}
					}
					instance->powerup[i].tick++;
					break;
				}
				case POWERUP_TYPE_UPGRADE:
				{
					break;
				}
			}
		}
	}
}

void powerup_render(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;
	float alpha;
	
	for(i = 0; i < GAME_MAX_POWERUPS; i++)
	{
		if(instance->powerup[i].active)
		{
			switch(instance->powerup[i].type)
			{
				case POWERUP_TYPE_SPIRIT:
				{
					alpha = (float)instance->powerup[i].var / 120.0;
					t3f_draw_animation(instance->animation[ANIMATION_SPIRIT], al_map_rgba_f(alpha, alpha, alpha, alpha), instance->powerup[i].tick, instance->powerup[i].x, instance->powerup[i].y, 0, 0);
					break;
				}
				case POWERUP_TYPE_UPGRADE:
				{
					break;
				}
			}
		}
	}
}
