#include "t3f/t3f.h"
#include "instance.h"
#include "main.h"
#include "title.h"

void download_leaderboard(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	al_stop_timer(t3f_timer);
	if(instance->leaderboard)
	{
		t3net_destroy_leaderboard(instance->leaderboard);
	}
	instance->leaderboard = t3net_get_leaderboard("http://www.t3-i.com/t3net2/leaderboards/query.php", "devil", "1.1", instance->game_mode_text[instance->game_mode], "0", 10, 0);
	menu_fix_leaderboard_text(data);
	al_start_timer(t3f_timer);
}

void leaderboard_logic(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->state_ticks++;
	title_bg_logic(data);
	if(t3f_key_pressed(ALLEGRO_KEY_ESCAPE) || t3f_key_pressed(ALLEGRO_KEY_BACK))
	{
		t3f_use_key_press(ALLEGRO_KEY_ESCAPE);
		t3f_use_key_press(ALLEGRO_KEY_BACK);
		instance->state = STATE_TITLE;
		select_menu(TITLE_MENU_MAIN, data);
	}
	title_process_menu(instance->menu[LEADERBOARD_MENU], data);
}

void leaderboard_render(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	ALLEGRO_COLOR color;
	int i;
	int offset = 0;
	int yoffset = 200 - 68;
	int size = t3f_get_font_line_height(instance->font[FONT_SMALL]);

	title_bg_render(instance->bitmap[0]);
	for(i = 0; i < 10; i++)
	{
		if(i < instance->leaderboard->entries)
		{
			if(instance->game_mode == instance->leaderboard_mode && i == instance->leaderboard_place)
			{
				if(instance->state_ticks % 4 < 2)
				{
					color = al_map_rgba(255, 255, 0, 255);
				}
				else
				{
					color = al_map_rgba_f(1.0, 1.0, 1.0, 1.0);
				}
				offset = -2;
			}
			else
			{
				color = al_map_rgba_f(1.0, 1.0, 1.0, 1.0);
				offset = 0;
			}
			t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 160 + 2, yoffset + i * size + 2, 0, ALLEGRO_ALIGN_RIGHT, "%2d.", i + 1);
			t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 160 + 2, yoffset + i * size + 2, 0, ALLEGRO_ALIGN_LEFT, " %s", instance->leaderboard->entry[i]->name);
			t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 160 + 320 + 2, yoffset + i * size + 2, 0, ALLEGRO_ALIGN_RIGHT, "%lu", (instance->leaderboard->entry[i]->score - 'f' - 'l' - 'o' - 'g' - 'v') / 2);
			t3f_draw_textf(instance->font[FONT_SMALL], color, 160 + offset, yoffset + i * size + offset, 0, ALLEGRO_ALIGN_RIGHT, "%2d.", i + 1);
			t3f_draw_textf(instance->font[FONT_SMALL], color, 160 + offset, yoffset + i * size + offset, 0, ALLEGRO_ALIGN_LEFT, " %s", instance->leaderboard->entry[i]->name);
			t3f_draw_textf(instance->font[FONT_SMALL], color, 160 + 320 + offset, yoffset + i * size + offset, 0, ALLEGRO_ALIGN_RIGHT, "%lu", (instance->leaderboard->entry[i]->score - 'f' - 'l' - 'o' - 'g' - 'v') / 2);
		}
		else
		{
			t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 160 + 2, yoffset + i * size + 2, 0, ALLEGRO_ALIGN_RIGHT, "%2d.", i + 1);
			t3f_draw_textf(instance->font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 160, yoffset + i * size, 0, ALLEGRO_ALIGN_RIGHT, "%2d.", i + 1);
		}
	}
	t3f_render_gui(instance->menu[LEADERBOARD_MENU], 0);
	render_mouse(data);
}
