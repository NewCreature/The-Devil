#ifndef ENEMY_H
#define ENEMY_H

void enemy_spawn_logic_easy(void * data);
void enemy_spawn_logic_normal(void * data);
void enemy_spawn_dark_orb(void * data);
void clear_enemies(void * data);
int count_enemies(void * data);
void enemy_logic(void * data);
void enemy_render(void * data);
void enemy_render_shadow(void * data);

#endif
