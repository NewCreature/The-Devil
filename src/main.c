#include "t3f/t3f.h"
#include "t3f/gui.h"
#include "t3f/sound.h"
#include "t3f/music.h"
#include "t3f/controller.h"
#include "t3f/resource.h"
#include "t3f/rng.h"
#include "t3net/t3net.h"
#include "cinema.h"
#include "main.h"
#include "title.h"
#include "game.h"
#include "t3_logo.h"
#include "leaderboard.h"

/* main data */
int state = STATE_LOGO;
int state_ticks = 0;
T3F_ANIMATION * animation[MAX_ANIMATIONS] = {NULL};
ALLEGRO_FONT * font[MAX_FONTS] = {NULL};
ALLEGRO_SAMPLE * sample[MAX_SAMPLES] = {NULL};
ALLEGRO_BITMAP * bitmap[MAX_BITMAPS] = {NULL};
T3F_ATLAS * atlas = NULL;
CINEMA * cinema = NULL;
CINEMA * ending_cinema = NULL;
T3F_CONTROLLER * controller = NULL;
int controller_type = -1; // indicate we should autodetect controller
bool click = false;
bool first_run = true;
bool mouse_disabled = false;
T3F_RNG_STATE rng_state;

/* menu data */
T3F_GUI * menu[TITLE_MAX_MENUS] = {NULL};
int current_menu = TITLE_MENU_MAIN;
char menu_text[32][64] = {{0}};

/* game data */
LEVEL_DATA level;
GAME_ENTITY player;
GAME_ENTITY enemy[GAME_MAX_ENEMIES];
GAME_ENTITY player_shot[GAME_MAX_PLAYER_SHOTS];
GAME_ENTITY enemy_shot[GAME_MAX_ENEMY_SHOTS];
GAME_ENTITY powerup[GAME_MAX_POWERUPS];
GAME_ENTITY particle[GAME_MAX_PARTICLES];
GAME_ENTITY_SPAWNER enemy_spawner[GAME_MAX_ENTITY_SPAWNERS];
TEXT_PARTICLE text_particle[GAME_MAX_TEXT_PARTICLES];
int enemy_spawners = 0;
int current_level = 0;
int score = 0;
int high_score[16] = {10000, 10000, 10000};
bool got_high_score = false;
int multiplier = 1;
int multiplier_tick = 0;
int weapon = 0;
int game_state = GAME_STATE_PLAY;
int resume_state = 0;
int die_state = 0;
int die_timer = 0;
int lives = 0;
void (*enemy_spawn_logic)() = NULL;
bool konami_mode = false;
bool fire_power = false;
bool finale_mode = false;
int game_mode = GAME_MODE_STORY;
char * game_mode_text[16] = {"Story", "Story-Easy", "Eternal"};
int death_time = 0;
int flash_time = 0;
int fade_time = 0;
TOUCH_STICK touch_stick[2];
float touch_size = 64.0;

/* network data */
int network_id_pos = 0;
bool network_id_entry = false;
T3NET_LEADERBOARD * leaderboard = NULL;
int leaderboard_place = -1;
int leaderboard_mode = GAME_MODE_STORY;

/* config */
char network_id[256] = "Anonymous";
bool upload_scores = true;
int display_type = 0;
bool force_aspect = true;

void event_handler(ALLEGRO_EVENT * event, void * data)
{
	switch(event->type)
	{
		case ALLEGRO_EVENT_MOUSE_AXES:
		{
			mouse_disabled = false;
			break;
		}
	}
	t3f_event_handler(event);
}

