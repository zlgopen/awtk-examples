#include "heart_rate.h"
#include <assert.h>
#include "../custom_widgets/bar_graph.h"
#include "../common/global.h"
#include "../custom_widgets/gesture.h"

#include "../common/log.h"
#include "../common/global.h"
#include "../slide_appliction/app.h"
#include "heart_rate_check.h"
#include "../custom_widgets/slide_menu_v.h"

static const char* s_window_heart_rate = "heart_rate";

static ret_t on_close_window(void* ctx, event_t* e) {
  (void)e;
  widget_t* win = WIDGET(ctx);

  str_t* prev_window = (str_t*)darray_tail(&(global_data.window_history));
  if (str_eq(prev_window, g_window_enum->app.str)) {
    app_init(win, "slide_right");
  }

  window_history_pop();

  return RET_OK;
}

static ret_t on_timer_label_heart(const timer_info_t* timer) {
  widget_t* widget = WIDGET(timer->ctx);

  uint32_t t = rand() % 50 + 60;
  char s[32];
  memset(s, '\0', sizeof(s));
  tk_snprintf(s, sizeof(s), "%d", t);

  widget_set_text_utf8(widget, s);

  return RET_REPEAT;
}

static ret_t on_timer_heart_rate_time(const timer_info_t* timer) {
  widget_t* widget = WIDGET(timer->ctx);

  char s[32];
  memset(s, '\0', sizeof(s));
  date_time_t dt;
  date_time_init(&dt);

  tk_snprintf(s, sizeof(s), "%0d:%0d", dt.hour, dt.minute);

  widget_set_text_utf8(widget, s);

  return RET_REPEAT;
}

// 开始检测心率
static ret_t on_heart_rate_bottom(void* ctx, event_t* e) {
  (void)e;
  (void)ctx;

  widget_t* check = window_open("heart_rate_check");
  if (check != NULL) {
    heart_rate_check_init(check);
  }

  return RET_OK;
}

static ret_t on_update_check(const timer_info_t* timer) {
  LOG("");
  widget_t* window = WIDGET(timer->ctx);
  widget_t* heart_rate_check_number = widget_lookup(window, "heart_rate_check_number", TRUE);
  if (heart_rate_check_number != NULL) {
    wstr_t wstr;
    wstr_init(&wstr, 0);
    wstr_from_int(&wstr, global_data.heart_rate.result);
    wstr_append(&wstr, L"次/分");
    widget_set_text(heart_rate_check_number, wstr.str);
    wstr_reset(&wstr);
  }

  widget_t* check_time = widget_lookup(window, "check_time", TRUE);
  if (check_time != NULL) {
    wstr_t wstr;
    wstr_init(&wstr, 0);

    date_time_t dt;
    date_time_init(&dt);

    heart_rate_t* heart_rate = &(global_data.heart_rate);
    int32_t hour = heart_rate->dt.hour;
    int32_t minute = heart_rate->dt.minute;

    if (dt.hour > hour) {
      wstr_from_int(&wstr, dt.hour - hour);
      wstr_append(&wstr, L"小时前");
    } else if (dt.minute > minute) {
      wstr_from_int(&wstr, dt.minute - minute);
      wstr_append(&wstr, L"分钟前");
    } else {
      wstr_append(&wstr, L"刚刚");
    }

    widget_set_text(check_time, wstr.str);

    wstr_reset(&wstr);
  }

  widget_t* heart_rate_bar = widget_lookup(window, "heart_rate_bar", TRUE);
  if (heart_rate_bar != NULL) {
    widget_invalidate(heart_rate_bar, NULL);
  }

  return RET_REPEAT;
}

void init_heart_rate(widget_t* widget) {
  assert(widget != NULL);

  widget_t* heart_rate_bar = widget_lookup(widget, "heart_rate_bar", TRUE);
  if (heart_rate_bar != NULL) {
    bar_graph_t* bar = BAR_GRAPH(heart_rate_bar);
    uint32_t size = widget_get_prop_int(heart_rate_bar, "size", -1);
    global_data.bar_max_size = size;
    bar->data = global_data.bar_data;
  }

  widget_t* label_heart = widget_lookup(widget, "label_heart", TRUE);
  if (label_heart != NULL) {
    widget_add_timer(label_heart, on_timer_label_heart, 1000);
  }

  widget_t* heart_rate_time = widget_lookup(widget, "heart_rate_time", TRUE);
  if (heart_rate_time != NULL) {
    widget_add_timer(heart_rate_time, on_timer_heart_rate_time, 1000);
  }

  widget_t* gesture = widget_lookup(widget, "heart_gesture", TRUE);
  assert(gesture != NULL);
  if (gesture != NULL) {
    widget_on(gesture, EVT_SLIDE_RIGHT, on_close_window, widget);
  }

  widget_t* window = widget_get_window(widget);
  widget_t* heart_rate_top = widget_lookup(window, "heart_rate_top", TRUE);
  assert(heart_rate_top != NULL);

  widget_t* heart_rate_bottom = widget_lookup(window, "heart_rate_bottom", TRUE);
  assert(heart_rate_bottom != NULL);

  widget_on(heart_rate_bottom, EVT_CLICK, on_heart_rate_bottom, heart_rate_bottom);
}

ret_t on_open_heart_rate(void* ctx, event_t* e) {
  widget_t* to_close = WIDGET(ctx);
  window_history_push(to_close);
  (void)e;
  LOG("");

  widget_t* win = window_open_and_close(s_window_heart_rate, to_close);
  if (win != NULL) {
    init_heart_rate(win);
    timer_info_t timer;
    timer.ctx = win;
    on_update_check(&timer);
    widget_add_timer(win, on_update_check, 1000);

    return RET_OK;
  }

  return RET_FAIL;
}
