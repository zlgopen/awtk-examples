/**
 * File:   window_main.c
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
 * 2019-10-24 Li Peihuang created
 *
 */
#include "awtk.h"
extern ret_t application_init(void);
static void init_children_widget(widget_t* widget);

#define PROP_BAR_TIMER "bar_timer_id"

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
 * 中英文互译
 */
static ret_t change_locale(const char* str) {
  char country[3];
  char language[3];

  strncpy(language, str, 2);
  strncpy(country, str + 3, 2);
  locale_info_change(locale_info(), language, country);

  return RET_OK;
}

/**
 * 点击中英文互译按钮
 */
static ret_t on_language(void* ctx, event_t* e) {
  (void)ctx;
  (void)e;

  const char* language = locale_info()->language;
  if (tk_str_eq(language, "en")) {
    change_locale("zh_CN");
  } else {
    change_locale("en_US");
  }
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
 * list_item点击事件
 */
static ret_t on_list_item_click(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* list_item = WIDGET(e->target);
  widget_t* item_label = widget_lookup_by_type(list_item, WIDGET_TYPE_LABEL, TRUE);
  widget_t* label = widget_lookup(win, "list_view_label", TRUE);
  value_t v;
  return_value_if_fail(item_label != NULL, RET_OK);

  value_set_wstr(&v, widget_get_text(item_label));
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
 * 打开窗口
 */
static void open_window(const char* name, widget_t* to_close) {
  widget_t* win = to_close ? window_open_and_close(name, to_close) : window_open(name);
  uint32_t timer_id;
  value_t v;
  init_children_widget(win);

  if (tk_str_eq(name, "basic")) {
    /*添加定时器 */
    timer_id = timer_add(on_add_bar_value, win, 1500);
    value_set_uint32(&v, timer_id);
    widget_set_prop(win, PROP_BAR_TIMER, &v);
  }
}

static ret_t on_open_window(void* ctx, event_t* e) {
  const char* name = (const char*)ctx;
  (void)e;

  open_window(name, NULL);

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

    if (tk_str_eq(name, "language_btn")) {
      widget_on(widget, EVT_CLICK, on_language, win);
    } else if (tk_str_eq(name, "edit")) {
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
    } else if (tk_str_eq(name, "blue_btn")) {
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
    } else if (tk_str_eq(name, "clone_self")) {
      widget_on(widget, EVT_CLICK, on_clone_self, win);
    } else if (tk_str_eq(name, "remove_self")) {
      widget_on(widget, EVT_CLICK, on_remove_self, win);
    } else if (strstr(name, "open:") != NULL) {
      widget_on(widget, EVT_CLICK, on_open_window, (void*)(name + 5));
      widget_on(widget, EVT_LONG_PRESS, on_open_window, (void*)(name + 5));
    } else if (tk_str_eq(name, "close")) {
      widget_on(widget, EVT_CLICK, on_close, win);
    }
  }

  const char* type = widget->vt->type;
  if (tk_str_eq(type, WIDGET_TYPE_LIST_ITEM)) {
    widget_on(widget, EVT_CLICK, on_list_item_click, win);
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

#if 0
    widget_t* widget = window_manager();
    window_manager_set_show_fps(widget, TRUE);
#endif
  }
  return RET_OK;
}
