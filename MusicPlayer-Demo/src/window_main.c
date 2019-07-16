/**
 * File:   window_record.c
 * Author: AWTK Develop Team
 * Brief:  Main window
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */
/**
 * History:
 * ================================================================
 * 2019-2-20  ZhongWei1 created
 *
 */

#include "awtk.h"
#include "custom_widgets/custom_widgets.h"
#include "custom_function/music_manager.h"
#include "custom_function/player_timer_manager.h"
#include "custom_widgets/photo_frame/frame_view.h"
#include "ext_widgets/image_animation/image_animation.h"
#include "widget_animators/widget_animator_opacity.h"
#include "widget_animators/widget_animator_rotation.h"
#include <time.h>

extern ret_t open_advanced_dialog();
extern ret_t open_equalizer_dialog();
extern ret_t open_play_list_window(widget_t* s_parent);
extern ret_t swtich_frame_rotation_animator(widget_t* win, bool_t start_anim);

extern ret_t application_init(void);

static ret_t on_advanced_open(void* ctx, event_t* e) {
  (void)e;

  return open_advanced_dialog();
}

static ret_t on_equalizer_open(void* ctx, event_t* e) {
  (void)e;

  return open_equalizer_dialog();
}

/**
 * 封面旋转动画
 */
ret_t swtich_frame_rotation_animator(widget_t* win, bool_t start_anim) {
  return_value_if_fail(win != NULL, RET_BAD_PARAMS);
  widget_t* frame_menu = widget_lookup(win, "frame_menu", TRUE);
  frame_view_t* frame_view = FRAME_VIEW(frame_menu);
  widget_t* btn_anim_switch = widget_lookup(win, "btn_anim_switch", TRUE);

  widget_t* frame_child = frame_menu->children->elms[frame_view->value];
  if (start_anim && tk_str_eq(btn_anim_switch->style, "s_anim_switch_p")) {
    widget_create_animator(frame_child->children->elms[0],
                           "rotation(from=0, to=628,  duration=800000, repeat_times=0)");
  } else {
    widget_stop_animator(frame_child->children->elms[0], "rotation");
    widget_destroy_animator(frame_child->children->elms[0], "rotation");
  }
  return RET_OK;
}

/**
 * 唱针动画
 */
ret_t swtich_frame_vinyl_head_animator(widget_t* win, bool_t start_anim) {
  return_value_if_fail(win != NULL, RET_BAD_PARAMS);
  widget_t* vinyl_head = widget_lookup(win, "vinyl_head", TRUE);
  if (start_anim) {
    widget_create_animator(vinyl_head, "value(from=-70, to=-35, duration=300, delay=0)");
  } else {
    widget_create_animator(vinyl_head, "value(from=-35, to=-70, duration=300, delay=0)");
  }

  return RET_OK;
}

/**
 * 设置封面动画开关
 */
static ret_t on_anim_switch(void* ctx, event_t* e) {
  widget_t* win = (widget_t*)ctx;
  widget_t* widget = WIDGET(e->target);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);
  widget_t* btn_play = widget_lookup(win, "btn_play", TRUE);

  if (tk_str_eq(widget->style, "s_anim_switch_n")) {
    widget_use_style(widget, "s_anim_switch_p");
    if (tk_str_eq(btn_play->style, "s_play_pause")) {
      swtich_frame_rotation_animator(win, TRUE);
    }
  } else if (tk_str_eq(widget->style, "s_anim_switch_p")) {
    widget_use_style(widget, "s_anim_switch_n");
    swtich_frame_rotation_animator(win, FALSE);
  } else {
    return RET_FAIL;
  }

  return RET_OK;
}
static ret_t on_play_list(void* ctx, event_t* e) {
  (void)e;
  widget_t* win = (widget_t*)ctx;
  return open_play_list_window(win);
}

/**
 * 拖动歌词
 */
static ret_t on_lrc_up(void* ctx, event_t* e) {
  value_t val;
  widget_t* win = (widget_t*)ctx;

  value_set_bool(&val, TRUE);
  widget_set_prop(win, "lrc_move", &val);

  return RET_OK;
}

static ret_t on_lrc_down(void* ctx, event_t* e) {
  widget_t* win = (widget_t*)ctx;
  value_t val;

  value_set_bool(&val, FALSE);
  widget_set_prop(win, "lrc_move", &val);

  return RET_OK;
}

static void time_now_str(char* str, size_t size) {
  date_time_t dt;
  date_time_init(&dt);
  snprintf(str, size, "%04d/%02d/%02d %02d:%02d:%02d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
}

/**
 * 获取并显示系统时间
 */
static ret_t on_systime_update(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);
  widget_t* label = widget_lookup(win, "sys_time", TRUE);

  if (label) {
    char text[64];
    time_now_str(text, sizeof(text));
    widget_set_text_utf8(label, text);
  }

  return RET_REPEAT;
}

/**
 * 播放
 */
