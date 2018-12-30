#include "t3f/t3f.h"
#include "t3f/gui.h"
#include "t3f/resource.h"
#include "t3f/view.h"
#include "t3f/draw.h"
#include "t3f/android.h"
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

static void menu_fix_controller_type_text(void)
{
	switch(controller_type)
	{
		case CONTROLLER_TYPE_NORMAL:
		{
			sprintf(menu_text[0], "Normal");
			break;
		}
		case CONTROLLER_TYPE_MOUSE:
		{
			sprintf(menu_text[0], "Mouse + Keyboard");
			break;
		}
		case CONTROLLER_TYPE_ANALOG:
		{
			sprintf(menu_text[0], "Dual Analog");
			break;
		}
		case CONTROLLER_TYPE_TOUCH_S:
		{
			sprintf(menu_text[0], "Touch Small");
			break;
		}
		case CONTROLLER_TYPE_TOUCH_M:
		{
			sprintf(menu_text[0], "Touch Medium");
			break;
		}
		case CONTROLLER_TYPE_TOUCH_L:
		{
			sprintf(menu_text[0], "Touch Large");
			break;
		}
	}
}

void menu_fix_controller_type_config(void)
{
	char buffer[32] = {0};

	sprintf(buffer, "%d", controller_type);
	al_set_config_value(t3f_config, "Controls", "Type", buffer);
}

void menu_fix_leaderboard_text(void)
{
	switch(game_mode)
	{
		case GAME_MODE_STORY:
		{
			sprintf(menu_text[10], "Story Mode");
			break;
		}
		case GAME_MODE_STORY_EASY:
		{
			sprintf(menu_text[10], "Story Mode (Easy)");
			break;
		}
		case GAME_MODE_ETERNAL:
		{
			sprintf(menu_text[10], "Eternal Mode");
			break;
		}
	}
}

void menu_fix_internet_text(void)
{
	sprintf(menu_text[9], "%s", upload_scores ? "Yes" : "No");
}

void menu_fix_display_text(void)
{
	char * display_type_name[4] = {"Small Window", "Medium Window", "Large Window", "Full Screen"};

	sprintf(menu_text[12], "%s", display_type_name[display_type]);
	sprintf(menu_text[13], "%s", force_aspect ? "Yes" : "No");
}

void menu_analog_logic(void)
{
	int dir = 0;

	switch(analog_type)
	{
		case 0:
		{
			sprintf(menu_text[14], "Configure Vertical Movement Axis");
			break;
		}
		case 1:
		{
			sprintf(menu_text[14], "Configure Horizontal Movement Axis");
			dir = 1;
			break;
		}
		case 2:
		{
			sprintf(menu_text[14], "Configure Vertical Firing Axis");
			break;
		}
		case 3:
		{
			sprintf(menu_text[14], "Configure Horizontal Firing Axis");
			dir = 1;
			break;
		}
	}
	t3f_read_controller(controller);
	switch(analog_state)
	{
		case 0:
		{
			if(controller->state[analog_type * 2].pos < analog_max)
			{
				analog_max = controller->state[analog_type * 2].pos;
			}
			sprintf(menu_text[15], "Move Axis All the Way %s (%0.2f)", dir == 0 ? "Up" : "Left", analog_max);
			break;
		}
		case 1:
		{
			if(controller->state[analog_type * 2].pos > analog_max)
			{
				analog_max = controller->state[analog_type * 2].pos;
			}
			sprintf(menu_text[15], "Move Axis All the Way %s (%0.2f)", dir == 0 ? "Down" : "Right", analog_max);
			break;
		}
		case 2:
		{
			sprintf(menu_text[15], "Allow Axis to Rest (%0.2f)", controller->state[analog_type * 2].pos);
			break;
		}
	}
}

void select_menu(int mmenu)
{
	current_menu = mmenu;
	menu[current_menu]->hover_element = -1;
	#ifndef T3F_ANDROID
		t3f_select_next_gui_element(menu[current_menu]);
	#endif
}

int menu_proc_game(void * data, int i, void * p)
{
	select_menu(TITLE_MENU_MAIN);
	return 1;
}

int menu_proc_play(void * data, int i, void * p)
{
	select_menu(TITLE_MENU_MODE);
	return 1;
}

int menu_proc_story(void * data, int i, void * p)
{
//	title_free_data();
	cinema = load_cinema("data/cinema/intro.cin", 0);
	cinema->position = 0;
	cinema->tick = 0;
	click = true;
	state = STATE_INTRO;
	return 1;
}

int menu_proc_leaderboards(void * data, int i, void * p)
{
	download_leaderboard();
	leaderboard_place = -1;
	menu_fix_leaderboard_text();
	al_start_timer(t3f_timer);
	if(leaderboard)
	{
		state = STATE_LEADERBOARDS;
	}
	return 1;
}

int menu_proc_settings(void * data, int i, void * p)
{
	select_menu(TITLE_MENU_SETTINGS);
	return 1;
}

int menu_proc_privacy(void * data, int i, void * p)
{
	select_menu(MENU_PRIVACY);
	return 1;
}

int menu_proc_quit(void * data, int i, void * p)
{
	state_ticks = 0;
	state = STATE_TITLE_OUT;
	return 1;
}

int menu_proc_play_story(void * data, int i, void * p)
{
	select_menu(TITLE_MENU_DIFFICULTY);
	return 1;
}

int menu_proc_play_easy(void * data, int i, void * p)
{
	enemy_spawn_logic = enemy_spawn_logic_easy;
	title_game_mode = GAME_MODE_STORY_EASY;
	state = STATE_TITLE_GAME;
	return 1;
}

int menu_proc_play_normal(void * data, int i, void * p)
{
	enemy_spawn_logic = enemy_spawn_logic_normal;
	title_game_mode = GAME_MODE_STORY;
	state = STATE_TITLE_GAME;
	return 1;
}

int menu_proc_play_eternal(void * data, int i, void * p)
{
	enemy_spawn_logic = enemy_spawn_logic_normal;
	title_game_mode = GAME_MODE_ETERNAL;
	state = STATE_TITLE_GAME;
	return 1;
}

