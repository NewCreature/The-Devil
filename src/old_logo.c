#include "t3f/t3f.h"
#include "t3f/sound.h"
#include "t3f/music.h"
#include "main.h"
#include "title.h"

int logo_timer = 0;

void old_logo_logic(void)
{
	if(t3f_read_key(0) && logo_timer < 240)
	{
		logo_timer = 120;
	}
	logo_timer++;
	if(logo_timer > 270)
	{
		title_init();
	}
}

void old_logo_render(void)
{
	ALLEGRO_COLOR color;
	float alpha;
	
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	if(logo_timer < 30)
	{
		alpha = (float)logo_timer / 30.0;
		color = al_map_rgba_f(alpha, alpha, alpha, alpha);
	}
	else if(logo_timer > 240)
	{
		alpha = 1.0 - (float)(logo_timer - 240) / 30.0;
		color = al_map_rgba_f(alpha, alpha, alpha, alpha);
	}
	else
	{
		color = al_map_rgba_f(1.0, 1.0, 1.0, 1.0);
	}
	al_hold_bitmap_drawing(false);
	t3f_draw_animation(animation[ANIMATION_LOGO], color, logo_timer, 320 - al_get_bitmap_width(animation[ANIMATION_LOGO]->bitmap[0]) / 2, 240 - al_get_bitmap_height(animation[ANIMATION_LOGO]->bitmap[0]) / 2, 0, 0);
}

typedef struct
{
	
	float x, y, z;
	float vz;
	ALLEGRO_COLOR color;
	
} INTRO_PIXEL;

static INTRO_PIXEL intro_pixel[1024];
static int intro_pixels = 0;
static int intro_pixel_list[1024];
static int intro_pixel_list_size = 0;

void logo_setup(void)
{
	int i, j, r;
	ALLEGRO_COLOR color;
	unsigned char cr, cg, cb, ca;
	
	/* find all pixels */
	al_lock_bitmap(animation[ANIMATION_LOGO]->bitmap[0], al_get_bitmap_format(animation[ANIMATION_LOGO]->bitmap[0]), ALLEGRO_LOCK_READONLY);
	for(i = 0; i < 40; i++)
	{
		for(j = 0; j < 40; j++)
		{
			color = al_get_pixel(animation[ANIMATION_LOGO]->bitmap[0], j * 10 + 5, i * 10 + 5);
			al_unmap_rgba(color, &cr, &cg, &cb, &ca);
			if(ca == 255)
			{
				intro_pixel[intro_pixels].x = j * 10 + 120;
				intro_pixel[intro_pixels].y = i * 10 + 40;
				intro_pixel[intro_pixels].color = color;
				intro_pixel_list[intro_pixel_list_size] = intro_pixels;
				intro_pixel_list_size++;
				intro_pixels++;
			}
		}
	}
	al_unlock_bitmap(animation[ANIMATION_LOGO]->bitmap[0]);
	
	/* place pixels */
	for(i = 0; i < intro_pixels; i++)
	{
		r = rand() % intro_pixel_list_size;
		intro_pixel[intro_pixel_list[r]].vz = 5.0 + (i * 0.05);
		intro_pixel[intro_pixel_list[r]].z = 0.0;
		for(j = 0; j < 120 + i / 10; j++)
		{
			intro_pixel[intro_pixel_list[r]].z -= intro_pixel[intro_pixel_list[r]].vz;
		}
		
		for(j = r; j < intro_pixel_list_size - 1; j++)
		{
			intro_pixel_list[j] = intro_pixel_list[j + 1];
		}
		intro_pixel_list_size--;
	}
}

void logo_logic(void)
{
	int i;
	
	for(i = 0; i < intro_pixels; i++)
	{
		if(intro_pixel[i].z < 0.0)
		{
			intro_pixel[i].z += intro_pixel[i].vz;
			if(intro_pixel[i].z > 0.0)
			{
				intro_pixel[i].z = 0.0;
			}
		}
	}
	
	state_ticks++;
	if(state_ticks == 180)
	{
		t3f_play_sample(sample[SAMPLE_LOGO], 1.0, 0.0, 1.0);
	}
	if(state_ticks >= 330)
	{
		t3f_play_music("data/music/title.xm");
		title_init();
		state = STATE_TITLE_IN;
		state_ticks = 0;
	}
}

void logo_render(void)
{
	int i;
	ALLEGRO_COLOR tint_color;
	float alpha;
	
	al_clear_to_color(al_map_rgb(255, 255, 255));
	al_hold_bitmap_drawing(false);
	if(state_ticks < 210)
	{
		for(i = 0; i < intro_pixels; i++)
		{
			al_draw_filled_rectangle(t3f_project_x(intro_pixel[i].x, intro_pixel[i].z), t3f_project_y(intro_pixel[i].y, intro_pixel[i].z), t3f_project_x(intro_pixel[i].x + 10, intro_pixel[i].z), t3f_project_y(intro_pixel[i].y + 10, intro_pixel[i].z), intro_pixel[i].color);
		}
	}
	if(state_ticks < 180)
	{
		tint_color = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
	}
	else if(state_ticks >= 180 && state_ticks < 210)
	{
		alpha = (float)(state_ticks - 180) / 30.0;
		tint_color = al_map_rgba_f(alpha, alpha, alpha, alpha);
	}
	else
	{
		tint_color = al_map_rgba_f(1.0, 1.0, 1.0, 1.0);
	}
	t3f_draw_animation(animation[ANIMATION_LOGO], tint_color, state_ticks, 120.0, 40.0, 0.0, 0);
	if(state_ticks > 270)
	{
		al_draw_filled_rectangle(0, 0, 640, 480, al_map_rgba_f(0.0, 0.0, 0.0, (float)(state_ticks - 270) / 60.0));
	}
}
