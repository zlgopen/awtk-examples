#include "watch_01.h"
#include "../custom_widgets/gesture.h"
#include "../window_main.h"
#include "../common/global.h"
#include "../custom_widgets/health_circle.h"
#include "../common/log.h"

ret_t on_open_weather(void* ctx, event_t* e);
ret_t on_open_health(void* ctx, event_t* e);

static ret_t on_watch_01_battery_timer(const timer_info_t* timer) {
  widget_t* widget = WIDGET(timer->ctx);

  watch_01_t* watch_01 = &(global_data.watch_01);
  if (watch_01->battery == 0) {
    watch_01->battery = 100;
  } else {
    watch_01->battery--;
  }
  health_circle_set_value_b(widget, (float_t)watch_01->battery);

  widget_t* window = widget_get_window(widget);
  widget_t* watch_01_battery_value = widget_lookup(window, "watch_01_battery_value", TRUE);
  if (watch_01_battery_value != NULL) {
    char str[4];
    memset(str, '\0', sizeof(str));
    tk_snprintf(str, sizeof(str), "%d%%", (int)HEALTH_CIRCLE(widget)->value_b);
    widget_set_text_utf8(watch_01_battery_value, str);
  }

  return RET_REPEAT;
}

static ret_t on_watch_01_foot_timer(const timer_info_t* timer) {
  widget_t* widget = WIDGET(timer->ctx);

  watch_01_t* watch_01 = &(global_data.watch_01);
  if (watch_01->foot == 20000) {
    watch_01->foot = 0;
  } else {
    watch_01->foot += 100;
  }
  health_circle_set_value_b(widget, (float_t)watch_01->foot);

  widget_t* window = widget_get_window(widget);
  widget_t* watch_01_foot_value = widget_lookup(window, "watch_01_foot_value", TRUE);
  if (watch_01_foot_value != NULL) {
    char str[6];
    memset(str, '\0', sizeof(str));
    tk_snprintf(str, sizeof(str), "%d", (int)HEALTH_CIRCLE(widget)->value_b);
    widget_set_text_utf8(watch_01_foot_value, str);
  }

  return RET_REPEAT;
}

ret_t on_battery_long_press(void* ctx, event_t* e) {
  (void)ctx;
  (void)e;
  LOG("");
  assert(FALSE);
  return RET_OK;
}

void watch_01_init(widget_t* window) {
  widget_t* gesture = widget_lookup(window, "gesture", TRUE);
  if (gesture != NULL) {
    global_data.watch_type = WATCH_01;
    window_init(window);
  }

  widget_t* watch_01_battery = widget_lookup(window, "watch_01_battery", TRUE);
  if (watch_01_battery != NULL) {
    timer_info_t timer;
    timer.ctx = watch_01_battery;
    on_watch_01_battery_timer(&timer);
    widget_add_timer(watch_01_battery, on_watch_01_battery_timer, 1000);
  }

  widget_t* watch_01_foot = widget_lookup(window, "watch_01_foot", TRUE);
  if (watch_01_foot != NULL) {
    timer_info_t timer;
    timer.ctx = watch_01_foot;
    on_watch_01_foot_timer(&timer);
    widget_add_timer(watch_01_foot, on_watch_01_foot_timer, 1000);
  }

  widget_t* weather = widget_lookup(window, "weather", TRUE);
  if (weather != NULL) {
    widget_on(weather, EVT_CLICK, on_open_weather, window);
  }

  widget_t* foot = widget_lookup(window, "foot", TRUE);
  if (foot != NULL) {
    widget_on(foot, EVT_CLICK, on_open_health, window);
  }

  widget_t* battery = widget_lookup(window, "battery", TRUE);
  if (battery != NULL) {
    widget_on(battery, EVT_LONG_PRESS, on_battery_long_press, NULL);
  }
}