int menu_proc_back(void * data, int i, void * p)
{
	switch(current_menu)
	{
		case TITLE_MENU_INTERNET:
		case TITLE_MENU_CONTROL:
		case TITLE_MENU_DISPLAY:
		{
			select_menu(TITLE_MENU_SETTINGS);
			menu_fix_internet_text();
			break;
		}
		case TITLE_MENU_NETWORK_ID:
		{
			select_menu(TITLE_MENU_INTERNET);
			al_set_config_value(t3f_config, "Network", "ID", network_id);
			al_set_config_value(t3f_config, "Network", "Upload", upload_scores ? "true" : "false");
			network_id_entry = false;
			menu_fix_internet_text();
			break;
		}
		case TITLE_MENU_CONTROL_NORMAL:
		case TITLE_MENU_CONTROL_MOUSE:
		case TITLE_MENU_CONTROL_ANALOG:
		{
			switch(controller_type)
			{
				case CONTROLLER_TYPE_NORMAL:
				{
					t3f_write_controller_config(t3f_config, "Normal Controls", controller);
					break;
				}
				case CONTROLLER_TYPE_MOUSE:
				{
					t3f_write_controller_config(t3f_config, "Mouse Controls", controller);
					break;
				}
				case CONTROLLER_TYPE_ANALOG:
				{
					t3f_write_controller_config(t3f_config, "Analog Controls", controller);
					break;
				}
			}
			menu_fix_controller_type_text();
			select_menu(TITLE_MENU_CONTROL);
			break;
		}
		case TITLE_MENU_DIFFICULTY:
		{
			select_menu(TITLE_MENU_MODE);
			break;
		}
		case TITLE_MENU_MAIN:
		{
			select_menu(MENU_TITLE);
			break;
		}
		case MENU_TITLE:
		{
			t3f_exit();
			break;
		}
		default:
		{
			select_menu(TITLE_MENU_MAIN);
			break;
		}
	}
	return 1;
}

#ifdef T3F_ANDROID
	static void fix_touch_size(void)
	{
		switch(controller_type)
		{
			case CONTROLLER_TYPE_TOUCH_S:
			{
				touch_size = 48.0;
				break;
			}
			case CONTROLLER_TYPE_TOUCH_M:
			{
				touch_size = 64.0;
				break;
			}
			case CONTROLLER_TYPE_TOUCH_L:
			{
				touch_size = 96.0;
				break;
			}
		}
	}
#endif

int menu_proc_controller_type_left(void * data, int i, void * p)
{
	char * controller_section[3] = {"Normal Controls", "Mouse Controls", "Analog Controls"};

	controller_type--;
	#ifndef T3F_ANDROID
		if(controller_type < 0)
		{
			controller_type = CONTROLLER_TYPE_ANALOG;
		}
	#else
		if(controller_type < CONTROLLER_TYPE_TOUCH_S)
		{
			controller_type = CONTROLLER_TYPE_TOUCH_L;
		}
		fix_touch_size();
	#endif
	if(controller_type == CONTROLLER_TYPE_ANALOG)
	{
		if(al_get_num_joysticks() <= 0)
		{
			controller_type = CONTROLLER_TYPE_MOUSE;
		}
	}
	if(controller_type < CONTROLLER_TYPE_TOUCH_S)
	{
		if(!t3f_read_controller_config(t3f_config, controller_section[controller_type], controller))
		{
			detect_controller(controller_type);
		}
	}
	menu_fix_controller_type_text();
	menu_fix_controller_type_config();
	return 1;
}

int menu_proc_controller_type_right(void * data, int i, void * p)
{
	char * controller_section[3] = {"Normal Controls", "Mouse Controls", "Analog Controls"};

	controller_type++;
	#ifndef T3F_ANDROID
		if(controller_type > CONTROLLER_TYPE_ANALOG)
		{
			controller_type = 0;
		}
	#else
		if(controller_type > CONTROLLER_TYPE_TOUCH_L)
		{
			controller_type = CONTROLLER_TYPE_TOUCH_S;
		}
		fix_touch_size();
	#endif
	if(controller_type == CONTROLLER_TYPE_ANALOG)
	{
		if(al_get_num_joysticks() <= 0)
		{
			controller_type = CONTROLLER_TYPE_NORMAL;
		}
	}
	if(controller_type < CONTROLLER_TYPE_TOUCH_S)
	{
		if(!t3f_read_controller_config(t3f_config, controller_section[controller_type], controller))
		{
			detect_controller(controller_type);
		}
	}
	menu_fix_controller_type_text();
	menu_fix_controller_type_config();
	return 1;
}

static void update_controller_menu_text(void)
{
	int j;

	switch(controller_type)
	{
		case CONTROLLER_TYPE_NORMAL:
		{
			for(j = 0; j < 8; j++)
			{
				sprintf(menu_text[j + 1], "%s", t3f_get_controller_binding_name(controller, j));
			}
			break;
		}
		case CONTROLLER_TYPE_MOUSE:
		{
			for(j = 0; j < 4; j++)
			{
				sprintf(menu_text[j + 1], "%s", t3f_get_controller_binding_name(controller, j));
			}
			break;
		}
		case CONTROLLER_TYPE_ANALOG:
		{
			for(j = 0; j < 4; j++)
			{
				sprintf(menu_text[j + 1], "%s", t3f_get_controller_binding_name(controller, j * 2));
			}
			break;
		}
	}
}

int menu_proc_controls(void * data, int i, void * p)
{
	select_menu(TITLE_MENU_CONTROL);
	return 1;
}

int menu_proc_internet(void * data, int i, void * p)
{
	select_menu(TITLE_MENU_INTERNET);
	return 1;
}

int menu_proc_display(void * data, int i, void * p)
{
	select_menu(TITLE_MENU_DISPLAY);
	return 1;
}

int menu_proc_controller_configure(void * data, int i, void * p)
{
	switch(controller_type)
	{
		case CONTROLLER_TYPE_NORMAL:
		{
			select_menu(TITLE_MENU_CONTROL_NORMAL);
			break;
		}
		case CONTROLLER_TYPE_MOUSE:
		{
			select_menu(TITLE_MENU_CONTROL_MOUSE);
			break;
		}
		case CONTROLLER_TYPE_ANALOG:
		{
			select_menu(TITLE_MENU_CONTROL_ANALOG);
			break;
		}
	}
	update_controller_menu_text();
	return 1;
}

int menu_proc_controller_autodetect(void * data, int i, void * p)
{
	detect_controller(-1);
	menu_proc_controller_configure(data, i, p);
	return 1;
}

static void bind_input(int slot)
{
	t3f_bind_controller(controller, slot);
	t3f_process_events(true);
	t3f_mouse_button[0] = false;
	update_controller_menu_text();
}

