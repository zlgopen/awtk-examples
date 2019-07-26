/**
 * File:   window_line_series.c
 * Author: AWTK Develop Team
 * Brief:  line series window
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
 * 2018-12-11 Xu ChaoZe <xuchaoze@zlg.cn> created
 *
 */

#include "awtk.h"
#include "custom_widgets/chart/chart_view.h"
#include "custom_widgets/chart/bar_series.h"

#define SERIES_TIMER_UPDATE 1

extern ret_t on_series_rset_rand_ufloat_data(void* ctx, event_t* e);
extern ret_t on_series_push_rand_ufloat_data(const timer_info_t* timer);
extern ret_t on_series_rset_rand_minmax_data(void* ctx, event_t* e);
extern ret_t on_series_push_rand_minmax_data(const timer_info_t* timer);
extern ret_t on_series_typeie_visible_changed(void* ctx, event_t* e);
extern ret_t on_series_dayas_visible_changed(void* ctx, event_t* e);
extern ret_t on_series_drean_visible_changed(void* ctx, event_t* e);
extern ret_t axis_time_init(widget_t* widget);
extern ret_t on_series_rset_rand_ufloat_data_4_timer(const timer_info_t* timer);
extern void on_series_rset_ufloat_data(widget_t* widget, uint32_t count);

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
  widget_t* win = widget_get_window(widget);

  (void)ctx;

  if (widget->name != NULL) {
    const char* name = widget->name;
    if (tk_str_eq(name, "close")) {
      widget_on(widget, EVT_CLICK, on_close, win);
    } else if (tk_str_eq(name, "typeie")) {
      widget_on(widget, EVT_CLICK, on_series_typeie_visible_changed, win);
    } else if (tk_str_eq(name, "dayas")) {
      widget_on(widget, EVT_CLICK, on_series_dayas_visible_changed, win);
    } else if (tk_str_eq(name, "drean")) {
      widget_on(widget, EVT_CLICK, on_series_drean_visible_changed, win);
    } else if (tk_str_eq(name, "chartview")) {
      on_series_rset_ufloat_data(widget, 7);
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

static void init_normal_bar_series_window(widget_t* widget) {
  widget_t* new_graph = widget_lookup(widget, "new_graph", TRUE);
  if (new_graph != NULL) {
    widget_on(new_graph, EVT_CLICK, on_series_rset_rand_ufloat_data, widget);
  }

  init_children_widget(widget);

#if SERIES_TIMER_UPDATE
  widget_add_timer(widget, on_series_rset_rand_ufloat_data_4_timer, 2000);
#endif
}

static void init_minmax_bar_series_window(widget_t* widget) {
  widget_t* new_graph = widget_lookup(widget, "new_graph", TRUE);
  if (new_graph != NULL) {
    widget_on(new_graph, EVT_CLICK, on_series_rset_rand_minmax_data, widget);
  }

  init_children_widget(widget);

#if SERIES_TIMER_UPDATE
  widget_add_timer(widget, on_series_push_rand_minmax_data, 2000);
#endif
}

/**
 * 打开柱形图表窗口
 */
ret_t open_bar_series_window(const char* name) {
  widget_t* win = window_open(name);
  if (win) {
    if (strstr(name, "_minmax") != NULL) {
      init_minmax_bar_series_window(win);
    } else {
      init_normal_bar_series_window(win);
    }

    axis_time_init(win);

    return RET_OK;
  }
  return RET_FAIL;
}
