#include <awtk.h>
#include "../custom_widgets/gesture.h"
#include "../custom_widgets/health_circle.h"
#include "../common/log.h"
#include "../common/global.h"
#include <ext_widgets/progress_circle/progress_circle.h>
#include "../common/animator.h"
#include "../slide_appliction/app.h"

#ifdef _WIN32
#pragma warning(disable : 4244)
#endif  // _WIN32

static widget_t* music_win = NULL;

static ret_t on_update_timer(const timer_info_t* timer);

static ret_t on_close_window(void* ctx, event_t* e) {
  (void)e;
  widget_t* win = WIDGET(ctx);
  // window_close(win);
  music_win = NULL;
  // music_state.backstage = TRUE;
  widget_t* window = widget_get_window(win);

  // global_data.music_state.play_state = 0;

  open_and_close(window);

  return RET_OK;
}

static ret_t on_music_pre_anim_end(void* ctx, event_t* e) {
  (void)e;
  uint8_t play_state = tk_pointer_to_int(ctx);
  music_state_t* music_state = &(global_data.music_state);

  // 刷新界面
  timer_info_t timer;
  timer.ctx = music_win;
  on_update_timer(&timer);

  widget_t* label = widget_lookup(music_win, "music:song_name", TRUE);
  if (label != NULL) {
    widget_create_animator_with(label, "x(from=%d, to=%d, duration=%d)", label->w, 0, 250);
  }

  music_state->play_state = play_state;
  return RET_OK;
}

/**
 * 点击上一首按钮
 */
static ret_t on_music_pre(void* ctx, event_t* e) {
  (void)e;
  widget_t* win = WIDGET(ctx);

  /* TODO: add music pre code */
  music_state_t* music_state = &(global_data.music_state);
  music_state->play_progress = 0;  // 进度变为0

  widget_t* widget = widget_lookup(win, "music:progress", TRUE);
  if (widget != NULL) {
    // progress_circle_set_value(widget, global_data.music_state.play_progress);
    health_circle_set_value_b(widget, music_state->play_progress);
    widget_invalidate(widget, NULL);
  }

  // 切歌
  widget = widget_lookup(win, "music:mode", TRUE);
  if (widget != NULL) {
    music_state->song_index = pre_index(music_state);
  }

  widget_t* label = widget_lookup(win, "music:song_name", TRUE);
  if (label != NULL) {
    // widget_set_text_utf8(label, song_list[music_state.song_index]);

    const char* name = "music_pre";
    widget_create_animator_with(label, "x(name=%s, from=%d, to=%d, duration=%d)", name, 0,
                                -label->w, 250);

    widget_animator_t* animator = widget_find_animator(label, name);
    if (animator != NULL) {
      uint8_t play_state = music_state->play_state;
      if (music_state->play_state == 1) {
        music_state->play_state = 0;  // 先暂停
      }
      widget_animator_on(animator, EVT_ANIM_END, on_music_pre_anim_end,
                         tk_pointer_from_int(play_state));
    }
  }
  return RET_OK;
}

static ret_t on_music_next_play_anim_end(void* ctx, event_t* e) {
  (void)e;
  uint8_t play_state = tk_pointer_to_int(ctx);
  music_state_t* music_state = &(global_data.music_state);

  timer_info_t timer;
  timer.ctx = music_win;
  on_update_timer(&timer);

  widget_t* label = widget_lookup(music_win, "music:song_name", TRUE);
  if (label != NULL) {
    widget_create_animator_with(label, "x(from=%d, to=%d, duration=%d)", -label->w, 0, 250);
  }

  music_state->play_state = play_state;
  return RET_OK;
}

/**
 * 点击下一首按钮
 */
