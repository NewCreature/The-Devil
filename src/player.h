#ifndef PLAYER_H
#define PLAYER_H

extern float fire_angle;

void player_lose_life(void * data);
void player_logic(void * data);
void player_render_shadow(void * data);
void player_render(void * data);

#endif
