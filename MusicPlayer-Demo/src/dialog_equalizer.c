/**
 * File:   window_record.c
 * Author: AWTK Develop Team
 * Brief:  equalizer window
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
 * 2019-2-20  ZhongWei1 created
 *
 */
#include "awtk.h"
#include "custom_widgets/chart/chart_view.h"
#include "custom_widgets/chart/line_series.h"
#include "custom_widgets/chart/bar_series.h"

#define SLIDER_COUNT 9
#define BUFF_LEN 32

static float val_slider[SLIDER_COUNT] = {50, 50, 50, 50, 50, 50, 50, 50, 50};



static ret_t on_close(void* ctx, event_t* e) {
  widget_t* dialog = (widget_t*)ctx;
  (void)e;

  return dialog_quit(dialog, RET_QUIT);
}

static ret_t on_dialog_state(void* ctx, event_t* e) {
  widget_t* dialog = (widget_t*)ctx;
  pointer_event_t* evt = (pointer_event_t*)e;
  widget_t* close = widget_lookup(dialog, "close", TRUE);
  if (evt->x > (dialog->x + dialog->w) || evt->x < dialog->x || evt->y < dialog->y ||
      evt->y > (dialog->y + dialog->h)) {
    event_t et = event_init(EVT_CLICK, close);
    widget_dispatch(close, &et);
  }

  return RET_OK;
}

static void set_series_data(float* slider_line, widget_t* widget, uint32_t count) {
  widget_t* series;

  series = chart_view_get_series(widget, WIDGET_TYPE_LINE_SERIES, 0);
  if (series) {
    line_series_set_data(series, slider_line, count);
    widget_invalidate(widget, NULL);
  }
}

/**
 * eq滑动条数值发生改变回调(联动char_view波形发生对应改变)
 */
static ret_t on_slider_changing(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* child = WIDGET(e->target);
  char name_buf[BUFF_LEN] = {0};
  strcpy(name_buf, child->name);
  strtok(name_buf, "_");
  int i = atoi(strtok(NULL, "_"));
  val_slider[i] = widget_get_value(child);

  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    set_series_data(val_slider, chart_view, SLIDER_COUNT);
  }
  return RET_OK;
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
      widget_t* dialog = widget_get_window(widget);

      if (dialog) {
        widget_on(widget, EVT_CLICK, on_close, dialog);
      }
    } else if (tk_str_eq(name, "frequency_0")) {
      widget_t* dialog = widget_get_window(widget);
      widget_on(widget, EVT_VALUE_CHANGING, on_slider_changing, dialog);
      slider_set_value(widget, val_slider[0]);
    } else if (tk_str_eq(name, "frequency_1")) {
      widget_t* dialog = widget_get_window(widget);
      widget_on(widget, EVT_VALUE_CHANGING, on_slider_changing, dialog);
      slider_set_value(widget, val_slider[1]);
    } else if (tk_str_eq(name, "frequency_2")) {
      widget_t* dialog = widget_get_window(widget);
      widget_on(widget, EVT_VALUE_CHANGING, on_slider_changing, dialog);
      slider_set_value(widget, val_slider[2]);
    } else if (tk_str_eq(name, "frequency_3")) {
      widget_t* dialog = widget_get_window(widget);
      widget_on(widget, EVT_VALUE_CHANGING, on_slider_changing, dialog);
      slider_set_value(widget, val_slider[3]);
    } else if (tk_str_eq(name, "frequency_4")) {
      widget_t* dialog = widget_get_window(widget);
      widget_on(widget, EVT_VALUE_CHANGING, on_slider_changing, dialog);
      slider_set_value(widget, val_slider[4]);
    } else if (tk_str_eq(name, "frequency_5")) {
      widget_t* dialog = widget_get_window(widget);
      widget_on(widget, EVT_VALUE_CHANGING, on_slider_changing, dialog);
      slider_set_value(widget, val_slider[5]);
    } else if (tk_str_eq(name, "frequency_6")) {
      widget_t* dialog = widget_get_window(widget);
      widget_on(widget, EVT_VALUE_CHANGING, on_slider_changing, dialog);
      slider_set_value(widget, val_slider[6]);
    } else if (tk_str_eq(name, "frequency_7")) {
      widget_t* dialog = widget_get_window(widget);
      widget_on(widget, EVT_VALUE_CHANGING, on_slider_changing, dialog);
      slider_set_value(widget, val_slider[7]);
    } else if (tk_str_eq(name, "frequency_8")) {
      widget_t* dialog = widget_get_window(widget);
      widget_on(widget, EVT_VALUE_CHANGING, on_slider_changing, dialog);
      slider_set_value(widget, val_slider[8]);
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
 * 打开均衡器窗口
 */
ret_t open_equalizer_dialog() {
  widget_t* dialog = dialog_open("equalizer");
  if (dialog) {
    init_children_widget(dialog);
    widget_on(dialog, EVT_POINTER_DOWN, on_dialog_state, dialog);
    widget_t* chart_view = widget_lookup(dialog, "chartview", TRUE);
    if (chart_view) {
      set_series_data(val_slider, chart_view, SLIDER_COUNT);
    }

    return dialog_modal(dialog);
  }

  return RET_FAIL;
}