void logic(void * data)
{
	int i;
	
	switch(state)
	{
		case STATE_LOGO:
		{
			t3_logo_logic();
			if(title_joystick_button_pressed())
			{
				mouse_disabled = true;
			}
			if(t3_logo_done)
			{
				t3f_play_music("data/music/title.xm");
				title_init();
				title_load_data();
				state = STATE_TITLE_IN;
				state_ticks = 0;
				if(mouse_disabled)
				{
					t3f_select_next_gui_element(menu[current_menu]);
				}
				al_hide_mouse_cursor(t3f_display);
			}
			break;
		}
		case STATE_INTRO:
		{
			title_bg_logic();
			cinema_logic(cinema);
			if(cinema->position >= cinema->frames)
			{
				destroy_cinema(cinema);
				cinema = NULL;
				state = STATE_TITLE;
			}
			break;
		}
		case STATE_TITLE_IN:
		{
			title_in_logic();
			break;
		}
		case STATE_TITLE:
		{
			title_logic();
			break;
		}
		case STATE_TITLE_OUT:
		{
			title_out_logic();
			break;
		}
		case STATE_TITLE_GAME:
		{
			title_game_logic();
			break;
		}
		case STATE_GAME:
		{
			game_logic();
			break;
		}
		case STATE_ENDING:
		{
			title_bg_logic();
			cinema_logic(ending_cinema);
			if(ending_cinema->position >= ending_cinema->frames)
			{
				destroy_cinema(ending_cinema);
				ending_cinema = NULL;
				if(upload_scores)
				{
					download_leaderboard();
					if(leaderboard)
					{
						leaderboard_place = -1;
						for(i = 0; i < leaderboard->entries; i++)
						{
							if(score * 2 + 'v' + 'g' + 'o' + 'l' + 'f' == leaderboard->entry[i]->score && !strcmp(network_id, leaderboard->entry[i]->name))
							{
								leaderboard_place = i;
								break;
							}
						}
						state = STATE_LEADERBOARDS;
					}
					else
					{
						current_menu = TITLE_MENU_MAIN;
						state = STATE_TITLE;
					}
				}
				else
				{
					state = STATE_TITLE;
					current_menu = TITLE_MENU_MAIN;
				}
			}
			break;
		}
		case STATE_LEADERBOARDS:
		{
			leaderboard_logic();
			break;
		}
	}
	t3f_poll_sound_queue();
}

void render_mouse(void)
{
	#ifndef T3F_ANDROID
		if(!t3f_mouse_hidden && !mouse_disabled)
		{
			t3f_draw_animation(animation[ANIMATION_CURSOR], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), state_ticks, t3f_mouse_x - 2 + 2, t3f_mouse_y + 2, 0, 0);
			t3f_draw_animation(animation[ANIMATION_CURSOR], t3f_color_white, state_ticks, t3f_mouse_x - 2, t3f_mouse_y, 0, 0);
		}
	#endif
}

void render(void * data)
{
	al_hold_bitmap_drawing(true);
	switch(state)
	{
		case STATE_LOGO:
		{
			t3_logo_render();
			break;
		}
		case STATE_INTRO:
		{
			title_bg_render();
			cinema_render(cinema);
			render_mouse();
			break;
		}
		case STATE_TITLE_IN:
		{
			title_in_render();
			break;
		}
		case STATE_TITLE:
		{
			title_render();
			break;
		}
		case STATE_TITLE_OUT:
		{
			title_out_render();
			break;
		}
		case STATE_TITLE_GAME:
		{
			title_game_render();
			break;
		}
		case STATE_GAME:
		{
			game_render();
			break;
		}
		case STATE_ENDING:
		{
			title_bg_render();
			cinema_render(ending_cinema);
			render_mouse();
			break;
		}
		case STATE_LEADERBOARDS:
		{
			leaderboard_render();
			break;
		}
	}
	al_hold_bitmap_drawing(false);
}

void process_arguments(int argc, char * argv[])
{
	int i;
	
	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "-v"))
		{
			printf("%s\n", THE_DEVIL_VERSION_STRING);
			exit(0);
		}
	}
}

bool is_dual_analog(int joy)
{
	int i;
	int c = 0;
	
	for(i = 0; i < al_get_joystick_num_sticks(t3f_joystick[joy]); i++)
	{
		if(al_get_joystick_num_axes(t3f_joystick[joy], i) >= 2)
		{
			c++;
		}
	}
	if(c >= 2)
	{
		return true;
	}
	return false;
}

