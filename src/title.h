#ifndef TITLE_H
#define TITLE_H

typedef struct
{
	
	char text[256];
	
} TEXT_LINE;

typedef struct
{
	
	T3F_FONT * font;
	TEXT_LINE line[64];
	int lines;
	float tab;
	
} TEXT_LINE_DATA;

void select_menu(int mmenu, void * data);
void menu_fix_leaderboard_text(void * data);
void menu_fix_controller_type_config(void * data);
bool title_load_data(void * data);
void title_free_data(void * data);
bool title_init(void * data);
void title_exit(void * data);
void title_bg_logic(void * data);
void title_bg_render(T3F_BITMAP * bp);
void title_in_logic(void * data);
void title_in_render(void * data);
bool title_joystick_button_pressed(void);
bool title_touched(void);
void title_process_menu(T3F_GUI * mp, void * data);
void title_logic(void * data);
void title_render(void * data);
void title_out_logic(void * data);
void title_out_render(void * data);
void title_game_logic(void * data);
void title_game_render(void * data);

#endif
