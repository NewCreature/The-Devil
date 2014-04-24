#ifndef PARTICLE_H
#define PARTICLE_H

#include <allegro5/allegro5.h>

typedef struct
{
	
	char text[64];
	float x, y;
	int tick;
	int life, total_life;
	bool active;
	
} TEXT_PARTICLE;

void generate_text_particle(char * text, float x, float y, int life);
void generate_particle(int type, float x, float y, float angle, float speed, int life);
void generate_death_particles(int type, float x, float y);
void particle_logic(void);
void particle_render(void);
void text_particle_logic(void);
void text_particle_render(void);

#endif
