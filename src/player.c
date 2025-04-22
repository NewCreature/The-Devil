#include "t3f/t3f.h"
#include "instance.h"
#include "main.h"
#include "enemy.h"
#include "particle.h"

static bool fire = false;
float fire_angle;

void player_fire_shot(float angle, void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;
	
	for(i = 0; i < GAME_MAX_PLAYER_SHOTS; i++)
	{
		if(!instance->player_shot[i].active)
		{
			instance->player_shot[i].x = instance->player.x + 8;
			instance->player_shot[i].y = instance->player.y + 8;
			instance->player_shot[i].angle = angle;
			instance->player_shot[i].vx = cos(angle) * 12.0;
			instance->player_shot[i].vy = sin(angle) * 12.0;
			instance->player_shot[i].active = true;
			generate_particle(PARTICLE_TYPE_PLAYER_SHOT, instance->player_shot[i].x + 4, instance->player_shot[i].y + 4, angle + t3f_drand(&instance->rng_state) / 2.0 - 0.25, 1.0 + t3f_drand(&instance->rng_state) / 2.0, 10 + rand() % 10, data);
			generate_particle(PARTICLE_TYPE_PLAYER_SHOT, instance->player_shot[i].x + 4, instance->player_shot[i].y + 4, angle + t3f_drand(&instance->rng_state) / 2.0 - 0.25, 1.0 + t3f_drand(&instance->rng_state) / 2.0, 10 + rand() % 10, data);
			break;
		}
	}
}

