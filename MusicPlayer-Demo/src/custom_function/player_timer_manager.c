#include "player_timer_manager.h"
#include "music_manager.h"
#include <stdio.h>

/**
 * 进度条时间进度显示刷新
 */
static void label_min_set(widget_t* win, int32_t value) {
  widget_t* label_min = widget_lookup(win, "slider_min", TRUE);
  char c_times[10] = {0};
  int32_t sec = value % 60;
  int32_t min = value / 60;
  sprintf(c_times, "%02d:%02d", min, sec);
  widget_set_text_utf8(label_min, c_times);
}

/**
 * 定时器控制进度条数值递增
 */
static ret_t update_play_slider(const timer_info_t* timer) {
  value_t val;
  widget_t* play_slider = WIDGET(timer->ctx);
  widget_t* win = widget_get_window(play_slider);
  widget_t* btn_play = widget_lookup(win, "btn_play", TRUE);
  slider_t* slider = SLIDER(play_slider);
  uint16_t max = slider->max;

  int value = widget_get_value(play_slider);
  value_set_bool(&val, TRUE);
  if(value >= max)
  {
    if(!slider->dragging){
    play_mode_choose(win);
    
    return RET_OK;
    }
  }
  if (widget_get_prop(btn_play, "btn_state", &val) == RET_OK && value_bool(&val) == TRUE) {
    return RET_OK;
  } else {
    label_min_set(win, value);
    update_lrc(value, win);
    slider_set_value(play_slider, ++value);
  }

  return RET_REPEAT;
}

/**
 * 定时器创建
 */
ret_t player_timer_start(widget_t* win) {
  value_t val;
  if (widget_get_prop(win, "timer_idex", &val) != RET_OK || value_uint32(&val) == -1) {
    widget_t* slider = widget_lookup(win, "play_slider", TRUE);
    value_set_uint32(&val, timer_add(update_play_slider, slider, 1000));
    widget_set_prop(win, "timer_idex", &val);
  }

  return RET_OK;
}

/**
 * 定时器关闭
 */
ret_t player_timer_clear(widget_t* win) {
  value_t val;
  if (widget_get_prop(win, "timer_idex", &val) == RET_OK && value_uint32(&val) != -1 &&
      timer_count() > 0) {
    timer_remove(value_uint32(&val));
    value_set_uint32(&val, -1);
    widget_set_prop(win, "timer_idex", &val);
  }

  return RET_OK;
}
