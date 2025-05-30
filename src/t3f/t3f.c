#include <allegro5/allegro5.h>

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_memfile.h>
#ifndef ALLEGRO_ANDROID
	#include <allegro5/allegro_native_dialog.h>
#endif

#ifdef ALLEGRO_ANDROID
	#include <allegro5/allegro_android.h>
	#include <allegro5/allegro_physfs.h>
	#include <physfs.h>
#endif

#include <stdio.h>
#include <string.h>

#include "t3f.h"
#include "memory.h"
#include "resource.h"
#include "view.h"
#include "music.h"
#include "android.h"
#include "input.h"
#ifndef ALLEGRO_ANDROID
	#include "menu.h"
#endif
#ifdef ALLEGRO_WINDOWS
	#include "windows.h"
#endif
#ifdef T3F_PNG
	#include "png.h"
#endif
#include "mouse.h"
#include "keyboard.h"
#include "touch.h"

/* display data */
int t3f_virtual_display_width = 0;
int t3f_virtual_display_height = 0;

/* joystick data */
ALLEGRO_JOYSTICK * t3f_joystick[T3F_MAX_JOYSTICKS] = {NULL};
ALLEGRO_JOYSTICK_STATE t3f_joystick_state[T3F_MAX_JOYSTICKS];
bool t3f_joystick_state_updated[T3F_MAX_JOYSTICKS] = {false};

/* blender data */
ALLEGRO_STATE t3f_state_stack[T3F_MAX_STACK];
int t3f_state_stack_size = 0;

bool t3f_quit = false;
int t3f_requested_flags = 0;
int t3f_flags = 0;
int t3f_option[T3F_MAX_OPTIONS] = {0};
static int _t3f_logic_ticks = 0;
static double _t3f_last_timer_tick_timestamp = 0.0;
static double _t3f_timer_tick_minimum_time = 0.0;

void (*t3f_logic_proc)(void * data) = NULL;
void (*t3f_render_proc)(void * data) = NULL;
static void * t3f_app_data = NULL;

ALLEGRO_DISPLAY * t3f_display = NULL;
ALLEGRO_TIMER * t3f_timer = NULL;
ALLEGRO_EVENT_QUEUE * t3f_queue = NULL;
char t3f_window_title[1024] = {0};

ALLEGRO_CONFIG * t3f_config = NULL;
ALLEGRO_CONFIG * t3f_user_data = NULL;
ALLEGRO_PATH * t3f_data_path = NULL;
ALLEGRO_PATH * t3f_config_path = NULL;
ALLEGRO_PATH * t3f_temp_path = NULL;
static char t3f_config_filename[1024] = {0};
static char t3f_user_data_filename[1024] = {0};

/* colors */
ALLEGRO_COLOR t3f_color_white;
ALLEGRO_COLOR t3f_color_black;

/* internal variables */
static bool t3f_need_redraw = false;
static int t3f_halted = 0;
static void (*t3f_event_handler_proc)(ALLEGRO_EVENT * event, void * data) = NULL;
static void (*t3f_queued_call_proc)(void * data) = NULL;
static void * t3f_queued_call_data = NULL;

static char * t3f_developer_name = NULL;
static char * t3f_package_name = NULL; // used to locate resources

static bool t3f_is_path_present(ALLEGRO_PATH * pp)
{
	return al_filename_exists(al_path_cstr(pp, '/'));
}

/* creates directory structure that leads to 'final' */
void t3f_setup_directories(ALLEGRO_PATH * final)
{
	ALLEGRO_PATH * working_path[16] = {NULL};
	int working_paths = 0;
	const char * cpath = NULL;
	int i;

	/* find first directory that exists */
	working_path[0] = al_clone_path(final);
	working_paths = 1;
	while(!t3f_is_path_present(working_path[working_paths - 1]) && working_paths < 16)
	{
		working_path[working_paths] = al_clone_path(working_path[working_paths - 1]);
		al_drop_path_tail(working_path[working_paths]);
		working_paths++;
	}

	/* iterate through working_path[] and make each directory */
	for(i = working_paths - 1; i >= 0; i--)
	{
		cpath = al_path_cstr(working_path[i], '/');
//		printf("make directory: %s\n", cpath);
		al_make_directory(cpath);
	}
	for(i = 0; i < working_paths; i++)
	{
		al_destroy_path(working_path[i]);
	}
}

static void t3f_get_options(void)
{
	const char * val;
	char buf[64];
	int i;

	for(i = 0; i < T3F_MAX_OPTIONS; i++)
	{
		snprintf(buf, 64, "Key %d", i);
		val = al_get_config_value(t3f_config, "Options", buf);
		if(val)
		{
			t3f_option[i] = atoi(val);
		}
	}
}

/* function to ease the burden of having resources located in different places
 * on different platforms, changes to the directory where it finds the specified
 * resource */
