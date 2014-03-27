#include "t3f/t3f.h"
#include "t3f/sound.h"
#include "main.h"
#include "powerup.h"
#include "player.h"
#include "particle.h"

void projectile_logic(void)
{
	int i, j, k;
	char buffer[64] = {0};
	
	/* move player shots */
	for(i = 0; i < GAME_MAX_PLAYER_SHOTS; i++)
	{
		if(player_shot[i].active)
		{
			player_shot[i].x += player_shot[i].vx;
			player_shot[i].y += player_shot[i].vy;
			t3f_move_collision_object_xy(player_shot[i].object, player_shot[i].x, player_shot[i].y);
			for(j = 0; j < GAME_MAX_ENEMIES; j++)
			{
				if(enemy[j].active)
				{
					if(t3f_check_object_collision(player_shot[i].object, enemy[j].object))
					{
						switch(enemy[j].type)
						{
							case ENEMY_TYPE_DEMON:
							{
								player_shot[i].active = false;
								enemy[j].active = false;
								generate_death_particles(PARTICLE_TYPE_DEMON_FLESH, enemy[j].x, enemy[j].y);
								score += 25 * multiplier;
								sprintf(buffer, "%d", 25 * multiplier);
								generate_text_particle(buffer, enemy[j].x + 16, enemy[j].y, 30);
								generate_powerup(POWERUP_TYPE_SPIRIT, enemy[j].x + 8, enemy[j].y + 8, enemy[j].vx / 2.0, enemy[j].vy / 2.0);
								al_play_sample(sample[SAMPLE_HIT], 1.0, t3f_get_sound_position(player.x, player.y, enemy[j].x, enemy[j].y), 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
								break;
							}
							case ENEMY_TYPE_ARCHDEMON:
							{
								player_shot[i].active = false;
								enemy[j].active = false;
								generate_death_particles(PARTICLE_TYPE_ARCHDEMON_FLESH, enemy[j].x, enemy[j].y);
								score += 100 * multiplier;
								sprintf(buffer, "%d", 100 * multiplier);
								generate_text_particle(buffer, enemy[j].x + 16, enemy[j].y, 30);
								generate_powerup(POWERUP_TYPE_SPIRIT, enemy[j].x + 8, enemy[j].y + 8, enemy[j].vx / 2.0, enemy[j].vy / 2.0);
								al_play_sample(sample[SAMPLE_HIT], 1.0, t3f_get_sound_position(player.x, player.y, enemy[j].x, enemy[j].y), 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
								break;
							}
							case ENEMY_TYPE_DARK_ORB:
							{
								player_shot[i].active = false;
								if(enemy[j].var2 > 0)
								{
									enemy[j].var = 30;
									enemy[j].var2--;
									if(enemy[j].var2 <= 0)
									{
										al_play_sample(sample[SAMPLE_ORB_DIE], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
										enemy[j].active = false;
										generate_death_particles(PARTICLE_TYPE_DARK_ORB, enemy[j].x, enemy[j].y);
										score += 10000 * multiplier;
										sprintf(buffer, "%d", 10000 * multiplier);
										generate_text_particle(buffer, enemy[j].x + 32, enemy[j].y, 30);
										for(k = 1; k < 33; k++)
										{
											enemy[k].active = false;
											generate_death_particles(PARTICLE_TYPE_DARK_SHIELD, enemy[k].x, enemy[k].y);
										}
										for(k = 0; k < GAME_MAX_ENEMY_SHOTS; k++)
										{
											enemy_shot[k].active = false;
										}
									}
								}
								break;
							}
							case ENEMY_TYPE_DARK_SHIELD:
							{
								if(enemy[j].var == 0)
								{
									player_shot[i].active = false;
									enemy[j].var = 30;
								}
								else
								{
									enemy[j].var--;
								}
								break;
							}
						}
					}
				}
			}
			if(player_shot[i].x < -16 || player_shot[i].x > 640 || player_shot[i].y < -16 || player_shot[i].y > 480)
			{
				player_shot[i].active = false;
			}
			player_shot[i].tick++;
		}
	}
	
	/* move enemy shots */
	for(i = 0; i < GAME_MAX_ENEMY_SHOTS; i++)
	{
		if(enemy_shot[i].active)
		{
			enemy_shot[i].x += enemy_shot[i].vx;
			enemy_shot[i].y += enemy_shot[i].vy;
			t3f_move_collision_object_xy(enemy_shot[i].object, enemy_shot[i].x, enemy_shot[i].y);
			if(player.active)
			{
				if(t3f_check_object_collision(enemy_shot[i].object, player.object))
				{
					enemy_shot[i].active = false;
					player_lose_life();
				}
			}
			if(enemy_shot[i].var == 0)
			{
				if(enemy_shot[i].x < -16 || enemy_shot[i].x > 640 || enemy_shot[i].y < -16 || enemy_shot[i].y > 480)
				{
					enemy_shot[i].active = false;
				}
			}
			else
			{
				if(enemy_shot[i].var2 <= 0)
				{
					if(enemy_shot[i].x < -16 || enemy_shot[i].x > 640 || enemy_shot[i].y < -16 || enemy_shot[i].y > 480)
					{
						enemy_shot[i].active = false;
					}
				}
				else
				{
					if(enemy_shot[i].x < 0 || enemy_shot[i].x >= 624)
					{
						enemy_shot[i].vx = -enemy_shot[i].vx;
						enemy_shot[i].var2--;
					}
					if(enemy_shot[i].y < 0 || enemy_shot[i].y >= 464)
					{
						enemy_shot[i].vy = -enemy_shot[i].vy;
						enemy_shot[i].var2--;
					}
				}
			}
			enemy_shot[i].tick++;
		}
	}
}

void projectile_render(void)
{
	int i;
	
	for(i = 0; i < GAME_MAX_PLAYER_SHOTS; i++)
	{
		if(player_shot[i].active)
		{
			t3f_draw_animation(animation[ANIMATION_PLAYER_SHOT], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), player_shot[i].tick, player_shot[i].x, player_shot[i].y, 0, 0);
		}
	}
	for(i = 0; i < GAME_MAX_ENEMY_SHOTS; i++)
	{
		if(enemy_shot[i].active)
		{
			t3f_draw_animation(animation[ANIMATION_ENEMY_SHOT], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), enemy_shot[i].tick, enemy_shot[i].x, enemy_shot[i].y, 0, 0);
		}
	}
}
