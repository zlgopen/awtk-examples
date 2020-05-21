/**
 * File:   window_basic.c
 * Author: AWTK Develop Team
 * Brief:  window main
 *
 * Copyright (c) 2018 - 2019  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2019-12-30 Li Peihuang created
 *
 */
#include "awtk.h"

#define PROP_BAR_TIMER "bar_timer_id"

/**
 * 正在编辑事件
 */
static ret_t on_changing(void* ctx, event_t* evt) {
  widget_t* target = WIDGET(evt->target);
  widget_t* win = WIDGET(ctx);
  widget_t* label = widget_lookup(win, "edit_label", TRUE);

  assert(label != NULL);

  widget_set_text(label, target->text.str);

  return RET_OK;
}

static ret_t on_slider_value_changing(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* bar = widget_lookup(win, "changing_bar", TRUE);
  widget_t* slider = WIDGET(e->target);
  value_t v;

  assert(bar != NULL);

  value_set_int32(&v, widget_get_value(slider));
  progress_bar_set_value(bar, value_int32(&v));

  return RET_OK;
}

static ret_t on_slider_value_changed(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* bar = widget_lookup(win, "changed_bar", TRUE);
  widget_t* slider = WIDGET(e->target);
  value_t v;

  assert(bar != NULL);

  value_set_int32(&v, widget_get_value(slider));
  progress_bar_set_value(bar, value_int32(&v));

  return RET_OK;
}

/**
 * 进度条动画
 */
static ret_t progress_bar_animate_delta(widget_t* win, const char* name, int32_t delta) {
  widget_t* progress_bar = widget_lookup(win, name, TRUE);
  assert(progress_bar != NULL);

  int32_t value = (PROGRESS_BAR(progress_bar)->value + delta);
  value = tk_min(100, value);
  value = tk_max(0, value);
  widget_animate_value_to(progress_bar, value, 500);

  return RET_OK;
}

/**
 * 递减按钮事件
 */
static ret_t on_dec_click(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  (void)e;

  progress_bar_animate_delta(win, "click_bar", -10);

  return RET_OK;
}

/**
 * 递增按钮事件
 */
static ret_t on_inc_click(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  (void)e;

  progress_bar_animate_delta(win, "click_bar", 10);

  return RET_OK;
}

static ret_t on_bar_value_changed(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* label = widget_lookup(win, "click_bar_label", TRUE);
  widget_t* bar = WIDGET(e->target);
  char text[32];
  value_t v;

  assert(bar != NULL);
  assert(label != NULL);

  value_set_int32(&v, widget_get_value(bar));
  tk_snprintf(text, sizeof(text), "%d%%", value_int32(&v));
  widget_set_text_utf8(label, text);

  return RET_OK;
}

/**
 * 定时器事件（定时增加progress_bar的值）
 */
static ret_t on_add_bar_value(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);
  widget_t* bar = widget_lookup(win, "click_bar", TRUE);
  value_t v;
  int32_t val;

  assert(bar != NULL);

  value_set_int32(&v, widget_get_value(bar));
  val = value_int32(&v);
  if (val >= 100) {
    progress_bar_set_value(bar, 0);
  }
  progress_bar_animate_delta(win, "click_bar", 10);

  return RET_REPEAT;
}

/**
 * 关闭窗口
 */
static ret_t on_close(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  value_t v;
  (void)e;

  /*移除定时器*/
  if (widget_get_prop(win, PROP_BAR_TIMER, &v) == RET_OK) {
    timer_remove(value_uint32(&v));
  }

  return window_close(win);
}

/**
 * 子控件初始化(主要是设置click回调、初始显示信息)
 */
static ret_t init_widget(void* ctx, const void* iter) {
  widget_t* widget = WIDGET(iter);
  widget_t* win = widget_get_window(widget);
  (void)ctx;

  if (widget->name != NULL) {
    const char* name = widget->name;

    if (tk_str_eq(name, "edit")) {
      widget_on(widget, EVT_VALUE_CHANGING, on_changing, win);
    } else if (tk_str_eq(name, "changing_slider")) {
      widget_on(widget, EVT_VALUE_CHANGING, on_slider_value_changing, win);
    } else if (tk_str_eq(name, "changed_slider")) {
      widget_on(widget, EVT_VALUE_CHANGED, on_slider_value_changed, win);
    } else if (tk_str_eq(name, "dec_btn")) {
      widget_on(widget, EVT_CLICK, on_dec_click, win);
    } else if (tk_str_eq(name, "inc_btn")) {
      widget_on(widget, EVT_CLICK, on_inc_click, win);
    } else if (tk_str_eq(name, "click_bar")) {
      widget_on(widget, EVT_VALUE_CHANGED, on_bar_value_changed, win);
    } else if (tk_str_eq(name, "close")) {
      widget_on(widget, EVT_CLICK, on_close, win);
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
 * 初始化
 */
ret_t open_basic_window() {
  value_t v;
  widget_t* win = window_open("basic/basic");
  if (win) {
    init_children_widget(win);
    /*添加定时器 */
    value_set_uint32(&v, timer_add(on_add_bar_value, win, 1500));
    widget_set_prop(win, PROP_BAR_TIMER, &v);
  }
  return RET_OK;
}