static bool t3f_locate_resource(const char * filename)
{
	ALLEGRO_PATH * path;
	ALLEGRO_PATH * file_path;
	bool found = false;

	/* handle Android first so we don't do unnecessary checks */
	#ifdef ALLEGRO_ANDROID

		PHYSFS_AndroidInit android_init;
		int ret;

		/* try to use PHYSFS to access data */
		path = al_get_standard_path(ALLEGRO_EXENAME_PATH);
		if(path)
		{
			/* Pass NULL here even though the docs say not to. We don't use the write
			   functionality of PhysFS anyway. */
//			android_init.jnienv = _al_android_get_jnienv();
//			android_init.context = _al_android_activity_object();
			if(PHYSFS_init(NULL))
			{
				ret = PHYSFS_addToSearchPath(al_path_cstr(path, '/'), 1);
				al_destroy_path(path);
				if(ret)
				{
					al_set_physfs_file_interface();
					al_change_directory("assets");
					if(al_filename_exists(filename))
					{
						return true;
					}
				}
			}
		}

		/* if PHYSFS setup failed, use APK file interface instead */
		al_android_set_apk_file_interface();
		return true;

	#endif

	/* if we are already in the correct directory */
	if(al_filename_exists(filename))
	{
		return true;
	}

	/* look in resources path */
	file_path = al_create_path(filename);
	if(!file_path)
	{
		return false;
	}
	path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	if(path)
	{
		al_join_paths(path, file_path);
		if(al_filename_exists(al_path_cstr(path, '/')))
		{
			found = true;
		}
//		printf("%s\n", al_path_cstr(path, '/'));
		al_destroy_path(path);
	}
	al_destroy_path(file_path);
	if(found)
	{
		path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
		al_change_directory(al_path_cstr(path, '/'));
		al_destroy_path(path);
		return true;
	}

	/* look in "/usr/share" if a package name is defined */
	if(t3f_package_name)
	{
		file_path = al_create_path(filename);
		if(!file_path)
		{
			return false;
		}
		path = al_create_path("/usr/share/");
		if(path)
		{
			al_append_path_component(path, t3f_package_name);
			al_join_paths(path, file_path);
			if(al_filename_exists(al_path_cstr(path, '/')))
			{
				al_change_directory(al_path_cstr(path, '/'));
				found = true;
			}
//			printf("%s\n", al_path_cstr(path, '/'));
			al_destroy_path(path);
		}
		al_destroy_path(file_path);
	}

	if(found)
	{
		path = al_create_path("/usr/share/");
		if(path)
		{
			al_append_path_component(path, t3f_package_name);
			al_change_directory(al_path_cstr(path, '/'));
			al_destroy_path(path);
			return true;
		}
	}

	/* look in "/usr/share" if a package name is defined */
	if(t3f_package_name)
	{
		file_path = al_create_path(filename);
		if(!file_path)
		{
			return false;
		}
		path = al_create_path("/usr/local/share/");
		if(path)
		{
			al_append_path_component(path, t3f_package_name);
			al_join_paths(path, file_path);
			if(al_filename_exists(al_path_cstr(path, '/')))
			{
				al_change_directory(al_path_cstr(path, '/'));
				found = true;
			}
//			printf("%s\n", al_path_cstr(path, '/'));
			al_destroy_path(path);
		}
		al_destroy_path(file_path);
	}

	if(found)
	{
		path = al_create_path("/usr/local/share/");
		if(path)
		{
			al_append_path_component(path, t3f_package_name);
			al_change_directory(al_path_cstr(path, '/'));
			al_destroy_path(path);
			return true;
		}
	}
	return false;
}

