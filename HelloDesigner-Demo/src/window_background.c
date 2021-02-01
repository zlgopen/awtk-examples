/**
 * File:   window_basic.c
 * Author: AWTK Develop Team
 * Brief:  window main
 *
 * Copyright (c) 2020 - 2020  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2020-2-13 Li Peihuang created
 *
 */
#include "awtk.h"

/**
 * 关闭窗口
 */
static ret_t on_close(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  (void)e;

  return window_close(win);
}

/**
 * 切换背景事件
 */
static ret_t on_blue_click(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  (void)e;

  widget_use_style(win, "blue");

  return RET_OK;
}

static ret_t on_green_click(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  (void)e;

  widget_use_style(win, "green");

  return RET_OK;
}

static ret_t on_yellow_click(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  (void)e;

  widget_use_style(win, "yellow");

  return RET_OK;
}

static ret_t on_default_click(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  (void)e;

  widget_use_style(win, "default");

  return RET_OK;
}

static ret_t on_pic_1_click(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  (void)e;

  widget_use_style(win, "pic_1");

  return RET_OK;
}

static ret_t on_pic_2_click(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  (void)e;

  widget_use_style(win, "pic_2");

  return RET_OK;
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

    if (tk_str_eq(name, "blue_btn")) {
      widget_on(widget, EVT_CLICK, on_blue_click, win);
    } else if (tk_str_eq(name, "yellow_btn")) {
      widget_on(widget, EVT_CLICK, on_yellow_click, win);
    } else if (tk_str_eq(name, "green_btn")) {
      widget_on(widget, EVT_CLICK, on_green_click, win);
    } else if (tk_str_eq(name, "default_btn")) {
      widget_on(widget, EVT_CLICK, on_default_click, win);
    } else if (tk_str_eq(name, "pic_1_btn")) {
      widget_on(widget, EVT_CLICK, on_pic_1_click, win);
    } else if (tk_str_eq(name, "pic_2_btn")) {
      widget_on(widget, EVT_CLICK, on_pic_2_click, win);
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
ret_t open_background_window() {
  value_t v;
  widget_t* win = window_open("background/background");
  if (win) {
    init_children_widget(win);
  }

  return RET_OK;
}