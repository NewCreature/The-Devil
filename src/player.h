#ifndef PLAYER_H
#define PLAYER_H

extern float fire_angle;

void player_lose_life(void);
void player_logic(void);
void player_render_shadow(void);
void player_render(void);

#endif
