#include "t3f/t3f.h"
#include "instance.h"
#include "main.h"
#include "title.h"
#include "enemy.h"
#include "leaderboard.h"

static int title_game_mode = GAME_MODE_STORY;
static int analog_state = 0;
static int analog_type = 0;
static float analog_max = 0.0;
static int button_press_counter = 0;
static bool block_buttons = false;

static const char * privacy_text =
	"Information We Collect:\n\n"
	"We collect only enough information to process your request. With regards to this "
	"software, that entails storing any scores you submit to our leaderboards (includes your "
	"profile name and score information).\n\n"
	"What We Do with Your Information:\n\n"
	"Your leaderboard data is accessible to everyone, so it is advisable to not put "
	"personal information you don't want to share in your screen name.";

static void menu_fix_controller_type_text(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	switch(instance->controller_type)
	{
		case CONTROLLER_TYPE_NORMAL:
		{
			sprintf(instance->menu_text[0], "Normal");
			break;
		}
		case CONTROLLER_TYPE_MOUSE:
		{
			sprintf(instance->menu_text[0], "Mouse + Keyboard");
			break;
		}
		case CONTROLLER_TYPE_ANALOG:
		{
			sprintf(instance->menu_text[0], "Dual Analog");
			break;
		}
		case CONTROLLER_TYPE_TOUCH_S:
		{
			sprintf(instance->menu_text[0], "Touch Small");
			break;
		}
		case CONTROLLER_TYPE_TOUCH_M:
		{
			sprintf(instance->menu_text[0], "Touch Medium");
			break;
		}
		case CONTROLLER_TYPE_TOUCH_L:
		{
			sprintf(instance->menu_text[0], "Touch Large");
			break;
		}
	}
}

void menu_fix_controller_type_config(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	char buffer[32] = {0};

	sprintf(buffer, "%d", instance->controller_type);
	al_set_config_value(t3f_config, "Controls", "Type", buffer);
}

void menu_fix_leaderboard_text(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	switch(instance->game_mode)
	{
		case GAME_MODE_STORY:
		{
			sprintf(instance->menu_text[10], "Story Mode");
			break;
		}
		case GAME_MODE_STORY_EASY:
		{
			sprintf(instance->menu_text[10], "Story Mode (Easy)");
			break;
		}
		case GAME_MODE_ETERNAL:
		{
			sprintf(instance->menu_text[10], "Eternal Mode");
			break;
		}
	}
}

void menu_fix_internet_text(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	sprintf(instance->menu_text[9], "%s", instance->upload_scores ? "Yes" : "No");
}

void menu_fix_display_text(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	char * display_type_name[4] = {"Small Window", "Medium Window", "Large Window", "Full Screen"};

	sprintf(instance->menu_text[12], "%s", display_type_name[instance->display_type]);
	sprintf(instance->menu_text[13], "%s", instance->force_aspect ? "Yes" : "No");
}

void menu_analog_logic(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int dir = 0;

	switch(analog_type)
	{
		case 0:
		{
			sprintf(instance->menu_text[14], "Configure Vertical Movement Axis");
			break;
		}
		case 1:
		{
			sprintf(instance->menu_text[14], "Configure Horizontal Movement Axis");
			dir = 1;
			break;
		}
		case 2:
		{
			sprintf(instance->menu_text[14], "Configure Vertical Firing Axis");
			break;
		}
		case 3:
		{
			sprintf(instance->menu_text[14], "Configure Horizontal Firing Axis");
			dir = 1;
			break;
		}
	}
	t3f_read_controller(instance->controller);
	switch(analog_state)
	{
		case 0:
		{
			if(instance->controller->state[analog_type * 2].pos < analog_max)
			{
				analog_max = instance->controller->state[analog_type * 2].pos;
			}
			sprintf(instance->menu_text[15], "Move Axis All the Way %s (%0.2f)", dir == 0 ? "Up" : "Left", analog_max);
			break;
		}
		case 1:
		{
			if(instance->controller->state[analog_type * 2].pos > analog_max)
			{
				analog_max = instance->controller->state[analog_type * 2].pos;
			}
			sprintf(instance->menu_text[15], "Move Axis All the Way %s (%0.2f)", dir == 0 ? "Down" : "Right", analog_max);
			break;
		}
		case 2:
		{
			sprintf(instance->menu_text[15], "Allow Axis to Rest (%0.2f)", instance->controller->state[analog_type * 2].pos);
			break;
		}
	}
}

void select_menu(int mmenu, void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->current_menu = mmenu;
	instance->menu[instance->current_menu]->hover_element = -1;
	#ifndef T3F_ANDROID
		t3f_select_next_gui_element(instance->menu[instance->current_menu]);
	#endif
}

int menu_proc_game(void * data, int i, void * p)
{
	select_menu(TITLE_MENU_MAIN, data);
	return 1;
}

int menu_proc_play(void * data, int i, void * p)
{
	select_menu(TITLE_MENU_MODE, data);
	return 1;
}

int menu_proc_story(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	//	title_free_data();
	instance->cinema = load_cinema("data/cinema/intro.cin", 0);
	instance->cinema->position = 0;
	instance->cinema->tick = 0;
//	click = true;
	instance->state = STATE_INTRO;
	return 1;
}

int menu_proc_leaderboards(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	download_leaderboard(data);
	instance->leaderboard_place = -1;
	menu_fix_leaderboard_text(p);
	al_start_timer(t3f_timer);
	if(instance->leaderboard)
	{
		instance->state = STATE_LEADERBOARDS;
	}
	return 1;
}

int menu_proc_settings(void * data, int i, void * p)
{
	select_menu(TITLE_MENU_SETTINGS, data);
	return 1;
}

int menu_proc_privacy(void * data, int i, void * p)
{
	select_menu(MENU_PRIVACY, data);
	return 1;
}

int menu_proc_quit(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->state_ticks = 0;
	instance->state = STATE_TITLE_OUT;
	return 1;
}

int menu_proc_play_story(void * data, int i, void * p)
{
	select_menu(TITLE_MENU_DIFFICULTY, data);
	return 1;
}

int menu_proc_play_easy(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->enemy_spawn_logic = enemy_spawn_logic_easy;
	title_game_mode = GAME_MODE_STORY_EASY;
	instance->state = STATE_TITLE_GAME;
	return 1;
}

int menu_proc_play_normal(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->enemy_spawn_logic = enemy_spawn_logic_normal;
	title_game_mode = GAME_MODE_STORY;
	instance->state = STATE_TITLE_GAME;
	return 1;
}

int menu_proc_play_eternal(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->enemy_spawn_logic = enemy_spawn_logic_normal;
	title_game_mode = GAME_MODE_ETERNAL;
	instance->state = STATE_TITLE_GAME;
	return 1;
}

