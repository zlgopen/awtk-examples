#include "heart_rate_check.h"
#include "../custom_widgets/gesture.h"
#include "../common/global.h"

static ret_t on_close_window(void* ctx, event_t* e) {
  (void)e;
  widget_t* win = WIDGET(ctx);

  window_close(win);

  return RET_OK;
}

static ret_t on_close(const timer_info_t* timer) {
  widget_t* window = WIDGET(timer->ctx);

  widget_t* show = widget_lookup(window, "show", TRUE);
  if (show != NULL) {
    int32_t t = widget_get_prop_int(window, "_time", -1);
    char str[4];
    memset(str, '\0', sizeof(str));
    if (t < 0) {
      window_close(window);
    } else {
      tk_snprintf(str, sizeof(str), "%d", t);
      widget_set_text_utf8(show, str);
      widget_set_prop_int(window, "_time", t - 1);
      widget_add_timer(window, on_close, 1000);
    }
  }

  return RET_REMOVE;
}

static ret_t on_timer(const timer_info_t* timer) {
  widget_t* window = WIDGET(timer->ctx);

  widget_t* show = widget_lookup(window, "show", TRUE);
  if (show != NULL) {
    // widget_set_text(show, L"检测结果：");
    wstr_t wstr;
    wstr_init(&wstr, 0);
    wstr_append(&wstr, L"检测结果:");
    int32_t number = 60 + rand() % 61;
    (void)number;

    char text[32];
    memset(text, '\0', sizeof(text));
    int32_t n = 60 + rand() % 61;

    heart_rate_t* heart_rate = &(global_data.heart_rate);
    heart_rate->result = n;
    date_time_init(&(heart_rate->dt));

    tk_snprintf(text, sizeof(text), "%d", n);
    wstr_t wt;
    wstr_init(&wt, 0);
    wstr_set_utf8(&wt, text);

    wstr_append(&wstr, wt.str);

    widget_set_text(show, wstr.str);

    wstr_reset(&wt);
    wstr_reset(&wstr);
    widget_add_timer(window, on_close, 1000);
  }

  widget_t* bottom = widget_lookup(window, "bottom", TRUE);
  if (bottom != NULL) {
    widget_set_text(bottom, L"即将关闭");
  }

  return RET_REMOVE;
}

void heart_rate_check_init(widget_t* window) {
  widget_t* gesture = widget_lookup(window, "heart_rate_check_gesture", TRUE);
  if (gesture != NULL) {
    widget_on(gesture, EVT_SLIDE_RIGHT, on_close_window, window);
    widget_add_timer(window, on_timer, 5000);
    widget_set_prop_int(window, "_time", 3);
  }
}