int detect_dual_analog(void)
{
	int i;
	
	for(i = 0; i < al_get_num_joysticks(); i++)
	{
		if(is_dual_analog(i))
		{
			return i;
		}
	}
	return -1;
}

int get_analog_stick(int joy, int stick)
{
	int i;
	int c = 0;
	
	for(i = 0; i < al_get_joystick_num_sticks(t3f_joystick[joy]); i++)
	{
		if(al_get_joystick_num_axes(t3f_joystick[joy], i) >= 2)
		{
			if(c == stick)
			{
				return i;
			}
			c++;
		}
	}
	return -1;
}

void detect_controller_pandora(int type)
{
	if(type < 0)
	{
		controller_type = CONTROLLER_TYPE_ANALOG;
	}
	else
	{
		controller_type = type;
	}
	switch(controller_type)
	{
		case CONTROLLER_TYPE_ANALOG:
		{
			al_get_joystick_state(t3f_joystick[0], &t3f_joystick_state[0]);
			al_get_joystick_state(t3f_joystick[1], &t3f_joystick_state[1]);
			controller->binding[CONTROLLER_UP].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
			controller->binding[CONTROLLER_UP].joystick = 0;
			controller->binding[CONTROLLER_UP].stick = 0;
			controller->binding[CONTROLLER_UP].axis = 1;
			controller->binding[CONTROLLER_UP].min = -1.0;
			controller->binding[CONTROLLER_UP].mid = t3f_joystick_state[0].stick[0].axis[1];
			controller->binding[CONTROLLER_UP].max = 1.0;
			controller->binding[CONTROLLER_LEFT].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
			controller->binding[CONTROLLER_LEFT].joystick = 0;
			controller->binding[CONTROLLER_LEFT].stick = 0;
			controller->binding[CONTROLLER_LEFT].axis = 0;
			controller->binding[CONTROLLER_LEFT].min = -1.0;
			controller->binding[CONTROLLER_LEFT].mid = t3f_joystick_state[0].stick[0].axis[0];
			controller->binding[CONTROLLER_LEFT].max = 1.0;
			controller->binding[CONTROLLER_FIRE_UP].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
			controller->binding[CONTROLLER_FIRE_UP].joystick = 1;
			controller->binding[CONTROLLER_FIRE_UP].stick = 0;
			controller->binding[CONTROLLER_FIRE_UP].axis = 1;
			controller->binding[CONTROLLER_FIRE_UP].min = -1.0;
			controller->binding[CONTROLLER_FIRE_UP].mid = t3f_joystick_state[1].stick[0].axis[1];
			controller->binding[CONTROLLER_FIRE_UP].max = 1.0;
			controller->binding[CONTROLLER_FIRE_LEFT].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
			controller->binding[CONTROLLER_FIRE_LEFT].joystick = 1;
			controller->binding[CONTROLLER_FIRE_LEFT].stick = 0;
			controller->binding[CONTROLLER_FIRE_LEFT].axis = 0;
			controller->binding[CONTROLLER_FIRE_LEFT].min = -1.0;
			controller->binding[CONTROLLER_FIRE_LEFT].mid = t3f_joystick_state[1].stick[0].axis[0];
			controller->binding[CONTROLLER_FIRE_LEFT].max = 1.0;
			break;
		}
		case CONTROLLER_TYPE_NORMAL:
		case CONTROLLER_TYPE_MOUSE:
		{
			controller->binding[CONTROLLER_UP].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_UP].button = ALLEGRO_KEY_UP;
			controller->binding[CONTROLLER_DOWN].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_DOWN].button = ALLEGRO_KEY_DOWN;
			controller->binding[CONTROLLER_LEFT].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_LEFT].button = ALLEGRO_KEY_LEFT;
			controller->binding[CONTROLLER_RIGHT].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_RIGHT].button = ALLEGRO_KEY_RIGHT;
			controller->binding[CONTROLLER_FIRE_UP].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_FIRE_UP].button = ALLEGRO_KEY_PGUP;
			controller->binding[CONTROLLER_FIRE_DOWN].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_FIRE_DOWN].button = ALLEGRO_KEY_PGDN;
			controller->binding[CONTROLLER_FIRE_LEFT].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_FIRE_LEFT].button = ALLEGRO_KEY_HOME;
			controller->binding[CONTROLLER_FIRE_RIGHT].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_FIRE_RIGHT].button = ALLEGRO_KEY_END;
			controller->binding[CONTROLLER_FIRE_UP].type = T3F_CONTROLLER_BINDING_KEY;
			break;
		}
	}
}