int menu_proc_back(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	switch(instance->current_menu)
	{
		case TITLE_MENU_INTERNET:
		case TITLE_MENU_CONTROL:
		case TITLE_MENU_DISPLAY:
		{
			select_menu(TITLE_MENU_SETTINGS, data);
			menu_fix_internet_text(p);
			break;
		}
		case TITLE_MENU_NETWORK_ID:
		{
			select_menu(TITLE_MENU_INTERNET, data);
			al_set_config_value(t3f_config, "Network", "ID", instance->network_id);
			al_set_config_value(t3f_config, "Network", "Upload", instance->upload_scores ? "true" : "false");
			instance->network_id_entry = false;
			menu_fix_internet_text(p);
			break;
		}
		case TITLE_MENU_CONTROL_NORMAL:
		case TITLE_MENU_CONTROL_MOUSE:
		case TITLE_MENU_CONTROL_ANALOG:
		{
			switch(instance->controller_type)
			{
				case CONTROLLER_TYPE_NORMAL:
				{
					t3f_write_controller_config(t3f_config, "Normal Controls", instance->controller);
					break;
				}
				case CONTROLLER_TYPE_MOUSE:
				{
					t3f_write_controller_config(t3f_config, "Mouse Controls", instance->controller);
					break;
				}
				case CONTROLLER_TYPE_ANALOG:
				{
					t3f_write_controller_config(t3f_config, "Analog Controls", instance->controller);
					break;
				}
			}
			menu_fix_controller_type_text(p);
			select_menu(TITLE_MENU_CONTROL, data);
			break;
		}
		case TITLE_MENU_DIFFICULTY:
		{
			select_menu(TITLE_MENU_MODE, data);
			break;
		}
		case TITLE_MENU_MAIN:
		{
			select_menu(MENU_TITLE, data);
			break;
		}
		case MENU_TITLE:
		{
			t3f_exit();
			break;
		}
		default:
		{
			select_menu(TITLE_MENU_MAIN, data);
			break;
		}
	}
	return 1;
}

#ifdef T3F_ANDROID
	static void fix_touch_size(void * data)
	{
		APP_INSTANCE * instance = (APP_INSTANCE *)data;

		switch(instance->controller_type)
		{
			case CONTROLLER_TYPE_TOUCH_S:
			{
				instance->touch_size = 48.0;
				break;
			}
			case CONTROLLER_TYPE_TOUCH_M:
			{
				instance->touch_size = 64.0;
				break;
			}
			case CONTROLLER_TYPE_TOUCH_L:
			{
				instance->touch_size = 96.0;
				break;
			}
		}
	}
#endif

int menu_proc_controller_type_left(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	char * controller_section[3] = {"Normal Controls", "Mouse Controls", "Analog Controls"};

	instance->controller_type--;
	#ifndef T3F_ANDROID
		if(instance->controller_type < 0)
		{
			instance->controller_type = CONTROLLER_TYPE_ANALOG;
		}
	#else
		if(instance->controller_type < CONTROLLER_TYPE_TOUCH_S)
		{
			instance->controller_type = CONTROLLER_TYPE_TOUCH_L;
		}
		fix_touch_size(p);
	#endif
	if(instance->controller_type == CONTROLLER_TYPE_ANALOG)
	{
		if(al_get_num_joysticks() <= 0)
		{
			instance->controller_type = CONTROLLER_TYPE_MOUSE;
		}
	}
	if(instance->controller_type < CONTROLLER_TYPE_TOUCH_S)
	{
		if(!t3f_read_controller_config(t3f_config, controller_section[instance->controller_type], instance->controller))
		{
			detect_controller(instance->controller_type, data);
		}
	}
	menu_fix_controller_type_text(p);
	menu_fix_controller_type_config(p);
	return 1;
}

int menu_proc_controller_type_right(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	char * controller_section[3] = {"Normal Controls", "Mouse Controls", "Analog Controls"};

	instance->controller_type++;
	#ifndef T3F_ANDROID
		if(instance->controller_type > CONTROLLER_TYPE_ANALOG)
		{
			instance->controller_type = 0;
		}
	#else
		if(instance->controller_type > CONTROLLER_TYPE_TOUCH_L)
		{
			instance->controller_type = CONTROLLER_TYPE_TOUCH_S;
		}
		fix_touch_size(p);
	#endif
	if(instance->controller_type == CONTROLLER_TYPE_ANALOG)
	{
		if(al_get_num_joysticks() <= 0)
		{
			instance->controller_type = CONTROLLER_TYPE_NORMAL;
		}
	}
	if(instance->controller_type < CONTROLLER_TYPE_TOUCH_S)
	{
		if(!t3f_read_controller_config(t3f_config, controller_section[instance->controller_type], instance->controller))
		{
			detect_controller(instance->controller_type, data);
		}
	}
	menu_fix_controller_type_text(p);
	menu_fix_controller_type_config(p);
	return 1;
}

static void update_controller_menu_text(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int j;

	switch(instance->controller_type)
	{
		case CONTROLLER_TYPE_NORMAL:
		{
			for(j = 0; j < 8; j++)
			{
				sprintf(instance->menu_text[j + 1], "%s", t3f_get_controller_binding_name(instance->controller, j));
			}
			break;
		}
		case CONTROLLER_TYPE_MOUSE:
		{
			for(j = 0; j < 4; j++)
			{
				sprintf(instance->menu_text[j + 1], "%s", t3f_get_controller_binding_name(instance->controller, j));
			}
			break;
		}
		case CONTROLLER_TYPE_ANALOG:
		{
			for(j = 0; j < 4; j++)
			{
				sprintf(instance->menu_text[j + 1], "%s", t3f_get_controller_binding_name(instance->controller, j * 2));
			}
			break;
		}
	}
}

int menu_proc_controls(void * data, int i, void * p)
{
	select_menu(TITLE_MENU_CONTROL, data);
	return 1;
}

int menu_proc_internet(void * data, int i, void * p)
{
	select_menu(TITLE_MENU_INTERNET, data);
	return 1;
}

int menu_proc_display(void * data, int i, void * p)
{
	select_menu(TITLE_MENU_DISPLAY, data);
	return 1;
}

int menu_proc_controller_configure(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	switch(instance->controller_type)
	{
		case CONTROLLER_TYPE_NORMAL:
		{
			select_menu(TITLE_MENU_CONTROL_NORMAL, data);
			break;
		}
		case CONTROLLER_TYPE_MOUSE:
		{
			select_menu(TITLE_MENU_CONTROL_MOUSE, data);
			break;
		}
		case CONTROLLER_TYPE_ANALOG:
		{
			select_menu(TITLE_MENU_CONTROL_ANALOG, data);
			break;
		}
	}
	update_controller_menu_text(p);
	return 1;
}

int menu_proc_controller_autodetect(void * data, int i, void * p)
{
	detect_controller(-1, data);
	menu_proc_controller_configure(data, i, data);
	return 1;
}

static void bind_input(int slot, void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	t3f_bind_controller(instance->controller, slot);
	t3f_process_events(true);
	t3f_use_mouse_button_press(0);
	update_controller_menu_text(data);
}

