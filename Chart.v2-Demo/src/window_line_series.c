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
#include "tkc/date_time.h"

#define SERIES_TIMER_UPDATE 1

extern ret_t on_series_rset_rand_ufloat_data(void* ctx, event_t* e);
extern ret_t on_series_push_rand_ufloat_data(const timer_info_t* timer);
extern ret_t on_series_rset_rand_float_data(void* ctx, event_t* e);
extern ret_t on_series_push_rand_float_data(const timer_info_t* timer);
extern ret_t on_series_rset_rand_colorful_data(void* ctx, event_t* e);
extern ret_t on_series_push_rand_colorful_data(const timer_info_t* timer);
extern ret_t on_series_line_show_changed(void* ctx, event_t* e);
extern ret_t on_series_area_show_changed(void* ctx, event_t* e);
extern ret_t on_series_symbol_show_changed(void* ctx, event_t* e);
extern ret_t on_series_smooth_changed(void* ctx, event_t* e);
extern ret_t on_series_typeie_visible_changed(void* ctx, event_t* e);
extern ret_t on_series_dayas_visible_changed(void* ctx, event_t* e);
extern ret_t on_series_typeie_bring_to_top(void* ctx, event_t* e);
extern ret_t on_series_dayas_bring_to_top(void* ctx, event_t* e);
extern ret_t on_series_bring_to_top(widget_t* win, uint32_t index, const char* icon_name,
                                    const char* unfocus, const char* focus);
extern ret_t axis_time_init(widget_t* widget);
extern void on_series_push_ufloat_data(widget_t* widget, uint32_t count);

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
  bool_t bring_to_top = (char*)ctx - (char*)NULL;
  return_value_if_fail(widget != NULL && win != NULL, RET_BAD_PARAMS);

  if (widget->name != NULL) {
    const char* name = widget->name;

    if (tk_str_eq(name, "line")) {
      widget_on(widget, EVT_CLICK, on_series_line_show_changed, win);
    } else if (tk_str_eq(name, "area")) {
      widget_on(widget, EVT_CLICK, on_series_area_show_changed, win);
    } else if (tk_str_eq(name, "symbol")) {
      widget_on(widget, EVT_CLICK, on_series_symbol_show_changed, win);
    } else if (tk_str_eq(name, "smooth")) {
      widget_on(widget, EVT_CLICK, on_series_smooth_changed, win);
    } else if (tk_str_eq(name, "close")) {
      widget_on(widget, EVT_CLICK, on_close, win);
    } else if (tk_str_eq(name, "typeie")) {
      if (bring_to_top) {
        widget_on(widget, EVT_CLICK, on_series_typeie_bring_to_top, win);
      } else {
        widget_on(widget, EVT_CLICK, on_series_typeie_visible_changed, win);
      }
    } else if (tk_str_eq(name, "dayas")) {
      if (bring_to_top) {
        widget_on(widget, EVT_CLICK, on_series_dayas_bring_to_top, win);
      } else {
        widget_on(widget, EVT_CLICK, on_series_dayas_visible_changed, win);
      }
    } else if (tk_str_eq(name, "chartview")) {
      on_series_push_ufloat_data(widget, 24);
    }
  }

  return RET_OK;
}

/**
 * 初始化窗口上的子控件
 */
static void init_children_widget(widget_t* widget, void* ctx) {
  widget_foreach(widget, init_widget, ctx);
}

static void init_line_series(widget_t* win, const char* name){
  widget_t* s = widget_lookup(win, name, TRUE);
  series_subpart_use_style(s, SERIES_SUBPART_LINE, name);
  series_subpart_use_style(s, SERIES_SUBPART_LINE_AREA, name);
  series_subpart_use_style(s, SERIES_SUBPART_SYMBOL, name);
  line_series_t* series = LINE_SERIES(s);
  series->line.smooth = TRUE;
  series->area.show = TRUE;
  series->symbol.show = TRUE;
}

static void init_normal_line_series_window(widget_t* widget) {
  widget_t* new_graph = widget_lookup(widget, "new_graph", TRUE);
  bool_t bring_to_top = strstr(widget->name, "_more_axis") != NULL ? TRUE : FALSE;
  
  init_line_series(widget, "s1");
  init_line_series(widget, "s2");
 
  if (new_graph != NULL) {
    widget_on(new_graph, EVT_CLICK, on_series_rset_rand_ufloat_data, widget);
  }

  if (bring_to_top) {
    on_series_bring_to_top(widget, 0, "typeie_icon", "typeie", "typeie_focus");
  }

  init_children_widget(widget, (char*)NULL + bring_to_top);

#if SERIES_TIMER_UPDATE
  widget_add_timer(widget, on_series_push_rand_ufloat_data, 2000);
#endif
}

static void init_colorful_line_series_window(widget_t* widget) {
  widget_t* new_graph = widget_lookup(widget, "new_graph", TRUE);
  if (new_graph != NULL) {
    widget_on(new_graph, EVT_CLICK, on_series_rset_rand_colorful_data, widget);
  }

  init_children_widget(widget, NULL);

#if SERIES_TIMER_UPDATE
  widget_add_timer(widget, on_series_push_rand_colorful_data, 2000);
#endif
}

static void init_category_line_series_window(widget_t* widget) {
  widget_t* new_graph = widget_lookup(widget, "new_graph", TRUE);
  if (new_graph != NULL) {
    widget_on(new_graph, EVT_CLICK, on_series_rset_rand_float_data, widget);
  }

  init_children_widget(widget, NULL);

#if SERIES_TIMER_UPDATE
  widget_add_timer(widget, on_series_push_rand_float_data, 2000);
#endif
}

/**
 * 打开波形图表窗口
 */
ret_t open_line_series_window(const char* name) {
  widget_t* win = window_open(name);
  return_value_if_fail(win != NULL, RET_BAD_PARAMS);

  if (strstr(name, "_colorful") != NULL) {
    init_colorful_line_series_window(win);
  } else if (strstr(name, "_category") != NULL) {
    init_category_line_series_window(win);
  } else {
    init_normal_line_series_window(win);
  }

  axis_time_init(win);

  return RET_OK;
}