void player_lose_life(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->lives--;
	al_play_sample(instance->sample[SAMPLE_DIE], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	generate_death_particles(PARTICLE_TYPE_PLAYER_FLESH, instance->player.x, instance->player.y, data);
	if(instance->lives <= 0)
	{
		instance->game_state = GAME_STATE_OVER;
		instance->die_timer = 120;
		instance->player.active = false;
		al_play_sample(instance->sample[SAMPLE_GAME_OVER], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
	else
	{
//		clear_enemies();
		instance->die_state = instance->game_state;
		instance->game_state = GAME_STATE_DIE;
		instance->die_timer = 120;
		instance->player.active = false;
	}
	instance->death_time = 180;
}

void player_control_normal(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	t3f_read_controller(instance->controller);
	t3f_update_controller(instance->controller);
	/* move player */
	instance->player.vx = 0;
	instance->player.vy = 0;
	if(instance->controller->state[CONTROLLER_LEFT].held)
	{
		instance->player.vx = -instance->player.speed;
		instance->player.vy = 0;
		instance->player.angle = ALLEGRO_PI;
	}
	if(instance->controller->state[CONTROLLER_UP].held)
	{
		instance->player.vx = 0;
		instance->player.vy = -instance->player.speed;
		instance->player.angle = ALLEGRO_PI * 1.5;
	}
	if(instance->controller->state[CONTROLLER_RIGHT].held)
	{
		instance->player.vx = instance->player.speed;
		instance->player.vy = 0;
		instance->player.angle = 0;
	}
	if(instance->controller->state[CONTROLLER_DOWN].held)
	{
		instance->player.vx = 0;
		instance->player.vy = instance->player.speed;
		instance->player.angle = ALLEGRO_PI * 0.5;
	}
	if(instance->controller->state[CONTROLLER_LEFT].held && instance->controller->state[CONTROLLER_UP].held)
	{
		instance->player.vx = cos(1.25 * ALLEGRO_PI) * instance->player.speed;
		instance->player.vy = sin(1.25 * ALLEGRO_PI) * instance->player.speed;
		instance->player.angle = ALLEGRO_PI * 1.25;
	}
	if(instance->controller->state[CONTROLLER_RIGHT].held && instance->controller->state[CONTROLLER_UP].held)
	{
		instance->player.vx = cos(1.75 * ALLEGRO_PI) * instance->player.speed;
		instance->player.vy = sin(1.75 * ALLEGRO_PI) * instance->player.speed;
		instance->player.angle = ALLEGRO_PI * 1.75;
	}
	if(instance->controller->state[CONTROLLER_LEFT].held && instance->controller->state[CONTROLLER_DOWN].held)
	{
		instance->player.vx = cos(0.75 * ALLEGRO_PI) * instance->player.speed;
		instance->player.vy = sin(0.75 * ALLEGRO_PI) * instance->player.speed;
		instance->player.angle = ALLEGRO_PI * 0.75;
	}
	if(instance->controller->state[CONTROLLER_RIGHT].held && instance->controller->state[CONTROLLER_DOWN].held)
	{
		instance->player.vx = cos(0.25 * ALLEGRO_PI) * instance->player.speed;
		instance->player.vy = sin(0.25 * ALLEGRO_PI) * instance->player.speed;
		instance->player.angle = ALLEGRO_PI * 0.25;
	}
	if(instance->player.var > 0)
	{
		instance->player.var--;
	}
	else
	{
		if(instance->controller->state[CONTROLLER_FIRE_LEFT].held)
		{
			fire = true;
			fire_angle = ALLEGRO_PI;
		}
		if(instance->controller->state[CONTROLLER_FIRE_UP].held)
		{
			fire = true;
			fire_angle = ALLEGRO_PI * 1.5;
		}
		if(instance->controller->state[CONTROLLER_FIRE_RIGHT].held)
		{
			fire = true;
			fire_angle = 0;
		}
		if(instance->controller->state[CONTROLLER_FIRE_DOWN].held)
		{
			fire = true;
			fire_angle = ALLEGRO_PI * 0.5;
		}
		if(instance->controller->state[CONTROLLER_FIRE_LEFT].held && instance->controller->state[CONTROLLER_FIRE_UP].held)
		{
			fire = true;
			fire_angle = ALLEGRO_PI * 1.25;
		}
		if(instance->controller->state[CONTROLLER_FIRE_RIGHT].held && instance->controller->state[CONTROLLER_FIRE_UP].held)
		{
			fire = true;
			fire_angle = ALLEGRO_PI * 1.75;
		}
		if(instance->controller->state[CONTROLLER_FIRE_LEFT].held && instance->controller->state[CONTROLLER_FIRE_DOWN].held)
		{
			fire = true;
			fire_angle = ALLEGRO_PI * 0.75;
		}
		if(instance->controller->state[CONTROLLER_FIRE_RIGHT].held && instance->controller->state[CONTROLLER_FIRE_DOWN].held)
		{
			fire = true;
			fire_angle = ALLEGRO_PI * 0.25;
		}
	}
}

void player_control_mouse(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	t3f_read_controller(instance->controller);
	t3f_update_controller(instance->controller);
	/* move player */
	instance->player.vx = 0;
	instance->player.vy = 0;
	if(instance->controller->state[CONTROLLER_LEFT].held)
	{
		instance->player.vx = -instance->player.speed;
		instance->player.vy = 0;
		instance->player.angle = ALLEGRO_PI;
	}
	if(instance->controller->state[CONTROLLER_UP].held)
	{
		instance->player.vx = 0;
		instance->player.vy = -instance->player.speed;
		instance->player.angle = ALLEGRO_PI * 1.5;
	}
	if(instance->controller->state[CONTROLLER_RIGHT].held)
	{
		instance->player.vx = instance->player.speed;
		instance->player.vy = 0;
		instance->player.angle = 0;
	}
	if(instance->controller->state[CONTROLLER_DOWN].held)
	{
		instance->player.vx = 0;
		instance->player.vy = instance->player.speed;
		instance->player.angle = ALLEGRO_PI * 0.5;
	}
	if(instance->controller->state[CONTROLLER_LEFT].held && instance->controller->state[CONTROLLER_UP].held)
	{
		instance->player.vx = cos(1.25 * ALLEGRO_PI) * instance->player.speed;
		instance->player.vy = sin(1.25 * ALLEGRO_PI) * instance->player.speed;
		instance->player.angle = ALLEGRO_PI * 1.25;
	}
	if(instance->controller->state[CONTROLLER_RIGHT].held && instance->controller->state[CONTROLLER_UP].held)
	{
		instance->player.vx = cos(1.75 * ALLEGRO_PI) * instance->player.speed;
		instance->player.vy = sin(1.75 * ALLEGRO_PI) * instance->player.speed;
		instance->player.angle = ALLEGRO_PI * 1.75;
	}
	if(instance->controller->state[CONTROLLER_LEFT].held && instance->controller->state[CONTROLLER_DOWN].held)
	{
		instance->player.vx = cos(0.75 * ALLEGRO_PI) * instance->player.speed;
		instance->player.vy = sin(0.75 * ALLEGRO_PI) * instance->player.speed;
		instance->player.angle = ALLEGRO_PI * 0.75;
	}
	if(instance->controller->state[CONTROLLER_RIGHT].held && instance->controller->state[CONTROLLER_DOWN].held)
	{
		instance->player.vx = cos(0.25 * ALLEGRO_PI) * instance->player.speed;
		instance->player.vy = sin(0.25 * ALLEGRO_PI) * instance->player.speed;
		instance->player.angle = ALLEGRO_PI * 0.25;
	}
	if(instance->player.var > 0)
	{
		instance->player.var--;
	}
	else
	{
		fire_angle = atan2(instance->player.y - t3f_get_mouse_y(), instance->player.x - t3f_get_mouse_x()) + ALLEGRO_PI;
		if(t3f_mouse_button_held(0))
		{
			fire = true;
		}
	}
}

void player_control_analog(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	float strength;
	
	t3f_read_controller(instance->controller);
	
	/* handle player movement */
	strength = t3f_distance(0.0, 0.0, instance->controller->state[2].pos, instance->controller->state[0].pos) * 1.5;
	if(strength > 1.0)
	{
		strength = 1.0;
	}
	if(strength > 0.1)
	{
		instance->player.angle = atan2(instance->controller->state[0].pos, instance->controller->state[2].pos);
	}
	else
	{
		strength = 0.0;
	}
	instance->player.vx = cos(instance->player.angle) * instance->player.speed * strength;
	instance->player.vy = sin(instance->player.angle) * instance->player.speed * strength;

	fire_angle = atan2(instance->controller->state[4].pos, instance->controller->state[6].pos);
	if(instance->player.var > 0)
	{
		instance->player.var--;
	}
	else
	{
		if(instance->controller->state[4].pos <= 0.2 && instance->controller->state[4].pos >= -0.2 && instance->controller->state[6].pos <= 0.2 && instance->controller->state[6].pos >= -0.2)
		{
		}
		else
		{
			fire = true;
		}
	}
}

void player_control_touch(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	float strength;
	
	/* player movement */
	if(!instance->touch_stick[0].active)
	{
		instance->player.vx = 0;
		instance->player.vy = 0;
	}
	else
	{
		/* handle player movement */
		strength = (t3f_distance(instance->touch_stick[0].pin_x, instance->touch_stick[0].pin_y, instance->touch_stick[0].pos_x, instance->touch_stick[0].pos_y) / instance->touch_size) * 1.5;
		if(strength > 1.0)
		{
			strength = 1.0;
		}
		if(strength > 0.1)
		{
			instance->player.angle = atan2(instance->touch_stick[0].pos_y - instance->touch_stick[0].pin_y, instance->touch_stick[0].pos_x - instance->touch_stick[0].pin_x);
		}
		else
		{
			strength = 0.0;
		}
		instance->player.vx = cos(instance->player.angle) * instance->player.speed * strength;
		instance->player.vy = sin(instance->player.angle) * instance->player.speed * strength;
	}

	/* player attack */
	if(!instance->touch_stick[1].active)
	{
	}
	else
	{
		instance->touch_stick[1].pos_x = t3f_get_touch_x(instance->touch_stick[1].touch_id);
		instance->touch_stick[1].pos_y = t3f_get_touch_y(instance->touch_stick[1].touch_id);
		strength = (t3f_distance(instance->touch_stick[1].pin_x, instance->touch_stick[1].pin_y, instance->touch_stick[1].pos_x, instance->touch_stick[1].pos_y) / instance->touch_size) * 1.5;
		if(strength > 1.0)
		{
			strength = 1.0;
		}
		if(strength > 0.1)
		{
			fire_angle = atan2(instance->touch_stick[1].pos_y - instance->touch_stick[1].pin_y, instance->touch_stick[1].pos_x - instance->touch_stick[1].pin_x);
		}
		else
		{
			strength = 0.0;
		}
		if(instance->player.var > 0)
		{
			instance->player.var--;
		}
		else
		{
			if(strength <= 0.4)
			{
			}
			else
			{
				fire = true;
			}
		}
	}
}

void player_logic(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	fire = false;
	
	if(instance->player.active)
	{
		switch(instance->controller_type)
		{
			case CONTROLLER_TYPE_NORMAL:
			{
				player_control_normal(data);
				break;
			}
			case CONTROLLER_TYPE_MOUSE:
			{
				player_control_mouse(data);
				break;
			}
			case CONTROLLER_TYPE_ANALOG:
			{
				player_control_analog(data);
				break;
			}
			case CONTROLLER_TYPE_TOUCH_S:
			case CONTROLLER_TYPE_TOUCH_M:
			case CONTROLLER_TYPE_TOUCH_L:
			{
				player_control_touch(data);
				break;
			}
		}
		instance->player.x += instance->player.vx;
		if(instance->player.x < 0)
		{
			instance->player.x = 0;
		}
		if(instance->player.x > 640 - 32)
		{
			instance->player.x = 640 - 32;
		}
		instance->player.y += instance->player.vy;
		if(instance->player.y < 0)
		{
			instance->player.y = 0;
		}
		if(instance->player.y > 480 - 32)
		{
			instance->player.y = 480 - 32;
		}
		t3f_move_collision_object_xy(instance->player.object, instance->player.x, instance->player.y);
		
		/* fire */
		if(fire)
		{
			switch(instance->weapon)
			{
				case 0:
				{
					player_fire_shot(fire_angle, data);
					break;
				}
				case 1:
				{
					player_fire_shot(fire_angle - 0.2, data);
					player_fire_shot(fire_angle + 0.2, data);
					break;
				}
				case 2:
				{
					player_fire_shot(fire_angle, data);
					player_fire_shot(fire_angle - 0.2, data);
					player_fire_shot(fire_angle + 0.2, data);
					break;
				}
			}
			instance->player.var = 6;
			al_play_sample(instance->sample[SAMPLE_SHOOT], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		}
		instance->player.tick++;
	}
}

void player_render_shadow(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	if(instance->player.active)
	{
		t3f_draw_rotated_animation(instance->animation[ANIMATION_PLAYER], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), instance->player.tick, 16, 16, instance->player.x + 16 + 4, instance->player.y + 16 + 4, 0, instance->player.angle, 0);
	}
}

void player_render(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	if(instance->player.active)
	{
		t3f_draw_rotated_animation(instance->animation[ANIMATION_PLAYER], t3f_color_white, instance->player.tick, 16, 16, instance->player.x + 16, instance->player.y + 16, instance->player.z, instance->player.angle, 0);
	}
}
