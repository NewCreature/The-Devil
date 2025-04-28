#include "t3f/t3f.h"
#include "t3f/extra/leaderboard.h"
#include "instance.h"
#include "cinema.h"
#include "main.h"
#include "title.h"
#include "game.h"
#include "t3_logo.h"
#include "leaderboard.h"


void event_handler(ALLEGRO_EVENT * event, void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	switch(event->type)
	{
		case ALLEGRO_EVENT_MOUSE_AXES:
		{
			instance->mouse_disabled = false;
			break;
		}
		case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
		{
			title_exit(data);
			break;
		}
	}
	t3f_event_handler(event);
	switch(event->type)
	{
		case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
		{
			title_init(data);
			break;
		}
	}
}

void logic(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;

	switch(instance->state)
	{
		case STATE_INTRO:
		{
			title_bg_logic(data);
			cinema_logic(instance->cinema);
			if(instance->cinema->position >= instance->cinema->frames || t3f_key_pressed(ALLEGRO_KEY_ESCAPE) || t3f_key_pressed(ALLEGRO_KEY_BACK))
			{
				destroy_cinema(instance->cinema);
				instance->cinema = NULL;
				title_load_data(data);
				instance->state = STATE_TITLE;
				instance->menu[instance->current_menu]->hover_element = -1;
				t3f_use_key_press(ALLEGRO_KEY_ESCAPE);
				t3f_use_key_press(ALLEGRO_KEY_BACK);
			}
			break;
		}
		case STATE_TITLE_IN:
		{
			title_in_logic(data);
			break;
		}
		case STATE_TITLE:
		{
			title_logic(data);
			break;
		}
		case STATE_TITLE_OUT:
		{
			title_out_logic(data);
			break;
		}
		case STATE_TITLE_GAME:
		{
			title_game_logic(data);
			break;
		}
		case STATE_GAME:
		{
			game_logic(data);
			break;
		}
		case STATE_ENDING:
		{
			title_bg_logic(data);
			cinema_logic(instance->ending_cinema);
			if(instance->ending_cinema->position >= instance->ending_cinema->frames || t3f_key_pressed(ALLEGRO_KEY_ESCAPE) || t3f_key_pressed(ALLEGRO_KEY_BACK))
			{
				destroy_cinema(instance->ending_cinema);
				instance->ending_cinema = NULL;
				title_load_data(data);
				if(instance->upload_scores)
				{
					download_leaderboard(data);
					if(instance->leaderboard)
					{
						instance->leaderboard_place = -1;
						for(i = 0; i < instance->leaderboard->data->entries; i++)
						{
							if(instance->score == instance->leaderboard->data->entry[i]->score && !strcmp(instance->network_id, instance->leaderboard->data->entry[i]->name))
							{
								instance->leaderboard_place = i;
								break;
							}
						}
						instance->state = STATE_LEADERBOARDS;
					}
					else
					{
						select_menu(TITLE_MENU_MAIN, data);
						instance->state = STATE_TITLE;
					}
				}
				else
				{
					instance->state = STATE_TITLE;
					select_menu(TITLE_MENU_MAIN, data);
				}
				instance->menu[instance->current_menu]->hover_element = -1;
				t3f_use_key_press(ALLEGRO_KEY_ESCAPE);
				t3f_use_key_press(ALLEGRO_KEY_BACK);
			}
			break;
		}
		case STATE_LEADERBOARDS:
		{
			leaderboard_logic(data);
			break;
		}
	}
	t3f_poll_sound_queue();
}

void render_mouse(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	#ifndef T3F_ANDROID
		if(!instance->mouse_disabled)
		{
			t3f_draw_animation(instance->animation[ANIMATION_CURSOR], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), instance->state_ticks, t3f_get_mouse_x() - 2 + 2, t3f_get_mouse_y() + 2, 0, 0);
			t3f_draw_animation(instance->animation[ANIMATION_CURSOR], t3f_color_white, instance->state_ticks, t3f_get_mouse_x() - 2, t3f_get_mouse_y(), 0, 0);
		}
	#endif
}