static ret_t on_music_play(void* ctx, event_t* e) {
  widget_t* win = (widget_t*)ctx;
  widget_t* frame_menu = widget_lookup(win, "frame_menu", TRUE);
  frame_view_t* frame_view = FRAME_VIEW(frame_menu);
  widget_t* btn_play = WIDGET(e->target);

  if (tk_str_eq(btn_play->style, "s_play_pause")) {
    swtich_frame_vinyl_head_animator(win, FALSE);
    swtich_frame_rotation_animator(win, FALSE);
    widget_use_style(btn_play, "s_play");
    player_timer_clear(win);

  } else if (tk_str_eq(btn_play->style, "s_play")) {
    swtich_frame_vinyl_head_animator(win, TRUE);
    swtich_frame_rotation_animator(win, TRUE);
    widget_use_style(btn_play, "s_play_pause");
    player_timer_clear(win);
    player_timer_start(win);
  } else
    return RET_FAIL;

  return RET_OK;
}

static ret_t on_frame_menu_vchange(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* frame_menu = WIDGET(e->target);
  frame_view_t* frame_view = FRAME_VIEW(frame_menu);
  init_player(win);
  swtich_frame_rotation_animator(win, TRUE);
  load_song(win, frame_view->value, TRUE);

  return RET_OK;
}

/**
 * 前一首
 */
static ret_t on_prve_music(void* ctx, event_t* e) {
  widget_t* win = (widget_t*)ctx;
  widget_t* frame_menu = widget_lookup(win, "frame_menu", TRUE);

  swtich_frame_vinyl_head_animator(win, TRUE);
  swtich_frame_rotation_animator(win, FALSE);
  frame_view_set_then(frame_menu, FALSE);

  return RET_OK;
}

/**
 * 下一首
 */
static ret_t on_next_music(void* ctx, event_t* e) {
  widget_t* win = (widget_t*)ctx;
  widget_t* frame_menu = widget_lookup(win, "frame_menu", TRUE);

  swtich_frame_vinyl_head_animator(win, TRUE);
  swtich_frame_rotation_animator(win, FALSE);
  frame_view_set_then(frame_menu, TRUE);

  return RET_OK;
}

/**
 * 播放模式
 */
static ret_t on_play_mode(void* ctx, event_t* e) {
  widget_t* child = WIDGET(e->target);
  value_t val;
  value_set_int32(&val, 0);
  if (widget_get_prop(child, "play_mode_style", &val) != RET_OK || value_int32(&val) == 0) {
    value_set_int32(&val, 1);
    widget_use_style(child, "s_play_mode_one");
  } else if (value_int32(&val) == 1) {
    value_set_int32(&val, 2);
    widget_use_style(child, "s_play_mode_rand");
  } else if (value_int32(&val) == 2) {
    value_set_int32(&val, 0);
    widget_use_style(child, "s_play_mode_clicle");
  }
  widget_invalidate(child, NULL);
  widget_set_prop(child, "play_mode_style", &val);

  return RET_OK;
}

/**
 * 子控件初始化(主要是设置click回调、初始显示信息)
 */
static ret_t init_widget(void* ctx, const void* iter) {
  widget_t* widget = WIDGET(iter);
  (void)ctx;
  if (widget->name != NULL) {
    const char* name = widget->name;
    if (tk_str_eq(name, "btn_equalizer")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_equalizer_open, win);
      }
    } else if (tk_str_eq(name, "btn_advanced")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_advanced_open, win);
      }
    } else if (tk_str_eq(name, "btn_anim_switch")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_anim_switch, win);
      }
    } else if (tk_str_eq(name, "lrc_view")) {
      value_t val;
      widget_t* win = widget_get_window(widget);
      value_set_bool(&val, TRUE);
      widget_set_prop(win, "lrc_move", &val);

      widget_on(widget, EVT_POINTER_DOWN, on_lrc_down, win);
      widget_on(widget, EVT_POINTER_UP, on_lrc_up, win);
    } else if (tk_str_eq(name, "btn_play")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_music_play, win);
    } else if (tk_str_eq(name, "btn_prve")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_prve_music, win);
    } else if (tk_str_eq(name, "btn_next")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_next_music, win);
    } else if (tk_str_eq(name, "btn_play_list")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_play_list, win);
    } else if (tk_str_eq(name, "btn_play_mode")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_play_mode, win);
    } else if (tk_str_eq(name, "frame_menu")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_VALUE_CHANGED, on_frame_menu_vchange, win);
    }
  }
  return RET_OK;
}

/**
 * 初始化窗口上的子控件
 */
static void init_children_widget(widget_t* widget) {
  widget_foreach(widget, init_widget, widget);
}

/**
 * 打开主界面窗口
 */
ret_t application_init(void) {
  
  /* 初始化自定义控件 */
  custom_widgets_init();

  widget_t* win = window_open("main");
  if (win) {
    init_children_widget(win);
    load_song(win, 0, FALSE);
    timer_add(on_systime_update, win, 1000);
    return RET_OK;
  }

  return RET_FAIL;
}