int menu_proc_set_move_up(void * data, int i, void * p)
{
	al_stop_timer(t3f_timer);
	sprintf(menu_text[1], "Select input...");
	t3f_render(NULL);
	bind_input(0);
	update_controller_menu_text();
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_move_down(void * data, int i, void * p)
{
	al_stop_timer(t3f_timer);
	sprintf(menu_text[2], "Select input...");
	t3f_render(NULL);
	bind_input(1);
	update_controller_menu_text();
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_move_left(void * data, int i, void * p)
{
	al_stop_timer(t3f_timer);
	sprintf(menu_text[3], "Select input...");
	t3f_render(NULL);
	bind_input(2);
	update_controller_menu_text();
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_move_right(void * data, int i, void * p)
{
	al_stop_timer(t3f_timer);
	sprintf(menu_text[4], "Select input...");
	t3f_render(NULL);
	bind_input(3);
	update_controller_menu_text();
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_fire_up(void * data, int i, void * p)
{
	al_stop_timer(t3f_timer);
	sprintf(menu_text[5], "Select input...");
	t3f_render(NULL);
	bind_input(4);
	update_controller_menu_text();
	block_buttons = true;
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_fire_down(void * data, int i, void * p)
{
	al_stop_timer(t3f_timer);
	sprintf(menu_text[6], "Select input...");
	t3f_render(NULL);
	bind_input(5);
	update_controller_menu_text();
	block_buttons = true;
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_fire_left(void * data, int i, void * p)
{
	al_stop_timer(t3f_timer);
	sprintf(menu_text[7], "Select input...");
	t3f_render(NULL);
	bind_input(6);
	update_controller_menu_text();
	block_buttons = true;
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_fire_right(void * data, int i, void * p)
{
	al_stop_timer(t3f_timer);
	sprintf(menu_text[8], "Select input...");
	t3f_render(NULL);
	bind_input(7);
	update_controller_menu_text();
	block_buttons = true;
	al_start_timer(t3f_timer);
	return 1;
}

int menu_proc_set_move_vertical(void * data, int i, void * p)
{
	T3F_CONTROLLER_BINDING old_binding;

	al_stop_timer(t3f_timer);
	sprintf(menu_text[1], "Move axis up...");
	t3f_render(NULL);
	memcpy(&old_binding, &controller->binding[4], sizeof(T3F_CONTROLLER_BINDING));
	bind_input(0);
	if(controller->binding[0].type != T3F_CONTROLLER_BINDING_JOYSTICK_AXIS)
	{
		memcpy(&controller->binding[0], &old_binding, sizeof(T3F_CONTROLLER_BINDING));
	}
	else
	{
		al_get_joystick_state(t3f_joystick[controller->binding[0].joystick], &t3f_joystick_state[controller->binding[0].joystick]); // read initial state
		t3f_read_controller(controller);
		if(controller->state[0].pos > 0)
		{
			controller->binding[0].flags |= T3F_CONTROLLER_FLAG_AXIS_INVERT;
		}
	}
	update_controller_menu_text();
	al_start_timer(t3f_timer);
	analog_state = 0;
	analog_type = 0;
	analog_max = 0.0;
	controller->binding[0].flags |= T3F_CONTROLLER_FLAG_AXIS_NO_ADJUST;
	select_menu(TITLE_MENU_ANALOG);
	return 1;
}

int menu_proc_set_move_horizontal(void * data, int i, void * p)
{
	T3F_CONTROLLER_BINDING old_binding;

	al_stop_timer(t3f_timer);
	sprintf(menu_text[2], "Move axis left...");
	t3f_render(NULL);
	memcpy(&old_binding, &controller->binding[4], sizeof(T3F_CONTROLLER_BINDING));
	bind_input(2);
	if(controller->binding[2].type != T3F_CONTROLLER_BINDING_JOYSTICK_AXIS)
	{
		memcpy(&controller->binding[2], &old_binding, sizeof(T3F_CONTROLLER_BINDING));
	}
	else
	{
		al_get_joystick_state(t3f_joystick[controller->binding[2].joystick], &t3f_joystick_state[controller->binding[2].joystick]); // read initial state
		t3f_read_controller(controller);
		if(controller->state[2].pos > 0)
		{
			controller->binding[2].flags |= T3F_CONTROLLER_FLAG_AXIS_INVERT;
		}
	}
	update_controller_menu_text();
	al_start_timer(t3f_timer);
	analog_state = 0;
	analog_type = 1;
	analog_max = 0.0;
	controller->binding[2].flags |= T3F_CONTROLLER_FLAG_AXIS_NO_ADJUST;
	select_menu(TITLE_MENU_ANALOG);
	return 1;
}

int menu_proc_set_fire_vertical(void * data, int i, void * p)
{
	T3F_CONTROLLER_BINDING old_binding;

	al_stop_timer(t3f_timer);
	sprintf(menu_text[3], "Move axis up...");
	t3f_render(NULL);
	memcpy(&old_binding, &controller->binding[4], sizeof(T3F_CONTROLLER_BINDING));
	bind_input(4);
	if(controller->binding[4].type != T3F_CONTROLLER_BINDING_JOYSTICK_AXIS)
	{
		memcpy(&controller->binding[4], &old_binding, sizeof(T3F_CONTROLLER_BINDING));
	}
	else
	{
		al_get_joystick_state(t3f_joystick[controller->binding[4].joystick], &t3f_joystick_state[controller->binding[4].joystick]); // read initial state
		t3f_read_controller(controller);
		if(controller->state[4].pos > 0)
		{
			controller->binding[4].flags |= T3F_CONTROLLER_FLAG_AXIS_INVERT;
		}
	}
	update_controller_menu_text();
	al_start_timer(t3f_timer);
	analog_state = 0;
	analog_type = 2;
	analog_max = 0.0;
	controller->binding[4].flags |= T3F_CONTROLLER_FLAG_AXIS_NO_ADJUST;
	select_menu(TITLE_MENU_ANALOG);
	return 1;
}

int menu_proc_set_fire_horizontal(void * data, int i, void * p)
{
	T3F_CONTROLLER_BINDING old_binding;

	al_stop_timer(t3f_timer);
	sprintf(menu_text[4], "Move axis left...");
	t3f_render(NULL);
	memcpy(&old_binding, &controller->binding[4], sizeof(T3F_CONTROLLER_BINDING));
	bind_input(6);
	if(controller->binding[6].type != T3F_CONTROLLER_BINDING_JOYSTICK_AXIS)
	{
		memcpy(&controller->binding[6], &old_binding, sizeof(T3F_CONTROLLER_BINDING));
	}
	else
	{
		al_get_joystick_state(t3f_joystick[controller->binding[6].joystick], &t3f_joystick_state[controller->binding[6].joystick]); // read initial state
		t3f_read_controller(controller);
		if(controller->state[6].pos > 0)
		{
			controller->binding[6].flags |= T3F_CONTROLLER_FLAG_AXIS_INVERT;
		}
	}
	update_controller_menu_text();
	al_start_timer(t3f_timer);
	analog_state = 0;
	analog_type = 3;
	analog_max = 0.0;
	controller->binding[6].flags |= T3F_CONTROLLER_FLAG_AXIS_NO_ADJUST;
	select_menu(TITLE_MENU_ANALOG);
	return 1;
}

int menu_proc_upload_toggle(void * data, int i, void * p)
{
	if(upload_scores)
	{
		upload_scores = false;
	}
	else
	{
		upload_scores = true;
	}
	menu_fix_internet_text();
	return 1;
}

#ifdef ALLEGRO_ANDROID
	static void menu_proc_network_id_callback(void * data)
	{
		if(strlen(network_id) <= 0)
		{
			strcpy(network_id, "Anonymous");
		}
		menu_proc_back(NULL, 0, NULL);
	}
#endif

int menu_proc_network_id(void * data, int i, void * p)
{
	select_menu(TITLE_MENU_NETWORK_ID);
	network_id_entry = true;
	network_id_pos = strlen(network_id);
	t3f_clear_keys();
	#ifdef ALLEGRO_ANDROID
		t3f_open_edit_box("Enter Name", network_id, 256, "CapWords", menu_proc_network_id_callback, NULL);
	#endif
	return 1;
}

int menu_proc_network_id_name(void * data, int i, void * p)
{
	return menu_proc_network_id(data, i, p);
}

int menu_proc_leaderboard_mode_left(void * data, int i, void * p)
{
	game_mode--;
	if(game_mode < 0)
	{
		game_mode = 2;
	}
	download_leaderboard();
	menu_fix_leaderboard_text();
	if(!leaderboard)
	{
		state = STATE_TITLE;
		select_menu(TITLE_MENU_MAIN);
	}
	return 1;
}

int menu_proc_leaderboard_mode_right(void * data, int i, void * p)
{
	game_mode++;
	if(game_mode > 2)
	{
		game_mode = 0;
	}
	download_leaderboard();
	menu_fix_leaderboard_text();
	if(!leaderboard)
	{
		state = STATE_TITLE;
		select_menu(TITLE_MENU_MAIN);
	}
	return 1;
}

int menu_proc_leaderboard_done(void * data, int i, void * p)
{
	state = STATE_TITLE;
	select_menu(TITLE_MENU_MAIN);
	return 1;
}

int menu_proc_pause_resume(void * data, int i, void * p)
{
	game_state = resume_state;
	return 1;
}

int menu_proc_pause_quit(void * data, int i, void * p)
{
	game_exit();
	return 1;
}

int menu_proc_first_yes(void * data, int i, void * p)
{
	upload_scores = true;
	al_set_config_value(t3f_config, "Network", "Upload", upload_scores ? "true" : "false");
	menu_fix_internet_text();
	select_menu(TITLE_MENU_INTERNET);
	return 1;
}

int menu_proc_first_no(void * data, int i, void * p)
{
	upload_scores = false;
	al_set_config_value(t3f_config, "Network", "Upload", upload_scores ? "true" : "false");
	menu_fix_internet_text();
	select_menu(TITLE_MENU_MAIN);
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

bool set_video_mode(int w, int h, int flags)
{
	/* must recreate display if we get error code 2 */
	if(t3f_set_gfx_mode(w, h, flags) == 2)
	{
		t3f_unload_resources();
		title_exit();
		al_unregister_event_source(t3f_queue, al_get_display_event_source(t3f_display));
		al_destroy_display(t3f_display);
		t3f_display = NULL;
		t3f_set_gfx_mode(640, 480, flags);
		al_register_event_source(t3f_queue, al_get_display_event_source(t3f_display));
		t3f_reload_resources();
		title_init();
		al_hide_mouse_cursor(t3f_display);
	}
	return true;
}

void initialize_video_mode(void)
{
	ALLEGRO_MONITOR_INFO info;
	int flags = force_aspect ? T3F_FORCE_ASPECT : 0;

	/* make sure we are using the configured display setting */
	switch(display_type)
	{
		case 0:
		{
			al_set_config_value(t3f_config, "T3F", "force_fullscreen", "false");
			set_video_mode(640, 480, flags);
			break;
		}
		case 1:
		{
			al_set_config_value(t3f_config, "T3F", "force_fullscreen", "false");
			set_video_mode(960, 720, flags);
			break;
		}
		case 2:
		{
			al_set_config_value(t3f_config, "T3F", "force_fullscreen", "false");
			set_video_mode(1280, 960, flags);
			break;
		}
		case 3:
		{
			al_get_monitor_info(0, &info);
			al_set_config_value(t3f_config, "T3F", "force_fullscreen", "true");
			set_video_mode(info.x2 - info.x1, info.y2 - info.y1, flags | T3F_USE_FULLSCREEN);
			t3f_default_view->need_update = true;
			t3f_select_view(t3f_default_view);
			break;
		}
	}
	t3f_select_view(t3f_default_view);
}

int menu_proc_display_left(void * data, int i, void * p)
{
	char buf[16] = {0};

	while(1)
	{
		display_type--;
		if(display_type < 0)
		{
			display_type = 3;
		}
		if(display_type_ok(display_type))
		{
			break;
		}
	}
	initialize_video_mode();
	sprintf(buf, "%d", display_type);
	al_set_config_value(t3f_config, "Display", "Type", buf);
	menu_fix_display_text();
	return 1;
}

int menu_proc_display_right(void * data, int i, void * p)
{
	char buf[16] = {0};

	while(1)
	{
		display_type++;
		if(display_type > 3)
		{
			display_type = 0;
		}
		if(display_type_ok(display_type))
		{
			break;
		}
	}
	initialize_video_mode();
	sprintf(buf, "%d", display_type);
	al_set_config_value(t3f_config, "Display", "Type", buf);
	menu_fix_display_text();
	return 1;
}

int menu_proc_display_toggle(void * data, int i, void * p)
{
	force_aspect = !force_aspect;
	menu_fix_display_text();
	if(!force_aspect && (t3f_flags & T3F_FORCE_ASPECT))
	{
		t3f_flags &= ~T3F_FORCE_ASPECT;
		t3f_set_gfx_mode(al_get_display_width(t3f_display), al_get_display_height(t3f_display), t3f_flags & ~T3F_FORCE_ASPECT);
		al_set_config_value(t3f_config, "T3F", "force_aspect_ratio", "false");
	}
	else if(force_aspect && !(t3f_flags & T3F_FORCE_ASPECT))
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
	switch(analog_state)
	{
		case 0:
		{
			controller->binding[analog_type * 2].min = analog_max;
			analog_max = 0.0;
			analog_state++;
			break;
		}
		case 1:
		{
			controller->binding[analog_type * 2].max = analog_max;
			analog_max = 0.0;
			analog_state++;
			break;
		}
		case 2:
		{
			controller->binding[analog_type * 2].mid = controller->state[analog_type * 2].pos;
			controller->binding[0].flags &= ~T3F_CONTROLLER_FLAG_AXIS_NO_ADJUST;
			select_menu(TITLE_MENU_CONTROL_ANALOG);
			break;
		}
	}
	return 1;
}

static int title_flicker = 0;

bool title_load_data(void)
{
	animation[ANIMATION_TITLE] = t3f_load_animation_from_bitmap("data/graphics/title_logo.png");
	if(!animation[ANIMATION_TITLE])
	{
		return false;
	}
	animation[ANIMATION_TITLE_EYES] = t3f_load_animation_from_bitmap("data/graphics/title_logo_eyes.png");
	if(!animation[ANIMATION_TITLE_EYES])
	{
		return false;
	}
	bitmap[0] = t3f_load_resource((void **)(&bitmap[0]), T3F_RESOURCE_TYPE_BITMAP, "data/graphics/bg00.png", 0, 0, 0);
	if(!bitmap[0])
	{
		return false;
	}
	return true;
}

void title_free_data(void)
{
	t3f_destroy_animation(animation[ANIMATION_TITLE]);
	animation[ANIMATION_TITLE] = NULL;
	t3f_destroy_animation(animation[ANIMATION_TITLE_EYES]);
	animation[ANIMATION_TITLE_EYES] = NULL;
	t3f_destroy_resource(bitmap[0]);
	bitmap[0] = NULL;
}

bool title_init(void)
{
	float total_height;
	float free_height;
	float space;
	float oy = 0;

	menu[TITLE_MENU_MAIN] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(menu[TITLE_MENU_MAIN], menu_proc_play, "Play Game", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_MAIN], menu_proc_story, "View Story", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_MAIN], menu_proc_leaderboards, "Leaderboards", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_MAIN], menu_proc_settings, "Settings", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_MAIN], menu_proc_privacy, "Privacy", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(menu[TITLE_MENU_MAIN], menu_proc_quit, "Exit to OS", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	t3f_center_gui(menu[TITLE_MENU_MAIN], 20, 480);

	oy = 0;
	menu[TITLE_MENU_DIFFICULTY] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(menu[TITLE_MENU_DIFFICULTY], menu_proc_play_easy, "Easy", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_DIFFICULTY], menu_proc_play_normal, "Normal", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(menu[TITLE_MENU_DIFFICULTY], menu_proc_back, "Back", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	t3f_center_gui(menu[TITLE_MENU_DIFFICULTY], 20, 480);

	oy = 0;
	menu[TITLE_MENU_MODE] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(menu[TITLE_MENU_MODE], menu_proc_play_story, "Story", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_MODE], menu_proc_play_eternal, "Eternal", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(menu[TITLE_MENU_MODE], menu_proc_back, "Back", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	t3f_center_gui(menu[TITLE_MENU_MODE], 20, 480);

	oy = 0;
	menu[TITLE_MENU_SETTINGS] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(menu[TITLE_MENU_SETTINGS], menu_proc_controls, "Controls", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_SETTINGS], menu_proc_internet, "Internet Options", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_SETTINGS], menu_proc_display, "Display Options", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(menu[TITLE_MENU_SETTINGS], menu_proc_back, "Back", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	menu_fix_controller_type_text();
	t3f_center_gui(menu[TITLE_MENU_SETTINGS], 20, 480);

	oy = 0;
	menu[TITLE_MENU_CONTROL] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL], NULL, "Control Type", font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL], NULL, menu_text[0], font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL], menu_proc_controller_type_left, "<", font[FONT_LARGE], 320 - al_get_text_width(font[FONT_LARGE], "Normal Controller") / 2 - al_get_text_width(font[FONT_LARGE], "<"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL], menu_proc_controller_type_right, ">", font[FONT_LARGE], 320 + al_get_text_width(font[FONT_LARGE], "Normal Controller") / 2 + al_get_text_width(font[FONT_LARGE], ">"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL], menu_proc_controller_autodetect, "Autodetect", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
		oy += al_get_font_line_height(font[FONT_LARGE]);
		t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL], menu_proc_controller_configure, "Configure", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
		oy += al_get_font_line_height(font[FONT_LARGE]);
	#endif
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL], menu_proc_back, "Done", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	menu_fix_controller_type_text();
	t3f_center_gui(menu[TITLE_MENU_CONTROL], 20, 480);

	oy = 0;
	menu[TITLE_MENU_CONTROL_NORMAL] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Move Up", font[FONT_SMALL], 160, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_move_up, menu_text[1], font[FONT_SMALL], 160, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Move Down", font[FONT_SMALL], 160, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_move_down, menu_text[2], font[FONT_SMALL], 160, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Move Left", font[FONT_SMALL], 160, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_move_left, menu_text[3], font[FONT_SMALL], 160, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Move Right", font[FONT_SMALL], 160, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_move_right, menu_text[4], font[FONT_SMALL], 160, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy = 0;
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Fire Up", font[FONT_SMALL], 480, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_fire_up, menu_text[5], font[FONT_SMALL], 480, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Fire Down", font[FONT_SMALL], 480, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_fire_down, menu_text[6], font[FONT_SMALL], 480, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Fire Left", font[FONT_SMALL], 480, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_fire_left, menu_text[7], font[FONT_SMALL], 480, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], NULL, "Fire Right", font[FONT_SMALL], 480, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_set_fire_right, menu_text[8], font[FONT_SMALL], 480, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_NORMAL], menu_proc_back, "Done", font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(menu[TITLE_MENU_CONTROL_NORMAL], 20, 480);

	oy = 0;
	menu[TITLE_MENU_CONTROL_MOUSE] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_MOUSE], NULL, "Move Up", font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_MOUSE], menu_proc_set_move_up, menu_text[1], font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_MOUSE], NULL, "Move Down", font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_MOUSE], menu_proc_set_move_down, menu_text[2], font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_MOUSE], NULL, "Move Left", font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_MOUSE], menu_proc_set_move_left, menu_text[3], font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_MOUSE], NULL, "Move Right", font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_MOUSE], menu_proc_set_move_right, menu_text[4], font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_MOUSE], menu_proc_back, "Done", font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(menu[TITLE_MENU_CONTROL_MOUSE], 20, 480);

	oy = 0;
	menu[TITLE_MENU_CONTROL_ANALOG] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_ANALOG], NULL, "Move Vertically", font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_ANALOG], menu_proc_set_move_vertical, menu_text[1], font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_ANALOG], NULL, "Move Horizontally", font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_ANALOG], menu_proc_set_move_horizontal, menu_text[2], font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_ANALOG], NULL, "Fire Vertically", font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_ANALOG], menu_proc_set_fire_vertical, menu_text[3], font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_ANALOG], NULL, "Fire Horizontally", font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_ANALOG], menu_proc_set_fire_horizontal, menu_text[4], font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_CONTROL_ANALOG], menu_proc_back, "Done", font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(menu[TITLE_MENU_CONTROL_ANALOG], 20, 480);

	oy = 0;
	menu[TITLE_MENU_ANALOG] = t3f_create_gui(0, 0);