void render(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	al_hold_bitmap_drawing(true);
	switch(instance->state)
	{
		case STATE_INTRO:
		{
			title_bg_render(instance->cinema->bg_image);
			cinema_render(instance->cinema);
			render_mouse(data);
			break;
		}
		case STATE_TITLE_IN:
		{
			title_in_render(data);
			break;
		}
		case STATE_TITLE:
		{
			title_render(data);
			break;
		}
		case STATE_TITLE_OUT:
		{
			title_out_render(data);
			break;
		}
		case STATE_TITLE_GAME:
		{
			title_game_render(data);
			break;
		}
		case STATE_GAME:
		{
			game_render(data);
			break;
		}
		case STATE_ENDING:
		{
			title_bg_render(instance->ending_cinema->bg_image);
			cinema_render(instance->ending_cinema);
			render_mouse(data);
			break;
		}
		case STATE_LEADERBOARDS:
		{
			leaderboard_render(data);
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

void detect_controller_pandora(int type, void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	if(type < 0)
	{
		instance->controller_type = CONTROLLER_TYPE_ANALOG;
	}
	else
	{
		instance->controller_type = type;
	}
	switch(instance->controller_type)
	{
		case CONTROLLER_TYPE_ANALOG:
		{
			al_get_joystick_state(t3f_joystick[0], &t3f_joystick_state[0]);
			al_get_joystick_state(t3f_joystick[1], &t3f_joystick_state[1]);
			instance->controller->binding[CONTROLLER_UP].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
			instance->controller->binding[CONTROLLER_UP].joystick = 0;
			instance->controller->binding[CONTROLLER_UP].stick = 0;
			instance->controller->binding[CONTROLLER_UP].axis = 1;
			instance->controller->binding[CONTROLLER_UP].min = -1.0;
			instance->controller->binding[CONTROLLER_UP].mid = t3f_joystick_state[0].stick[0].axis[1];
			instance->controller->binding[CONTROLLER_UP].max = 1.0;
			instance->controller->binding[CONTROLLER_LEFT].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
			instance->controller->binding[CONTROLLER_LEFT].joystick = 0;
			instance->controller->binding[CONTROLLER_LEFT].stick = 0;
			instance->controller->binding[CONTROLLER_LEFT].axis = 0;
			instance->controller->binding[CONTROLLER_LEFT].min = -1.0;
			instance->controller->binding[CONTROLLER_LEFT].mid = t3f_joystick_state[0].stick[0].axis[0];
			instance->controller->binding[CONTROLLER_LEFT].max = 1.0;
			instance->controller->binding[CONTROLLER_FIRE_UP].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
			instance->controller->binding[CONTROLLER_FIRE_UP].joystick = 1;
			instance->controller->binding[CONTROLLER_FIRE_UP].stick = 0;
			instance->controller->binding[CONTROLLER_FIRE_UP].axis = 1;
			instance->controller->binding[CONTROLLER_FIRE_UP].min = -1.0;
			instance->controller->binding[CONTROLLER_FIRE_UP].mid = t3f_joystick_state[1].stick[0].axis[1];
			instance->controller->binding[CONTROLLER_FIRE_UP].max = 1.0;
			instance->controller->binding[CONTROLLER_FIRE_LEFT].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
			instance->controller->binding[CONTROLLER_FIRE_LEFT].joystick = 1;
			instance->controller->binding[CONTROLLER_FIRE_LEFT].stick = 0;
			instance->controller->binding[CONTROLLER_FIRE_LEFT].axis = 0;
			instance->controller->binding[CONTROLLER_FIRE_LEFT].min = -1.0;
			instance->controller->binding[CONTROLLER_FIRE_LEFT].mid = t3f_joystick_state[1].stick[0].axis[0];
			instance->controller->binding[CONTROLLER_FIRE_LEFT].max = 1.0;
			break;
		}
		case CONTROLLER_TYPE_NORMAL:
		case CONTROLLER_TYPE_MOUSE:
		{
			instance->controller->binding[CONTROLLER_UP].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_UP].button = ALLEGRO_KEY_UP;
			instance->controller->binding[CONTROLLER_DOWN].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_DOWN].button = ALLEGRO_KEY_DOWN;
			instance->controller->binding[CONTROLLER_LEFT].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_LEFT].button = ALLEGRO_KEY_LEFT;
			instance->controller->binding[CONTROLLER_RIGHT].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_RIGHT].button = ALLEGRO_KEY_RIGHT;
			instance->controller->binding[CONTROLLER_FIRE_UP].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_FIRE_UP].button = ALLEGRO_KEY_PGUP;
			instance->controller->binding[CONTROLLER_FIRE_DOWN].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_FIRE_DOWN].button = ALLEGRO_KEY_PGDN;
			instance->controller->binding[CONTROLLER_FIRE_LEFT].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_FIRE_LEFT].button = ALLEGRO_KEY_HOME;
			instance->controller->binding[CONTROLLER_FIRE_RIGHT].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_FIRE_RIGHT].button = ALLEGRO_KEY_END;
			instance->controller->binding[CONTROLLER_FIRE_UP].type = T3F_CONTROLLER_BINDING_KEY;
			break;
		}
	}
}

