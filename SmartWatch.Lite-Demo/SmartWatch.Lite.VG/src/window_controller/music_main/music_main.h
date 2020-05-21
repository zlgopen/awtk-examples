#ifndef MUSIC_MAIN_H
#define MUSIC_MAIN_H

#include "awtk.h"
#include "../controller_base.h"

BEGIN_C_DECLS

typedef enum _music_list_state_type_t {
  MUSIC_STATE_PLAY_LOOP_TYPE = 0,
  MUSIC_STATE_PLAY_LOOP_SINGLE_TYPE,
  MUSIC_STATE_PLAY_IMMEDITATELY_TYPE,
  MUSIC_STATE_COUNT,
} music_list_state_type_t;

static const char *music_list_state_type_string_list[] = {
    "music_loop", "music_loop_single", "music_immeditately"};

typedef struct _music_main_t {
  controller_base_t base;

  bool_t is_play;
  int32_t song_id;
  int32_t music_vol;
  uint32_t timer_id;
  uint32_t song_time;
  widget_t *song_name_lable;
  widget_t *song_play_button;
  widget_t *music_vol_slider;
  music_list_state_type_t music_state;
} music_main_t;

music_main_t *music_main_init(widget_t *gesture_manager,
                              widget_t *window_anim_menu);

ret_t music_main_set_enable(music_main_t *handle, bool_t enable);

END_C_DECLS

#endif /*MUSIC_MAIN_H*/
