/**
 * File:   edit.c
 * Author: AWTK Develop Team
 * Brief:  window main
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
extern ret_t application_init(void);
/**
 * Label文本的数值 + offset
 */
static ret_t label_add(widget_t* label, int32_t offset) {
  if (label) {
    int32_t val = 0;
    if (wstr_to_int(&(label->text), &val) == RET_OK) {
      char text[32];
      val += offset;
      val = tk_max(-200, tk_min(val, 200));
      tk_snprintf(text, sizeof(text), "%d", val);
      widget_set_text_utf8(label, text);

      return RET_OK;
    }
  }

  return RET_FAIL;
}

/**
 * 递增按钮事件
 */
static ret_t on_inc_click(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* label = widget_lookup(win, "label_4_btn", TRUE);
  label_add(label, 1);

  return RET_OK;
}

/**
 * 递减按钮事件
 */
static ret_t on_dec_click(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* label = widget_lookup(win, "label_4_btn", TRUE);
  label_add(label, -1);

  return RET_OK;
}

/**
 * 正在编辑事件
 */
static ret_t on_changing(void* ctx, event_t* evt) {
  widget_t* target = WIDGET(evt->target);
  widget_t* win = WIDGET(ctx);
  widget_t* label = widget_lookup(win, "label_4_edit", TRUE);
  widget_set_text(label, target->text.str);

  return RET_OK;
}

/**
 * 子控件初始化(主要是设置click回调、初始显示信息)
 */
static ret_t init_widget(void* ctx, const void* iter) {
  (void)ctx;
  widget_t* widget = WIDGET(iter);
  widget_t* win = widget_get_window(widget);

  if (widget->name != NULL) {
    const char* name = widget->name;
    if (tk_str_eq(name, "edit")) {
      widget_on(widget, EVT_VALUE_CHANGING, on_changing, win);
    } else if (tk_str_eq(name, "dec_btn")) {
      widget_on(widget, EVT_CLICK, on_dec_click, win);
    } else if (tk_str_eq(name, "inc_btn")) {
      widget_on(widget, EVT_CLICK, on_inc_click, win);
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
ret_t application_init() {
  widget_t* win = window_open("main");
  if (win) {
    init_children_widget(win);
  }
  return RET_OK;
}
