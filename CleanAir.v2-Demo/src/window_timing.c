/**
 * File:   window_timing.c
 * Author: AWTK Develop Team
 * Brief:  timing window
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
#include "awtk.h"
#include <time.h>
#include "text_selector/text_selector.h"

static int32_t hour_value = 0;
static int32_t min_value = 0;
static int32_t sec_value = 0;

static ret_t save_text_selector(widget_t* win) {
  widget_t* hour_selector = widget_lookup(win, "hour_selector", TRUE);
  if (hour_selector) {
    hour_value = text_selector_get_value(hour_selector);
  }

  widget_t* min_selector = widget_lookup(win, "min_selector", TRUE);
  if (min_selector) {
    min_value = text_selector_get_value(min_selector);
  }

  widget_t* sec_selector = widget_lookup(win, "sec_selector", TRUE);
  if (sec_selector) {
    sec_value = text_selector_get_value(sec_selector);
  }

  return RET_OK;
}
static ret_t on_ok(void* ctx, event_t* e) {
  widget_t* dialog = (widget_t*)ctx;

  (void)e;

  save_text_selector(dialog);
  dialog_quit(dialog, RET_OK);

  return RET_OK;
}

static ret_t on_cancle(void* ctx, event_t* e) {
  widget_t* dialog = (widget_t*)ctx;

  (void)e;

  hour_value = 0;
  min_value = 0;
  sec_value = 0;
  dialog_quit(dialog, RET_QUIT);

  return RET_OK;
}

static ret_t init_widget(void* ctx, const void* iter) {
  widget_t* widget = WIDGET(iter);

  (void)ctx;

  if (widget->name != NULL) {
    const char* name = widget->name;
    if (tk_str_eq(name, "ok")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_ok, win);
      }
    } else if (tk_str_eq(name, "cancle")) {
      widget_t* win = widget_get_window(widget);
      if (win) {
        widget_on(widget, EVT_CLICK, on_cancle, win);
      }
    }
  }

  return RET_OK;
}

static void init_children_widget(widget_t* widget) {
  widget_foreach(widget, init_widget, widget);
}

static ret_t set_text_selector(widget_t* win) {
  widget_t* hour_selector = widget_lookup(win, "hour_selector", TRUE);
  if (hour_selector) {
#ifdef TEXT_SELECTOR_PROP_VALUE_ANIMATOR
    widget_set_prop_bool(hour_selector, TEXT_SELECTOR_PROP_VALUE_ANIMATOR, FALSE);
#endif
    text_selector_set_value(hour_selector, hour_value);
  }

  widget_t* min_selector = widget_lookup(win, "min_selector", TRUE);
  if (min_selector) {
#ifdef TEXT_SELECTOR_PROP_VALUE_ANIMATOR
    widget_set_prop_bool(min_selector, TEXT_SELECTOR_PROP_VALUE_ANIMATOR, FALSE);
#endif
    text_selector_set_value(min_selector, min_value);
  }

  widget_t* sec_selector = widget_lookup(win, "sec_selector", TRUE);
  if (sec_selector) {
#ifdef TEXT_SELECTOR_PROP_VALUE_ANIMATOR
    widget_set_prop_bool(sec_selector, TEXT_SELECTOR_PROP_VALUE_ANIMATOR, FALSE);
#endif
    text_selector_set_value(sec_selector, sec_value);
  }

  return RET_OK;
}

ret_t open_timing_window(time_t* result) {
  widget_t* win = window_open("timing/timing");
  if (win) {
    set_text_selector(win);
    init_children_widget(win);
    return dialog_modal(win);
  }

  return RET_FAIL;
}
