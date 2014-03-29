#include <math.h>
#include "t3f/t3f.h"
#include "t3f/rng.h"
#include "main.h"
#include "enemy.h"
#include "particle.h"

static bool fire = false;
static float fire_angle;

void player_fire_shot(float angle)
{
	int i;
	
	for(i = 0; i < GAME_MAX_PLAYER_SHOTS; i++)
	{
		if(!player_shot[i].active)
		{
			player_shot[i].x = player.x + 8;
			player_shot[i].y = player.y + 8;
			player_shot[i].angle = angle;
			player_shot[i].vx = cos(angle) * 12.0;
			player_shot[i].vy = sin(angle) * 12.0;
			player_shot[i].active = true;
			generate_particle(PARTICLE_TYPE_PLAYER_SHOT, player_shot[i].x + 4, player_shot[i].y + 4, angle + t3f_drand(&rng_state) / 2.0 - 0.25, 1.0 + t3f_drand(&rng_state) / 2.0, 10 + rand() % 10);
			generate_particle(PARTICLE_TYPE_PLAYER_SHOT, player_shot[i].x + 4, player_shot[i].y + 4, angle + t3f_drand(&rng_state) / 2.0 - 0.25, 1.0 + t3f_drand(&rng_state) / 2.0, 10 + rand() % 10);
			break;
		}
	}
}

