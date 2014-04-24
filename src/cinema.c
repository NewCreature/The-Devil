#include "t3f/t3f.h"
#include "t3f/resource.h"
#include "main.h"
#include "cinema.h"
#include "title.h"

CINEMA * create_cinema(void)
{
	CINEMA * cp;
	
	cp = malloc(sizeof(CINEMA));
	if(!cp)
	{
		return NULL;
	}
	cp->animations = 0;
	cp->frames = 0;
	return cp;
}

void destroy_cinema(CINEMA * cp)
{
	int i;
	
	for(i = 0; i < cp->animations; i++)
	{
		t3f_destroy_animation(cp->animation[i]);
	}
	t3f_destroy_resource(cp->font);
	t3f_destroy_resource(cp->bg_image);
	free(cp);
}

void add_cinema_animation(CINEMA * cp, T3F_ANIMATION * ap)
{
	if(cp && ap && cp->animations < CINEMA_MAX_ANIMATIONS)
	{
		cp->animation[cp->animations] = ap;
		cp->animations++;
	}
}

void delete_cinema_animation(CINEMA * cp, int index)
{
	int i;
	
	if(cp && index < cp->animations)
	{
		t3f_destroy_animation(cp->animation[index]);
		for(i = index; i < cp->animations - 1; i++)
		{
			cp->animation[i] = cp->animation[i + 1];
		}
		cp->animations--;
	}
}

CINEMA * load_cinema(const char * fn, int flags)
{
	CINEMA * cp;
	char header[16] = {0};
	ALLEGRO_FILE * fp;
	int i, j, t;
	
	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		return NULL;
	}
	if(al_fread(fp, header, 16) != 16)
	{
		al_fclose(fp);
		return NULL;
	}
	if(memcmp(header, "CINMA", 5))
	{
		al_fclose(fp);
		return NULL;
	}
	cp = create_cinema();
	if(!cp)
	{
		al_fclose(fp);
		return NULL;
	}
	cp->animations = al_fread16le(fp);
	for(i = 0; i < cp->animations; i++)
	{
		cp->animation[i] = t3f_load_animation_f(fp, fn);
	}
	cp->frames = al_fread16le(fp);
	for(i = 0; i < cp->frames; i++)
	{
		cp->frame[i].entities = al_fread16le(fp);
		for(j = 0; j < cp->frame[i].entities; j++)
		{
			cp->frame[i].entity[j].type = al_fgetc(fp);
			cp->frame[i].entity[j].index = al_fread16le(fp);
			cp->frame[i].entity[j].x = t3f_fread_float(fp);
			cp->frame[i].entity[j].y = t3f_fread_float(fp);
			cp->frame[i].entity[j].z = t3f_fread_float(fp);
			cp->frame[i].entity[j].scale = t3f_fread_float(fp);
			cp->frame[i].entity[j].angle = t3f_fread_float(fp);
			memset(cp->frame[i].entity[j].data, 0, 256);
			t = al_fread16le(fp);
			al_fread(fp, cp->frame[i].entity[j].data, t);
		}
		cp->frame[i].ticks = al_fread16le(fp);
	}
	al_fclose(fp);

	cp->bg_image = t3f_load_resource((void **)(&cp->bg_image), T3F_RESOURCE_TYPE_BITMAP, "data/graphics/bg00.png", 0, 0, 0);
	if(!cp->bg_image)
	{
		return false;
	}

	cp->font = t3f_load_resource((void **)(&cp->font), T3F_RESOURCE_TYPE_FONT, "data/fonts/isle_of_the_dead.ttf", 36, 0, 0);
	if(!cp->font)
	{
		return NULL;
	}
	return cp;
}

bool save_cinema(CINEMA * cp, const char * fn)
{
	char header[16] = {'C', 'I', 'N', 'M', 'A'};
	ALLEGRO_FILE * fp;
	int i, j;
	
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		return false;
	}
	if(al_fwrite(fp, header, 16) != 16)
	{
		al_fclose(fp);
		return false;
	}
	al_fwrite16le(fp, cp->animations);
	for(i = 0; i < cp->animations; i++)
	{
		t3f_save_animation_f(cp->animation[i], fp);
	}
	al_fwrite16le(fp, cp->frames);
	for(i = 0; i < cp->frames; i++)
	{
		al_fwrite16le(fp, cp->frame[i].entities);
		for(j = 0; j < cp->frame[i].entities; j++)
		{
			al_fputc(fp, cp->frame[i].entity[j].type);
			al_fwrite16le(fp, cp->frame[i].entity[j].index);
			t3f_fwrite_float(fp, cp->frame[i].entity[j].x);
			t3f_fwrite_float(fp, cp->frame[i].entity[j].y);
			t3f_fwrite_float(fp, cp->frame[i].entity[j].z);
			t3f_fwrite_float(fp, cp->frame[i].entity[j].scale);
			t3f_fwrite_float(fp, cp->frame[i].entity[j].angle);
			al_fwrite16le(fp, strlen(cp->frame[i].entity[j].data));
			al_fwrite(fp, cp->frame[i].entity[j].data, strlen(cp->frame[i].entity[j].data));
		}
		al_fwrite16le(fp, cp->frame[i].ticks);
	}
	al_fclose(fp);
	return true;
}

