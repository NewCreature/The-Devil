#include "t3f/t3f.h"
#include "main.h"
#include "game.h"

static int current_text_particle = 0;

void generate_text_particle(char * text, float x, float y, int life)
{
	strcpy(text_particle[current_text_particle].text, text);
	text_particle[current_text_particle].x = x;
	text_particle[current_text_particle].y = y;
	text_particle[current_text_particle].life = life;
	text_particle[current_text_particle].total_life = life;
	text_particle[current_text_particle].active = true;
	current_text_particle++;
	if(current_text_particle >= GAME_MAX_TEXT_PARTICLES)
	{
		current_text_particle = 0;
	}
}

void generate_particle(int type, float x, float y, float angle, float speed, int life)
{
	int i;
	
	for(i = 0; i < GAME_MAX_PARTICLES; i++)
	{
		if(!particle[i].active)
		{
			particle[i].type = type;
			particle[i].x = x;
			particle[i].y = y;
			particle[i].angle = angle;
			particle[i].vx = cos(angle);
			particle[i].vy = sin(angle);
			particle[i].var = life;
			particle[i].var2 = life;
			particle[i].active = true;
			break;
		}
	}
}

void generate_death_particles(int type, float x, float y)
{
	int i;
	
	if(type == PARTICLE_TYPE_DARK_ORB)
	{
		for(i = 0; i < 64; i++)
		{
			generate_particle(type, x + 16 + rand() % 32, y + 16 + rand() % 32, t3f_drand(&rng_state) * ALLEGRO_PI * 2, 1.0 + t3f_drand(&rng_state) * 1.5, 20 + rand() % 20);
		}
	}
	else if(type == PARTICLE_TYPE_DARK_SHIELD)
	{
		for(i = 0; i < 4; i++)
		{
			generate_particle(type, x + rand() % 16, y + rand() % 16, t3f_drand(&rng_state) * ALLEGRO_PI * 2, 1.0 + t3f_drand(&rng_state) * 1.5, 20 + rand() % 20);
		}
	}
	else
	{
		for(i = 0; i < 6; i++)
		{
			generate_particle(type, x + 8 + rand() % 8 - 4, y + 8 + rand() % 8 - 4, t3f_drand(&rng_state) * ALLEGRO_PI * 2, 1.0 + t3f_drand(&rng_state) * 1.5, 20 + rand() % 20);
		}
		for(i = 0; i < 3; i++)
		{
			generate_particle(type + 1, x + 8 + rand() % 8 - 4, y + 8 + rand() % 8 - 4, t3f_drand(&rng_state) * ALLEGRO_PI * 2, 1.0 + t3f_drand(&rng_state) * 1.5, 20 + rand() % 20);
		}
	}
}

void particle_logic(void)
{
	int i;
	
	for(i = 0; i < GAME_MAX_PARTICLES; i++)
	{
		if(particle[i].active)
		{
			particle[i].x += particle[i].vx;
			particle[i].y += particle[i].vy;
			particle[i].tick++;
			particle[i].var--;
			if(particle[i].var <= 0)
			{
				particle[i].active = 0;
			}
		}
	}
}

void particle_render(void)
{
	ALLEGRO_COLOR color;
	int i;
	float alpha;
	
	for(i = 0; i < GAME_MAX_PARTICLES; i++)
	{
		if(particle[i].active)
		{
			alpha = ((float)particle[i].var / (float)particle[i].var2);
			switch(particle[i].type)
			{
				case PARTICLE_TYPE_PLAYER_SHOT:
				{
					color = al_map_rgba_f((119.0 / 255.0) * alpha, alpha, (74.0 / 255.0) * alpha, alpha);
					break;
				}
				case PARTICLE_TYPE_ENEMY_SHOT:
				{
					color = al_map_rgba_f(alpha, (119.0 / 255.0) * alpha, (74.0 / 255.0) * alpha, alpha);
					break;
				}
				case PARTICLE_TYPE_REMAINS:
				{
					color = al_map_rgba_f((74.0 / 255.0) * alpha, (119.0 / 255.0) * alpha, alpha, alpha);
					break;
				}
				case PARTICLE_TYPE_PLAYER_FLESH:
				{
					color = al_map_rgba_f((231.0 / 255.0) * alpha, 0, 0, alpha);
					break;
				}
				case PARTICLE_TYPE_PLAYER_CLOTHES:
				{
					color = al_map_rgba_f(0, 0, 0, alpha);
					break;
				}
				case PARTICLE_TYPE_DEMON_FLESH:
				{
					color = al_map_rgba_f((198.0 / 255.0) * alpha, (97.0 / 255.0) * alpha, (39.0 / 255.0) * alpha, alpha);
					break;
				}
				case PARTICLE_TYPE_DEMON_CLOTHES:
				{
					color = al_map_rgba_f((36.0 / 255.0) * alpha, (35.0 / 255.0) * alpha, (122.0 / 255.0) * alpha, alpha);
					break;
				}
				case PARTICLE_TYPE_ARCHDEMON_FLESH:
				{
					color = al_map_rgba_f((110.0 / 255.0) * alpha, (161.0 / 255.0) * alpha, (23.0 / 255.0) * alpha, alpha);
					break;
				}
				case PARTICLE_TYPE_ARCHDEMON_CLOTHES:
				{
					color = al_map_rgba_f((185.0 / 255.0) * alpha, (20.0 / 255.0) * alpha, (22.0 / 255.0) * alpha, alpha);
					break;
				}
				case PARTICLE_TYPE_DARK_ORB:
				{
					color = al_map_rgba_f(0, 0, 0, alpha);
					break;
				}
				case PARTICLE_TYPE_DARK_SHIELD:
				{
					color = al_map_rgba_f(0, alpha, 0, alpha);
					break;
				}
			}
			t3f_draw_animation(animation[ANIMATION_PLAYER_SHOT_PARTICLE], color, particle[i].tick, particle[i].x, particle[i].y, 0, 0);
		}
	}
}

void text_particle_logic(void)
{
	int i;
	
	for(i = 0; i < GAME_MAX_TEXT_PARTICLES; i++)
	{
		if(text_particle[i].active)
		{
			text_particle[i].y -= 0.5;
			text_particle[i].life--;
			if(text_particle[i].life <= 0)
			{
				text_particle[i].active = 0;
			}
		}
	}
}

void text_particle_render(void)
{
	int i;
	
	for(i = 0; i < GAME_MAX_TEXT_PARTICLES; i++)
	{
		if(text_particle[i].active)
		{
			al_draw_text(font[FONT_TINY], al_map_rgba_f(1.0, 1.0, 1.0, (float)text_particle[i].life / (float)text_particle[i].total_life), text_particle[i].x, text_particle[i].y, ALLEGRO_ALIGN_CENTRE, text_particle[i].text);
		}
	}
}
