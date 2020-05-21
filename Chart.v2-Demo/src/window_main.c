/**
 * File:   window_main.c
 * Author: AWTK Develop Team
 * Brief:  main window
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
 * 2018-10-29 TangJunJie created
 *
 */
#include <time.h>
#include "awtk.h"
#include "custom_widgets/custom_widgets.h"

extern ret_t open_meter_window();
extern ret_t open_pie_window();
extern ret_t open_line_series_window(const char* name);
extern ret_t open_bar_series_window(const char* name);

extern ret_t application_init(void);

static ret_t on_meter(void* ctx, event_t* e) {
  (void)ctx;
  (void)e;

  return open_meter_window();
}

static ret_t on_pie(void* ctx, event_t* e) {
  (void)ctx;
  (void)e;

  return open_pie_window();
}

static ret_t on_graph(void* ctx, event_t* e) {
  (void)ctx;
  (void)e;

  return open_line_series_window("window_line/window_line");
}

static ret_t on_histogram(void* ctx, event_t* e) {
  (void)ctx;
  (void)e;

  return open_bar_series_window("window_bar/window_bar");
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
    if (tk_str_eq(name, "meter") || tk_str_eq(name, "meter_image")) {
      widget_on(widget, EVT_CLICK, on_meter, win);
    } else if (tk_str_eq(name, "pie") || tk_str_eq(name, "pie_image")) {
      widget_on(widget, EVT_CLICK, on_pie, win);
    } else if ((strstr(name, "window_line_series") != NULL) || tk_str_eq(name, "graph_image")) {
      widget_on(widget, EVT_CLICK, on_graph, win);
    } else if ((strstr(name, "window_bar_series") != NULL) || tk_str_eq(name, "histogram_image")) {
      widget_on(widget, EVT_CLICK, on_histogram, win);
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
 * 改变样式
 */
static ret_t change_style(widget_t* win, const char* name, const char* style, bool_t flag) {
  widget_t* w = widget_lookup(window_manager(), name, TRUE);
  if (w) {
    if (flag) {
      char style_en[20];
      tk_snprintf(style_en, sizeof(style_en), "%s_en", style);
      widget_use_style(w, style_en);
    } else {
      widget_use_style(w, style);
    }
  }

  return RET_OK;
}

/**
 * 改变语言和样式
 */
static ret_t change_func(widget_t* win, bool_t flag) {
  change_style(win, "meter", "main_meter", flag);
  change_style(win, "pie", "main_pie", flag);
  change_style(win, "window_line_series", "main_graph", flag);
  change_style(win, "window_bar_series", "main_histogram", flag);

  return RET_OK;
}

/**
 * 点击中英文互译按钮
 */
static ret_t on_language(void* ctx, event_t* e) {
  (void)ctx;
  value_t v;

  widget_t* lang_btn = WIDGET(e->target);
  widget_get_prop(lang_btn, WIDGET_PROP_STYLE, &v);
  const char* style = value_str(&v);
  if (strstr(style, "zh") != NULL) {
    widget_use_style(lang_btn, "language_en");
    change_locale("en_US");
    change_func(lang_btn, TRUE);
  } else {
    widget_use_style(lang_btn, "language_zh");
    change_locale("zh_CN");
    change_func(lang_btn, FALSE);
  }

  return RET_OK;
}

/**
 * 打开主窗口
 */
ret_t application_init(void) {
  /* 初始化自定义控件 */
  custom_widgets_init();
  system_info_set_default_font(system_info(), "default");
  window_manager_set_cursor(window_manager(), NULL);

  widget_t* sys_bar = window_open("system_bar/system_bar");
  if (sys_bar) {
    change_locale("zh_CN");
    widget_t* lang_btn = widget_lookup(sys_bar, "language_btn", TRUE);
    widget_on(lang_btn, EVT_CLICK, on_language, sys_bar);
  }

  widget_t* win = window_open("home_page/home_page");
  if (win) {
    init_children_widget(win);

    return RET_OK;
  }

  return RET_FAIL;
}

/**
* 退出
*/
ret_t application_exit(void) {
        log_debug("application_exit");
        return RET_OK;
}
