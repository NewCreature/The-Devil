#include "t3f/t3f.h"
#include "main.h"
#include "instance.h"

APP_INSTANCE * td_create_instance(void)
{
  APP_INSTANCE * instance;

  instance = malloc(sizeof(APP_INSTANCE));
  if(!instance)
  {
    goto fail;
  }
  memset(instance, 0, sizeof(APP_INSTANCE));

  instance->state = STATE_TITLE_IN;
  instance->controller_type = -1; // indicate we should autodetect controller
  instance->first_run = true;
  instance->current_menu = MENU_TITLE;
  instance->high_score[0] = 10000;
  instance->high_score[1] = 10000;
  instance->high_score[2] = 10000;
  instance->multiplier = 1;
  instance->game_state = GAME_STATE_PLAY;
  instance->game_mode = GAME_MODE_STORY;
  instance->game_mode_text[0] = strdup("Story");
  if(!instance->game_mode_text[0])
  {
    goto fail;
  }
  instance->game_mode_text[1] = strdup("Story-Easy");
  if(!instance->game_mode_text[1])
  {
    goto fail;
  }
  instance->game_mode_text[2] = strdup("Eternal");
  if(!instance->game_mode_text[2])
  {
    goto fail;
  }
  instance->touch_size = 64.0;
  instance->leaderboard_place = -1;
  instance->leaderboard_mode = GAME_MODE_STORY;
  strcpy(instance->network_id, "Anonymous");
  instance->upload_scores = true;
  instance->force_aspect = true;

  return instance;

  fail:
  {
    td_destroy_instance(instance);
    return NULL;
  }
}

void td_destroy_instance(APP_INSTANCE * instance)
{
  int i;

  if(instance)
  {
    for(i = 0; i < 16; i++)
    {
      if(instance->game_mode_text[i])
      {
        free(instance->game_mode_text[i]);
      }
    }
    free(instance);
  }
}
