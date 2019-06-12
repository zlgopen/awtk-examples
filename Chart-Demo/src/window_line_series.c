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
#include "custom_widgets/chart/line_series.h"
#include "custom_widgets/chart/bar_series.h"

#include <math.h>
#include <stdlib.h>

#define PROP_RANDOM_TIMER "timer_id"

static ret_t on_series_prop_changed(void* ctx, event_t* e, const char* prop, const char* btn_name,
                                    const char* style, const char* style_select) {
  widget_t* win = WIDGET(ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    widget_t* series;
    uint32_t i;
    uint32_t nr = chart_view_count_series(chart_view, NULL);
    for (i = 0; i < nr; i++) {
      series = chart_view_get_series(chart_view, NULL, i);
      if (series) {
        value_t v;
        if (widget_get_prop(series, prop, &v) == RET_OK) {
          value_set_bool(&v, !value_bool(&v));
          widget_set_prop(series, prop, &v);

          widget_t* btn = widget_lookup(win, btn_name, TRUE);
          if (btn) {
            widget_use_style(btn, value_bool(&v) ? style_select : style);
          }
        }
      }
    }
  }

  return RET_OK;
}

static ret_t on_series_visible(void* ctx, event_t* e, uint32_t index, const char* icon_name,
                               const char* show, const char* hide) {
  widget_t* win = WIDGET(ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    widget_t* series = chart_view_get_series(chart_view, NULL, index);
    if (series) {
      value_t v;
      if (widget_get_prop(series, WIDGET_PROP_VISIBLE, &v) == RET_OK) {
        value_set_bool(&v, !value_bool(&v));
        widget_set_prop(series, WIDGET_PROP_VISIBLE, &v);
      }

      widget_t* icon = widget_lookup(win, icon_name, TRUE);
      if (icon) {
        widget_use_style(icon, series->visible ? show : hide);
      }
    }
  }

  return RET_OK;
}

static ret_t on_line_changed(void* ctx, event_t* e) {
  return on_series_prop_changed(ctx, e, SERIES_PROP_LINE_SHOW, "line", "line", "line_select");
}

static ret_t on_area_changed(void* ctx, event_t* e) {
  return on_series_prop_changed(ctx, e, SERIES_PROP_AREA_SHOW, "area", "area", "area_select");
}

static ret_t on_symbol_changed(void* ctx, event_t* e) {
  return on_series_prop_changed(ctx, e, SERIES_PROP_SYMBOL_SHOW, "symbol", "symbol",
                                "symbol_select");
}

static ret_t on_smooth_changed(void* ctx, event_t* e) {
  return on_series_prop_changed(ctx, e, SERIES_PROP_LINE_SMOOTH, "smooth", "smooth",
                                "smooth_select");
}

ret_t on_series_typeie_visible_changed(void* ctx, event_t* e) {
  return on_series_visible(ctx, e, 0, "typeie_icon", "typeie", "series_hide");
}

ret_t on_series_dayas_visible_changed(void* ctx, event_t* e) {
  return on_series_visible(ctx, e, 1, "dayas_icon", "dayas", "series_hide");
}

ret_t on_series_drean_visible_changed(void* ctx, event_t* e) {
  return on_series_visible(ctx, e, 2, "drean_icon", "drean", "series_hide");
}

static void on_append_series_data(widget_t* widget, uint32_t count) {
  float_t* new_value = TKMEM_ZALLOCN(float, count);
  widget_t* series;
  uint32_t i, j;
  uint32_t nr = chart_view_count_series(widget, WIDGET_TYPE_LINE_SERIES);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < count; j++) {
      new_value[j] = rand() % 120 + 10;
    }

    series = chart_view_get_series(widget, WIDGET_TYPE_LINE_SERIES, i);
    if (series) {
      line_series_append(series, new_value, count);
    }
  }
  TKMEM_FREE(new_value);
}

static void on_set_series_data(widget_t* widget, uint32_t count) {
  float_t* new_value = TKMEM_ZALLOCN(float, count);
  widget_t* series;
  uint32_t i, j;
  uint32_t nr = chart_view_count_series(widget, WIDGET_TYPE_LINE_SERIES);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < count; j++) {
      new_value[j] = rand() % 120 + 10;
    }

    series = chart_view_get_series(widget, WIDGET_TYPE_LINE_SERIES, i);
    if (series) {
      line_series_set_data(series, new_value, count);
    }
  }
  TKMEM_FREE(new_value);
}

static ret_t on_new_random_data(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    on_set_series_data(chart_view, 12);
  }
  return RET_OK;
}

static ret_t on_timer_random_data(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    on_append_series_data(chart_view, 1);
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
    if (tk_str_eq(name, "line")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_line_changed, win);
      }
    } else if (tk_str_eq(name, "area")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_area_changed, win);
      }
    } else if (tk_str_eq(name, "symbol")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_symbol_changed, win);
      }
    } else if (tk_str_eq(name, "smooth")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_smooth_changed, win);
      }
    } else if (tk_str_eq(name, "new_graph")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_new_random_data, win);
      }
    } else if (tk_str_eq(name, "close")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_close, win);
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
    } else if (tk_str_eq(name, "chartview")) {
      on_append_series_data(widget, 12);
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
 * 打开波形图表窗口
 */
ret_t open_line_series_window(void) {
  widget_t* win = window_open("window_line_series");
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