/* this gets Allegro ready */
int t3f_initialize(const char * name, int w, int h, double fps, void (*logic_proc)(void * data), void (*render_proc)(void * data), int flags, void * data)
{
	int i;
	ALLEGRO_PATH * temp_path = NULL;
	const ALLEGRO_FILE_INTERFACE * old_interface;

	// compile time configuration
	#ifdef T3F_DEVELOPER_NAME
		t3f_developer_name = malloc(strlen(T3F_DEVELOPER_NAME) + 1);
		if(t3f_developer_name)
		{
			strcpy(t3f_developer_name, T3F_DEVELOPER_NAME);
		}
	#endif
	#ifdef T3F_PACKAGE_NAME
		t3f_package_name = malloc(strlen(T3F_PACKAGE_NAME) + 1);
		if(t3f_package_name)
		{
			strcpy(t3f_package_name, T3F_PACKAGE_NAME);
		}
	#endif

	/* initialize Allegro */
	if(!al_init())
	{
		printf("Could not initialize Allegro!\n");
		return 0;
	}

	al_set_app_name(T3F_APP_DATA_DIR);
	if(t3f_developer_name)
	{
		al_set_org_name(t3f_developer_name);
	}

	/* set up application path */
	t3f_config_path = al_get_standard_path(ALLEGRO_USER_SETTINGS_PATH);
	t3f_data_path = al_get_standard_path(ALLEGRO_USER_DATA_PATH);
	#ifdef ALLEGRO_WINDOWS
		al_append_path_component(t3f_config_path, "config");
		al_append_path_component(t3f_data_path, "data");
	#endif
	t3f_temp_path = al_get_standard_path(ALLEGRO_TEMP_PATH);
	t3f_setup_directories(t3f_config_path);
	t3f_setup_directories(t3f_data_path);

	/* set default options */
	#ifdef ALLEGRO_ANDROID
		t3f_option[T3F_OPTION_RENDER_MODE] = T3F_RENDER_MODE_ALWAYS_CLEAR;
	#endif

	/* set up configuration file */
	temp_path = al_clone_path(t3f_config_path);
	al_set_path_filename(temp_path, "settings.ini");
	strcpy(t3f_config_filename, al_path_cstr(temp_path, '/'));
	al_destroy_path(temp_path);
	old_interface = al_get_new_file_interface();
	al_set_standard_file_interface();
	t3f_config = al_load_config_file(t3f_config_filename);
	al_set_new_file_interface(old_interface);
	if(!t3f_config)
	{
		t3f_config = al_create_config();
	}
	t3f_get_options();

	/* set up user data file */
	temp_path = al_clone_path(t3f_data_path);
	al_set_path_filename(temp_path, "user_data.ini");
	strcpy(t3f_user_data_filename, al_path_cstr(temp_path, '/'));
	al_destroy_path(temp_path);
	old_interface = al_get_new_file_interface();
	al_set_standard_file_interface();
	t3f_user_data = al_load_config_file(t3f_user_data_filename);
	al_set_new_file_interface(old_interface);
	if(!t3f_user_data)
	{
		t3f_user_data = al_create_config();
	}

	if(!al_init_image_addon())
	{
		printf("Failed to initialize image add-on!\n");
		return 0;
	}
	#ifdef T3F_PNG
		t3f_enable_internal_png_handler();
	#endif
	al_init_font_addon();
	if(!al_init_ttf_addon())
	{
		printf("Failed to initialize TTF add-on!\n");
		return 0;
	}
	if(flags & T3F_USE_SOUND)
	{
		if(al_install_audio() && al_reserve_samples(16) && al_init_acodec_addon())
		{
			t3f_flags |= T3F_USE_SOUND;
		}
	}
	if(flags & T3F_USE_KEYBOARD)
	{
		if(_t3f_initialize_keyboard())
		{
			t3f_flags |= T3F_USE_KEYBOARD;
		}
	}
	if(flags & T3F_USE_MOUSE)
	{
		if(_t3f_initialize_mouse())
		{
			t3f_flags |= T3F_USE_MOUSE;
		}
	}
	if(flags & T3F_USE_JOYSTICK)
	{
		if(al_install_joystick())
		{
			t3f_flags |= T3F_USE_JOYSTICK;
		}
	}

	if(!_t3f_initialize_touch_data())
	{
		return 0;
	}
	if(flags & T3F_USE_TOUCH)
	{
		if(_t3f_initialize_touch())
		{
			t3f_flags |= T3F_USE_TOUCH;
		}
	}
	al_init_primitives_addon();
	#ifndef ALLEGRO_ANDROID
		al_init_native_dialog_addon();
	#endif

	_t3f_initialize_resource_manager(1024);

	strcpy(t3f_window_title, name);
	al_set_new_window_title(t3f_window_title);

	t3f_timer = al_create_timer(1.000 / fps);
	if(!t3f_timer)
	{
		printf("Failed to create timer!\n");
		return 0;
	}
	_t3f_timer_tick_minimum_time = (1.000 / fps) / 2.0;

	t3f_queue = al_create_event_queue();
	if(!t3f_queue)
	{
		printf("Failed to create event queue!\n");
		return 0;
	}

	/* create display unless we have opted for no display */
	if(!(flags & T3F_NO_DISPLAY))
	{
		if(!t3f_set_gfx_mode(w, h, flags))
		{
			printf("Failed to create display!\n");
			return 0;
		}
	}
	else
	{
		t3f_flags |= T3F_NO_DISPLAY;
	}

	if(t3f_flags & T3F_USE_KEYBOARD)
	{
		al_register_event_source(t3f_queue, al_get_keyboard_event_source());
	}
	if(t3f_flags & T3F_USE_MOUSE)
	{
		al_register_event_source(t3f_queue, al_get_mouse_event_source());
	}
	if(t3f_flags & T3F_USE_JOYSTICK)
	{
		al_register_event_source(t3f_queue, al_get_joystick_event_source());
		for(i = 0; i < al_get_num_joysticks(); i++)
		{
			t3f_joystick[i] = al_get_joystick(i);
		}
	}
	if(t3f_flags & T3F_USE_TOUCH)
	{
		al_register_event_source(t3f_queue, al_get_touch_input_event_source());
	}
	al_register_event_source(t3f_queue, al_get_timer_event_source(t3f_timer));

	if(!(t3f_flags & T3F_NO_DISPLAY))
	{
		/* create a default view */
		t3f_default_view = t3f_create_view(NULL, 0, 0, al_get_display_width(t3f_display), al_get_display_height(t3f_display), t3f_virtual_display_width / 2, t3f_virtual_display_height / 2, t3f_flags);
		if(!t3f_default_view)
		{
			printf("Failed to create default view!\n");
			return 0;
		}
		t3f_select_view(t3f_default_view);
	}

	t3f_color_white = al_map_rgba_f(1.0, 1.0, 1.0, 1.0);
	t3f_color_black = al_map_rgba_f(0.0, 0.0, 0.0, 1.0);
	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
	#ifndef T3F_NO_KEEP_AWAKE
		al_inhibit_screensaver(true); // stop screensaver from showing
	#endif

	t3f_logic_proc = logic_proc;
	t3f_render_proc = render_proc;
	t3f_app_data = data;

	/* locate user resources */
	t3f_locate_resource("data/t3f.dat");

	/* initialize subsystems */
	if(!_t3f_init_bitmap_system())
	{
		return 0;
	}
	if(!_t3f_init_animation_system())
	{
		return 0;
	}
	_t3f_reset_android_bg_color();

	return 1;
}

void t3f_set_option(int option, int value)
{
	char buf[64] = {0};
	char vbuf[64] = {0};

	t3f_option[option] = value;
	snprintf(buf, 64, "Key %d", option);
	snprintf(vbuf, 64, "%d", value);
	al_set_config_value(t3f_config, "Options", buf, vbuf);
}

bool t3f_option_is_set(int option)
{
	char buf[64] = {0};
	const char * val;

	sprintf(buf, "Key %d", option);
	val = al_get_config_value(t3f_config, "Options", buf);

	if(val)
	{
		return true;
	}
	return false;
}

