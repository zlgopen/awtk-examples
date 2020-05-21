/**
 * File:   window_meter.c
 * Author: AWTK Develop Team
 * Brief:  window meter
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
 * 2018-11-20 TangJunJie created
 *
 */
#include "awtk.h"

/**
 * 点击开始/关闭按钮时改变样式
 */
static ret_t set_btn_style(widget_t* win, event_t* e) {
  widget_t* target = e->target;
  widget_t* start_btn = widget_lookup(win, "function_start", TRUE);

  if (start_btn) {
    value_t v;
    widget_get_prop(start_btn, WIDGET_PROP_STYLE, &v);
    const char* style = value_str(&v);
    widget_use_style(start_btn,
                     strstr(style, "pressed") == NULL ? "meter_start_pressed" : "meter_start");
  }

  widget_t* stop_btn = widget_lookup(win, "function_stop", TRUE);
  if (stop_btn) {
    value_t v;
    widget_get_prop(stop_btn, WIDGET_PROP_STYLE, &v);
    const char* style = value_str(&v);
    widget_use_style(stop_btn,
                     strstr(style, "pressed") == NULL ? "meter_stop_pressed" : "meter_stop");
  }

  if (target == start_btn) {
    widget_use_style(stop_btn, "meter_stop");
    widget_set_enable(stop_btn, TRUE);
  } else {
    widget_use_style(start_btn, "meter_start");
    widget_set_enable(start_btn, TRUE);
  }
  widget_set_enable(target, FALSE);

  return RET_OK;
}

/**
 * 点击开始按钮
 */
static ret_t on_start(void* ctx, event_t* e) {
  widget_t* win = (widget_t*)ctx;
  set_btn_style(win, e);
  widget_start_animator(NULL, NULL);

  return RET_OK;
}

/**
 * 点击停止按钮
 */
static ret_t on_stop(void* ctx, event_t* e) {
  widget_t* win = (widget_t*)ctx;
  set_btn_style(win, e);
  widget_stop_animator(NULL, NULL);

  return RET_OK;
}

/**
 * 点击关闭按钮
 */
static ret_t on_close(void* ctx, event_t* e) {
  widget_t* win = (widget_t*)ctx;
  (void)e;

  return window_close(win);
}

/**
 * 子控件初始化(主要是设置click回调、初始显示信息)
 */
static ret_t init_widget(void* ctx, const void* iter) {
  widget_t* widget = WIDGET(iter);

  (void)ctx;

  if (widget->name != NULL) {
    const char* name = widget->name;
    if (strstr(name, "start")) {
      widget_t* win = widget_get_window(widget);
      widget_set_enable(widget, FALSE);
      widget_use_style(widget, "meter_start_pressed");
      widget_on(widget, EVT_CLICK, on_start, win);
    } else if (strstr(name, "stop")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_stop, win);
    } else if (tk_str_eq(name, "close")) {
      widget_t* win = widget_get_window(widget);
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
 * 打开仪表图窗口
 */
ret_t open_meter_window(void) {
  widget_t* win = window_open("window_meter/window_meter");
  if (win) {
    init_children_widget(win);

    return RET_OK;
  }

  return RET_FAIL;
}
