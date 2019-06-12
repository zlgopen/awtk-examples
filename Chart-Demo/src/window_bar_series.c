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

#include <math.h>
#include <stdlib.h>

#define PROP_RANDOM_TIMER "timer_id"

extern ret_t on_series_typeie_visible_changed(void* ctx, event_t* e);
extern ret_t on_series_dayas_visible_changed(void* ctx, event_t* e);
extern ret_t on_series_drean_visible_changed(void* ctx, event_t* e);

static void random_series_data(widget_t* widget) {
  const uint32_t count = 7;
  float_t new_value[7];
  widget_t* series;
  uint32_t i, j;
  uint32_t nr = chart_view_count_series(widget, NULL);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < count; j++) {
      new_value[j] = rand() % 140;
    }
    series = chart_view_get_series(widget, NULL, i);
    if (series) {
      bar_series_set_data(series, new_value, count);
    }
  }
}

static ret_t on_new_random_data(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    random_series_data(chart_view);
  }
  return RET_OK;
}

static ret_t on_timer_random_data(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    random_series_data(chart_view);
  }
  return RET_REPEAT;
}

static ret_t on_close(void* ctx, event_t* e) {
  widget_t* win = (widget_t*)ctx;
  value_t val;
  (void)e;

  if (widget_get_prop(win, PROP_RANDOM_TIMER, &val) == RET_OK) {
    timer_remove(value_uint32(&val));
  }

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
    if (tk_str_eq(name, "close")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_close, win);
      }
    } else if (tk_str_eq(name, "new_graph")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_new_random_data, win);
      }
    } else if (tk_str_eq(name, "typeie")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_series_typeie_visible_changed, win);
      }
    } else if (tk_str_eq(name, "dayas")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_series_dayas_visible_changed, win);
      }
    } else if (tk_str_eq(name, "drean")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_series_drean_visible_changed, win);
      }
    } else if (tk_str_eq(name, "chartview")) {
      random_series_data(widget);
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
 * 打开柱形图表窗口
 */
ret_t open_bar_series_window(void) {
  widget_t* win = window_open("window_bar_series");
  if (win) {
    init_children_widget(win);

#if 1
    value_t val;
    value_set_uint32(&val, timer_add(on_timer_random_data, win, 3000));
    widget_set_prop(win, PROP_RANDOM_TIMER, &val);
#endif

    return RET_OK;
  }
  return RET_FAIL;
}