static void handle_view_resize(void)
{
	t3f_adjust_view(t3f_default_view, 0, 0, al_get_display_width(t3f_display), al_get_display_height(t3f_display), t3f_virtual_display_width / 2, t3f_virtual_display_height / 2, t3f_flags);
	t3f_default_view->need_update = true;
	t3f_select_view(t3f_default_view);
	al_set_clipping_rectangle(0, 0, al_get_display_width(t3f_display), al_get_display_height(t3f_display));
	al_clear_to_color(al_map_rgb_f(0.0, 0.0, 0.0));
	al_flip_display();
	al_clear_to_color(al_map_rgb_f(0.0, 0.0, 0.0));
	t3f_select_view(t3f_current_view);
}

static int t3f_set_new_gfx_mode(int w, int h, int flags)
{
	char val[128] = {0};
	int ret = 1;
	int new_flags = 0;

	if(flags & T3F_USE_FULLSCREEN)
	{
		new_flags = ALLEGRO_FULLSCREEN_WINDOW;
	}
	if(flags & T3F_RESIZABLE)
	{
		new_flags = ALLEGRO_RESIZABLE;
	}
	if(flags & T3F_RESIZABLE)
	{
		if(!(t3f_flags & T3F_RESIZABLE))
		{
			ret = 2;
		}
	}
	else
	{
		if(t3f_flags & T3F_RESIZABLE)
		{
			ret = 2;
		}
	}

	/* don't attempt to set new video mode if we already know we need to destroy the display
	 * to get the type of display requested */
	if(ret != 2)
	{
		if(flags & T3F_USE_FULLSCREEN)
		{
			/* toggle flag if going from window to full screen */
			if(!(t3f_flags & T3F_USE_FULLSCREEN))
			{
				if(!al_toggle_display_flag(t3f_display, ALLEGRO_FULLSCREEN_WINDOW, true))
				{
					ret = 2;
				}
				else
				{
					t3f_flags |= T3F_USE_FULLSCREEN;
				}
			}
		}
		else
		{
			/* if we are switching from full screen to window */
			if(t3f_flags & T3F_USE_FULLSCREEN)
			{
				if(!al_toggle_display_flag(t3f_display, ALLEGRO_FULLSCREEN_WINDOW, false))
				{
					ret = 2;
				}
				else
				{
					t3f_flags &= ~T3F_USE_FULLSCREEN;
					if(!al_resize_display(t3f_display, w, h))
					{
						ret = 0;
					}
				}
			}
			else
			{
				if(!al_resize_display(t3f_display, w, h))
				{
					ret = 0;
				}
			}
		}
	}

	/* destroy old display and create a new one if operation failed in some way */
	if(ret != 1)
	{
		t3f_unload_atlases();
		t3f_unload_resources();
		al_destroy_display(t3f_display);
		al_set_new_display_flags(new_flags);
		t3f_display = al_create_display(w, h);
		if(t3f_display)
		{
			t3f_reload_resources();
			t3f_rebuild_atlases();
			ret = 1;
		}
		if(flags & T3F_USE_FULLSCREEN)
		{
			t3f_flags |= T3F_USE_FULLSCREEN;
		}
		else
		{
			t3f_flags &= ~T3F_USE_FULLSCREEN;
		}
		if(flags & T3F_RESIZABLE)
		{
			t3f_flags |= T3F_RESIZABLE;
		}
		else
		{
			t3f_flags &= ~T3F_RESIZABLE;
		}
	}
	else
	{
		#ifdef ALLEGRO_WINDOWS
			if(!(t3f_flags & T3F_USE_OPENGL))
			{
				t3f_unload_atlases();
				t3f_unload_resources();
				t3f_reload_resources();
				t3f_rebuild_atlases();
			}
		#endif
	}

	/* update settings if we successfully set the new mode */
	if(ret == 1)
	{
		if(flags & T3F_RESET_DISPLAY)
		{
			t3f_default_view->virtual_width = w;
			t3f_default_view->virtual_height = h;
			t3f_virtual_display_width = w;
			t3f_virtual_display_height = h;
		}
		handle_view_resize();
		if(t3f_flags & T3F_USE_FULLSCREEN)
		{
			al_set_config_value(t3f_config, "T3F", "force_fullscreen", "true");
		}
		else
		{
			al_set_config_value(t3f_config, "T3F", "force_fullscreen", "false");
		}
		sprintf(val, "%d", al_get_display_width(t3f_display));
		al_set_config_value(t3f_config, "T3F", "display_width", val);
		sprintf(val, "%d", al_get_display_height(t3f_display));
		al_set_config_value(t3f_config, "T3F", "display_height", val);
		t3f_default_view->need_update = true;
		t3f_select_view(t3f_default_view);
	}

	return ret;
}

