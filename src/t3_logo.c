#include "t3f/t3f.h"
#include "t3f/sound.h"
#include "t3f/view.h"
#include "t3f/draw.h"

typedef struct
{
	
	float x, y, z;
	float vz;
	ALLEGRO_COLOR color;
	
} T3_LOGO_PIXEL;

static T3_LOGO_PIXEL t3_logo_pixel[1024];
static int t3_logo_pixels = 0;
static int t3_logo_pixel_list[1024];
static int t3_logo_pixel_list_size = 0;
static ALLEGRO_BITMAP * t3_logo_bitmap = NULL;
static ALLEGRO_BITMAP * t3_logo_pixel_parent_bitmap = NULL;
static ALLEGRO_BITMAP * t3_logo_pixel_bitmap = NULL;
static ALLEGRO_SAMPLE * t3_logo_sample = NULL;
static unsigned long t3_logo_tick = 0;
static unsigned long t3_logo_fade = 0;
bool t3_logo_done = false;

static ALLEGRO_COLOR alpha_color(ALLEGRO_COLOR color, float alpha)
{
	float r, g, b;
	
	al_unmap_rgb_f(color, &r, &g, &b);
	return al_map_rgba_f(r * alpha, g * alpha, b * alpha, alpha);
}

static int pixel_sorter(const void * item_1, const void * item_2)
{
	T3_LOGO_PIXEL * pixel_1 = (T3_LOGO_PIXEL *)item_1;
	T3_LOGO_PIXEL * pixel_2 = (T3_LOGO_PIXEL *)item_2;
	
	if(pixel_1->z < pixel_2->z)
	{
		return 1;
	}
	else if(pixel_2->z < pixel_1->z)
	{
		return -1;
	}
	return 0;
}