void detect_controller_desktop(int type)
{
	int i = 0;
	int stick;
	
	/* autodetect dual analog */
	if(type < 0)
	{
		i = detect_dual_analog();
		if(i >= 0)
		{
			controller_type = CONTROLLER_TYPE_ANALOG;
		}
		else
		{
			controller_type = CONTROLLER_TYPE_NORMAL;
		}
	}
	switch(controller_type)
	{
		case CONTROLLER_TYPE_ANALOG:
		{
			stick = get_analog_stick(i, 0);
			if(stick >= 0)
			{
				al_get_joystick_state(t3f_joystick[i], &t3f_joystick_state[i]);
				controller->binding[CONTROLLER_UP].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
				controller->binding[CONTROLLER_UP].joystick = i;
				controller->binding[CONTROLLER_UP].stick = stick;
				controller->binding[CONTROLLER_UP].axis = 1;
				controller->binding[CONTROLLER_UP].min = -1.0;
				controller->binding[CONTROLLER_UP].mid = t3f_joystick_state[i].stick[stick].axis[1];
				controller->binding[CONTROLLER_UP].max = 1.0;
				controller->binding[CONTROLLER_LEFT].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
				controller->binding[CONTROLLER_LEFT].joystick = i;
				controller->binding[CONTROLLER_LEFT].stick = stick;
				controller->binding[CONTROLLER_LEFT].axis = 0;
				controller->binding[CONTROLLER_LEFT].min = -1.0;
				controller->binding[CONTROLLER_LEFT].mid = t3f_joystick_state[i].stick[stick].axis[0];
				controller->binding[CONTROLLER_LEFT].max = 1.0;
			}
			else
			{
				controller->binding[CONTROLLER_UP].type = T3F_CONTROLLER_BINDING_KEY;
				controller->binding[CONTROLLER_UP].button = ALLEGRO_KEY_UP;
				controller->binding[CONTROLLER_LEFT].type = T3F_CONTROLLER_BINDING_KEY;
				controller->binding[CONTROLLER_LEFT].button = ALLEGRO_KEY_LEFT;
			}
			stick = get_analog_stick(i, 1);
			if(stick >= 0)
			{
				controller->binding[CONTROLLER_FIRE_UP].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
				controller->binding[CONTROLLER_FIRE_UP].joystick = i;
				controller->binding[CONTROLLER_FIRE_UP].stick = stick;
				controller->binding[CONTROLLER_FIRE_UP].axis = 1;
				controller->binding[CONTROLLER_FIRE_UP].min = -1.0;
				controller->binding[CONTROLLER_FIRE_UP].mid = t3f_joystick_state[i].stick[stick].axis[1];
				controller->binding[CONTROLLER_FIRE_UP].max = 1.0;
				controller->binding[CONTROLLER_FIRE_LEFT].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
				controller->binding[CONTROLLER_FIRE_LEFT].joystick = i;
				controller->binding[CONTROLLER_FIRE_LEFT].stick = stick;
				controller->binding[CONTROLLER_FIRE_LEFT].axis = 0;
				controller->binding[CONTROLLER_FIRE_LEFT].min = -1.0;
				controller->binding[CONTROLLER_FIRE_LEFT].mid = t3f_joystick_state[i].stick[stick].axis[0];
				controller->binding[CONTROLLER_FIRE_LEFT].max = 1.0;
			}
			else
			{
				controller->binding[CONTROLLER_FIRE_UP].type = T3F_CONTROLLER_BINDING_KEY;
				controller->binding[CONTROLLER_FIRE_UP].button = ALLEGRO_KEY_W;
				controller->binding[CONTROLLER_FIRE_LEFT].type = T3F_CONTROLLER_BINDING_KEY;
				controller->binding[CONTROLLER_FIRE_LEFT].button = ALLEGRO_KEY_A;
			}
			break;
		}
		case CONTROLLER_TYPE_NORMAL:
		case CONTROLLER_TYPE_MOUSE:
		{
			controller->binding[CONTROLLER_UP].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_UP].button = ALLEGRO_KEY_UP;
			controller->binding[CONTROLLER_DOWN].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_DOWN].button = ALLEGRO_KEY_DOWN;
			controller->binding[CONTROLLER_LEFT].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_LEFT].button = ALLEGRO_KEY_LEFT;
			controller->binding[CONTROLLER_RIGHT].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_RIGHT].button = ALLEGRO_KEY_RIGHT;
			controller->binding[CONTROLLER_FIRE_UP].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_FIRE_UP].button = ALLEGRO_KEY_W;
			controller->binding[CONTROLLER_FIRE_DOWN].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_FIRE_DOWN].button = ALLEGRO_KEY_S;
			controller->binding[CONTROLLER_FIRE_LEFT].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_FIRE_LEFT].button = ALLEGRO_KEY_A;
			controller->binding[CONTROLLER_FIRE_RIGHT].type = T3F_CONTROLLER_BINDING_KEY;
			controller->binding[CONTROLLER_FIRE_RIGHT].button = ALLEGRO_KEY_D;
			break;
		}
	}
}