void detect_controller_desktop(int type, void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i = 0;
	int stick;

	/* autodetect dual analog */
	if(type < 0)
	{
		i = detect_dual_analog();
		if(i >= 0)
		{
			instance->controller_type = CONTROLLER_TYPE_ANALOG;
		}
		else
		{
			if(t3f_flags & T3F_USE_MOUSE)
			{
				instance->controller_type = CONTROLLER_TYPE_MOUSE;
			}
			else
			{
				instance->controller_type = CONTROLLER_TYPE_NORMAL;
			}
		}
	}
	switch(instance->controller_type)
	{
		case CONTROLLER_TYPE_ANALOG:
		{
			stick = get_analog_stick(i, 0);
			if(stick >= 0)
			{
				al_get_joystick_state(t3f_joystick[i], &t3f_joystick_state[i]);
				instance->controller->binding[CONTROLLER_UP].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
				instance->controller->binding[CONTROLLER_UP].joystick = i;
				instance->controller->binding[CONTROLLER_UP].stick = stick;
				instance->controller->binding[CONTROLLER_UP].axis = 1;
				instance->controller->binding[CONTROLLER_UP].min = -1.0;
				instance->controller->binding[CONTROLLER_UP].mid = t3f_joystick_state[i].stick[stick].axis[1];
				instance->controller->binding[CONTROLLER_UP].max = 1.0;
				instance->controller->binding[CONTROLLER_LEFT].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
				instance->controller->binding[CONTROLLER_LEFT].joystick = i;
				instance->controller->binding[CONTROLLER_LEFT].stick = stick;
				instance->controller->binding[CONTROLLER_LEFT].axis = 0;
				instance->controller->binding[CONTROLLER_LEFT].min = -1.0;
				instance->controller->binding[CONTROLLER_LEFT].mid = t3f_joystick_state[i].stick[stick].axis[0];
				instance->controller->binding[CONTROLLER_LEFT].max = 1.0;
			}
			else
			{
				instance->controller->binding[CONTROLLER_UP].type = T3F_CONTROLLER_BINDING_KEY;
				instance->controller->binding[CONTROLLER_UP].button = ALLEGRO_KEY_UP;
				instance->controller->binding[CONTROLLER_LEFT].type = T3F_CONTROLLER_BINDING_KEY;
				instance->controller->binding[CONTROLLER_LEFT].button = ALLEGRO_KEY_LEFT;
			}
			stick = get_analog_stick(i, 1);
			if(stick >= 0)
			{
				instance->controller->binding[CONTROLLER_FIRE_UP].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
				instance->controller->binding[CONTROLLER_FIRE_UP].joystick = i;
				instance->controller->binding[CONTROLLER_FIRE_UP].stick = stick;
				instance->controller->binding[CONTROLLER_FIRE_UP].axis = 1;
				instance->controller->binding[CONTROLLER_FIRE_UP].min = -1.0;
				instance->controller->binding[CONTROLLER_FIRE_UP].mid = t3f_joystick_state[i].stick[stick].axis[1];
				instance->controller->binding[CONTROLLER_FIRE_UP].max = 1.0;
				instance->controller->binding[CONTROLLER_FIRE_LEFT].type = T3F_CONTROLLER_BINDING_JOYSTICK_AXIS;
				instance->controller->binding[CONTROLLER_FIRE_LEFT].joystick = i;
				instance->controller->binding[CONTROLLER_FIRE_LEFT].stick = stick;
				instance->controller->binding[CONTROLLER_FIRE_LEFT].axis = 0;
				instance->controller->binding[CONTROLLER_FIRE_LEFT].min = -1.0;
				instance->controller->binding[CONTROLLER_FIRE_LEFT].mid = t3f_joystick_state[i].stick[stick].axis[0];
				instance->controller->binding[CONTROLLER_FIRE_LEFT].max = 1.0;
			}
			else
			{
				instance->controller->binding[CONTROLLER_FIRE_UP].type = T3F_CONTROLLER_BINDING_KEY;
				instance->controller->binding[CONTROLLER_FIRE_UP].button = ALLEGRO_KEY_W;
				instance->controller->binding[CONTROLLER_FIRE_LEFT].type = T3F_CONTROLLER_BINDING_KEY;
				instance->controller->binding[CONTROLLER_FIRE_LEFT].button = ALLEGRO_KEY_A;
			}
			break;
		}
		case CONTROLLER_TYPE_NORMAL:
		case CONTROLLER_TYPE_MOUSE:
		{
			instance->controller->binding[CONTROLLER_UP].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_UP].button = ALLEGRO_KEY_W;
			instance->controller->binding[CONTROLLER_DOWN].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_DOWN].button = ALLEGRO_KEY_S;
			instance->controller->binding[CONTROLLER_LEFT].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_LEFT].button = ALLEGRO_KEY_A;
			instance->controller->binding[CONTROLLER_RIGHT].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_RIGHT].button = ALLEGRO_KEY_D;
			instance->controller->binding[CONTROLLER_FIRE_UP].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_FIRE_UP].button = ALLEGRO_KEY_UP;
			instance->controller->binding[CONTROLLER_FIRE_DOWN].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_FIRE_DOWN].button = ALLEGRO_KEY_DOWN;
			instance->controller->binding[CONTROLLER_FIRE_LEFT].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_FIRE_LEFT].button = ALLEGRO_KEY_LEFT;
			instance->controller->binding[CONTROLLER_FIRE_RIGHT].type = T3F_CONTROLLER_BINDING_KEY;
			instance->controller->binding[CONTROLLER_FIRE_RIGHT].button = ALLEGRO_KEY_RIGHT;
			break;
		}
	}
}

