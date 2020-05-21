#include "music_main.h"
#include "../../custom_widgets/custom_widgets.h"

#define MUSICL_VOL "vol"
#define PREV_BUTTON "prev"
#define NEXT_BUTTON "next"
#define SONG_NAME_LABLE "song_name"
#define SONG_UPDATE_PROGRESS_TIME 1000
#define MUSIC_STATE_BUTTON "music_state"

typedef struct _song_info_t {
  char *song_name;
  uint32_t song_max_time;
} song_info_t;

extern void controller_base_send_exit_fun(controller_base_t *handle);
extern ret_t controller_base_set_enable(controller_base_t *handle,
                                        bool_t enable);

static song_info_t song_name_list[] = {
    {"Shape of you", 60},
    {"That Girl", 100},
    {"Listen", 30},
    {"Wolves", 240},
    {"Trouble is a firend", 90},
    {"Fire", 180},
};

static ret_t music_main_on_click_gesture_fun(gesture_type_t gesture_type,
                                             point_t *start_point,
                                             point_t *curr_point, void *ctx) {
  music_main_t *handle = (music_main_t *)(ctx);
  controller_base_t *base = (controller_base_t *)ctx;

  return_value_if_fail(base->enable, RET_BAD_PARAMS);

  if (gesture_type == GESTURE_LEFT_TO_RIGHT_TYPE) {
    music_main_set_enable(handle, FALSE);
    controller_base_send_exit_fun(base);
  }

  return RET_OK;
}

static ret_t music_main_set_play_button_style(widget_t *song_play_button,
                                              bool_t is_play) {
  ret_t ret = RET_OK;
  if (is_play) {
    ret = widget_use_style(song_play_button, "play");
  } else {
    ret = widget_use_style(song_play_button, "stop");
  }
  widget_invalidate(song_play_button, NULL);
  return ret;
}

static void music_main_on_play_switch_song(music_main_t *handle,
                                           bool_t is_next) {
  int32_t len = ARRAY_LEN(song_name_list);
  if (handle->music_state == MUSIC_STATE_PLAY_LOOP_TYPE ||
      handle->music_state == MUSIC_STATE_PLAY_LOOP_SINGLE_TYPE) {
    if (is_next) {
      handle->song_id++;
      if (handle->song_id >= len) {
        handle->song_id = 0;
      }
    } else {
      handle->song_id--;
      if (handle->song_id < 0) {
        handle->song_id = len - 1;
      }
    }
  } else if (handle->music_state == MUSIC_STATE_PLAY_IMMEDITATELY_TYPE) {
    handle->song_id = rand() % len;
  } else {
    assert(!"not find music song play state");
  }

  widget_set_text_utf8(handle->song_name_lable,
                       song_name_list[handle->song_id].song_name);
  music_play_set_max_value(handle->song_play_button,
                           song_name_list[handle->song_id].song_max_time);
}

static ret_t music_main_on_play_timer(const timer_info_t *timer) {

  music_main_t *handle = (music_main_t *)(timer->ctx);

  if (handle->is_play) {
    handle->song_time++;

    if (handle->song_time >= song_name_list[handle->song_id].song_max_time) {
      handle->song_time = 0;
      if (handle->music_state != MUSIC_STATE_PLAY_LOOP_SINGLE_TYPE) {
        music_main_on_play_switch_song(handle, TRUE);
      }
    }

    music_play_set_value(handle->song_play_button, handle->song_time);
  }

  return RET_REPEAT;
}

static ret_t music_main_on_prev_song(void *ctx, event_t *e) {

  music_main_t *handle = (music_main_t *)ctx;

  handle->song_time = 0;
  music_main_on_play_switch_song(handle, FALSE);
  music_play_set_value(handle->song_play_button, handle->song_time);

  timer_remove(handle->timer_id);
  handle->timer_id =
      timer_add(music_main_on_play_timer, handle, SONG_UPDATE_PROGRESS_TIME);

  handle->is_play = TRUE;
  music_main_set_play_button_style(handle->song_play_button, TRUE);

  return RET_OK;
}

static ret_t music_main_on_next_song(void *ctx, event_t *e) {

  music_main_t *handle = (music_main_t *)ctx;

  handle->song_time = 0;
  music_main_on_play_switch_song(handle, TRUE);
  music_play_set_value(handle->song_play_button, handle->song_time);

  timer_remove(handle->timer_id);
  handle->timer_id =
      timer_add(music_main_on_play_timer, handle, SONG_UPDATE_PROGRESS_TIME);

  handle->is_play = TRUE;
  music_main_set_play_button_style(handle->song_play_button, TRUE);
  return RET_OK;
}

static ret_t music_main_set_music_state(music_main_t *handle,
                                        widget_t *music_state) {
  int32_t music_list_state = (int32_t)(handle->music_state);
  return widget_use_style(music_state,
                          music_list_state_type_string_list[music_list_state]);
}