void player_lose_life(void)
{
	lives--;
	al_play_sample(sample[SAMPLE_DIE], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	generate_death_particles(PARTICLE_TYPE_PLAYER_FLESH, player.x, player.y);
	if(lives <= 0)
	{
		game_state = GAME_STATE_OVER;
		die_timer = 120;
		player.active = false;
		al_play_sample(sample[SAMPLE_GAME_OVER], 1.0, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
	else
	{
//		clear_enemies();
		die_state = game_state;
		game_state = GAME_STATE_DIE;
		die_timer = 120;
		player.active = false;
	}
	death_time = 180;
}

void player_control_normal(void)
{
	t3f_read_controller(controller);
	t3f_update_controller(controller);
	/* move player */
	player.vx = 0;
	player.vy = 0;
	if(controller->state[CONTROLLER_LEFT].held)
	{
		player.vx = -player.speed;
		player.vy = 0;
		player.angle = ALLEGRO_PI;
	}
	if(controller->state[CONTROLLER_UP].held)
	{
		player.vx = 0;
		player.vy = -player.speed;
		player.angle = ALLEGRO_PI * 1.5;
	}
	if(controller->state[CONTROLLER_RIGHT].held)
	{
		player.vx = player.speed;
		player.vy = 0;
		player.angle = 0;
	}
	if(controller->state[CONTROLLER_DOWN].held)
	{
		player.vx = 0;
		player.vy = player.speed;
		player.angle = ALLEGRO_PI * 0.5;
	}
	if(controller->state[CONTROLLER_LEFT].held && controller->state[CONTROLLER_UP].held)
	{
		player.vx = cos(1.25 * ALLEGRO_PI) * player.speed;
		player.vy = sin(1.25 * ALLEGRO_PI) * player.speed;
		player.angle = ALLEGRO_PI * 1.25;
	}
	if(controller->state[CONTROLLER_RIGHT].held && controller->state[CONTROLLER_UP].held)
	{
		player.vx = cos(1.75 * ALLEGRO_PI) * player.speed;
		player.vy = sin(1.75 * ALLEGRO_PI) * player.speed;
		player.angle = ALLEGRO_PI * 1.75;
	}
	if(controller->state[CONTROLLER_LEFT].held && controller->state[CONTROLLER_DOWN].held)
	{
		player.vx = cos(0.75 * ALLEGRO_PI) * player.speed;
		player.vy = sin(0.75 * ALLEGRO_PI) * player.speed;
		player.angle = ALLEGRO_PI * 0.75;
	}
	if(controller->state[CONTROLLER_RIGHT].held && controller->state[CONTROLLER_DOWN].held)
	{
		player.vx = cos(0.25 * ALLEGRO_PI) * player.speed;
		player.vy = sin(0.25 * ALLEGRO_PI) * player.speed;
		player.angle = ALLEGRO_PI * 0.25;
	}
	if(player.var > 0)
	{
		player.var--;
	}
	else
	{
		if(controller->state[CONTROLLER_FIRE_LEFT].held)
		{
			fire = true;
			fire_angle = ALLEGRO_PI;
		}
		if(controller->state[CONTROLLER_FIRE_UP].held)
		{
			fire = true;
			fire_angle = ALLEGRO_PI * 1.5;
		}
		if(controller->state[CONTROLLER_FIRE_RIGHT].held)
		{
			fire = true;
			fire_angle = 0;
		}
		if(controller->state[CONTROLLER_FIRE_DOWN].held)
		{
			fire = true;
			fire_angle = ALLEGRO_PI * 0.5;
		}
		if(controller->state[CONTROLLER_FIRE_LEFT].held && controller->state[CONTROLLER_FIRE_UP].held)
		{
			fire = true;
			fire_angle = ALLEGRO_PI * 1.25;
		}
		if(controller->state[CONTROLLER_FIRE_RIGHT].held && controller->state[CONTROLLER_FIRE_UP].held)
		{
			fire = true;
			fire_angle = ALLEGRO_PI * 1.75;
		}
		if(controller->state[CONTROLLER_FIRE_LEFT].held && controller->state[CONTROLLER_FIRE_DOWN].held)
		{
			fire = true;
			fire_angle = ALLEGRO_PI * 0.75;
		}
		if(controller->state[CONTROLLER_FIRE_RIGHT].held && controller->state[CONTROLLER_FIRE_DOWN].held)
		{
			fire = true;
			fire_angle = ALLEGRO_PI * 0.25;
		}
	}
}

void player_control_mouse(void)
{
	t3f_read_controller(controller);
	t3f_update_controller(controller);
	/* move player */
	player.vx = 0;
	player.vy = 0;
	if(controller->state[CONTROLLER_LEFT].held)
	{
		player.vx = -player.speed;
		player.vy = 0;
		player.angle = ALLEGRO_PI;
	}
	if(controller->state[CONTROLLER_UP].held)
	{
		player.vx = 0;
		player.vy = -player.speed;
		player.angle = ALLEGRO_PI * 1.5;
	}
	if(controller->state[CONTROLLER_RIGHT].held)
	{
		player.vx = player.speed;
		player.vy = 0;
		player.angle = 0;
	}
	if(controller->state[CONTROLLER_DOWN].held)
	{
		player.vx = 0;
		player.vy = player.speed;
		player.angle = ALLEGRO_PI * 0.5;
	}
	if(controller->state[CONTROLLER_LEFT].held && controller->state[CONTROLLER_UP].held)
	{
		player.vx = cos(1.25 * ALLEGRO_PI) * player.speed;
		player.vy = sin(1.25 * ALLEGRO_PI) * player.speed;
		player.angle = ALLEGRO_PI * 1.25;
	}
	if(controller->state[CONTROLLER_RIGHT].held && controller->state[CONTROLLER_UP].held)
	{
		player.vx = cos(1.75 * ALLEGRO_PI) * player.speed;
		player.vy = sin(1.75 * ALLEGRO_PI) * player.speed;
		player.angle = ALLEGRO_PI * 1.75;
	}
	if(controller->state[CONTROLLER_LEFT].held && controller->state[CONTROLLER_DOWN].held)
	{
		player.vx = cos(0.75 * ALLEGRO_PI) * player.speed;
		player.vy = sin(0.75 * ALLEGRO_PI) * player.speed;
		player.angle = ALLEGRO_PI * 0.75;
	}
	if(controller->state[CONTROLLER_RIGHT].held && controller->state[CONTROLLER_DOWN].held)
	{
		player.vx = cos(0.25 * ALLEGRO_PI) * player.speed;
		player.vy = sin(0.25 * ALLEGRO_PI) * player.speed;
		player.angle = ALLEGRO_PI * 0.25;
	}
	if(player.var > 0)
	{
		player.var--;
	}
	else
	{
		fire_angle = atan2(player.y - t3f_mouse_y, player.x - t3f_mouse_x) + ALLEGRO_PI;
		if(t3f_mouse_button[0])
		{
			fire = true;
		}
	}
}

void player_control_analog(void)
{
	float strength;
	
	t3f_read_controller(controller);
	
	/* handle player movement */
	strength = t3f_distance(0.0, 0.0, controller->state[2].pos, controller->state[0].pos) * 1.5;
	if(strength > 1.0)
	{
		strength = 1.0;
	}
	if(strength > 0.1)
	{
		player.angle = atan2(controller->state[0].pos, controller->state[2].pos);
	}
	else
	{
		strength = 0.0;
	}
	player.vx = cos(player.angle) * player.speed * strength;
	player.vy = sin(player.angle) * player.speed * strength;

	fire_angle = atan2(controller->state[4].pos, controller->state[6].pos);
	if(player.var > 0)
	{
		player.var--;
	}
	else
	{
		if(controller->state[4].pos <= 0.2 && controller->state[4].pos >= -0.2 && controller->state[6].pos <= 0.2 && controller->state[6].pos >= -0.2)
		{
		}
		else
		{
			fire = true;
		}
	}
}

void player_control_touch(void)
{
	float strength;
	int touch_used[2] = {-1, -1};
	int i;
	
	/* see which touches are already in use */
	for(i = 0; i < 2; i++)
	{
		if(touch_stick[i].active)
		{
			touch_used[i] = touch_stick[i].touch_id;
		}
	}

	/* player movement */
	if(!touch_stick[0].active)
	{
		for(i = 0; i < T3F_MAX_TOUCHES; i++)
		{
			if(i != touch_used[0] && i != touch_used[1] && t3f_touch[i].active && t3f_touch[i].x < 320.0)
			{
				touch_stick[0].touch_id = i;
				touch_stick[0].pin_x = t3f_touch[i].x;
				touch_stick[0].pin_y = t3f_touch[i].y;
				touch_stick[0].pos_x = t3f_touch[i].x;
				touch_stick[0].pos_y = t3f_touch[i].y;
				touch_stick[0].active = true;
				break;
			}
		}
		player.vx = 0;
		player.vy = 0;
	}
	else
	{
		if(!t3f_touch[touch_stick[0].touch_id].active)
		{
			touch_stick[0].active = false;
		}
		else
		{
			touch_stick[0].pos_x = t3f_touch[touch_stick[0].touch_id].x;
			touch_stick[0].pos_y = t3f_touch[touch_stick[0].touch_id].y;
			/* handle player movement */
			strength = (t3f_distance(touch_stick[0].pin_x, touch_stick[0].pin_y, touch_stick[0].pos_x, touch_stick[0].pos_y) / touch_size) * 1.5;
			if(strength > 1.0)
			{
				strength = 1.0;
			}
			if(strength > 0.1)
			{
				player.angle = atan2(touch_stick[0].pos_y - touch_stick[0].pin_y, touch_stick[0].pos_x - touch_stick[0].pin_x);
			}
			else
			{
				strength = 0.0;
			}
			player.vx = cos(player.angle) * player.speed * strength;
			player.vy = sin(player.angle) * player.speed * strength;
		}
	}

	/* player attack */
	if(!touch_stick[1].active)
	{
		for(i = 0; i < T3F_MAX_TOUCHES; i++)
		{
			if(i != touch_used[0] && i != touch_used[1] && t3f_touch[i].active && t3f_touch[i].x >= 320.0)
			{
				touch_stick[1].touch_id = i;
				touch_stick[1].pin_x = t3f_touch[i].x;
				touch_stick[1].pin_y = t3f_touch[i].y;
				touch_stick[1].pos_x = t3f_touch[i].x;
				touch_stick[1].pos_y = t3f_touch[i].y;
				touch_stick[1].active = true;
				break;
			}
		}
		player.vx = 0;
		player.vy = 0;
	}
	else
	{
		if(!t3f_touch[touch_stick[1].touch_id].active)
		{
			touch_stick[1].active = false;
		}
		else
		{
			touch_stick[1].pos_x = t3f_touch[touch_stick[0].touch_id].x;
			touch_stick[1].pos_y = t3f_touch[touch_stick[0].touch_id].y;
			strength = (t3f_distance(touch_stick[1].pin_x, touch_stick[1].pin_y, touch_stick[1].pos_x, touch_stick[1].pos_y) / touch_size) * 1.5;
			if(strength > 1.0)
			{
				strength = 1.0;
			}
			if(strength > 0.1)
			{
				fire_angle = atan2(touch_stick[1].pos_y - touch_stick[1].pin_y, touch_stick[1].pos_x - touch_stick[1].pin_x);
			}
			else
			{
				strength = 0.0;
			}
			if(player.var > 0)
			{
				player.var--;
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
}

void player_logic(void)
{
	fire = false;
	
	if(player.active)
	{
		switch(controller_type)
		{
			case CONTROLLER_TYPE_NORMAL:
			{
				player_control_normal();
				break;
			}
			case CONTROLLER_TYPE_MOUSE:
			{
				player_control_mouse();
				break;
			}
			case CONTROLLER_TYPE_ANALOG:
			{
				player_control_analog();
				break;
			}
			case CONTROLLER_TYPE_TOUCH:
			{
				player_control_touch();
				break;
			}
		}
		player.x += player.vx;
		if(player.x < 0)
		{
			player.x = 0;
		}
		if(player.x > 640 - 32)
		{
			player.x = 640 - 32;
		}
		player.y += player.vy;
		if(player.y < 0)
		{
			player.y = 0;
		}
		if(player.y > 480 - 32)
		{
			player.y = 480 - 32;
		}
		t3f_move_collision_object_xy(player.object, player.x, player.y);
		
		/* fire */
		if(fire)
		{
			switch(weapon)
			{
				case 0:
				{
					player_fire_shot(fire_angle);
					break;
				}
				case 1:
				{
					player_fire_shot(fire_angle - 0.2);
					player_fire_shot(fire_angle + 0.2);
					break;
				}
				case 2:
				{
					player_fire_shot(fire_angle);
					player_fire_shot(fire_angle - 0.2);
					player_fire_shot(fire_angle + 0.2);
					break;
				}
			}
			player.var = 6;
			al_play_sample(sample[SAMPLE_SHOOT], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		}
		player.tick++;
	}
}

void player_render_shadow(void)
{
	if(player.active)
	{
		t3f_draw_rotated_animation(animation[ANIMATION_PLAYER], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), player.tick, 16, 16, player.x + 16 + 4, player.y + 16 + 4, 0, player.angle, 0);
	}
}

void player_render(void)
{
	if(player.active)
	{
		t3f_draw_rotated_animation(animation[ANIMATION_PLAYER], t3f_color_white, player.tick, 16, 16, player.x + 16, player.y + 16, player.z, player.angle, 0);
	}
}