int menu_proc_set_move_up(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	al_stop_timer(t3f_timer);
	sprintf(instance->menu_text[1], "Select input...");
	t3f_render(p);
	bind_input(0, data);
	update_controller_menu_text(p);
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_move_down(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	al_stop_timer(t3f_timer);
	sprintf(instance->menu_text[2], "Select input...");
	t3f_render(p);
	bind_input(1, data);
	update_controller_menu_text(p);
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_move_left(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	al_stop_timer(t3f_timer);
	sprintf(instance->menu_text[3], "Select input...");
	t3f_render(p);
	bind_input(2, data);
	update_controller_menu_text(p);
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_move_right(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	al_stop_timer(t3f_timer);
	sprintf(instance->menu_text[4], "Select input...");
	t3f_render(p);
	bind_input(3, data);
	update_controller_menu_text(p);
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_fire_up(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	al_stop_timer(t3f_timer);
	sprintf(instance->menu_text[5], "Select input...");
	t3f_render(p);
	bind_input(4, data);
	update_controller_menu_text(p);
	block_buttons = true;
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_fire_down(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	al_stop_timer(t3f_timer);
	sprintf(instance->menu_text[6], "Select input...");
	t3f_render(p);
	bind_input(5, data);
	update_controller_menu_text(p);
	block_buttons = true;
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_fire_left(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	al_stop_timer(t3f_timer);
	sprintf(instance->menu_text[7], "Select input...");
	t3f_render(p);
	bind_input(6, data);
	update_controller_menu_text(p);
	block_buttons = true;
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_fire_right(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	al_stop_timer(t3f_timer);
	sprintf(instance->menu_text[8], "Select input...");
	t3f_render(p);
	bind_input(7, data);
	update_controller_menu_text(p);
	block_buttons = true;
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_move_vertical(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	T3F_CONTROLLER_BINDING old_binding;

	al_stop_timer(t3f_timer);
	sprintf(instance->menu_text[1], "Move axis up...");
	t3f_render(p);
	memcpy(&old_binding, &instance->controller->binding[4], sizeof(T3F_CONTROLLER_BINDING));
	bind_input(0, data);
	if(instance->controller->binding[0].type != T3F_CONTROLLER_BINDING_JOYSTICK_AXIS)
	{
		memcpy(&instance->controller->binding[0], &old_binding, sizeof(T3F_CONTROLLER_BINDING));
	}
	else
	{
		al_get_joystick_state(t3f_joystick[instance->controller->binding[0].joystick], &t3f_joystick_state[instance->controller->binding[0].joystick]); // read initial state
		t3f_read_controller(instance->controller);
		if(instance->controller->state[0].pos > 0)
		{
			instance->controller->binding[0].flags |= T3F_CONTROLLER_FLAG_AXIS_INVERT;
		}
	}
	update_controller_menu_text(p);
	al_start_timer(t3f_timer);
	analog_state = 0;
	analog_type = 0;
	analog_max = 0.0;
	instance->controller->binding[0].flags |= T3F_CONTROLLER_FLAG_AXIS_NO_ADJUST;
	select_menu(TITLE_MENU_ANALOG, data);
	return 1;
}

int menu_proc_set_move_horizontal(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	T3F_CONTROLLER_BINDING old_binding;

	al_stop_timer(t3f_timer);
	sprintf(instance->menu_text[2], "Move axis left...");
	t3f_render(p);
	memcpy(&old_binding, &instance->controller->binding[4], sizeof(T3F_CONTROLLER_BINDING));
	bind_input(2, data);
	if(instance->controller->binding[2].type != T3F_CONTROLLER_BINDING_JOYSTICK_AXIS)
	{
		memcpy(&instance->controller->binding[2], &old_binding, sizeof(T3F_CONTROLLER_BINDING));
	}
	else
	{
		al_get_joystick_state(t3f_joystick[instance->controller->binding[2].joystick], &t3f_joystick_state[instance->controller->binding[2].joystick]); // read initial state
		t3f_read_controller(instance->controller);
		if(instance->controller->state[2].pos > 0)
		{
			instance->controller->binding[2].flags |= T3F_CONTROLLER_FLAG_AXIS_INVERT;
		}
	}
	update_controller_menu_text(p);
	al_start_timer(t3f_timer);
	analog_state = 0;
	analog_type = 1;
	analog_max = 0.0;
	instance->controller->binding[2].flags |= T3F_CONTROLLER_FLAG_AXIS_NO_ADJUST;
	select_menu(TITLE_MENU_ANALOG, data);
	return 1;
}

int menu_proc_set_fire_vertical(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	T3F_CONTROLLER_BINDING old_binding;

	al_stop_timer(t3f_timer);
	sprintf(instance->menu_text[3], "Move axis up...");
	t3f_render(p);
	memcpy(&old_binding, &instance->controller->binding[4], sizeof(T3F_CONTROLLER_BINDING));
	bind_input(4, data);
	if(instance->controller->binding[4].type != T3F_CONTROLLER_BINDING_JOYSTICK_AXIS)
	{
		memcpy(&instance->controller->binding[4], &old_binding, sizeof(T3F_CONTROLLER_BINDING));
	}
	else
	{
		al_get_joystick_state(t3f_joystick[instance->controller->binding[4].joystick], &t3f_joystick_state[instance->controller->binding[4].joystick]); // read initial state
		t3f_read_controller(instance->controller);
		if(instance->controller->state[4].pos > 0)
		{
			instance->controller->binding[4].flags |= T3F_CONTROLLER_FLAG_AXIS_INVERT;
		}
	}
	update_controller_menu_text(p);
	al_start_timer(t3f_timer);
	analog_state = 0;
	analog_type = 2;
	analog_max = 0.0;
	instance->controller->binding[4].flags |= T3F_CONTROLLER_FLAG_AXIS_NO_ADJUST;
	select_menu(TITLE_MENU_ANALOG, data);
	return 1;
}

int menu_proc_set_fire_horizontal(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	T3F_CONTROLLER_BINDING old_binding;

	al_stop_timer(t3f_timer);
	sprintf(instance->menu_text[4], "Move axis left...");
	t3f_render(p);
	memcpy(&old_binding, &instance->controller->binding[4], sizeof(T3F_CONTROLLER_BINDING));
	bind_input(6, data);
	if(instance->controller->binding[6].type != T3F_CONTROLLER_BINDING_JOYSTICK_AXIS)
	{
		memcpy(&instance->controller->binding[6], &old_binding, sizeof(T3F_CONTROLLER_BINDING));
	}
	else
	{
		al_get_joystick_state(t3f_joystick[instance->controller->binding[6].joystick], &t3f_joystick_state[instance->controller->binding[6].joystick]); // read initial state
		t3f_read_controller(instance->controller);
		if(instance->controller->state[6].pos > 0)
		{
			instance->controller->binding[6].flags |= T3F_CONTROLLER_FLAG_AXIS_INVERT;
		}
	}
	update_controller_menu_text(p);
	al_start_timer(t3f_timer);
	analog_state = 0;
	analog_type = 3;
	analog_max = 0.0;
	instance->controller->binding[6].flags |= T3F_CONTROLLER_FLAG_AXIS_NO_ADJUST;
	select_menu(TITLE_MENU_ANALOG, data);
	return 1;
}

int menu_proc_upload_toggle(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	if(instance->upload_scores)
	{
		instance->upload_scores = false;
	}
	else
	{
		instance->upload_scores = true;
	}
	menu_fix_internet_text(p);
	return 1;
}

#ifdef ALLEGRO_ANDROID
	static void menu_proc_network_id_callback(void * data)
	{
		APP_INSTANCE * instance = (APP_INSTANCE *)data;

		if(strlen(network_id) <= 0)
		{
			strcpy(network_id, "Anonymous");
		}
		menu_proc_back(NULL, 0, data);
	}
#endif

int menu_proc_network_id(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	select_menu(TITLE_MENU_NETWORK_ID, data);
	instance->network_id_entry = true;
	instance->network_id_pos = strlen(instance->network_id);
	t3f_clear_chars();
	#ifdef ALLEGRO_ANDROID
		t3f_open_edit_box("Enter Name", network_id, 256, "CapWords", menu_proc_network_id_callback, NULL);
	#endif
	return 1;
}

int menu_proc_network_id_name(void * data, int i, void * p)
{
	return menu_proc_network_id(data, i, data);
}

int menu_proc_leaderboard_mode_left(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->game_mode--;
	if(instance->game_mode < 0)
	{
		instance->game_mode = 2;
	}
	download_leaderboard(p);
	menu_fix_leaderboard_text(p);
	if(!instance->leaderboard)
	{
		instance->state = STATE_TITLE;
		select_menu(TITLE_MENU_MAIN, data);
	}
	return 1;
}

int menu_proc_leaderboard_mode_right(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->game_mode++;
	if(instance->game_mode > 2)
	{
		instance->game_mode = 0;
	}
	download_leaderboard(p);
	menu_fix_leaderboard_text(p);
	if(!instance->leaderboard)
	{
		instance->state = STATE_TITLE;
		select_menu(TITLE_MENU_MAIN, data);
	}
	return 1;
}

int menu_proc_leaderboard_done(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->state = STATE_TITLE;
	select_menu(TITLE_MENU_MAIN, data);
	return 1;
}

int menu_proc_pause_resume(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->game_state = instance->resume_state;
	return 1;
}

int menu_proc_pause_quit(void * data, int i, void * p)
{
	game_exit(p);
	return 1;
}

int menu_proc_first_yes(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->upload_scores = true;
	al_set_config_value(t3f_config, "Network", "Upload", instance->upload_scores ? "true" : "false");
	menu_fix_internet_text(p);
	select_menu(TITLE_MENU_INTERNET, data);
	return 1;
}

int menu_proc_first_no(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->upload_scores = false;
	al_set_config_value(t3f_config, "Network", "Upload", instance->upload_scores ? "true" : "false");
	menu_fix_internet_text(p);
	select_menu(TITLE_MENU_MAIN, data);
	return 1;
}

bool display_type_ok(int i)
{
	ALLEGRO_MONITOR_INFO info;

	al_get_monitor_info(0, &info);
	switch(i)
	{
		case 1:
		{
			if(info.x2 - info.x1 > 960 && info.y2 - info.y1 > 720)
			{
				return true;
			}
			return false;
		}
		case 2:
		{
			if(info.x2 - info.x1 > 1280 && info.y2 - info.y1 > 960)
			{
				return true;
			}
			return false;
		}
		default:
		{
			return true;
		}
	}
}

bool set_video_mode(int w, int h, int flags, void * data)
{
	/* must recreate display if we get error code 2 */
	if(t3f_set_gfx_mode(w, h, flags) == 2)
	{
		t3f_unload_resources();
		title_exit(data);
		al_unregister_event_source(t3f_queue, al_get_display_event_source(t3f_display));
		al_destroy_display(t3f_display);
		t3f_display = NULL;
		t3f_set_gfx_mode(640, 480, flags);
		al_register_event_source(t3f_queue, al_get_display_event_source(t3f_display));
		t3f_reload_resources();
		title_init(data);
		al_hide_mouse_cursor(t3f_display);
	}
	return true;
}

void initialize_video_mode(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	ALLEGRO_MONITOR_INFO info;
	int flags = instance->force_aspect ? T3F_FORCE_ASPECT : 0;

	/* make sure we are using the configured display setting */
	switch(instance->display_type)
	{
		case 0:
		{
			al_set_config_value(t3f_config, "T3F", "force_fullscreen", "false");
			set_video_mode(640, 480, flags, data);
			break;
		}
		case 1:
		{
			al_set_config_value(t3f_config, "T3F", "force_fullscreen", "false");
			set_video_mode(960, 720, flags, data);
			break;
		}
		case 2:
		{
			al_set_config_value(t3f_config, "T3F", "force_fullscreen", "false");
			set_video_mode(1280, 960, flags, data);
			break;
		}
		case 3:
		{
			al_get_monitor_info(0, &info);
			al_set_config_value(t3f_config, "T3F", "force_fullscreen", "true");
			set_video_mode(info.x2 - info.x1, info.y2 - info.y1, flags | T3F_USE_FULLSCREEN, data);
			t3f_default_view->need_update = true;
			t3f_select_view(t3f_default_view);
			break;
		}
	}
	t3f_select_view(t3f_default_view);
}

int menu_proc_display_left(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	char buf[16] = {0};

	while(1)
	{
		instance->display_type--;
		if(instance->display_type < 0)
		{
			instance->display_type = 3;
		}
		if(display_type_ok(instance->display_type))
		{
			break;
		}
	}
	initialize_video_mode(p);
	sprintf(buf, "%d", instance->display_type);
	al_set_config_value(t3f_config, "Display", "Type", buf);
	menu_fix_display_text(p);
	return 1;
}

int menu_proc_display_right(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	char buf[16] = {0};

	while(1)
	{
		instance->display_type++;
		if(instance->display_type > 3)
		{
			instance->display_type = 0;
		}
		if(display_type_ok(instance->display_type))
		{
			break;
		}
	}
	initialize_video_mode(p);
	sprintf(buf, "%d", instance->display_type);
	al_set_config_value(t3f_config, "Display", "Type", buf);
	menu_fix_display_text(p);
	return 1;
}

int menu_proc_display_toggle(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->force_aspect = !instance->force_aspect;
	menu_fix_display_text(p);
	if(!instance->force_aspect && (t3f_flags & T3F_FORCE_ASPECT))
	{
		t3f_flags &= ~T3F_FORCE_ASPECT;
		t3f_set_gfx_mode(al_get_display_width(t3f_display), al_get_display_height(t3f_display), t3f_flags & ~T3F_FORCE_ASPECT);
		al_set_config_value(t3f_config, "T3F", "force_aspect_ratio", "false");
	}
	else if(instance->force_aspect && !(t3f_flags & T3F_FORCE_ASPECT))
	{
		al_clear_to_color(t3f_color_black);
		al_flip_display();
		al_clear_to_color(t3f_color_black);
		t3f_flags |= T3F_FORCE_ASPECT;
		t3f_set_gfx_mode(al_get_display_width(t3f_display), al_get_display_height(t3f_display), t3f_flags | T3F_FORCE_ASPECT);
		al_set_config_value(t3f_config, "T3F", "force_aspect_ratio", "true");
	}
	return 1;
}

int menu_proc_analog_done(void * data, int i, void * p)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	switch(analog_state)
	{
		case 0:
		{
			instance->controller->binding[analog_type * 2].min = analog_max;
			analog_max = 0.0;
			analog_state++;
			break;
		}
		case 1:
		{
			instance->controller->binding[analog_type * 2].max = analog_max;
			analog_max = 0.0;
			analog_state++;
			break;
		}
		case 2:
		{
			instance->controller->binding[analog_type * 2].mid = instance->controller->state[analog_type * 2].pos;
			instance->controller->binding[0].flags &= ~T3F_CONTROLLER_FLAG_AXIS_NO_ADJUST;
			select_menu(TITLE_MENU_CONTROL_ANALOG, data);
			break;
		}
	}
	return 1;
}

static int title_flicker = 0;

bool title_load_data(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->animation[ANIMATION_TITLE] = t3f_load_animation("data/graphics/title_logo.png", 0, false);
	if(!instance->animation[ANIMATION_TITLE])
	{
		return false;
	}
	instance->animation[ANIMATION_TITLE_EYES] = t3f_load_animation("data/graphics/title_logo_eyes.png", 0, false);
	if(!instance->animation[ANIMATION_TITLE_EYES])
	{
		return false;
	}
	instance->bitmap[0] = t3f_load_bitmap("data/graphics/bg00.png", 0, false);
	if(!instance->bitmap[0])
	{
		return false;
	}
	return true;
}

void title_free_data(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	t3f_destroy_animation(instance->animation[ANIMATION_TITLE]);
	instance->animation[ANIMATION_TITLE] = NULL;
	t3f_destroy_animation(instance->animation[ANIMATION_TITLE_EYES]);
	instance->animation[ANIMATION_TITLE_EYES] = NULL;
	t3f_destroy_bitmap(instance->bitmap[0]);
	instance->bitmap[0] = NULL;
}

bool title_init(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	float total_height;
	float free_height;
	float space;
	float oy = 0;

	instance->menu[TITLE_MENU_MAIN] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_MAIN], menu_proc_play, "Play Game", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_MAIN], menu_proc_story, "View Story", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_MAIN], menu_proc_leaderboards, "Leaderboards", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_MAIN], menu_proc_settings, "Settings", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_MAIN], menu_proc_privacy, "Privacy", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(instance->menu[TITLE_MENU_MAIN], menu_proc_quit, "Exit to OS", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	t3f_center_gui(instance->menu[TITLE_MENU_MAIN], 20, 480);

	oy = 0;
	instance->menu[TITLE_MENU_DIFFICULTY] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_DIFFICULTY], menu_proc_play_easy, "Easy", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_DIFFICULTY], menu_proc_play_normal, "Normal", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(instance->menu[TITLE_MENU_DIFFICULTY], menu_proc_back, "Back", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	t3f_center_gui(instance->menu[TITLE_MENU_DIFFICULTY], 20, 480);

	oy = 0;
	instance->menu[TITLE_MENU_MODE] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_MODE], menu_proc_play_story, "Story", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_MODE], menu_proc_play_eternal, "Eternal", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(instance->menu[TITLE_MENU_MODE], menu_proc_back, "Back", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	t3f_center_gui(instance->menu[TITLE_MENU_MODE], 20, 480);

	oy = 0;
	instance->menu[TITLE_MENU_SETTINGS] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_SETTINGS], menu_proc_controls, "Controls", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_SETTINGS], menu_proc_internet, "Internet Options", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_SETTINGS], menu_proc_display, "Display Options", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(instance->menu[TITLE_MENU_SETTINGS], menu_proc_back, "Back", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	menu_fix_controller_type_text(data);
	t3f_center_gui(instance->menu[TITLE_MENU_SETTINGS], 20, 480);

	oy = 0;
	instance->menu[TITLE_MENU_CONTROL] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL], NULL, "Control Type", (void **)&instance->font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL], NULL, instance->menu_text[0], (void **)&instance->font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL], menu_proc_controller_type_left, "<", (void **)&instance->font[FONT_LARGE], 320 - t3f_get_text_width(instance->font[FONT_LARGE], "Normal Controller") / 2 - t3f_get_text_width(instance->font[FONT_LARGE], "<"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL], menu_proc_controller_type_right, ">", (void **)&instance->font[FONT_LARGE], 320 + t3f_get_text_width(instance->font[FONT_LARGE], "Normal Controller") / 2 + t3f_get_text_width(instance->font[FONT_LARGE], ">"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL], menu_proc_controller_autodetect, "Autodetect", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
		oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
		t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL], menu_proc_controller_configure, "Configure", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
		oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	#endif
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL], menu_proc_back, "Done", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	menu_fix_controller_type_text(data);
	t3f_center_gui(instance->menu[TITLE_MENU_CONTROL], 20, 480);

	oy = 0;
	instance->menu[TITLE_MENU_CONTROL_NORMAL] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Move Up", (void **)&instance->font[FONT_SMALL], 160, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_move_up, instance->menu_text[1], (void **)&instance->font[FONT_SMALL], 160, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Move Down", (void **)&instance->font[FONT_SMALL], 160, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_move_down, instance->menu_text[2], (void **)&instance->font[FONT_SMALL], 160, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Move Left", (void **)&instance->font[FONT_SMALL], 160, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_move_left, instance->menu_text[3], (void **)&instance->font[FONT_SMALL], 160, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Move Right", (void **)&instance->font[FONT_SMALL], 160, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_move_right, instance->menu_text[4], (void **)&instance->font[FONT_SMALL], 160, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy = 0;
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Fire Up", (void **)&instance->font[FONT_SMALL], 480, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_fire_up, instance->menu_text[5], (void **)&instance->font[FONT_SMALL], 480, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Fire Down", (void **)&instance->font[FONT_SMALL], 480, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_fire_down, instance->menu_text[6], (void **)&instance->font[FONT_SMALL], 480, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Fire Left", (void **)&instance->font[FONT_SMALL], 480, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_fire_left, instance->menu_text[7], (void **)&instance->font[FONT_SMALL], 480, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Fire Right", (void **)&instance->font[FONT_SMALL], 480, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_fire_right, instance->menu_text[8], (void **)&instance->font[FONT_SMALL], 480, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_back, "Done", (void **)&instance->font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(instance->menu[TITLE_MENU_CONTROL_NORMAL], 20, 480);

	oy = 0;
	instance->menu[TITLE_MENU_CONTROL_MOUSE] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_MOUSE], NULL, "Move Up", (void **)&instance->font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_MOUSE], menu_proc_set_move_up, instance->menu_text[1], (void **)&instance->font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_MOUSE], NULL, "Move Down", (void **)&instance->font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_MOUSE], menu_proc_set_move_down, instance->menu_text[2], (void **)&instance->font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_MOUSE], NULL, "Move Left", (void **)&instance->font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_MOUSE], menu_proc_set_move_left, instance->menu_text[3], (void **)&instance->font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_MOUSE], NULL, "Move Right", (void **)&instance->font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_MOUSE], menu_proc_set_move_right, instance->menu_text[4], (void **)&instance->font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_MOUSE], menu_proc_back, "Done", (void **)&instance->font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(instance->menu[TITLE_MENU_CONTROL_MOUSE], 20, 480);

	oy = 0;
	instance->menu[TITLE_MENU_CONTROL_ANALOG] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_ANALOG], NULL, "Move Vertically", (void **)&instance->font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_ANALOG], menu_proc_set_move_vertical, instance->menu_text[1], (void **)&instance->font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_ANALOG], NULL, "Move Horizontally", (void **)&instance->font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_ANALOG], menu_proc_set_move_horizontal, instance->menu_text[2], (void **)&instance->font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_ANALOG], NULL, "Fire Vertically", (void **)&instance->font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_ANALOG], menu_proc_set_fire_vertical, instance->menu_text[3], (void **)&instance->font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_ANALOG], NULL, "Fire Horizontally", (void **)&instance->font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_ANALOG], menu_proc_set_fire_horizontal, instance->menu_text[4], (void **)&instance->font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_CONTROL_ANALOG], menu_proc_back, "Done", (void **)&instance->font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(instance->menu[TITLE_MENU_CONTROL_ANALOG], 20, 480);

	oy = 0;
	instance->menu[TITLE_MENU_ANALOG] = t3f_create_gui(0, 0);