bool t3_logo_setup(const char * bfn, const char * sfn)
{
	int i, j, r;
	ALLEGRO_COLOR color;
	unsigned char cr, cg, cb, ca;
	
	t3_logo_bitmap = al_load_bitmap(bfn);
	if(!t3_logo_bitmap)
	{
		return false;
	}
	t3_logo_pixel_parent_bitmap = al_load_bitmap("data/graphics/intro_pixel.png");
	if(!t3_logo_pixel_parent_bitmap)
	{
		return false;
	}
	t3_logo_pixel_bitmap = al_create_sub_bitmap(t3_logo_pixel_parent_bitmap, 1, 1, 10, 10);
	if(!t3_logo_pixel_bitmap)
	{
		return false;
	}
	t3_logo_sample = al_load_sample(sfn);
	
	/* find all pixels */
	al_lock_bitmap(t3_logo_bitmap, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
	for(i = 0; i < 40; i++)
	{
		for(j = 0; j < 40; j++)
		{
			color = al_get_pixel(t3_logo_bitmap, j * 10 + 5, i * 10 + 5);
			al_unmap_rgba(color, &cr, &cg, &cb, &ca);
			if(ca == 255)
			{
				t3_logo_pixel[t3_logo_pixels].x = j * 10 + 120;
				t3_logo_pixel[t3_logo_pixels].y = i * 10 + 40;
				t3_logo_pixel[t3_logo_pixels].color = color;
				t3_logo_pixel_list[t3_logo_pixel_list_size] = t3_logo_pixels;
				t3_logo_pixel_list_size++;
				t3_logo_pixels++;
			}
		}
	}
	al_unlock_bitmap(t3_logo_bitmap);
	
	/* place pixels */
	for(i = 0; i < t3_logo_pixels; i++)
	{
		r = rand() % t3_logo_pixel_list_size;
		t3_logo_pixel[t3_logo_pixel_list[r]].vz = 5.0 + (i * 0.05);
		t3_logo_pixel[t3_logo_pixel_list[r]].z = 0.0;
		for(j = 0; j < 120 + i / 10; j++)
		{
			t3_logo_pixel[t3_logo_pixel_list[r]].z -= t3_logo_pixel[t3_logo_pixel_list[r]].vz;
		}
		
		for(j = r; j < t3_logo_pixel_list_size - 1; j++)
		{
			t3_logo_pixel_list[j] = t3_logo_pixel_list[j + 1];
		}
		t3_logo_pixel_list_size--;
	}
	qsort(t3_logo_pixel, t3_logo_pixels, sizeof(T3_LOGO_PIXEL), pixel_sorter);
	return true;
}

void t3_logo_finish(void)
{
	al_destroy_bitmap(t3_logo_bitmap);
	al_destroy_bitmap(t3_logo_pixel_bitmap);
	al_destroy_bitmap(t3_logo_pixel_parent_bitmap);
	if(t3_logo_sample)
	{
		al_destroy_sample(t3_logo_sample);
	}
}

static bool t3_logo_touched(void)
{
	int i, j;
	
	for(i = 0; i < T3F_MAX_TOUCHES; i++)
	{
		if(t3f_touch[i].released)
		{
			t3f_touch[i].released = false;
			return true;
		}
	}
	for(i = 0; i < al_get_num_joysticks(); i++)
	{
		for(j = 0; j < al_get_joystick_num_buttons(t3f_joystick[i]); j++)
		{
			if(t3f_joystick_state[i].button[j])
			{
				return true;
			}
		}
	}
	if(t3f_key[ALLEGRO_KEY_ENTER])
	{
		return true;
	}
	return false;
}

void t3_logo_logic(void)
{
	int i;
	
	for(i = 0; i < t3_logo_pixels; i++)
	{
		if(t3_logo_pixel[i].z < 0.0)
		{
			t3_logo_pixel[i].z += t3_logo_pixel[i].vz;
			if(t3_logo_pixel[i].z > 0.0)
			{
				t3_logo_pixel[i].z = 0.0;
			}
		}
	}
	
	t3_logo_tick++;
	t3_logo_fade++;
	if(t3_logo_tick == 180)
	{
		if(t3_logo_sample)
		{
			t3f_play_sample(t3_logo_sample, 1.0, 0.0, 1.0);
		}
	}
	if(t3_logo_fade >= 330)
	{
		t3_logo_done = true;
	}
	if(t3_logo_touched() && t3_logo_fade < 270)
	{
		t3_logo_fade = 270;
	}
}

void t3_logo_render(void)
{
	int i;
	ALLEGRO_COLOR tint_color;
	float alpha;
	
	t3f_select_view(t3f_default_view);
	al_hold_bitmap_drawing(true);
	al_clear_to_color(al_map_rgb(255, 255, 255));
	if(t3_logo_tick < 210)
	{
		for(i = 0; i < t3_logo_pixels; i++)
		{
			t3f_draw_bitmap(t3_logo_pixel_bitmap, alpha_color(t3_logo_pixel[i].color, 1.0 - fabs(t3_logo_pixel[i].z / 640.0)), t3_logo_pixel[i].x, t3_logo_pixel[i].y, t3_logo_pixel[i].z, 0);
//			al_draw_filled_rectangle(t3f_project_x(t3_logo_pixel[i].x, t3_logo_pixel[i].z), t3f_project_y(t3_logo_pixel[i].y, t3_logo_pixel[i].z), t3f_project_x(t3_logo_pixel[i].x + 10, t3_logo_pixel[i].z), t3f_project_y(t3_logo_pixel[i].y + 10, t3_logo_pixel[i].z), alpha_color(t3_logo_pixel[i].color, 1.0 - fabs(t3_logo_pixel[i].z / 640.0)));
		}
	}
	al_hold_bitmap_drawing(false);
	if(t3_logo_tick < 180)
	{
		tint_color = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);
	}
	else if(t3_logo_tick >= 180 && t3_logo_tick < 210)
	{
		alpha = (float)(t3_logo_tick - 180) / 30.0;
		tint_color = al_map_rgba_f(alpha, alpha, alpha, alpha);
	}
	else
	{
		tint_color = al_map_rgba_f(1.0, 1.0, 1.0, 1.0);
	}
	t3f_draw_bitmap(t3_logo_bitmap, tint_color, 120.0, 40.0, 0.0, 0);
	if(t3_logo_fade > 270)
	{
		alpha = (float)(t3_logo_fade - 270) / 60.0;
		al_draw_filled_rectangle(0, 0, 640, 480, al_map_rgba_f(0.0, 0.0, 0.0, alpha));
	}
}