void detect_controller(int type)
{
	#ifdef PANDORA
		detect_controller_pandora(type);
	#else
		detect_controller_desktop(type);
	#endif
	menu_fix_controller_type_config();
}

bool initialize(int argc, char * argv[])
{
	const char * val;
	char * controller_section[3] = {"Normal Controls", "Mouse Controls", "Analog Controls"};
	
	process_arguments(argc, argv);
	if(!t3f_initialize("The Devil", 640, 480, 60.0, logic, render, T3F_DEFAULT | T3F_USE_MOUSE | T3F_USE_JOYSTICK | T3F_USE_TOUCH | T3F_FORCE_ASPECT | T3F_FILL_SCREEN, NULL))
	{
		return false;
	}
	t3f_set_event_handler(event_handler);
	al_inhibit_screensaver(true);
	animation[ANIMATION_LOGO] = t3f_load_animation_from_bitmap("data/graphics/logo.png");
	if(!animation[ANIMATION_LOGO])
	{
		return false;
	}
	animation[ANIMATION_CURSOR] = t3f_load_animation_from_bitmap("data/graphics/cursor.png");
	if(!animation[ANIMATION_CURSOR])
	{
		return false;
	}
	
	#ifndef ALLEGRO_MACOSX
		bitmap[BITMAP_ICON] = t3f_load_resource((void **)(&bitmap[BITMAP_ICON]), T3F_RESOURCE_TYPE_BITMAP, "data/graphics/icon.png", 0, 0, 0);
		if(!bitmap[BITMAP_ICON])
		{
			return false;
		}
		al_set_display_icon(t3f_display, bitmap[BITMAP_ICON]);
	#endif
	
	font[FONT_LARGE] = t3f_load_resource((void **)(&font[FONT_LARGE]), T3F_RESOURCE_TYPE_FONT, "data/fonts/isle_of_the_dead.ttf", 32, 0, 0);
	if(!font[FONT_LARGE])
	{
		return false;
	}
	font[FONT_SMALL] = t3f_load_resource((void **)(&font[FONT_SMALL]), T3F_RESOURCE_TYPE_FONT, "data/fonts/isle_of_the_dead.ttf", 24, 0, 0);
	if(!font[FONT_SMALL])
	{
		return false;
	}
	font[FONT_TINY] = t3f_load_resource((void **)(&font[FONT_TINY]), T3F_RESOURCE_TYPE_FONT, "data/fonts/isle_of_the_dead.ttf", 16, 0, 0);
	if(!font[FONT_TINY])
	{
		return false;
	}

	sample[SAMPLE_LOGO] = al_load_sample("data/sounds/logo.ogg");
	if(!sample[SAMPLE_LOGO])
	{
		return false;
	}
	t3f_set_gui_driver(NULL);
	controller = t3f_create_controller(8);
	if(!controller)
	{
		return false;
	}
	val = al_get_config_value(t3f_config, "Settings", "First Run");
	if(val)
	{
		if(!strcmp(val, "false"))
		{
			first_run = false;
		}
	}
	if(first_run)
	{
		current_menu = TITLE_MENU_FIRST;
		al_set_config_value(t3f_config, "Settings", "First Run", "false");
	}
	val = al_get_config_value(t3f_config, "Controls", "Type");
	if(val)
	{
		controller_type = atoi(val);
		if(controller_type >= CONTROLLER_TYPES)
		{
			controller_type = -1;
		}
	}
	val = al_get_config_value(t3f_config, "Save Data", "High Score (S)");
	if(val)
	{
		high_score[GAME_MODE_STORY] = atoi(val);
	}
	val = al_get_config_value(t3f_config, "Save Data", "High Score (SE)");
	if(val)
	{
		high_score[GAME_MODE_STORY_EASY] = atoi(val);
	}
	val = al_get_config_value(t3f_config, "Save Data", "High Score (E)");
	if(val)
	{
		high_score[GAME_MODE_ETERNAL] = atoi(val);
	}
	val = al_get_config_value(t3f_config, "Network", "Upload");
	if(val)
	{
		if(strcmp(val, "true"))
		{
			upload_scores = false;
		}
	}
	val = al_get_config_value(t3f_config, "Network", "ID");
	if(val)
	{
		if(strlen(val) > 0)
		{
			strcpy(network_id, val);
		}
	}
	val = al_get_config_value(t3f_config, "Display", "Type");
	if(val)
	{
		display_type = atoi(val);
		if(display_type < 0 || display_type > 3)
		{
			display_type = 0;
		}
	}
	val = al_get_config_value(t3f_config, "T3F", "force_aspect_ratio");
	if(val)
	{
		if(!strcmp(val, "true"))
		{
			force_aspect = true;
		}
		else
		{
			force_aspect = false;
		}
	}
	
	/* get controller config */
	if(controller_type < 0)
	{
		detect_controller(-1);
	}
	else
	{
		if(controller_type != CONTROLLER_TYPE_TOUCH)
		{
			if(!t3f_read_controller_config(t3f_config, controller_section[controller_type], controller))
			{
				detect_controller(-1);
			}
		}
	}
	t3f_srand(&rng_state, time(0));
	t3_logo_setup("data/graphics/logo.png", "data/sounds/logo.ogg");
	state = STATE_LOGO;
	return true;
}

void uninitialize(void)
{
	int i;
	
	for(i = 0; i < MAX_FONTS; i++)
	{
		if(font[i])
		{
			t3f_destroy_resource(font[i]);
		}
	}
	for(i = 0; i < MAX_ANIMATIONS; i++)
	{
		if(animation[i])
		{
			t3f_destroy_animation(animation[i]);
		}
	}
	for(i = 0; i < MAX_SAMPLES; i++)
	{
		if(sample[i])
		{
			al_destroy_sample(sample[i]);
		}
	}
}

int main(int argc, char * argv[])
{
	if(!initialize(argc, argv))
	{
		return -1;
	}
	t3f_run();
	t3f_stop_music();
	uninitialize();
	return 0;
}