static ret_t on_music_next(void* ctx, event_t* e) {
  (void)e;
  widget_t* win = WIDGET(ctx);

  /* TODO: add music pre code */
  music_state_t* music_state = &(global_data.music_state);
  music_state->play_progress = 0;  // 进度变为0

  widget_t* widget = widget_lookup(win, "music:progress", TRUE);
  if (widget) {
    // progress_circle_set_value(widget, global_data.music_state.play_progress);
    health_circle_set_value_b(widget, music_state->play_progress);
    widget_invalidate(widget, NULL);
  }

  // 切歌
  widget = widget_lookup(win, "music:mode", TRUE);
  if (widget != NULL) {
    music_state->song_index = next_index(music_state);
  }

  widget_t* label = widget_lookup(win, "music:song_name", TRUE);
  if (label != NULL) {
    const char* name = "music_next";
    widget_create_animator_with(label, "x(name=%s, from=%d, to=%d, duration=%d)", name, 0, label->w,
                                250);

    widget_animator_t* animator = widget_find_animator(label, name);
    if (animator != NULL) {
      uint8_t play_state = music_state->play_state;
      if (music_state->play_state == 1) {
        music_state->play_state = 0;  // 先暂停
      }
      widget_animator_on(animator, EVT_ANIM_END, on_music_next_play_anim_end,
                         tk_pointer_from_int(play_state));
    }
  }
  return RET_OK;
}

/**
 * 点击播放、暂停按钮
 */
static ret_t on_music_play_pause(void* ctx, event_t* e) {
  (void)ctx;
  widget_t* button = e->target;
  widget_t* win = widget_get_window(button);
  widget_t* progress = widget_lookup(win, "music:progress", TRUE);
  (void)progress;

  if (global_data.music_state.play_state == 0) {
    widget_use_style(button, "play_pause");
    // global_data.music_state.timer_id = widget_add_timer(win, on_music_timer, 1000);
    global_data.music_state.play_state = 1;
  } else {
    widget_use_style(button, "play_play");
    global_data.music_state.play_state = 0;
  }

  return RET_OK;
}

static char* play_mode[3] = {"music_shun", "music_loop", "music_rand"};
static ret_t on_music_mode(void* ctx, event_t* e) {
  (void)ctx;
  widget_t* button = e->target;
  if (++(global_data.music_state.play_mode) == 3) {
    global_data.music_state.play_mode = 0;
  }
  widget_use_style(button, play_mode[global_data.music_state.play_mode]);

  return RET_OK;
}

static ret_t on_music_vol(void* ctx, event_t* e) {
  (void)e;
  widget_t* win = WIDGET(ctx);
  widget_t* vol = widget_lookup(win, "music:vol", TRUE);
  if (vol) {
    global_data.music_state.play_vol = widget_get_value(vol);
  }

  return RET_OK;
}

static ret_t on_music_list(void* ctx, event_t* e) {
  (void)e;
  (void)ctx;
  return RET_OK;
}

static ret_t on_music_vol_wheel(void* ctx, event_t* e) {
  wheel_event_t* event = (wheel_event_t*)e;
  LOG("dy:%d", event->dy);

  widget_t* win = WIDGET(ctx);
  widget_t* vol = widget_lookup(win, "music:vol", TRUE);
  if (vol) {
    int32_t o_v = widget_get_value(vol);
    widget_set_value(vol, widget_get_value(vol) + event->dy);
    int32_t n_v = widget_get_value(vol);
    // 如果不打日志就用不上
    (void)o_v;
    (void)n_v;
    LOG("old value:%d new value:%d", o_v, n_v);
  }

  return RET_OK;
}

typedef struct _fpoint_t {
  float_t x;
  float_t y;
} fpoint_t;

static ret_t init_widget(void* ctx, const void* iter) {
  (void)ctx;
  widget_t* widget = WIDGET(iter);

  if (widget != NULL && widget->name != NULL) {
    widget_t* win = widget_get_window(widget);
    if (strstr(widget->name, "music:")) {
      char* name = widget->name + 6;
      if (tk_str_eq(name, "gesture")) {
        widget_on(widget, EVT_SLIDE_RIGHT, on_close_window, win);
      } else if (tk_str_eq(name, "pre")) {
        widget_on(widget, EVT_CLICK, on_music_pre, win);
      } else if (tk_str_eq(name, "pause_play")) {
        widget_on(widget, EVT_CLICK, on_music_play_pause, win);
      } else if (tk_str_eq(name, "next")) {
        widget_on(widget, EVT_CLICK, on_music_next, win);
      } else if (tk_str_eq(name, "mode")) {
        widget_on(widget, EVT_CLICK, on_music_mode, win);
      } else if (tk_str_eq(name, "vol")) {
        widget_on(widget, EVT_WHEEL, on_music_vol_wheel, win);
        widget_on(widget, EVT_VALUE_CHANGED, on_music_vol, win);
      } else if (tk_str_eq(name, "list")) {
        widget_on(widget, EVT_CLICK, on_music_list, win);
      } else if (tk_str_eq(name, "progress")) {
        // widget_on(widget, EVT_VALUE_CHANGED, on_progress_value_change, win);
        // widget_on(widget, EVT_AFTER_PAINT, on_process_paint, widget);
      }
    }
  }

  return RET_OK;
}

