#ifndef POWERUP_H
#define POWERUP_H

void generate_powerup(int type, float x, float y, float vx, float vy, void * data);
void powerup_logic(void * data);
void powerup_render(void * data);

#endif