static ret_t music_main_on_switch_music_state(void *ctx, event_t *e) {

  music_main_t *handle = (music_main_t *)ctx;
  widget_t *music_state = WIDGET(e->target);
  int32_t music_list_state = (int32_t)(handle->music_state);

  music_list_state++;

  if (music_list_state >= MUSIC_STATE_COUNT) {
    music_list_state = 0;
  }

  handle->music_state = (music_list_state_type_t)music_list_state;

  return music_main_set_music_state(handle, music_state);
}

static ret_t music_main_on_switch_paly_and_stop(void *ctx, event_t *e) {

  music_main_t *handle = (music_main_t *)ctx;

  handle->is_play = !handle->is_play;

  music_main_set_play_button_style(handle->song_play_button, handle->is_play);

  return RET_OK;
}

static ret_t music_main_on_save_music_vol(void *ctx, event_t *e) {

  music_main_t *handle = (music_main_t *)ctx;
  handle->music_vol = widget_get_value(WIDGET(e->target));

  return RET_OK;
}

static ret_t music_main_widget_remove_timer_on_destroy(void *ctx, event_t *e) {
  music_main_t *handle = (music_main_t *)ctx;

  timer_remove(handle->timer_id);

  return RET_REMOVE;
}

static ret_t music_main_on_window_anim_load_widget_func(widget_t *load_widget,
                                                        void *ctx) {
  music_main_t *handle = (music_main_t *)ctx;
  widget_t *prev_button = widget_lookup(load_widget, PREV_BUTTON, TRUE);
  widget_t *next_button = widget_lookup(load_widget, NEXT_BUTTON, TRUE);
  widget_t *music_state_button =
      widget_lookup(load_widget, MUSIC_STATE_BUTTON, TRUE);
  return_value_if_fail(prev_button != NULL && next_button != NULL &&
                           music_state_button != NULL,
                       RET_FAIL);

  widget_on(prev_button, EVT_CLICK, music_main_on_prev_song, handle);
  widget_on(next_button, EVT_CLICK, music_main_on_next_song, handle);
  widget_on(music_state_button, EVT_CLICK, music_main_on_switch_music_state,
            handle);

  music_main_set_music_state(handle, music_state_button);

  handle->music_vol_slider = widget_lookup(load_widget, MUSICL_VOL, TRUE);
  handle->song_name_lable = widget_lookup(load_widget, SONG_NAME_LABLE, TRUE);
  handle->song_play_button =
      widget_lookup_by_type(load_widget, WIDGET_TYPE_MUSIC_PLAY_BUTTON, TRUE);
  return_value_if_fail(handle->song_play_button != NULL &&
                           handle->song_name_lable != NULL &&
                           handle->music_vol_slider != NULL,
                       RET_FAIL);

  widget_set_value(handle->music_vol_slider, handle->music_vol);
  widget_on(handle->music_vol_slider, EVT_VALUE_CHANGED,
            music_main_on_save_music_vol, handle);

  widget_set_text_utf8(handle->song_name_lable,
                       song_name_list[handle->song_id].song_name);
  music_main_set_play_button_style(handle->song_play_button, handle->is_play);
  music_play_set_value(handle->song_play_button, handle->song_time);
  music_play_set_max_value(handle->song_play_button,
                           song_name_list[handle->song_id].song_max_time);

  handle->timer_id =
      timer_add(music_main_on_play_timer, handle, SONG_UPDATE_PROGRESS_TIME);
  widget_on(handle->song_play_button, EVT_CLICK,
            music_main_on_switch_paly_and_stop, handle);
  widget_on(handle->song_play_button, EVT_DESTROY,
            music_main_widget_remove_timer_on_destroy, handle);

  return RET_OK;
}

ret_t music_main_set_enable(music_main_t *handle, bool_t enable) {
  controller_base_t *base = (controller_base_t *)handle;
  return_value_if_fail(controller_base_set_enable(base, enable) == RET_OK,
                       RET_BAD_PARAMS);

  handle->is_play = enable;

  if (enable) {
    gesture_on_click_event(base->gesture_manager,
                           music_main_on_click_gesture_fun,
                           GESTURE_LEFT_TO_RIGHT_TYPE, handle);

    window_anim_menu_on_load_widget(base->window_anim_menu,
                                    music_main_on_window_anim_load_widget_func,
                                    handle);
    window_anim_menu_on_dispose_widget(base->window_anim_menu, NULL, NULL);
  } else {
    return gesture_remove_all(base->gesture_manager);
  }

  return RET_OK;
}

music_main_t *music_main_init(widget_t *gesture_manager,
                              widget_t *window_anim_menu) {
  music_main_t *handle = TKMEM_ZALLOC(music_main_t);

  return_value_if_fail(handle != NULL, NULL);

  controller_base_init((controller_base_t *)handle, gesture_manager,
                       window_anim_menu);

  handle->base.enable = FALSE;

  handle->song_id = 0;
  handle->song_time = 0;
  handle->music_vol = 50;
  handle->music_state = MUSIC_STATE_PLAY_LOOP_TYPE;

  return handle;
}