void detect_controller(int type, void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	#ifdef PANDORA
		detect_controller_pandora(type, data);
	#else
		#ifndef T3F_ANDROID
			detect_controller_desktop(type, data);
		#else
			instance->controller_type = CONTROLLER_TYPE_TOUCH_M;
		#endif
	#endif
	menu_fix_controller_type_config(data);
}

bool initialize(int argc, char * argv[], void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	const char * val;
	char * controller_section[3] = {"Normal Controls", "Mouse Controls", "Analog Controls"};
	int i;

	process_arguments(argc, argv);
	if(!t3f_initialize("The Devil", 640, 480, 60.0, logic, render, T3F_DEFAULT | T3F_USE_FULLSCREEN, data))
	{
		return false;
	}
	t3f_initialize_leaderboards("Leaderboard", "the_devil", "1.5", "https://www.tcubedsoftware.com/scripts/leaderboards/get_user_key.php", "https://www.tcubedsoftware.com/scripts/leaderboards/set_user_name.php", "https://www.tcubedsoftware.com/scripts/leaderboards/update.php", "https://www.tcubedsoftware.com/scripts/leaderboards/query.php");
	t3f_define_leaderboard_obfuscation(7, 29);
	t3f_set_event_handler(event_handler);
	#ifdef T3F_NO_UTF8
		t3f_windows_text_to_utf8(T3F_APP_COPYRIGHT, instance->copyright_message, 256);
	#else
		strcpy(instance->copyright_message, T3F_APP_COPYRIGHT);
	#endif
	instance->copyright_message_ustr = al_ustr_new(instance->copyright_message);
	if(!instance->copyright_message_ustr)
	{
		return false;
	}
	instance->copyright_message_uchar = al_ustr_new(" ");
	if(!instance->copyright_message_uchar)
	{
		return false;
	}
	al_inhibit_screensaver(true);
	instance->animation[ANIMATION_CURSOR] = t3f_load_animation("data/graphics/cursor.png", 0, false);
	if(!instance->animation[ANIMATION_CURSOR])
	{
		return false;
	}
	instance->font[FONT_LARGE] = t3f_load_font("data/fonts/isle_of_the_dead.ttf", T3F_FONT_TYPE_AUTO, 60, 0, false);
	if(!instance->font[FONT_LARGE])
	{
		return false;
	}
	instance->font[FONT_SMALL] = t3f_load_font("data/fonts/isle_of_the_dead.ttf", T3F_FONT_TYPE_AUTO, 24, 0, false);
	if(!instance->font[FONT_SMALL])
	{
		return false;
	}
	instance->font[FONT_TINY] = t3f_load_font("data/fonts/isle_of_the_dead.ttf", T3F_FONT_TYPE_AUTO, 16, 0, false);
	if(!instance->font[FONT_TINY])
	{
		return false;
	}

/*	sample[SAMPLE_LOGO] = al_load_sample("data/sounds/logo.ogg");
	if(!sample[SAMPLE_LOGO])
	{
		return false;
	} */
	t3f_set_gui_driver(NULL);
	instance->controller = t3f_create_controller(8);
	if(!instance->controller)
	{
		return false;
	}
	val = al_get_config_value(t3f_config, "Settings", "First Run");
	if(val)
	{
		if(!strcmp(val, "false"))
		{
			instance->first_run = false;
		}
	}
	val = al_get_config_value(t3f_config, "Controls", "Type");
	if(val)
	{
		instance->controller_type = atoi(val);
		#ifndef T3F_ANDROID
			if(instance->controller_type >= CONTROLLER_TYPES)
			{
				instance->controller_type = -1;
			}
		#else
			if(instance->controller_type < CONTROLLER_TYPE_TOUCH_S || instance->controller_type > CONTROLLER_TYPE_TOUCH_L)
			{
				instance->controller_type = -1;
			}
		#endif
	}
	for(i = 0; i < 3; i++)
	{
		instance->high_score[i] = t3f_retrieve_leaderboard_score("Leaderboard", instance->game_mode_text[i], "0");
		if(instance->high_score[i] < 0)
		{
			instance->high_score[i] = 0;
		}
	}
	val = al_get_config_value(t3f_config, "Network", "Upload");
	if(val)
	{
		if(!strcasecmp(val, "true"))
		{
			instance->upload_scores = true;
		}
	}
	t3f_enable_leaderboard_uploads(instance->upload_scores);
	val = al_get_config_value(t3f_config, "Network", "ID");
	if(val)
	{
		if(strlen(val) > 0)
		{
			strcpy(instance->network_id, val);
		}
	}
	val = al_get_config_value(t3f_config, "Display", "Type");
	if(val)
	{
		instance->display_type = atoi(val);
		if(instance->display_type < 0 || instance->display_type > 3)
		{
			instance->display_type = 0;
		}
	}
	val = al_get_config_value(t3f_config, "T3F", "force_aspect_ratio");
	if(val)
	{
		if(!strcmp(val, "true"))
		{
			instance->force_aspect = true;
		}
		else
		{
			instance->force_aspect = false;
		}
	}

	/* get controller config */
	if(instance->controller_type < 0)
	{
		detect_controller(-1, data);
	}
	else
	{
		if(instance->controller_type < CONTROLLER_TYPE_TOUCH_S)
		{
			if(!t3f_read_controller_config(t3f_config, controller_section[instance->controller_type], instance->controller))
			{
				detect_controller(-1, data);
			}
		}
	}
	t3f_srand(&instance->rng_state, time(0));
//	t3_logo_setup("data/graphics/logo.png", "data/sounds/logo.ogg");
	t3f_play_music("data/music/title.xm");
	title_load_data(data);
	title_init(data);
	if(instance->first_run)
	{
		select_menu(TITLE_MENU_FIRST, data);
		al_set_config_value(t3f_config, "Settings", "First Run", "false");
	}
	instance->state = STATE_TITLE_IN;
	instance->state_ticks = 0;
	if(instance->mouse_disabled)
	{
		t3f_select_next_gui_element(instance->menu[instance->current_menu]);
	}
	al_hide_mouse_cursor(t3f_display);
	return true;
}

void uninitialize(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;

	al_ustr_free(instance->copyright_message_ustr);
	al_ustr_free(instance->copyright_message_uchar);
	for(i = 0; i < MAX_FONTS; i++)
	{
		if(instance->font[i])
		{
			t3f_destroy_font(instance->font[i]);
		}
	}
	for(i = 0; i < MAX_ANIMATIONS; i++)
	{
		if(instance->animation[i])
		{
			t3f_destroy_animation(instance->animation[i]);
		}
	}
	for(i = 0; i < MAX_SAMPLES; i++)
	{
		if(instance->sample[i])
		{
			al_destroy_sample(instance->sample[i]);
		}
	}
}

int main(int argc, char * argv[])
{
	APP_INSTANCE * app;

	app = td_create_instance();
	if(!app)
	{
		goto fail;
	}
	if(!initialize(argc, argv, app))
	{
		return -1;
	}
	t3f_run();
	t3f_stop_music();
	uninitialize(app);
	t3f_finish();
	return 0;

	fail:
	{
		return -1;
	}
}