/* returns 1 on success, 0 on failure, 2 if toggling fullscreen/window failed */
int t3f_set_gfx_mode(int w, int h, int flags)
{
	const char * cvalue = NULL;
	const char * cvalue2 = NULL;
	int dflags = ALLEGRO_PROGRAMMABLE_PIPELINE;
	int dx, dy, doy;
	int dw, dh;
	int ret = 1;
	bool restore_pos = false;

	bool fsw_supported = true;
	#ifdef ALLEGRO_ANDROID
		bool no_windowed = true; // is full screen window supported?
	#else
		bool no_windowed = false;
	#endif

	if(flags & T3F_USE_FIXED_PIPELINE)
	{
		dflags = 0;
	}
	/* disable fsw support if the config file says to */
	cvalue = al_get_config_value(t3f_config, "T3F", "real_fullscreen");
	if(cvalue && !strcmp(cvalue, "true"))
	{
		fsw_supported = false;
	}
	cvalue = al_get_config_value(t3f_config, "T3F", "force_resizable");
	if(cvalue && !strcmp(cvalue, "true"))
	{
		flags |= T3F_RESIZABLE;
	}

	if(t3f_display)
	{
		ret = t3f_set_new_gfx_mode(w, h, flags);
	}

	/* first time creating display */
	else
	{
		/* see if we want a menu */
		if(flags & T3F_USE_MENU)
		{
			/* use GTK on Linux so menus will work */
			#ifdef ALLEGRO_GTK_TOPLEVEL
				dflags |= ALLEGRO_GTK_TOPLEVEL;
			#endif
			t3f_flags |= T3F_USE_MENU;
		}

		if(flags & T3F_NO_SCALE)
		{
			t3f_flags |= T3F_NO_SCALE;
		}
		if(flags & T3F_ANY_ORIENTATION)
		{
			al_set_new_display_option(ALLEGRO_SUPPORTED_ORIENTATIONS, ALLEGRO_DISPLAY_ORIENTATION_ALL, ALLEGRO_REQUIRE);
			t3f_flags |= T3F_ANY_ORIENTATION;
		}
		else
		{
			if(w > h)
			{
				al_set_new_display_option(ALLEGRO_SUPPORTED_ORIENTATIONS, ALLEGRO_DISPLAY_ORIENTATION_LANDSCAPE, ALLEGRO_REQUIRE);
			}
			else
			{
				al_set_new_display_option(ALLEGRO_SUPPORTED_ORIENTATIONS, ALLEGRO_DISPLAY_ORIENTATION_PORTRAIT, ALLEGRO_REQUIRE);
			}
		}
		/* if we are using console (for a server, for instance) don't create display */
		cvalue = al_get_config_value(t3f_config, "T3F", "force_fullscreen");
		cvalue2 = al_get_config_value(t3f_config, "T3F", "force_window");
		if(((flags & T3F_USE_FULLSCREEN || (cvalue && !strcmp(cvalue, "true"))) && !(cvalue2 && !strcmp(cvalue2, "true"))) || no_windowed)
		{
			if(fsw_supported)
			{
				dflags |= ALLEGRO_FULLSCREEN_WINDOW;
			}
			else
			{
				dflags |= ALLEGRO_FULLSCREEN;
			}
			t3f_flags |= T3F_USE_FULLSCREEN;
		}
		else
		{
			t3f_flags &= ~T3F_USE_FULLSCREEN;
		}
		if(flags & T3F_RESIZABLE)
		{
			dflags |= ALLEGRO_RESIZABLE;
			t3f_flags |= T3F_RESIZABLE;
		}
		else
		{
			t3f_flags &= ~T3F_RESIZABLE;
		}
		cvalue = al_get_config_value(t3f_config, "T3F", "disable_aa");
		if(cvalue && !strcmp(cvalue, "true"))
		{
			flags &= ~T3F_ANTIALIASING;
		}
		cvalue = al_get_config_value(t3f_config, "T3F", "enable_aa");
		if(cvalue && !strcmp(cvalue, "false"))
		{
			flags &= ~T3F_ANTIALIASING;
		}
		if((cvalue && !strcmp(cvalue, "true")) || (flags & T3F_ANTIALIASING))
		{
			al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
			al_set_new_display_option(ALLEGRO_SAMPLES, 4, ALLEGRO_SUGGEST);
			t3f_flags |= T3F_ANTIALIASING;
		}
		else
		{
			t3f_flags &= ~T3F_ANTIALIASING;
		}
		cvalue = al_get_config_value(t3f_config, "T3F", "force_opengl");
		if((cvalue && !strcmp(cvalue, "true")) || (flags & T3F_USE_OPENGL))
		{
			dflags |= ALLEGRO_OPENGL;
			t3f_flags |= T3F_USE_OPENGL;
		}
		cvalue = al_get_config_value(t3f_config, "T3F", "force_aspect_ratio");
		if(cvalue)
		{
			if(!strcmp(cvalue, "true"))
			{
				t3f_flags |= T3F_FORCE_ASPECT;
			}
			else
			{
				t3f_flags &= ~T3F_FORCE_ASPECT;
			}
		}
		else
		{
			if(flags & T3F_FORCE_ASPECT)
			{
				t3f_flags |= T3F_FORCE_ASPECT;
			}
			else
			{
				t3f_flags &= ~T3F_FORCE_ASPECT;
			}
		}

		if(flags & T3F_FILL_SCREEN)
		{
			t3f_flags |= T3F_FILL_SCREEN;
		}
		cvalue = al_get_config_value(t3f_config, "T3F", "force_letterbox");
		if(cvalue)
		{
			if(!strcmp(cvalue, "true"))
			{
				t3f_flags &= ~T3F_FILL_SCREEN;
			}
		}

		al_set_new_display_flags(dflags);
		al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
		cvalue = al_get_config_value(t3f_config, "T3F", "display_width");
		cvalue2 = al_get_config_value(t3f_config, "T3F", "display_height");
		if(cvalue && cvalue2)
		{
			dw = atoi(cvalue);
			dh = atoi(cvalue2);
		}
		else
		{
			dw = w;
			dh = h;
		}
		/* always create 800x480 display on OpenPandora */
		#ifdef PANDORA
			dw = 800;
			dh = 480;
		#endif
		cvalue = al_get_config_value(t3f_config, "T3F", "save_window_pos");
		if(cvalue)
		{
			if(!strcmp(cvalue, "true"))
			{
				cvalue = al_get_config_value(t3f_config, "T3F", "window_pos_x");
				if(cvalue)
				{
					cvalue2 = al_get_config_value(t3f_config, "T3F", "window_pos_y");
					if(cvalue2)
					{
						restore_pos = true;
						doy = 0;
						dx = atoi(cvalue);
						dy = atoi(cvalue2);
						al_set_new_window_position(dx, dy + doy ? (doy + doy / 2 + 3) : 0);
					}
				}
			}
		}
		t3f_display = al_create_display(dw, dh);
		if(!t3f_display)
		{
			if(!(flags & T3F_USE_FIXED_PIPELINE))
			{
				dflags = ALLEGRO_PROGRAMMABLE_PIPELINE;
			}
			if(flags & T3F_RESIZABLE)
			{
				dflags |= ALLEGRO_RESIZABLE;
			}
			al_set_new_display_flags(dflags);
			al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 0, ALLEGRO_SUGGEST);
			al_set_new_display_option(ALLEGRO_SAMPLES, 0, ALLEGRO_SUGGEST);
			t3f_display = al_create_display(dw, dh);
			if(!t3f_display)
			{
				return 0;
			}
			t3f_flags = t3f_flags & ~T3F_USE_FULLSCREEN;
			ret = 3;
		}
		al_register_event_source(t3f_queue, al_get_display_event_source(t3f_display));
		t3f_virtual_display_width = w;
		t3f_virtual_display_height = h;
		al_set_window_title(t3f_display, t3f_window_title);
		if(restore_pos)
		{
			al_set_window_position(t3f_display, dx, dy);
		}
		if(t3f_default_view)
		{
			handle_view_resize();
		}
	}
	#ifdef ALLEGRO_WINDOWS
		t3f_set_windows_icon("allegro_icon");
	#endif
	al_set_new_window_position(INT_MAX, INT_MAX);
	return ret;
}

