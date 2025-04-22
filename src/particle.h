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

void generate_text_particle(char * text, float x, float y, int life, void * data);
void generate_particle(int type, float x, float y, float angle, float speed, int life, void * data);
void generate_death_particles(int type, float x, float y, void * data);
void particle_logic(void * data);
void particle_render(void * data);
void text_particle_logic(void * data);
void text_particle_render(void * data);

#endif