static void init_children_widget(widget_t* widget) {
  widget_foreach(widget, init_widget, widget);
}

/**
 * 打开窗口时初始化播放状态，如播放模式、音量等
 */
static void init_music_state(widget_t* win) {
  return_if_fail(win != NULL);
  music_state_t* music_state = &(global_data.music_state);

  /* 设置播放状态 */
  widget_t* widget = widget_lookup(win, "music:pause_play", TRUE);
  if (widget != NULL) {
    if (music_state->play_state == 0) {
      widget_use_style(widget, "play_play");
    } else {
      widget_use_style(widget, "play_pause");
    }
  }

  /* 设置播放模式 */
  widget = widget_lookup(win, "music:mode", TRUE);
  if (widget != NULL) {
    switch (music_state->play_mode) {
      case 0:
        widget_use_style(widget, "music_shun");
        break;
      case 1:
        widget_use_style(widget, "music_loop");
        break;
      case 2:
        widget_use_style(widget, "music_rand");
        break;
      default:
        break;
    }
  }

  /* 设置播放音量 */
  widget = widget_lookup(win, "music:vol", TRUE);
  if (widget) {
    widget_set_value(widget, music_state->play_vol);
  }

  /* 设置播放进度 */
  widget = widget_lookup(win, "music:progress", TRUE);
  if (widget != NULL) {
    value_t v;
    /* 设置歌曲长度 */
    if (music_state->play_max == 0) {
      music_state->play_max = 100;
    }
    widget_set_prop(widget, HEALTH_CIRCLE_PROP_MAX_B, value_set_float(&v, music_state->play_max));
    // progress_circle_set_max(widget, global_data.music_state.play_max);

    /* 设置当前进度 */
    value_reset(&v);

    widget_set_prop(widget, HEALTH_CIRCLE_PROP_VALUE_B,
                    value_set_float(&v, music_state->play_progress));
    // progress_circle_set_value(widget, global_data.music_state.play_progress);
  }
}

static ret_t on_update_timer(const timer_info_t* timer) {
  widget_t* window = WIDGET(timer->ctx);
  widget_t* widget = widget_lookup(window, "music:progress", TRUE);
  music_state_t* music_state = &(global_data.music_state);
  LOG("play_state:%d", music_state->play_state);
  if (widget != NULL) {
    // progress_circle_set_value(widget, global_data.music_state.play_progress);
    if (music_state->play_state == 1) {
      health_circle_set_value_b(widget, music_state->play_progress);
    }
  }

  widget_t* song_name = widget_lookup(window, "music:song_name", TRUE);
  if (song_name != NULL) {
    int32_t song_index = music_state->song_index;
    str_t** song_list = (str_t**)music_state->song_list.elms;
    widget_set_text_utf8(song_name, song_list[song_index]->str);
    LOG("name:%s", song_list[song_index]->str);
  }

  return RET_REPEAT;
}

/**
 * 打开音乐窗口
 */
ret_t on_open_music(void* ctx, event_t* e) {
  (void)e;
  widget_t* to_close = WIDGET(ctx);
  music_win = window_open_and_close(g_window_enum->music.str, to_close);
  if (music_win != NULL) {
    // music_state.backstage = FALSE;
    init_music_state(music_win);
    init_children_widget(music_win);
    timer_info_t timer;
    timer.ctx = music_win;
    on_update_timer(&timer);
    widget_add_timer(music_win, on_update_timer, 1000);
    return RET_OK;
  }

  return RET_FAIL;
}