/* set the clipping rectangle, taking the current transformation into account,
 * used in conjunction with the view system you will pass virtual screen
 * coordinates */
void t3f_set_clipping_rectangle(int x, int y, int w, int h)
{
	float tx, ty;
	float twx, twy;
	float ox = 0.0, oy = 0.0;

	if(x < t3f_current_view->left)
	{
		x = t3f_current_view->left;
	}
	else if(x > t3f_current_view->right)
	{
		x = t3f_current_view->right;
	}
	if(y < t3f_current_view->top)
	{
		y = t3f_current_view->top;
	}
	else if(y > t3f_current_view->bottom)
	{
		y = t3f_current_view->bottom;
	}

	/* convert virtual screen coordinates to real display coordinates */
	al_transform_coordinates(&t3f_current_view->transform, &ox, &oy);
	if(w != 0 && h != 0)
	{
		tx = x;
		ty = y;
		twx = x + w;
		twy = y + h;
	}
	else
	{
		tx = t3f_current_view->left;
		ty = t3f_current_view->top;
		twx = t3f_current_view->right;
		twy = t3f_current_view->bottom;
	}
	al_transform_coordinates(&t3f_current_view->transform, &tx, &ty);
	al_transform_coordinates(&t3f_current_view->transform, &twx, &twy);
	al_set_clipping_rectangle(tx + 0.5, ty + 0.5, twx - tx + 0.5, twy - ty + 0.5);
}

void t3f_set_event_handler(void (*proc)(ALLEGRO_EVENT * event, void * data))
{
	t3f_event_handler_proc = proc;
}

bool t3f_save_config(void)
{
	const ALLEGRO_FILE_INTERFACE * old_interface;
	bool ret = false;

	if(t3f_config)
	{
		old_interface = al_get_new_file_interface();
		al_set_standard_file_interface();
		ret = al_save_config_file(t3f_config_filename, t3f_config);
		al_set_new_file_interface(old_interface);
	}

	return ret;
}

bool t3f_save_user_data(void)
{
	const ALLEGRO_FILE_INTERFACE * old_interface;
	bool ret = false;

	if(t3f_user_data)
	{
		old_interface = al_get_new_file_interface();
		al_set_standard_file_interface();
		ret = al_save_config_file(t3f_user_data_filename, t3f_user_data);
		al_set_new_file_interface(old_interface);
	}

	return ret;
}

void t3f_exit(void)
{
	char buf[256];
	int x, y;

	al_get_window_position(t3f_display, &x, &y);
	sprintf(buf, "%d", x);
	al_set_config_value(t3f_config, "T3F", "window_pos_x", buf);
	sprintf(buf, "%d", y);
	al_set_config_value(t3f_config, "T3F", "window_pos_y", buf);
	t3f_quit = true;
}

float t3f_distance(float x1, float y1, float x2, float y2)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	return sqrt(dx * dx + dy * dy);
}

bool t3f_push_state(int flags)
{
	if(t3f_state_stack_size < T3F_MAX_STACK)
	{
		al_store_state(&t3f_state_stack[t3f_state_stack_size], flags);
		t3f_state_stack_size++;
		return true;
	}
	return false;
}

bool t3f_pop_state(void)
{
	if(t3f_state_stack_size > 0)
	{
		al_restore_state(&t3f_state_stack[t3f_state_stack_size - 1]);
		t3f_state_stack_size--;
		return true;
	}
	return false;
}

