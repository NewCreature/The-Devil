#include "t3f/t3f.h"
#include "t3f/animation.h"
#include <allegro5/allegro_native_dialog.h>
#include "cinema.h"

#define CE_VIEW_ANIMATIONS 0
#define CE_VIEW_FRAMES     1

ALLEGRO_FONT * font[8] = {NULL};

char * ce_view_name[3] = 
{
	"Animations",
	"Fonts",
	"Frames"
};

int ce_view = CE_VIEW_ANIMATIONS;
int ce_current_animation = 0;
int ce_current_frame = 0;
int ce_current_entity = 0;
int ce_tick = 0;
bool click = false;

ALLEGRO_FILECHOOSER * ce_file_select = NULL;
const char * ce_file_path = NULL;
CINEMA * ce_cinema = NULL;
T3F_FONT * ce_font = NULL;
char ce_string[256] = {0};
int ce_string_pos = 0;
int ce_string_mode = 0;

void ce_delete_entity(int frame, int entity)
{
	int i;
	
	for(i = entity; i < ce_cinema->frame[frame].entities - 1; i++)
	{
		memcpy(&ce_cinema->frame[frame].entity[i], &ce_cinema->frame[frame].entity[i + 1], sizeof(CINEMA_ENTITY));
	}
	ce_cinema->frame[frame].entities--;
}

void ce_fix_frame_text(int frame, int entity)
{
	int i;
	
	for(i = entity + 1; i < ce_cinema->frame[frame].entities; i++)
	{
		if(ce_cinema->frame[frame].entity[i].type == CINEMA_ENTITY_TEXT)
		{
			strcat(ce_cinema->frame[frame].entity[entity].data, " ");
			strcat(ce_cinema->frame[frame].entity[entity].data, ce_cinema->frame[frame].entity[i].data);
		}
	}
	for(i = ce_cinema->frame[frame].entities - 1; i > entity; i--)
	{
		if(ce_cinema->frame[frame].entity[i].type == CINEMA_ENTITY_TEXT)
		{
			ce_delete_entity(frame, i);
		}
	}
	if(ce_current_entity >= ce_cinema->frame[ce_current_frame].entities)
	{
		ce_current_entity = ce_cinema->frame[ce_current_frame].entities - 1;
	}
}

void ce_fix_cinema_text(void)
{
	int i, j;
	
	for(i = 0; i < ce_cinema->frames; i++)
	{
		for(j = 0; j < ce_cinema->frame[i].entities; j++)
		{
			if(ce_cinema->frame[i].entity[j].type == CINEMA_ENTITY_TEXT)
			{
				ce_fix_frame_text(i, j);
				printf("fixed text: %s\n", ce_cinema->frame[i].entity[j].data);
				break;
			}
		}
	}
}