//	t3f_add_gui_text_element(instance->menu[TITLE_MENU_ANALOG], NULL, instance->menu_text[14], (void **)&instance->font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
//	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_ANALOG], NULL, instance->menu_text[15], (void **)&instance->font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_ANALOG], menu_proc_analog_done, "Done", (void **)&instance->font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(instance->menu[TITLE_MENU_ANALOG], 20, 480);

	oy = 0;
	instance->menu[TITLE_MENU_INTERNET] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_INTERNET], NULL, "Upload Scores", (void **)&instance->font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_INTERNET], NULL, instance->menu_text[9], (void **)&instance->font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_INTERNET], menu_proc_upload_toggle, "<", (void **)&instance->font[FONT_LARGE], 320 - t3f_get_text_width(instance->font[FONT_LARGE], "Yes") / 2 - t3f_get_text_width(instance->font[FONT_LARGE], "<"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_INTERNET], menu_proc_upload_toggle, ">", (void **)&instance->font[FONT_LARGE], 320 + t3f_get_text_width(instance->font[FONT_LARGE], "Yes") / 2 + t3f_get_text_width(instance->font[FONT_LARGE], ">"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_INTERNET], menu_proc_network_id, "Network ID", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(instance->menu[TITLE_MENU_INTERNET], menu_proc_back, "Done", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	menu_fix_internet_text(data);
	t3f_center_gui(instance->menu[TITLE_MENU_INTERNET], 20, 480);

	oy = 0;
	instance->menu[TITLE_MENU_NETWORK_ID] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_NETWORK_ID], NULL, "Network ID", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_NETWORK_ID], NULL, instance->network_id, (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(instance->menu[TITLE_MENU_NETWORK_ID], menu_proc_back, "Done", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	menu_fix_internet_text(data);
	#ifdef T3F_ANDROID
		t3f_center_gui(instance->menu[TITLE_MENU_NETWORK_ID], 20 + t3f_default_view->top, 240);
	#else
		t3f_center_gui(instance->menu[TITLE_MENU_NETWORK_ID], 20, 480);
	#endif

	oy = 0;
	instance->menu[TITLE_MENU_DISPLAY] = t3f_create_gui(0, 0);
	#ifndef PANDORA
		#ifndef T3F_ANDROID
			t3f_add_gui_text_element(instance->menu[TITLE_MENU_DISPLAY], NULL, "Display Type", (void **)&instance->font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
			oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
			t3f_add_gui_text_element(instance->menu[TITLE_MENU_DISPLAY], NULL, instance->menu_text[12], (void **)&instance->font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
			t3f_add_gui_text_element(instance->menu[TITLE_MENU_DISPLAY], menu_proc_display_left, "<", (void **)&instance->font[FONT_LARGE], 320 - t3f_get_text_width(instance->font[FONT_LARGE], "Medium Window") / 2 - t3f_get_text_width(instance->font[FONT_LARGE], "<"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
			t3f_add_gui_text_element(instance->menu[TITLE_MENU_DISPLAY], menu_proc_display_right, ">", (void **)&instance->font[FONT_LARGE], 320 + t3f_get_text_width(instance->font[FONT_LARGE], "Medium Window") / 2 + t3f_get_text_width(instance->font[FONT_LARGE], ">"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
			oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
		#endif
	#endif
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_DISPLAY], NULL, "Force 4:3 Aspect Ratio", (void **)&instance->font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_DISPLAY], NULL, instance->menu_text[13], (void **)&instance->font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_DISPLAY], menu_proc_display_toggle, "<", (void **)&instance->font[FONT_LARGE], 320 - t3f_get_text_width(instance->font[FONT_LARGE], "Yes") / 2 - t3f_get_text_width(instance->font[FONT_LARGE], "<"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_DISPLAY], menu_proc_display_toggle, ">", (void **)&instance->font[FONT_LARGE], 320 + t3f_get_text_width(instance->font[FONT_LARGE], "Yes") / 2 + t3f_get_text_width(instance->font[FONT_LARGE], ">"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(instance->menu[TITLE_MENU_DISPLAY], menu_proc_back, "Done", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	menu_fix_display_text(data);
	t3f_center_gui(instance->menu[TITLE_MENU_DISPLAY], 20, 480);

	oy = 0;
	instance->menu[LEADERBOARD_MENU] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(instance->menu[LEADERBOARD_MENU], NULL, instance->menu_text[10], (void **)&instance->font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	t3f_add_gui_text_element(instance->menu[LEADERBOARD_MENU], menu_proc_leaderboard_mode_left, "<", (void **)&instance->font[FONT_LARGE], 320 - t3f_get_text_width(instance->font[FONT_LARGE], "Story Mode (Easy)") / 2 - t3f_get_text_width(instance->font[FONT_LARGE], "<"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(instance->menu[LEADERBOARD_MENU], menu_proc_leaderboard_mode_right, ">", (void **)&instance->font[FONT_LARGE], 320 + t3f_get_text_width(instance->font[FONT_LARGE], "Story Mode (Easy)") / 2 + t3f_get_text_width(instance->font[FONT_LARGE], ">"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(instance->menu[LEADERBOARD_MENU], menu_proc_leaderboard_done, "Done", (void **)&instance->font[FONT_LARGE], 320, 318, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#else
		t3f_add_gui_text_element(instance->menu[LEADERBOARD_MENU], NULL, "", (void **)&instance->font[FONT_LARGE], 320, 318, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	t3f_center_gui(instance->menu[LEADERBOARD_MENU], 20, 480);

	oy = 0;
	instance->menu[PAUSE_MENU] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(instance->menu[PAUSE_MENU], menu_proc_pause_resume, "Resume Game", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[PAUSE_MENU], menu_proc_pause_quit, "Quit", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(instance->menu[PAUSE_MENU], 20, 480);

	oy = 0;
	instance->menu[TITLE_MENU_FIRST] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_FIRST], NULL, "Would you like your scores", (void **)&instance->font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_FIRST], NULL, "to be uploaded to t3-i.com?", (void **)&instance->font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_FIRST], menu_proc_first_yes, "Yes", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += t3f_get_font_line_height(instance->font[FONT_LARGE]);
	t3f_add_gui_text_element(instance->menu[TITLE_MENU_FIRST], menu_proc_first_no, "No", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(instance->menu[TITLE_MENU_FIRST], 20, 480);

	total_height = (t3f_default_view->bottom - t3f_default_view->top) - t3f_get_font_line_height(instance->font[FONT_SMALL]);
	free_height = total_height - (t3f_get_bitmap_height(instance->animation[ANIMATION_TITLE_EYES]->data->bitmaps->bitmap[0]) + t3f_get_font_line_height(instance->font[FONT_LARGE]));
	space = free_height / 3;
	instance->logo_pos_x = 320 - t3f_get_bitmap_width(instance->animation[ANIMATION_TITLE_EYES]->data->bitmaps->bitmap[0]) / 2;
	instance->logo_pos_y = t3f_default_view->top + space;
	oy = t3f_default_view->top + space + t3f_get_bitmap_height(instance->animation[ANIMATION_TITLE_EYES]->data->bitmaps->bitmap[0]) + space;
	instance->menu[MENU_TITLE] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(instance->menu[MENU_TITLE], menu_proc_game, "Game", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);

	oy = t3f_default_view->bottom - t3f_get_font_line_height(instance->font[FONT_LARGE]);
	instance->menu[MENU_PRIVACY] = t3f_create_gui(0, 0);
//	t3f_add_gui_text_element(instance->menu[TITLE_MENU_ANALOG], NULL, instance->menu_text[14], (void **)&instance->font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
//	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
//	t3f_add_gui_text_element(instance->menu[MENU_PRIVACY], NULL, instance->menu_text[15], (void **)&instance->font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
//	oy += t3f_get_font_line_height(instance->font[FONT_SMALL]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(instance->menu[MENU_PRIVACY], menu_proc_back, "Done", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#else
		t3f_add_gui_text_element(instance->menu[MENU_PRIVACY], menu_proc_back, "", (void **)&instance->font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
//	t3f_center_gui(instance->menu[TITLE_MENU_ANALOG], 20, 480);

	return true;
}

void title_exit(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;

	for(i = 0; i < TITLE_MAX_MENUS; i++)
	{
		if(instance->menu[i])
		{
			t3f_destroy_gui(instance->menu[i]);
			instance->menu[i] = NULL;
		}
	}
}

void title_bg_logic(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	title_flicker++;
}

void title_bg_render(T3F_BITMAP * bp)
{
	float alpha;

	t3f_draw_bitmap(bp, t3f_color_white, 0, 0, 0, 0);
	alpha = (float)(title_flicker % 512) / 512.0;
	t3f_draw_bitmap(bp, al_map_rgba_f(alpha, alpha, alpha, alpha), 0, 0, (float)(title_flicker % 512) / 2.0 - 256.0, 0);
	alpha = 1.0 - (float)(title_flicker % 512) / 512.0;
	t3f_draw_bitmap(bp, al_map_rgba_f(alpha, alpha, alpha, alpha), 0, 0, (float)-(title_flicker % 512) / 2.0, 0);
}

static bool check_for_skip(void)
{
	int i;

	if(t3f_char_in_buffer())
	{
		return true;
	}
	if(title_joystick_button_pressed())
	{
		button_press_counter = 100;
		block_buttons = true;
		return true;
	}
	for(i = 0; i < 16; i++)
	{
		if(t3f_mouse_button_pressed(i))
		{
			return true;
		}
	}
	for(i = 0; i < T3F_MAX_TOUCHES; i++)
	{
		if(t3f_touch_pressed(i))
		{
			return true;
		}
	}
	return false;
}

static void clear_mouse_buttons(void)
{
	int i;

	for(i = 0; i < 16; i++)
	{
		t3f_use_mouse_button_press(i);
	}
}

void title_in_logic(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	if(check_for_skip())
	{
		instance->state_ticks = 240;
	}
	title_bg_logic(data);
	instance->state_ticks++;
	if(instance->state_ticks >= 240)
	{
		clear_mouse_buttons();
		t3f_clear_touch_state();
		t3f_clear_controller_state(instance->controller);
		t3f_select_next_gui_element(instance->menu[MENU_TITLE]);
		instance->state = STATE_TITLE;
		instance->state_ticks = 0;
	}
}

void title_in_render(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	float alpha;
	float x, y;

	x = instance->logo_pos_x;
	y = instance->logo_pos_y;
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	if(instance->state_ticks < 60)
	{
		alpha = (float)(instance->state_ticks) / 60.0;
		t3f_draw_animation(instance->animation[ANIMATION_TITLE_EYES], al_map_rgba_f(alpha, alpha, alpha, alpha), instance->state_ticks, x, y, 0, 0);
	}
	else if(instance->state_ticks < 180)
	{
		t3f_draw_animation(instance->animation[ANIMATION_TITLE_EYES], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), instance->state_ticks, x, y, 0, 0);
	}
	else
	{
		title_bg_render(instance->bitmap[0]);
		t3f_draw_animation(instance->animation[ANIMATION_TITLE_EYES], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), instance->state_ticks, x + 4, y + 4, 0, 0);
		t3f_draw_animation(instance->animation[ANIMATION_TITLE], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), instance->state_ticks, x + 4, y + 4, 0, 0);
		t3f_draw_animation(instance->animation[ANIMATION_TITLE], t3f_color_white, instance->state_ticks, x, y, 0, 0);
		al_hold_bitmap_drawing(false);
		al_draw_filled_rectangle(0.0, 0.0, 640, 480, al_map_rgba_f(0.0, 0.0, 0.0, 1.0 - (float)(instance->state_ticks - 180) / 60.0));
		t3f_draw_animation(instance->animation[ANIMATION_TITLE_EYES], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), instance->state_ticks, x, y, 0, 0);
	}
}

void title_name_entry_logic(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int input;

	input = t3f_get_char(0);
	if(input)
	{
		if(input == '\b' || input == 127)
		{
			if(instance->network_id_pos > 0)
			{
				instance->network_id_pos--;
				instance->network_id[instance->network_id_pos] = '\0';
			}
		}
		else if(input == '\r')
		{
			instance->network_id_entry = false;
			if(strlen(instance->network_id) <= 0)
			{
				strcpy(instance->network_id, "Anonymous");
			}
			menu_fix_internet_text(data);
			menu_proc_back(NULL, 0, data);
			t3f_use_key_press(ALLEGRO_KEY_ENTER);
			t3f_show_soft_keyboard(false);
		}
		else if(input >= 32 && input < 127)
		{
			instance->network_id[instance->network_id_pos] = input;
			instance->network_id_pos++;
			instance->network_id[instance->network_id_pos] = '\0';
		}
	}
}

bool title_joystick_button_pressed(void)
{
	int i, j;

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
	if(t3f_key_pressed(ALLEGRO_KEY_ENTER))
	{
		t3f_use_key_press(ALLEGRO_KEY_ENTER);
		return true;
	}
	return false;
}

bool title_touched(void)
{
	int i;

	for(i = 0; i < T3F_MAX_TOUCHES; i++)
	{
		if(t3f_touch_pressed(i))
		{
			t3f_use_touch_press(i);
			return true;
		}
	}
	return false;
}

void title_joystick_logic(T3F_GUI * mp, void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	bool pressed = false;

	switch(instance->controller_type)
	{
		case CONTROLLER_TYPE_ANALOG:
		{
			t3f_read_controller(instance->controller);

			/* don't change selection when viewing analog config menu */
			if(instance->current_menu != TITLE_MENU_ANALOG)
			{
				if(instance->controller->state[0].pos < -0.2)
				{
					button_press_counter++;
					if(button_press_counter == 1)
					{
						t3f_select_previous_gui_element(mp);
					}
					instance->mouse_disabled = true;
					pressed = true;
				}
				else if(instance->controller->state[0].pos > 0.2)
				{
					button_press_counter++;
					if(button_press_counter == 1)
					{
						t3f_select_next_gui_element(mp);
					}
					instance->mouse_disabled = true;
					pressed = true;
				}
			}
			if(title_joystick_button_pressed())
			{
				button_press_counter++;
				pressed = true;
			}
			if(!pressed)
			{
				button_press_counter = 0;
			}
			if(title_joystick_button_pressed() && button_press_counter == 1)
			{
				t3f_activate_selected_gui_element(mp, NULL);
			}
			break;
		}
		case CONTROLLER_TYPE_NORMAL:
		{
			t3f_read_controller(instance->controller);
			t3f_update_controller(instance->controller);
			if(!block_buttons)
			{
				if(instance->controller->state[0].pressed)
				{
					t3f_select_previous_gui_element(mp);
				}
				if(instance->controller->state[1].pressed)
				{
					t3f_select_next_gui_element(mp);
				}
				if(instance->controller->state[4].pressed || instance->controller->state[5].pressed || instance->controller->state[6].pressed || instance->controller->state[7].pressed || t3f_key_pressed(ALLEGRO_KEY_ENTER))
				{
					t3f_activate_selected_gui_element(mp, data);
					t3f_use_key_press(ALLEGRO_KEY_ENTER);
					block_buttons = true;
				}
			}
			else
			{
				if(!instance->controller->state[4].pressed && !instance->controller->state[5].pressed && !instance->controller->state[6].pressed && !instance->controller->state[7].pressed && !t3f_key_pressed(ALLEGRO_KEY_ENTER))
				{
					t3f_use_key_press(ALLEGRO_KEY_ENTER);
					block_buttons = false;
				}
			}
			break;
		}
	}
}

void title_process_menu(T3F_GUI * mp, void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	int i;

	title_joystick_logic(mp, data);
	if(!instance->mouse_disabled)
	{
		t3f_process_gui(mp, 0, data);
	}
	for(i = 0; i < mp->elements; i++)
	{
		if(!(mp->element[i].flags & T3F_GUI_ELEMENT_STATIC))
		{
			if(i == mp->hover_element)
			{
				mp->element[i].color = al_map_rgba_f(1.0, 0.0, 0.0, 1.0);
			}
			else
			{
				mp->element[i].color = al_map_rgba_f(0.75, 0.0, 0.0, 1.0);
			}
		}
	}
}

/* need to make this not rely on spaces, sometimes there might be long stretches with no space which need to be broken up 'mid-word' */
void create_text_line_data(TEXT_LINE_DATA * lp, T3F_FONT * fp, float w, float tab, const char * text)
{
	char current_line[256];
	int current_line_pos = 0;
	int current_line_start_pos = 0;
	int last_space = -1;
	int i;
	float wi = w;

	lp->font = fp;
	lp->tab = tab;
	lp->lines = 0;
	strcpy(lp->line[lp->lines].text, "");
	if(strlen(text) < 1)
	{
		return;
	}

	/* divide text into lines */
	for(i = 0; i < (int)strlen(text); i++)
	{
		current_line[current_line_pos] = text[i];
		current_line[current_line_pos + 1] = '\0';
		if(text[i] == ' ')
		{
			last_space = current_line_pos;
		}
		current_line_pos++;

		/* copy line since we encountered a manual new line */
		if(text[i] == '\n')
		{
			if(current_line_pos > 0)
			{
				current_line[current_line_pos - 1] = '\0';
			}
			strcpy(lp->line[lp->lines].text, current_line);
			current_line_start_pos += i + 1;
			lp->lines++;
			strcpy(lp->line[lp->lines].text, "");
			current_line_pos = 0;
			current_line[current_line_pos] = '\0';
			wi = w - tab;
		}

		/* copy this line to our list of lines because it is long enough */
		else if(t3f_get_text_width(fp, current_line) > wi)
		{
			current_line[last_space] = '\0';
			strcpy(lp->line[lp->lines].text, current_line);
			current_line_start_pos += last_space + 1;
			while(text[i] != ' ' && i >= 0)
			{
				i--;
			}
			lp->lines++;
			strcpy(lp->line[lp->lines].text, "");
			current_line_pos = 0;
			current_line[current_line_pos] = '\0';
			wi = w - tab;
		}
	}
	strcpy(lp->line[lp->lines].text, current_line);
	lp->lines++;
}

void draw_text_lines(TEXT_LINE_DATA * lines, ALLEGRO_COLOR color, float x, float y)
{
	int i;
	float px = x;
	float py = y;

	for(i = 0; i < lines->lines; i++)
	{
		t3f_draw_text(lines->font, color, px, py, 0, 0, lines->line[i].text);
		px = x + lines->tab;
		py += t3f_get_font_line_height(lines->font);
	}
}

void draw_multiline_text(T3F_FONT * fp, ALLEGRO_COLOR color, float x, float y, float w, float tab, int flags, const char * text)
{
	TEXT_LINE_DATA line_data;
	float pos = x;
	bool held;

	if(strlen(text) < 1)
	{
		return;
	}
	held = al_is_bitmap_drawing_held();
	if(!held)
	{
		al_hold_bitmap_drawing(true);
	}
	if(flags & T3F_FONT_ALIGN_CENTER)
	{
		pos -= t3f_get_text_width(fp, text) / 2.0;
	}
	else if(flags & T3F_FONT_ALIGN_RIGHT)
	{
		pos -= t3f_get_text_width(fp, text);
	}
	if(w > 0.0)
	{
		create_text_line_data(&line_data, fp, w, tab, text);
		draw_text_lines(&line_data, color, x, y);
	}
	else
	{
		t3f_draw_text(fp, color, x, y, 0, 0, text);
	}
	if(!held)
	{
		al_hold_bitmap_drawing(false);
	}
}

static float get_copyright_message_width(T3F_FONT * fp, T3F_FONT * fp2, const char * text)
{
	return t3f_get_text_width(fp, text);
}

static void draw_copyright_message(T3F_FONT * fp, T3F_FONT * fp2, ALLEGRO_COLOR color, float x, float y, int flags, const char * text)
{
	t3f_draw_text(fp, color, x, y, 0, flags, text);
}

void title_render(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;
	float x, y;
	float w;

	title_bg_render(instance->bitmap[0]);

	/* render title logo if we are on title menu */
	if(instance->current_menu == MENU_TITLE)
	{
		x = instance->logo_pos_x;
		y = instance->logo_pos_y;
		t3f_draw_animation(instance->animation[ANIMATION_TITLE_EYES], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), instance->state_ticks, x + 4, y + 4, 0, 0);
		t3f_draw_animation(instance->animation[ANIMATION_TITLE], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), instance->state_ticks, x + 4, y + 4, 0, 0);
		t3f_draw_animation(instance->animation[ANIMATION_TITLE_EYES], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), instance->state_ticks, x, y, 0, 0);
		t3f_draw_animation(instance->animation[ANIMATION_TITLE], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), instance->state_ticks, x, y, 0, 0);
		draw_copyright_message(instance->font[FONT_SMALL], instance->font[FONT_TINY], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 320, t3f_default_view->bottom - t3f_get_font_line_height(instance->font[FONT_SMALL]), T3F_FONT_ALIGN_CENTER, instance->copyright_message);
		draw_copyright_message(instance->font[FONT_SMALL], instance->font[FONT_TINY], t3f_color_white, 320 - 2, t3f_default_view->bottom - t3f_get_font_line_height(instance->font[FONT_SMALL]) - 2, T3F_FONT_ALIGN_CENTER, instance->copyright_message);
	}
	else if(instance->current_menu == MENU_PRIVACY)
	{
		w = t3f_default_view->right - t3f_default_view->left;
		draw_multiline_text(instance->font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), t3f_default_view->left + 6.0, t3f_default_view->top + 6.0, w, 0.0, 0, privacy_text);
		draw_multiline_text(instance->font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), t3f_default_view->left + 4.0, t3f_default_view->top + 4.0, w, 0.0, 0, privacy_text);
	}

	t3f_render_gui(instance->menu[instance->current_menu], 0);

	/* render blinking cursor */
	if(instance->current_menu == TITLE_MENU_NETWORK_ID && instance->network_id_entry)
	{
		if(instance->state_ticks % 20 < 10)
		{
			t3f_draw_text(instance->font[FONT_LARGE], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), instance->menu[instance->current_menu]->ox + instance->menu[instance->current_menu]->element[1].ox + t3f_get_text_width(instance->font[FONT_LARGE], instance->network_id) / 2, instance->menu[instance->current_menu]->oy + instance->menu[instance->current_menu]->element[1].oy, 0, 0, "_");
			t3f_draw_text(instance->font[FONT_LARGE], t3f_color_white, instance->menu[instance->current_menu]->ox + instance->menu[instance->current_menu]->element[1].ox + t3f_get_text_width(instance->font[FONT_LARGE], instance->network_id) / 2 - 2, instance->menu[instance->current_menu]->oy + instance->menu[instance->current_menu]->element[1].oy - 2, 0, 0, "_");
		}
	}
	render_mouse(data);
}

void title_logic(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->state_ticks++;
	title_bg_logic(data);
	if(instance->current_menu == TITLE_MENU_ANALOG)
	{
		menu_analog_logic(data);
	}
	if(instance->network_id_entry)
	{
		title_name_entry_logic(data);
	}
	if(t3f_key_pressed(ALLEGRO_KEY_ESCAPE) || t3f_key_pressed(ALLEGRO_KEY_BACK))
	{
		t3f_use_key_press(ALLEGRO_KEY_ESCAPE);
		t3f_use_key_press(ALLEGRO_KEY_BACK);
		menu_proc_back(NULL, 0, data);
	}
	else
	{
		title_process_menu(instance->menu[instance->current_menu], data);
	}
}

void title_out_logic(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	instance->state_ticks++;
	title_bg_logic(data);
	if(instance->state_ticks >= 60)
	{
		t3f_exit();
	}
}

void title_out_render(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	al_hold_bitmap_drawing(true);
	title_bg_render(instance->bitmap[0]);
	t3f_render_gui(instance->menu[instance->current_menu], 0);
	al_hold_bitmap_drawing(false);
	al_draw_filled_rectangle(0.0, 0.0, 640, 480, al_map_rgba_f(0.0, 0.0, 0.0, (float)instance->state_ticks / 60.0));
}

void title_game_logic(void * data)
{
	int i;

	for(i = 0; i < 8; i++)
	{
		title_bg_logic(data);
		if(title_flicker % 512 == 0)
		{
			title_free_data(data);
			if(!game_init(title_game_mode, data))
			{
				printf("Failed to initialize gameplay!\n");
			}
			break;
		}
	}
}

void title_game_render(void * data)
{
	APP_INSTANCE * instance = (APP_INSTANCE *)data;

	al_hold_bitmap_drawing(true);
	title_bg_render(instance->bitmap[0]);
	al_hold_bitmap_drawing(false);
}
