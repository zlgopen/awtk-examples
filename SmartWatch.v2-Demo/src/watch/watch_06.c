#include "watch_06.h"
#include "../common/global.h"
#include "../window_main.h"
#include "../common/log.h"
#include "../custom_widgets/gesture.h"

ret_t on_open_weather(void* ctx, event_t* e);
ret_t on_open_health(void* ctx, event_t* e);
ret_t on_open_music(void* ctx, event_t* e);
ret_t on_open_weather(void* ctx, event_t* e);
ret_t on_open_health(void* ctx, event_t* e);
ret_t on_open_calendar(void* ctx, event_t* e);
static ret_t on_weather_update_timer(const timer_info_t* timer);

void watch_06_update(widget_t* window) {
  widget_t* wday = widget_lookup(window, "wday", TRUE);
  if (wday != NULL) {
    date_time_t dt;
    date_time_init(&dt);

    wstr_t str_wday;
    wstr_init(&str_wday, 0);

    switch (dt.wday) {
      case 0: {
        wstr_append(&str_wday, L"周日");
      } break;
      case 1: {
        wstr_append(&str_wday, L"周一");
      } break;
      case 2: {
        wstr_append(&str_wday, L"周二");
      } break;
      case 3: {
        wstr_append(&str_wday, L"周三");
      } break;
      case 4: {
        wstr_append(&str_wday, L"周四");
      } break;
      case 5: {
        wstr_append(&str_wday, L"周五");
      } break;
      case 6: {
        wstr_append(&str_wday, L"周六");
      } break;
      default:
        break;
    }

    widget_set_text(wday, str_wday.str);
    wstr_reset(&str_wday);
  }

  widget_t* minute = widget_lookup(window, "minute", TRUE);
  if (minute != NULL) {
  }

  widget_t* number = widget_lookup(window, "number", TRUE);
  if (number != NULL) {
  }
}

static ret_t on_watch_06_timer(const timer_info_t* timer) {
  widget_t* window = WIDGET(timer->ctx);

  watch_06_update(window);

  widget_t* heart_rate_bar = widget_lookup(window, "heart_rate_bar", TRUE);
  if (heart_rate_bar != NULL) {
    widget_invalidate(heart_rate_bar, NULL);
  }

  widget_t* weather = widget_lookup(window, g_window_enum->weather.str, TRUE);
  if (weather != NULL) {
    // widget_add_timer(weather, on_weather_update_timer, 1000);
    timer_info_t timer;
    timer.ctx = weather;
    on_weather_update_timer(&timer);
  }

  return RET_REPEAT;
}

static ret_t on_watch_06_click(void* ctx, event_t* e) {
  (void)e;
  widget_t* widget = WIDGET(ctx);
  widget_t* window = widget_get_window(widget);
  const char* name = widget->name;
  if (str_eq(&(g_window_enum->weather), name)) {
    on_open_weather(window, NULL);
  } else if (str_eq(&(g_window_enum->calendar), name)) {
    on_open_calendar(window, NULL);
  } else if (str_eq(&(g_window_enum->health), name)) {
    on_open_health(window, NULL);
  } else if (str_eq(&(g_window_enum->music), name)) {
    on_open_music(window, NULL);
  }
  return RET_OK;
}

static ret_t on_watch_06_visit(void* ctx, const void* data) {
  (void)ctx;
  widget_t* widget = WIDGET(data);
  const char* type = widget_get_type(widget);
  if (tk_str_eq(type, WIDGET_TYPE_BUTTON)) {
    widget_on(widget, EVT_CLICK, on_watch_06_click, widget);
  }
  return RET_OK;
}

static ret_t on_weather_update_timer(const timer_info_t* timer) {
  widget_t* widget = WIDGET(timer->ctx);

  if (!tk_str_eq(widget->state, WIDGET_STATE_OVER)) {
    uint32_t r = rand() % 4;
    LOG("r:%d", r);
    char style[64];
    memset(style, '\0', sizeof(style));

    tk_snprintf(style, sizeof(style), "watch_06_weather_%02d", r);
    widget_use_style(widget, style);
  }

  return RET_REPEAT;
}

void watch_06_init(widget_t* window) {
  widget_t* gesture = widget_lookup(window, "gesture", TRUE);
  if (gesture != NULL) {
    global_data.watch_type = WATCH_06;
    window_init(window);
  }

  // 心率数据
  widget_t* heart_rate_bar = widget_lookup(window, "heart_rate_bar", TRUE);
  if (heart_rate_bar != NULL) {
    bar_graph_t* bar = BAR_GRAPH(heart_rate_bar);
    uint32_t size = widget_get_prop_int(heart_rate_bar, "size", -1);

    bar->data = &(global_data.watch_06.data);
    global_data.watch_06.max_size = size;
  }

  timer_info_t timer;
  timer.ctx = window;
  on_watch_06_timer(&timer);

  widget_foreach(window, on_watch_06_visit, window);

  widget_add_timer(window, on_watch_06_timer, 1000);
}