void logic(void)
{
	char input;
	int ox, oy, oz;
	int i, j;
	
	if(t3f_key[ALLEGRO_KEY_ESCAPE])
	{
		t3f_exit();
	}
	if(t3f_key[ALLEGRO_KEY_F2])
	{
		ce_file_select = al_create_native_file_dialog(NULL, "Select File", "*.*;*.cin", ALLEGRO_FILECHOOSER_SAVE);
		al_show_native_file_dialog(t3f_display, ce_file_select);
		if(al_get_native_file_dialog_count(ce_file_select))
		{
			ce_file_path = al_get_native_file_dialog_path(ce_file_select, 0);
			if(ce_file_path)
			{
				save_cinema(ce_cinema, ce_file_path);
			}
		}
		al_destroy_native_file_dialog(ce_file_select);
		t3f_key[ALLEGRO_KEY_F2] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_F3])
	{
		ce_file_select = al_create_native_file_dialog(NULL, "Select File", "*.*;*.cin", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
		al_show_native_file_dialog(t3f_display, ce_file_select);
		if(al_get_native_file_dialog_count(ce_file_select))
		{
			ce_file_path = al_get_native_file_dialog_path(ce_file_select, 0);
			if(ce_file_path)
			{
				ce_cinema = load_cinema(ce_file_path, CINEMA_LOAD_FLAG_EDITOR);
			}
		}
		al_destroy_native_file_dialog(ce_file_select);
		if(ce_cinema)
		{
			ce_fix_cinema_text();
		}
		t3f_key[ALLEGRO_KEY_F3] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_TAB])
	{
		ce_view++;
		if(ce_view > 1)
		{
			ce_view = 0;
		}
		t3f_key[ALLEGRO_KEY_TAB] = 0;
	}
	input = t3f_read_key(0);
	if(input)
	{
		if(input == '\b')
		{
			if(ce_string_pos > 0)
			{
				ce_string_pos--;
				ce_string[ce_string_pos] = '\0';
			}
		}
		else if(input >= 32 && input < 127)
		{
			ce_string[ce_string_pos] = input;
			ce_string_pos++;
			ce_string[ce_string_pos] = '\0';
		}
	}
	switch(ce_view)
	{
		case CE_VIEW_ANIMATIONS:
		{
			if(t3f_key[ALLEGRO_KEY_INSERT])
			{
				ce_file_select = al_create_native_file_dialog(NULL, "Select File", "*.*;*.ani", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
				al_show_native_file_dialog(t3f_display, ce_file_select);
				if(al_get_native_file_dialog_count(ce_file_select))
				{
					ce_file_path = al_get_native_file_dialog_path(ce_file_select, 0);
					if(ce_file_path)
					{
						add_cinema_animation(ce_cinema, t3f_load_animation_from_bitmap(ce_file_path));
					}
				}
				al_destroy_native_file_dialog(ce_file_select);
				t3f_key[ALLEGRO_KEY_INSERT] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_ENTER])
			{
				ce_file_select = al_create_native_file_dialog(NULL, "Select File", "*.*;*.png;*.bmp;*.pcx;*.tga", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
				al_show_native_file_dialog(t3f_display, ce_file_select);
				if(al_get_native_file_dialog_count(ce_file_select))
				{
					ce_file_path = al_get_native_file_dialog_path(ce_file_select, 0);
					if(ce_file_path)
					{
						t3f_destroy_animation(ce_cinema->animation[ce_current_animation]);
						ce_cinema->animation[ce_current_animation] = t3f_load_animation_from_bitmap(ce_file_path);
					}
				}
				al_destroy_native_file_dialog(ce_file_select);
				t3f_key[ALLEGRO_KEY_ENTER] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_LEFT] && ce_current_animation > 0)
			{
				ce_current_animation--;
				t3f_key[ALLEGRO_KEY_LEFT] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_RIGHT] && ce_current_animation < ce_cinema->animations - 1)
			{
				ce_current_animation++;
				t3f_key[ALLEGRO_KEY_RIGHT] = 0;
			}
			break;
		}
		case CE_VIEW_FRAMES:
		{
			if(t3f_key[ALLEGRO_KEY_INSERT])
			{
				if(t3f_key[ALLEGRO_KEY_LCTRL])
				{
					if(t3f_key[ALLEGRO_KEY_LSHIFT])
					{
						ce_cinema->frame[ce_current_frame].entity[ce_cinema->frame[ce_current_frame].entities].type = CINEMA_ENTITY_TEXT;
						strcpy(ce_cinema->frame[ce_current_frame].entity[ce_cinema->frame[ce_current_frame].entities].data, ce_string);
						ce_cinema->frame[ce_current_frame].entities++;
					}
					else
					{
						ce_cinema->frame[ce_current_frame].entity[ce_cinema->frame[ce_current_frame].entities].type = CINEMA_ENTITY_ANIMATION;
						ce_cinema->frame[ce_current_frame].entity[ce_cinema->frame[ce_current_frame].entities].index = ce_current_animation;
						ce_cinema->frame[ce_current_frame].entities++;
					}
				}
				else
				{
					ce_cinema->frame[ce_cinema->frames].entities = 0;
					ce_cinema->frames++;
					memcpy(&ce_cinema->frame[ce_cinema->frames - 1], &ce_cinema->frame[ce_current_frame], sizeof(CINEMA_FRAME));
					ce_current_frame = ce_cinema->frames - 1;
				}
				t3f_key[ALLEGRO_KEY_INSERT] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_DELETE])
			{
				int i;
				
				if(t3f_key[ALLEGRO_KEY_LCTRL])
				{
					ce_delete_entity(ce_current_frame, ce_current_entity);
					if(ce_current_entity >= ce_cinema->frame[ce_current_frame].entities)
					{
						ce_current_entity = ce_cinema->frame[ce_current_frame].entities - 1;
					}
				}
				else
				{
					for(i = ce_current_frame; i < ce_cinema->frames - 1; i++)
					{
						memcpy(&ce_cinema->frame[i], &ce_cinema->frame[i + 1], sizeof(CINEMA_FRAME));
						if(ce_current_frame >= ce_cinema->frames)
						{
							ce_current_frame = ce_cinema->frames - 1;
						}
					}
					ce_cinema->frames--;
				}
				t3f_key[ALLEGRO_KEY_DELETE] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_LEFT] && ce_current_frame > 0)
			{
				ce_current_frame--;
				ce_current_entity = 0;
				ce_string_mode = 0;
				t3f_key[ALLEGRO_KEY_LEFT] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_RIGHT] && ce_current_frame < ce_cinema->frames - 1)
			{
				ce_current_frame++;
				ce_current_entity = 0;
				ce_string_mode = 0;
				t3f_key[ALLEGRO_KEY_RIGHT] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_UP] && ce_current_entity > 0)
			{
				ce_current_entity--;
				ce_string_mode = 0;
				t3f_key[ALLEGRO_KEY_UP] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_DOWN] && ce_current_entity < ce_cinema->frame[ce_current_frame].entities - 1)
			{
				ce_current_entity++;
				ce_string_mode = 0;
				t3f_key[ALLEGRO_KEY_DOWN] = 0;
			}
			if(ce_cinema->frame[ce_current_frame].entity[ce_current_entity].type == CINEMA_ENTITY_ANIMATION)
			{
				if(t3f_key[ALLEGRO_KEY_PGUP] && ce_cinema->frame[ce_current_frame].entity[ce_current_entity].index > 0)
				{
					ce_cinema->frame[ce_current_frame].entity[ce_current_entity].index--;
					t3f_key[ALLEGRO_KEY_PGUP] = 0;
				}
				if(t3f_key[ALLEGRO_KEY_PGDN] && ce_cinema->frame[ce_current_frame].entity[ce_current_entity].index < ce_cinema->animations - 1)
				{
					ce_cinema->frame[ce_current_frame].entity[ce_current_entity].index++;
					t3f_key[ALLEGRO_KEY_PGDN] = 0;
				}
			}
			if(t3f_key[ALLEGRO_KEY_ENTER])
			{
				if(ce_string_mode == 0)
				{
					ce_string_mode = 1;
				}
				else
				{
					strcpy(ce_cinema->frame[ce_current_frame].entity[ce_current_entity].data, ce_string);
					ce_string_mode = 0;
				}
			}
			if(t3f_mouse_button[0])
			{
				ox = t3f_mouse_x - ce_cinema->frame[ce_current_frame].entity[ce_current_entity].x;
				oy = t3f_mouse_y - ce_cinema->frame[ce_current_frame].entity[ce_current_entity].y;
				if(t3f_key[ALLEGRO_KEY_LSHIFT])
				{
					for(i = 0; i < ce_cinema->frames; i++)
					{
						for(j = 0; j < ce_cinema->frame[i].entities; j++)
						{
							if(ce_cinema->frame[i].entity[j].type == ce_cinema->frame[ce_current_frame].entity[ce_current_entity].type && ce_cinema->frame[i].entity[j].index == ce_cinema->frame[ce_current_frame].entity[ce_current_entity].index)
							{
								ce_cinema->frame[i].entity[j].x += ox;
								ce_cinema->frame[i].entity[j].y += oy;
							}
						}
					}
				}
				else
				{
					ce_cinema->frame[ce_current_frame].entity[ce_current_entity].x += ox;
					ce_cinema->frame[ce_current_frame].entity[ce_current_entity].y += oy;
				}
			}
			if(t3f_mouse_button[1])
			{
				oz = t3f_mouse_y - ce_cinema->frame[ce_current_frame].entity[ce_current_entity].y - ce_cinema->frame[ce_current_frame].entity[ce_current_entity].z;
				if(t3f_key[ALLEGRO_KEY_LSHIFT])
				{
					for(i = 0; i < ce_cinema->frames; i++)
					{
						for(j = 0; j < ce_cinema->frame[i].entities; j++)
						{
							if(ce_cinema->frame[i].entity[j].type == ce_cinema->frame[ce_current_frame].entity[ce_current_entity].type && ce_cinema->frame[i].entity[j].index == ce_cinema->frame[ce_current_frame].entity[ce_current_entity].index)
							{
								ce_cinema->frame[i].entity[j].z += oz;
							}
						}
					}
				}
				else
				{
					ce_cinema->frame[ce_current_frame].entity[ce_current_entity].z += oz;
				}
			}
			ce_cinema->position = ce_current_frame;
			break;
		}
	}
}