void cinema_logic(CINEMA * cp)
{
	
	/* skip cinema */
	if(t3f_key[ALLEGRO_KEY_ESCAPE])
	{
		cp->position = cp->frames;
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
	}
	cp->tick++;
	if(t3f_key[ALLEGRO_KEY_SPACE] || t3f_key[ALLEGRO_KEY_ENTER] || ((title_touched() || title_joystick_button_pressed()) && !click) || cp->tick > 600)
	{
		cp->position++;
		t3f_key[ALLEGRO_KEY_SPACE] = 0;
		t3f_key[ALLEGRO_KEY_ENTER] = 0;
		click = true;
		cp->tick = 0;
	}
	if(!title_touched() && !title_joystick_button_pressed())
	{
		click = false;
	}
	
}

static float cinema_get_text_tab(ALLEGRO_FONT * fp, const char * text)
{
	char buffer[64] = {0};
	int i;
	
	for(i = 0; i < strlen(text); i++)
	{
		buffer[i] = text[i];
		if(text[i - 1] == ':' && text[i] == ' ')
		{
			break;
		}
	}
	return al_get_text_width(fp, buffer);
}

typedef struct
{
	
	char text[256];
	
} CINEMA_TEXT_LINE;

typedef struct
{
	
	ALLEGRO_FONT * font;
	CINEMA_TEXT_LINE line[64];
	int lines;
	float tab;
	
} CINEMA_TEXT_LINE_DATA;

void cinema_draw_text(ALLEGRO_FONT * fp, ALLEGRO_COLOR color, float x, float y, float z, float w, float tab, int flags, const char * text);

/* need to make this not rely on spaces, sometimes there might be long stretches with no space which need to be broken up 'mid-word' */
void cinema_create_text_line_data(CINEMA_TEXT_LINE_DATA * lp, ALLEGRO_FONT * fp, float w, float tab, const char * text)
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
			current_line[current_line_pos] = '\0';
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

void cinema_draw_text_lines(CINEMA_TEXT_LINE_DATA * lines, ALLEGRO_COLOR color, float x, float y, float z)
{
	int i;
	float px = x;
	float py = y;
	
	for(i = 0; i < lines->lines; i++)
	{
		cinema_draw_text(lines->font, color, px, py, z, 0.0, 0.0, 0, lines->line[i].text);
		px = x + lines->tab;
		py += al_get_font_line_height(lines->font);
	}
}

void cinema_draw_text(ALLEGRO_FONT * fp, ALLEGRO_COLOR color, float x, float y, float z, float w, float tab, int flags, const char * text)
{
	CINEMA_TEXT_LINE_DATA line_data;
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
		pos -=al_get_text_width(fp, text) / 2.0;
	}
	else if(flags & T3F_FONT_ALIGN_RIGHT)
	{
		pos -= al_get_text_width(fp, text);
	}
	if(w > 0.0)
	{
		cinema_create_text_line_data(&line_data, fp, w, tab, text);
		cinema_draw_text_lines(&line_data, color, x, y, z);
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

void cinema_render(CINEMA * cp)
{
	int i;
	float tab;
	
	if(cp->position >= cp->frames)
	{
		return;
	}
	for(i = 0; i < cp->frame[cp->position].entities; i++)
	{
		switch(cp->frame[cp->position].entity[i].type)
		{
			case CINEMA_ENTITY_TEXT:
			{
				tab = cinema_get_text_tab(cp->font, cp->frame[cp->position].entity[i].data);
				cinema_draw_text(cp->font, al_map_rgba_f(0.0, 0.0, 0.0, 0.8), 4 + 2, t3f_display_top + 4 + 2, 0, 640 - 8, tab, 0, cp->frame[cp->position].entity[i].data);
				cinema_draw_text(cp->font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 4, t3f_display_top + 4, 0, 640 - 8, tab, 0, cp->frame[cp->position].entity[i].data);
				break;
			}
			case CINEMA_ENTITY_ANIMATION:
			{
				t3f_draw_animation(cp->animation[cp->frame[cp->position].entity[i].index], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 0, cp->frame[cp->position].entity[i].x, cp->frame[cp->position].entity[i].y, cp->frame[cp->position].entity[i].z, 0);
				break;
			}
		}
	}
}