int t3f_get_joystick_number(ALLEGRO_JOYSTICK * jp)
{
	int i;

	for(i = 0; i < al_get_num_joysticks(); i++)
	{
		if(jp == t3f_joystick[i] && t3f_joystick[i] != NULL)
		{
			return i;
		}
	}
	return -1;
}

ALLEGRO_FILE * t3f_open_file(ALLEGRO_PATH * pp, const char * fn, const char * m)
{
	ALLEGRO_PATH * tpath = al_clone_path(pp);
	al_set_path_filename(tpath, fn);
	return al_fopen(al_path_cstr(tpath, '/'), m);
}

unsigned long t3f_checksum_file(const char * fn)
{
	ALLEGRO_FILE * fp;
	unsigned long sum = 0;
	int c;

	fp = al_fopen(fn, "rb");
	if(fp)
	{
		while(!al_feof(fp))
		{
			c = al_fgetc(fp);
			if(c != EOF)
			{
				sum += c;
			}
		}
		al_fclose(fp);
	}
	return sum;
}

bool t3f_copy_file(const char * src, const char * dest)
{
	ALLEGRO_FILE * fsrc;
	ALLEGRO_FILE * fdest;
	char c;

	fsrc = al_fopen(src, "rb");
	if(!fsrc)
	{
		return false;
	}
	fdest = al_fopen(dest, "wb");
	if(!fdest)
	{
		al_fclose(fsrc);
		return false;
	}
	while(!al_feof(fsrc))
	{
		c = al_fgetc(fsrc);
		al_fputc(fdest, c);
	}
	al_fclose(fdest);
	al_fclose(fsrc);
	return true;
}

void t3f_event_handler(ALLEGRO_EVENT * event)
{
	_t3f_handle_keyboard_event(event);
	_t3f_handle_mouse_event(event);
	_t3f_handle_touch_event(event);
	switch(event->type)
	{

		/* user pressed close button */
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
		{
			if(event->display.source == t3f_display)
			{
				t3f_exit();
			}
			break;
		}

		/* window was resized */
		case ALLEGRO_EVENT_DISPLAY_RESIZE:
		{
			char val[8] = {0};
			al_acknowledge_resize(t3f_display);
			#ifndef ALLEGRO_ANDROID
				t3f_handle_menu_resize();
			#endif
			#ifdef ALLEGRO_WINDOWS
				if(!(t3f_flags & T3F_USE_OPENGL))
				{
					al_stop_timer(t3f_timer);
					t3f_unload_atlases();
					t3f_unload_resources();
					t3f_reload_resources();
					t3f_rebuild_atlases();
					al_start_timer(t3f_timer);
				}
			#endif
			handle_view_resize();
			sprintf(val, "%d", al_get_display_width(t3f_display));
			al_set_config_value(t3f_config, "T3F", "display_width", val);
			sprintf(val, "%d", al_get_display_height(t3f_display));
			al_set_config_value(t3f_config, "T3F", "display_height", val);
			break;
		}

		case ALLEGRO_EVENT_DISPLAY_FOUND:
		{
			al_stop_timer(t3f_timer);
			t3f_unload_atlases();
			t3f_unload_resources();
			t3f_reload_resources();
			t3f_rebuild_atlases();
			al_start_timer(t3f_timer);
			break;
		}

		case ALLEGRO_EVENT_DISPLAY_ORIENTATION:
		{
			break;
		}

		case ALLEGRO_EVENT_JOYSTICK_CONFIGURATION:
		{
			int i;
			al_reconfigure_joysticks();
			memset(t3f_joystick, 0, sizeof(ALLEGRO_JOYSTICK *));
			for(i = 0; i < T3F_MAX_JOYSTICKS; i++)
			{
				memset(&t3f_joystick_state[i], 0, sizeof(ALLEGRO_JOYSTICK_STATE));
			}
			memset(t3f_joystick_state_updated, 0, sizeof(bool));
			for(i = 0; i < al_get_num_joysticks(); i++)
			{
				t3f_joystick[i] = al_get_joystick(i);
			}
			_t3f_input_handle_joystick_event(event);
			break;
		}

		case ALLEGRO_EVENT_JOYSTICK_AXIS:
		case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN:
		case ALLEGRO_EVENT_JOYSTICK_BUTTON_UP:
		{
			int jn = t3f_get_joystick_number(event->joystick.id);
			if(jn >= 0)
			{
				al_get_joystick_state(event->joystick.id, &t3f_joystick_state[jn]);
				t3f_joystick_state_updated[jn] = true;
			}
			_t3f_input_handle_joystick_event(event);
			break;
		}

		/* handle drawing halt */
		case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
		{
			al_stop_timer(t3f_timer);
			t3f_unload_atlases();
			t3f_unload_resources();
			t3f_halted = 1;
			if(t3f_stream)
			{
				t3f_pause_music();
			}
			t3f_save_config();
			t3f_save_user_data();
			break;
		}
		case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
		{
			al_acknowledge_drawing_resume(t3f_display);
			t3f_halted = 0;
			t3f_reload_resources();
			t3f_rebuild_atlases();
			if(t3f_stream)
			{
				t3f_resume_music();
			}
			t3f_set_clipping_rectangle(0, 0, 0, 0);
			al_start_timer(t3f_timer);
			break;
		}
		#ifndef ALLEGRO_ANDROID
			case ALLEGRO_EVENT_MENU_CLICK:
			{
				t3f_process_menu_click(event->user.data1, t3f_app_data);
				break;
			}
		#endif

		/* this keeps your program running */
		case ALLEGRO_EVENT_TIMER:
		{
			if(event->any.timestamp - _t3f_last_timer_tick_timestamp >= _t3f_timer_tick_minimum_time)
			{
				_t3f_last_timer_tick_timestamp = event->any.timestamp;
				_t3f_logic_ticks++;
			}
			break;
		}
	}
}