void render(void)
{
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	switch(ce_view)
	{
		case CE_VIEW_ANIMATIONS:
		{
			if(ce_cinema->animations > 0 && ce_cinema->animation[ce_current_animation])
			{
				t3f_draw_animation(ce_cinema->animation[ce_current_animation], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), ce_tick, 0, 0, 0, 0);
			}
			break;
		}
		case CE_VIEW_FRAMES:
		{
			cinema_render(ce_cinema);
			t3f_draw_textf(ce_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 16, 0, 0, 0, 0, "Frame %d/%d, Entity %d/%d", ce_current_frame + 1, ce_cinema->frames, ce_current_entity + 1, ce_cinema->frame[ce_current_frame].entities);
			t3f_draw_textf(ce_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 32, 0, 0, 0, 0, "%s", ce_string);
			break;
		}
	}
	t3f_draw_text(ce_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, 0, 0, 0, 0, 0, ce_view_name[ce_view]);
}

void process_arguments(int argc, char * argv[])
{
}

bool initialize(int argc, char * argv[])
{
	process_arguments(argc, argv);
	if(!t3f_initialize("Cinema Editor", 640, 480, 60.0, logic, render, T3F_DEFAULT | T3F_USE_MOUSE))
	{
		return false;
	}
	al_init_ttf_addon();
	ce_font = t3f_load_font("data/fonts/isle_of_the_dead.ttf", 16, 0);
	if(!ce_font)
	{
		return false;
	}
	font[2] = al_load_font("data/fonts/isle_of_the_dead.ttf", 16, 0);
	if(!font[2])
	{
		return false;
	}
	ce_cinema = create_cinema();
	if(!ce_cinema)
	{
		return false;
	}
	return true;
}

int main(int argc, char * argv[])
{
	if(!initialize(argc, argv))
	{
		return -1;
	}
	t3f_run();
	return 0;
}
