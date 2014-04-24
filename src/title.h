#ifndef TITLE_H
#define TITLE_H

void menu_fix_leaderboard_text(void);
void menu_fix_controller_type_config(void);
bool title_load_data(void);
void title_free_data(void);
bool title_init(void);
void title_exit(void);
void title_bg_logic(void);
void title_bg_render(void);
void title_in_logic(void);
void title_in_render(void);
bool title_joystick_button_pressed(void);
bool title_touched(void);
void title_process_menu(T3F_GUI * mp);
void title_logic(void);
void title_render(void);
void title_out_logic(void);
void title_out_render(void);
void title_game_logic(void);
void title_game_render(void);

#endif