/* called when it's time to render */
void t3f_render(bool flip)
{
	if(t3f_display && t3f_render_proc && !t3f_halted)
 	{
		/* some video drivers and compositors may leave junk in the buffers,
		   this config file setting will work around the issue by clearing the
		   entire buffer before drawing anything */
		if(t3f_option[T3F_OPTION_RENDER_MODE] == T3F_RENDER_MODE_ALWAYS_CLEAR)
		{
			al_set_clipping_rectangle(0, 0, al_get_display_width(t3f_display), al_get_display_height(t3f_display));
			al_clear_to_color(al_map_rgb_f(0.0, 0.0, 0.0));
			t3f_select_view(t3f_current_view);
		}
		al_use_transform(&t3f_current_view->transform);
		t3f_render_proc(t3f_app_data);
		if(flip)
		{
			al_flip_display();
			t3f_need_redraw = false;
		}
	}
}

void t3f_process_events(bool ignore)
{
	ALLEGRO_EVENT event;

	while(al_get_next_event(t3f_queue, &event))
	{
		if(!ignore)
		{
			if(t3f_event_handler_proc)
			{
				t3f_event_handler_proc(&event, t3f_app_data);
			}
			else
			{
				t3f_event_handler(&event);
			}
		}
	}
}

/* this function is where it's at
   somewhere in your logic code you need to set t3f_quit = true to exit */
void t3f_run(void)
{
	ALLEGRO_EVENT event;
	int i;

	al_start_timer(t3f_timer);
	while(!t3f_quit)
	{
		/* call queued up procudure */
		if(t3f_queued_call_proc)
		{
			t3f_queued_call_proc(t3f_queued_call_data);
			t3f_queued_call_proc = NULL;
			t3f_queued_call_data = NULL;
		}
		al_wait_for_event(t3f_queue, &event);
		if(t3f_event_handler_proc)
		{
			t3f_event_handler_proc(&event, t3f_app_data);
		}
		else
		{
			t3f_event_handler(&event);
		}

		for(i = 0; i < _t3f_logic_ticks && !t3f_quit; i++)
		{
			t3f_android_support_helper();
			#ifndef ALLEGRO_ANDROID
				t3f_update_menus(t3f_app_data);
			#endif
			if(!(t3f_flags & T3F_NO_DISPLAY))
			{
				t3f_select_input_view(t3f_default_view);
			}
			t3f_logic_proc(t3f_app_data);
			t3f_need_redraw = true;
		}
		_t3f_logic_ticks = 0;

       	/* draw after we have run all the logic */
		if(t3f_need_redraw && al_event_queue_is_empty(t3f_queue))
		{
			t3f_render(true);
		}
		if(t3f_halted == 1)
		{
			al_acknowledge_drawing_halt(t3f_display);
			t3f_halted = 2;
		}
	}
	al_stop_timer(t3f_timer);
	while(!al_event_queue_is_empty(t3f_queue))
	{
		al_wait_for_event(t3f_queue, &event);
	}
}

void t3f_finish(void)
{
	t3f_save_config();
	t3f_save_user_data();
	_t3f_exit_animation_system();
	_t3f_exit_bitmap_system();
	_t3f_uninitialize_resource_manager();
	if(t3f_timer)
	{
		al_destroy_timer(t3f_timer);
	}
	if(t3f_display)
	{
		al_destroy_display(t3f_display);
	}
	if(t3f_queue)
	{
		al_destroy_event_queue(t3f_queue);
	}
	if(t3f_flags & T3F_USE_SOUND)
	{
		_t3f_clean_up_sound_data();
		t3f_stop_music();
	}
	if(t3f_flags & T3F_USE_KEYBOARD)
	{
		_t3f_uninitialize_keyboard();
	}
	if(t3f_flags & T3F_USE_MOUSE)
	{
		_t3f_uninitialize_mouse();
	}
	_t3f_uninitialize_touch_data();
	if(t3f_developer_name)
	{
		free(t3f_developer_name);
	}
	if(t3f_package_name)
	{
		free(t3f_package_name);
	}
}

char * t3f_get_filename(ALLEGRO_PATH * path, const char * fn, char * buffer, int buffer_size)
{
	const char * path_cstr;
	ALLEGRO_PATH * temp_path = al_clone_path(path);
	if(!temp_path)
	{
		return NULL;
	}
	al_set_path_filename(temp_path, fn);
	path_cstr = al_path_cstr(temp_path, '/');
	if(strlen(path_cstr) < buffer_size)
	{
		strcpy(buffer, path_cstr);
	}
	al_destroy_path(temp_path);
	return buffer;
}

void t3f_destroy_view(T3F_VIEW * vp)
{
	al_free(vp);
}

void t3f_store_state(T3F_VIEW * sp)
{
	memcpy(sp, t3f_current_view, sizeof(T3F_VIEW));
}

void t3f_restore_state(T3F_VIEW * sp)
{
	memcpy(t3f_current_view, sp, sizeof(T3F_VIEW));
}

bool t3f_queue_call(void (*proc)(void * data), void * data)
{
	if(!t3f_queued_call_proc)
	{
		t3f_queued_call_proc = proc;
		t3f_queued_call_data = data;
		return true;
	}
	return false;
}
