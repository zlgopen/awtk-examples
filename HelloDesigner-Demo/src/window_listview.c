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
 * list_item点击事件
 */
static ret_t on_list_item_click(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* list_item = WIDGET(e->target);
  widget_t* label = widget_lookup(win, "list_view_label", TRUE);
  value_t v;

  value_set_wstr(&v, widget_get_text(list_item));
  widget_set_text(label, value_wstr(&v));

  return RET_OK;
}

static ret_t on_remove_self(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(e->target);
  (void)ctx;

  widget_remove_child(widget->parent, widget);
  widget_destroy(widget);

  return RET_OK;
}

static ret_t on_clone_self(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(e->target);
  widget_t* clone = widget_clone(widget, widget->parent);
  (void)ctx;

  widget_on(clone, EVT_CLICK, on_clone_self, clone);

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

    if (tk_str_eq(name, "clone_self")) {
      widget_on(widget, EVT_CLICK, on_clone_self, win);
    } else if (tk_str_eq(name, "remove_self")) {
      widget_on(widget, EVT_CLICK, on_remove_self, win);
    } else if (tk_str_eq(name, "close")) {
      widget_on(widget, EVT_CLICK, on_close, win);
    } else if (tk_str_eq(name, "list_item")) {
      widget_on(widget, EVT_CLICK, on_list_item_click, win);
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
ret_t open_listview_window() {
  value_t v;
  widget_t* win = window_open("listview/listview");
  if (win) {
    init_children_widget(win);
  }

  return RET_OK;
}