//	t3f_add_gui_text_element(menu[TITLE_MENU_ANALOG], NULL, menu_text[14], font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
//	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_ANALOG], NULL, menu_text[15], font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_SMALL]);
	t3f_add_gui_text_element(menu[TITLE_MENU_ANALOG], menu_proc_analog_done, "Done", font[FONT_SMALL], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(menu[TITLE_MENU_ANALOG], 20, 480);

	oy = 0;
	menu[TITLE_MENU_INTERNET] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(menu[TITLE_MENU_INTERNET], NULL, "Upload Scores", font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_INTERNET], NULL, menu_text[9], font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	t3f_add_gui_text_element(menu[TITLE_MENU_INTERNET], menu_proc_upload_toggle, "<", font[FONT_LARGE], 320 - al_get_text_width(font[FONT_LARGE], "Yes") / 2 - al_get_text_width(font[FONT_LARGE], "<"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(menu[TITLE_MENU_INTERNET], menu_proc_upload_toggle, ">", font[FONT_LARGE], 320 + al_get_text_width(font[FONT_LARGE], "Yes") / 2 + al_get_text_width(font[FONT_LARGE], ">"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_INTERNET], menu_proc_network_id, "Network ID", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(menu[TITLE_MENU_INTERNET], menu_proc_back, "Done", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	menu_fix_internet_text();
	t3f_center_gui(menu[TITLE_MENU_INTERNET], 20, 480);

	oy = 0;
	menu[TITLE_MENU_NETWORK_ID] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(menu[TITLE_MENU_NETWORK_ID], NULL, "Network ID", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_NETWORK_ID], NULL, network_id, font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(menu[TITLE_MENU_NETWORK_ID], menu_proc_back, "Done", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	menu_fix_internet_text();
	#ifdef T3F_ANDROID
		t3f_center_gui(menu[TITLE_MENU_NETWORK_ID], 20 + t3f_default_view->top, 240);
	#else
		t3f_center_gui(menu[TITLE_MENU_NETWORK_ID], 20, 480);
	#endif

	oy = 0;
	menu[TITLE_MENU_DISPLAY] = t3f_create_gui(0, 0);
	#ifndef PANDORA
		#ifndef T3F_ANDROID
			t3f_add_gui_text_element(menu[TITLE_MENU_DISPLAY], NULL, "Display Type", font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
			oy += al_get_font_line_height(font[FONT_LARGE]);
			t3f_add_gui_text_element(menu[TITLE_MENU_DISPLAY], NULL, menu_text[12], font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
			t3f_add_gui_text_element(menu[TITLE_MENU_DISPLAY], menu_proc_display_left, "<", font[FONT_LARGE], 320 - al_get_text_width(font[FONT_LARGE], "Medium Window") / 2 - al_get_text_width(font[FONT_LARGE], "<"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
			t3f_add_gui_text_element(menu[TITLE_MENU_DISPLAY], menu_proc_display_right, ">", font[FONT_LARGE], 320 + al_get_text_width(font[FONT_LARGE], "Medium Window") / 2 + al_get_text_width(font[FONT_LARGE], ">"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
			oy += al_get_font_line_height(font[FONT_LARGE]);
		#endif
	#endif
	t3f_add_gui_text_element(menu[TITLE_MENU_DISPLAY], NULL, "Force 4:3 Aspect Ratio", font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_DISPLAY], NULL, menu_text[13], font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	t3f_add_gui_text_element(menu[TITLE_MENU_DISPLAY], menu_proc_display_toggle, "<", font[FONT_LARGE], 320 - al_get_text_width(font[FONT_LARGE], "Yes") / 2 - al_get_text_width(font[FONT_LARGE], "<"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(menu[TITLE_MENU_DISPLAY], menu_proc_display_toggle, ">", font[FONT_LARGE], 320 + al_get_text_width(font[FONT_LARGE], "Yes") / 2 + al_get_text_width(font[FONT_LARGE], ">"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(menu[TITLE_MENU_DISPLAY], menu_proc_back, "Done", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	menu_fix_display_text();
	t3f_center_gui(menu[TITLE_MENU_DISPLAY], 20, 480);

	oy = 0;
	menu[LEADERBOARD_MENU] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(menu[LEADERBOARD_MENU], NULL, menu_text[10], font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	t3f_add_gui_text_element(menu[LEADERBOARD_MENU], menu_proc_leaderboard_mode_left, "<", font[FONT_LARGE], 320 - al_get_text_width(font[FONT_LARGE], "Story Mode (Easy)") / 2 - al_get_text_width(font[FONT_LARGE], "<"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(menu[LEADERBOARD_MENU], menu_proc_leaderboard_mode_right, ">", font[FONT_LARGE], 320 + al_get_text_width(font[FONT_LARGE], "Story Mode (Easy)") / 2 + al_get_text_width(font[FONT_LARGE], ">"), oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(menu[LEADERBOARD_MENU], menu_proc_leaderboard_done, "Done", font[FONT_LARGE], 320, 318, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#else
		t3f_add_gui_text_element(menu[LEADERBOARD_MENU], NULL, "", font[FONT_LARGE], 320, 318, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
	t3f_center_gui(menu[LEADERBOARD_MENU], 20, 480);

	oy = 0;
	menu[PAUSE_MENU] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(menu[PAUSE_MENU], menu_proc_pause_resume, "Resume Game", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[PAUSE_MENU], menu_proc_pause_quit, "Quit", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(menu[PAUSE_MENU], 20, 480);

	oy = 0;
	menu[TITLE_MENU_FIRST] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(menu[TITLE_MENU_FIRST], NULL, "Would you like your scores", font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_FIRST], NULL, "to be uploaded to t3-i.com?", font[FONT_LARGE], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_FIRST], menu_proc_first_yes, "Yes", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	oy += al_get_font_line_height(font[FONT_LARGE]);
	t3f_add_gui_text_element(menu[TITLE_MENU_FIRST], menu_proc_first_no, "No", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(menu[TITLE_MENU_FIRST], 20, 480);

	total_height = (t3f_default_view->bottom - t3f_default_view->top) - al_get_font_line_height(font[FONT_SMALL]);
	free_height = total_height - (al_get_bitmap_height(animation[ANIMATION_TITLE_EYES]->bitmaps->bitmap[0]) + al_get_font_line_height(font[FONT_LARGE]));
	space = free_height / 3;
	logo_pos_x = 320 - al_get_bitmap_width(animation[ANIMATION_TITLE_EYES]->bitmaps->bitmap[0]) / 2;
	logo_pos_y = t3f_default_view->top + space;
	oy = t3f_default_view->top + space + al_get_bitmap_height(animation[ANIMATION_TITLE_EYES]->bitmaps->bitmap[0]) + space;
	menu[MENU_TITLE] = t3f_create_gui(0, 0);
	t3f_add_gui_text_element(menu[MENU_TITLE], menu_proc_game, "Game", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);

	oy = t3f_default_view->bottom - al_get_font_line_height(font[FONT_LARGE]);
	menu[MENU_PRIVACY] = t3f_create_gui(0, 0);
//	t3f_add_gui_text_element(menu[TITLE_MENU_ANALOG], NULL, menu_text[14], font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
//	oy += al_get_font_line_height(font[FONT_SMALL]);
//	t3f_add_gui_text_element(menu[MENU_PRIVACY], NULL, menu_text[15], font[FONT_SMALL], 320, oy, t3f_color_white, T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_STATIC);
//	oy += al_get_font_line_height(font[FONT_SMALL]);
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(menu[MENU_PRIVACY], menu_proc_back, "Done", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#else
		t3f_add_gui_text_element(menu[MENU_PRIVACY], menu_proc_back, "", font[FONT_LARGE], 320, oy, al_map_rgba_f(1.0, 0.0, 0.0, 1.0), T3F_GUI_ELEMENT_CENTRE | T3F_GUI_ELEMENT_SHADOW);
	#endif
//	t3f_center_gui(menu[TITLE_MENU_ANALOG], 20, 480);

	return true;
}

void title_exit(void)
{
	int i;

	for(i = 0; i < TITLE_MAX_MENUS; i++)
	{
		if(menu[i])
		{
			t3f_destroy_gui(menu[i]);
			menu[i] = NULL;
		}
	}
}

void title_bg_logic(void)
{
	title_flicker++;
}

void title_bg_render(ALLEGRO_BITMAP * bp)
{
	float alpha;
	al_draw_bitmap(bp, 0, 0, 0);
	alpha = (float)(title_flicker % 512) / 512.0;
	t3f_draw_bitmap(bp, al_map_rgba_f(alpha, alpha, alpha, alpha), 0, 0, (float)(title_flicker % 512) / 2.0 - 256.0, 0);
	alpha = 1.0 - (float)(title_flicker % 512) / 512.0;
	t3f_draw_bitmap(bp, al_map_rgba_f(alpha, alpha, alpha, alpha), 0, 0, (float)-(title_flicker % 512) / 2.0, 0);
}

static bool check_for_skip(void)
{
	int i;

	if(t3f_key_pressed())
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
		if(t3f_mouse_button[i])
		{
			return true;
		}
	}
	for(i = 0; i < T3F_MAX_TOUCHES; i++)
	{
		if(t3f_touch[i].active)
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
		t3f_mouse_button[i] = false;
	}
}

void title_in_logic(void)
{
	if(check_for_skip())
	{
		state_ticks = 240;
	}
	title_bg_logic();
	state_ticks++;
	if(state_ticks >= 240)
	{
		clear_mouse_buttons();
		t3f_clear_touch_data();
		t3f_clear_controller_state(controller);
		t3f_select_next_gui_element(menu[MENU_TITLE]);
		state = STATE_TITLE;
		state_ticks = 0;
	}
}

void title_in_render(void)
{
	float alpha;
	float x, y;

	x = logo_pos_x;
	y = logo_pos_y;
	al_clear_to_color(al_map_rgba_f(0.0, 0.0, 0.0, 1.0));
	if(state_ticks < 60)
	{
		alpha = (float)(state_ticks) / 60.0;
		t3f_draw_animation(animation[ANIMATION_TITLE_EYES], al_map_rgba_f(alpha, alpha, alpha, alpha), state_ticks, x, y, 0, 0);
	}
	else if(state_ticks < 180)
	{
		t3f_draw_animation(animation[ANIMATION_TITLE_EYES], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), state_ticks, x, y, 0, 0);
	}
	else
	{
		title_bg_render(bitmap[0]);
		t3f_draw_animation(animation[ANIMATION_TITLE_EYES], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), state_ticks, x + 4, y + 4, 0, 0);
		t3f_draw_animation(animation[ANIMATION_TITLE], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), state_ticks, x + 4, y + 4, 0, 0);
		t3f_draw_animation(animation[ANIMATION_TITLE], t3f_color_white, state_ticks, x, y, 0, 0);
		al_hold_bitmap_drawing(false);
		al_draw_filled_rectangle(0.0, 0.0, 640, 480, al_map_rgba_f(0.0, 0.0, 0.0, 1.0 - (float)(state_ticks - 180) / 60.0));
		t3f_draw_animation(animation[ANIMATION_TITLE_EYES], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), state_ticks, x, y, 0, 0);
	}
}

void title_name_entry_logic(void)
{
	int input;

	input = t3f_read_key(0);
	if(input)
	{
		if(input == '\b' || input == 127)
		{
			if(network_id_pos > 0)
			{
				network_id_pos--;
				network_id[network_id_pos] = '\0';
			}
		}
		else if(input == '\r')
		{
			network_id_entry = false;
			if(strlen(network_id) <= 0)
			{
				strcpy(network_id, "Anonymous");
			}
			menu_fix_internet_text();
			menu_proc_back(NULL, 0, NULL);
			t3f_key[ALLEGRO_KEY_ENTER] = 0;
			t3f_show_soft_keyboard(false);
		}
		else if(input >= 32 && input < 127)
		{
			network_id[network_id_pos] = input;
			network_id_pos++;
			network_id[network_id_pos] = '\0';
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
	if(t3f_key[ALLEGRO_KEY_ENTER])
	{
		return true;
	}
	return false;
}

bool title_touched(void)
{
	int i;

	for(i = 0; i < T3F_MAX_TOUCHES; i++)
	{
		if(t3f_touch[i].active)
		{
			return true;
		}
	}
	return false;
}

void title_joystick_logic(T3F_GUI * mp)
{
	bool pressed = false;

	switch(controller_type)
	{
		case CONTROLLER_TYPE_ANALOG:
		{
			t3f_read_controller(controller);

			/* don't change selection when viewing analog config menu */
			if(current_menu != TITLE_MENU_ANALOG)
			{
				if(controller->state[0].pos < -0.2)
				{
					button_press_counter++;
					if(button_press_counter == 1)
					{
						t3f_select_previous_gui_element(mp);
					}
					mouse_disabled = true;
					pressed = true;
				}
				else if(controller->state[0].pos > 0.2)
				{
					button_press_counter++;
					if(button_press_counter == 1)
					{
						t3f_select_next_gui_element(mp);
					}
					mouse_disabled = true;
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
			t3f_read_controller(controller);
			t3f_update_controller(controller);
			if(!block_buttons)
			{
				if(controller->state[0].pressed)
				{
					t3f_select_previous_gui_element(mp);
				}
				if(controller->state[1].pressed)
				{
					t3f_select_next_gui_element(mp);
				}
				if(controller->state[4].pressed || controller->state[5].pressed || controller->state[6].pressed || controller->state[7].pressed || t3f_key[ALLEGRO_KEY_ENTER])
				{
					t3f_activate_selected_gui_element(mp, NULL);
					block_buttons = true;
				}
			}
			else
			{
				if(!controller->state[4].pressed && !controller->state[5].pressed && !controller->state[6].pressed && !controller->state[7].pressed && !t3f_key[ALLEGRO_KEY_ENTER])
				{
					block_buttons = false;
				}
			}
			break;
		}
	}
}

void title_process_menu(T3F_GUI * mp)
{
	int i;

	title_joystick_logic(mp);
	if(!mouse_disabled)
	{
		t3f_process_gui(mp, NULL);
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
void create_text_line_data(TEXT_LINE_DATA * lp, ALLEGRO_FONT * fp, float w, float tab, const char * text)
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
		else if(al_get_text_width(fp, current_line) > wi)
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
		al_draw_text(lines->font, color, px, py, 0, lines->line[i].text);
		px = x + lines->tab;
		py += al_get_font_line_height(lines->font);
	}
}

void draw_multiline_text(ALLEGRO_FONT * fp, ALLEGRO_COLOR color, float x, float y, float w, float tab, int flags, const char * text)
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
		pos -= al_get_text_width(fp, text) / 2.0;
	}
	else if(flags & T3F_FONT_ALIGN_RIGHT)
	{
		pos -= al_get_text_width(fp, text);
	}
	if(w > 0.0)
	{
		create_text_line_data(&line_data, fp, w, tab, text);
		draw_text_lines(&line_data, color, x, y);
	}
	else
	{
		al_draw_text(fp, color, x, y, 0, text);
	}
	if(!held)
	{
		al_hold_bitmap_drawing(false);
	}
}

static float get_copyright_message_width(ALLEGRO_FONT * fp, ALLEGRO_FONT * fp2, const char * text)
{
	ALLEGRO_FONT * font;
	int read_pos = 0;
	int32_t read_char = 0;
	float text_width = 0.0;
	int i;

	if(copyright_message_width <= 0.0)
	{
		for(i = 0; i < al_ustr_length(copyright_message_ustr); i++)
		{
			read_char = al_ustr_get_next(copyright_message_ustr, &read_pos);
			if(read_char == 179)
			{
				font = fp2;
				read_char = '3';
			}
			else
			{
				font = fp;
			}
			al_ustr_set_chr(copyright_message_uchar, 0, read_char);
			text_width += al_get_text_width(font, al_cstr(copyright_message_uchar));
		}
	}
	return text_width;
}

static void draw_copyright_message(ALLEGRO_FONT * fp, ALLEGRO_FONT * fp2, ALLEGRO_COLOR color, float x, float y, int flags, const char * text)
{
	ALLEGRO_FONT * font;
	int read_pos = 0;
	int32_t read_char = 0;
	float pos = x;
	float oy;
	int i;

	if(flags & ALLEGRO_ALIGN_CENTRE)
	{
		pos -= get_copyright_message_width(fp, fp2, text) / 2;
	}
	for(i = 0; i < al_ustr_length(copyright_message_ustr); i++)
	{
		read_char = al_ustr_get_next(copyright_message_ustr, &read_pos);
		if(read_char == 179)
		{
			font = fp2;
			read_char = '3';
			oy = -4.0;
		}
		else
		{
			font = fp;
			oy = 0.0;
		}
		al_ustr_set_chr(copyright_message_uchar, 0, read_char);
		al_draw_text(font, color, pos, y + oy, 0, al_cstr(copyright_message_uchar));
		pos += al_get_text_width(font, al_cstr(copyright_message_uchar));
	}
}

void title_render(void)
{
	float x, y;
	float w;

	title_bg_render(bitmap[0]);

	/* render title logo if we are on title menu */
	if(current_menu == MENU_TITLE)
	{
		x = logo_pos_x;
		y = logo_pos_y;
		t3f_draw_animation(animation[ANIMATION_TITLE_EYES], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), state_ticks, x + 4, y + 4, 0, 0);
		t3f_draw_animation(animation[ANIMATION_TITLE], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), state_ticks, x + 4, y + 4, 0, 0);
		t3f_draw_animation(animation[ANIMATION_TITLE_EYES], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), state_ticks, x, y, 0, 0);
		t3f_draw_animation(animation[ANIMATION_TITLE], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), state_ticks, x, y, 0, 0);
		draw_copyright_message(font[FONT_SMALL], font[FONT_TINY], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 320, t3f_default_view->bottom - al_get_font_line_height(font[FONT_SMALL]), ALLEGRO_ALIGN_CENTRE, copyright_message);
		draw_copyright_message(font[FONT_SMALL], font[FONT_TINY], t3f_color_white, 320 - 2, t3f_default_view->bottom - al_get_font_line_height(font[FONT_SMALL]) - 2, ALLEGRO_ALIGN_CENTRE, copyright_message);
	}
	else if(current_menu == MENU_PRIVACY)
	{
		w = t3f_default_view->right - t3f_default_view->left;
		draw_multiline_text(font[FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), t3f_default_view->left + 6.0, t3f_default_view->top + 60.0 + 6.0, w, 0.0, 0, privacy_text);
		draw_multiline_text(font[FONT_SMALL], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), t3f_default_view->left + 4.0, t3f_default_view->top + 60.0 + 4.0, w, 0.0, 0, privacy_text);
	}

	t3f_render_gui(menu[current_menu]);

	/* render blinking cursor */
	if(current_menu == TITLE_MENU_NETWORK_ID && network_id_entry)
	{
		if(state_ticks % 20 < 10)
		{
			al_draw_text(font[FONT_LARGE], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), menu[current_menu]->ox + menu[current_menu]->element[1].ox + al_get_text_width(font[FONT_LARGE], network_id) / 2, menu[current_menu]->oy + menu[current_menu]->element[1].oy, 0, "_");
			al_draw_text(font[FONT_LARGE], t3f_color_white, menu[current_menu]->ox + menu[current_menu]->element[1].ox + al_get_text_width(font[FONT_LARGE], network_id) / 2 - 2, menu[current_menu]->oy + menu[current_menu]->element[1].oy - 2, 0, "_");
		}
	}
	render_mouse();
}

void title_logic(void)
{
	state_ticks++;
	title_bg_logic();
	if(current_menu == TITLE_MENU_ANALOG)
	{
		menu_analog_logic();
	}
	if(network_id_entry)
	{
		title_name_entry_logic();
	}
	if(t3f_key[ALLEGRO_KEY_ESCAPE] || t3f_key[ALLEGRO_KEY_BACK])
	{
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
		t3f_key[ALLEGRO_KEY_BACK] = 0;
		menu_proc_back(NULL, 0, NULL);
	}
	else
	{
		title_process_menu(menu[current_menu]);
	}
}

void title_out_logic(void)
{
	state_ticks++;
	title_bg_logic();
	if(state_ticks >= 60)
	{
		t3f_exit();
	}
}

void title_out_render(void)
{
	al_hold_bitmap_drawing(true);
	title_bg_render(bitmap[0]);
	t3f_render_gui(menu[current_menu]);
	al_hold_bitmap_drawing(false);
	al_draw_filled_rectangle(0.0, 0.0, 640, 480, al_map_rgba_f(0.0, 0.0, 0.0, (float)state_ticks / 60.0));
}

void title_game_logic(void)
{
	int i;

	for(i = 0; i < 8; i++)
	{
		title_bg_logic();
		if(title_flicker % 512 == 0)
		{
			title_free_data();
			game_init(title_game_mode);
			break;
		}
	}
}

void title_game_render(void)
{
	al_hold_bitmap_drawing(true);
	title_bg_render(bitmap[0]);
	al_hold_bitmap_drawing(false);
}
