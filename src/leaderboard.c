#include "t3f/t3f.h"
#include "main.h"
#include "title.h"

void download_leaderboard(void)
{
	al_stop_timer(t3f_timer);
	if(leaderboard)
	{
		t3net_destroy_leaderboard(leaderboard);
	}
	leaderboard = t3net_get_leaderboard("http://www.t3-i.com/leaderboards/query_2.php", "devil", "1.1", game_mode_text[game_mode], "0", 10, 0);
	menu_fix_leaderboard_text();
	al_start_timer(t3f_timer);
}

void leaderboard_logic(void)
{
	state_ticks++;
	title_bg_logic();
	title_process_menu(menu[LEADERBOARD_MENU]);
}

void leaderboard_render(void)
{
	ALLEGRO_COLOR color;
	int i;
	int offset = 0;
	int yoffset = 200 - 68;
	int size = al_get_font_line_height(font[FONT_SMALL]);
	
	title_bg_render();
	for(i = 0; i < 10; i++)
	{
		if(i < leaderboard->entries)
		{
			if(game_mode == leaderboard_mode && i == leaderboard_place)
			{
				if(state_ticks % 4 < 2)
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
			al_draw_textf(font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 160 + 2, yoffset + i * size + 2, ALLEGRO_ALIGN_RIGHT, "%2d.", i + 1);
			al_draw_textf(font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 160 + 2, yoffset + i * size + 2, ALLEGRO_ALIGN_LEFT, " %s", leaderboard->entry[i]->name);
			al_draw_textf(font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 160 + 320 + 2, yoffset + i * size + 2, ALLEGRO_ALIGN_RIGHT, "%lu", (leaderboard->entry[i]->score - 'f' - 'l' - 'o' - 'g' - 'v') / 2);
			al_draw_textf(font[FONT_SMALL], color, 160 + offset, yoffset + i * size + offset, ALLEGRO_ALIGN_RIGHT, "%2d.", i + 1);
			al_draw_textf(font[FONT_SMALL], color, 160 + offset, yoffset + i * size + offset, ALLEGRO_ALIGN_LEFT, " %s", leaderboard->entry[i]->name);
			al_draw_textf(font[FONT_SMALL], color, 160 + 320 + offset, yoffset + i * size + offset, ALLEGRO_ALIGN_RIGHT, "%lu", (leaderboard->entry[i]->score - 'f' - 'l' - 'o' - 'g' - 'v') / 2);
		}
		else
		{
			al_draw_textf(font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 160 + 2, yoffset + i * size + 2, ALLEGRO_ALIGN_RIGHT, "%2d.", i + 1);
			al_draw_textf(font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 160, yoffset + i * size, ALLEGRO_ALIGN_RIGHT, "%2d.", i + 1);
		}
	}
	t3f_render_gui(menu[LEADERBOARD_MENU]);
	render_mouse();
}
