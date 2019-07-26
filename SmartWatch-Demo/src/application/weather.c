#include <awtk.h>
#include "../custom_widgets/gesture.h"
#include <widget_animators/widget_animator_opacity.h>
#include "../custom_widgets/hour_weather.h"
#include "../common/log.h"
#include "../common/global.h"
#include "../window_main.h"
#include "../slide_appliction/app.h"

static ret_t on_temperature_update_timer(const timer_info_t* timer);

#ifdef _WIN32
#pragma warning(disable : 4244)
#endif  // _WIN32

static uint32_t active_old = 0;

static ret_t on_close_window(void* ctx, event_t* e) {
  (void)e;
  widget_t* win = WIDGET(ctx);
  value_t val;
  value_set_uint32(&val, 0);
  if (widget_get_prop(win, "timer_id", &val) == RET_OK && value_uint32(&val) != 0) {
    timer_remove(value_uint32(&val));
  }

  open_and_close(win);

  return RET_OK;
}

static void set_random_int(widget_t* label, int32_t min_num, int32_t max_num, char* unit) {
  if (label) {
    char text[32] = {0};
    int32_t val = fmod(rand(), max_num - min_num) + min_num;
    if (unit) {
      tk_snprintf(text, sizeof(text), "%d%s", val, unit);
    } else {
      tk_snprintf(text, sizeof(text), "%d", val);
    }

    widget_set_text_utf8(label, text);
  }
}

static void set_random_int_wstr(widget_t* label, int32_t min_num, int32_t max_num, wchar_t* unit) {
  if (label) {
    char text[32] = {0};
    (void)text;
    int32_t val = fmod(rand(), max_num - min_num) + min_num;

    wstr_t result;
    wstr_init(&result, 0);

    char val_text[4];
    memset(val_text, '\0', sizeof(val_text));
    tk_snprintf(val_text, sizeof(val_text), "%d", val);
    wstr_set_utf8(&result, val_text);

    wstr_append(&result, unit);

    widget_set_text(label, result.str);
    wstr_reset(&result);
  }
}

/**
 * 定时刷新天气数据
 */
static ret_t on_update_weather(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);
  widget_t* temp_now = widget_lookup(win, "weather:temp", TRUE);
  widget_t* temp_min = widget_lookup(win, "weather:temp_min", TRUE);
  widget_t* temp_max = widget_lookup(win, "weather:temp_max", TRUE);
  widget_t* aqi = widget_lookup(win, "weather:aqi", TRUE);
  widget_t* temp_n_min = widget_lookup(win, "weather:temp_n_min", TRUE);
  widget_t* temp_n_max = widget_lookup(win, "weather:temp_n_max", TRUE);
  widget_t* temp_nn_min = widget_lookup(win, "weather:temp_nn_min", TRUE);
  widget_t* temp_nn_max = widget_lookup(win, "weather:temp_nn_max", TRUE);
  widget_t* update_time = widget_lookup(win, "weather:update", TRUE);
  widget_t* wind = widget_lookup(win, "weather:wind", TRUE);
  widget_t* uvi = widget_lookup(win, "weather:uvi", TRUE);
  widget_t* humi = widget_lookup(win, "weather:humi", TRUE);
  widget_t* temp_feel = widget_lookup(win, "weather:temp_feel", TRUE);
  widget_t* air_p = widget_lookup(win, "weather:air_p", TRUE);

  set_random_int_wstr(temp_now, 20, 30, L"°");
  set_random_int(aqi, 10, 400, NULL);
  set_random_int_wstr(temp_min, 10, 20, L"°");
  set_random_int_wstr(temp_max, 30, 35, L"°");
  set_random_int_wstr(temp_n_min, 10, 20, L"°");
  set_random_int_wstr(temp_n_max, 30, 35, L"°");
  set_random_int_wstr(temp_nn_min, 10, 20, L"°");
  set_random_int_wstr(temp_nn_max, 30, 35, L"°");
  set_random_int(wind, 1, 15, NULL);
  set_random_int(uvi, 1, 12, NULL);
  set_random_int(humi, 64, 100, "%");
  set_random_int_wstr(temp_feel, 15, 35, L"°");
  set_random_int(air_p, 900, 1200, "hPa");

  date_time_t date;
  date_time_init(&date);
  char text[32] = {0};
  tk_snprintf(text, sizeof(text), "%02d:%02d", date.hour, date.minute);
  widget_set_text_utf8(update_time, text);

  return RET_REPEAT;
}

/**
 * 在page1、page2之间切换时切换背景
 */
static ret_t on_slide_active_changed(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* bg = widget_lookup(win, "weather:bg", TRUE);
  slide_view_t* watch_slide_view = SLIDE_VIEW(e->target);
  uint32_t active = watch_slide_view->active;

  if (active == 1 && active_old == 0) {
    widget_animator_t* animator = widget_animator_opacity_create(bg, 500, 0, EASING_SIN_OUT);
    widget_animator_opacity_set_params(animator, 255, 0);
    widget_animator_start(animator);
  } else if (active == 0 && active_old == 1) {
    widget_animator_t* animator = widget_animator_opacity_create(bg, 500, 0, EASING_SIN_OUT);
    widget_animator_opacity_set_params(animator, 0, 255);
    widget_animator_start(animator);
  }
  active_old = active;
  widget_invalidate(win, NULL);
  return RET_OK;
}

static ret_t on_change_select(void* ctx, event_t* e) {
  (void)e;
  (void)ctx;
  widget_t* widget = WIDGET(ctx);
  widget_t* window = widget_get_window(widget);

  widget_t* temperature = widget_lookup(window, "temperature", TRUE);
  if (temperature != NULL) {
    timer_info_t timer;
    timer.ctx = temperature;
    on_temperature_update_timer(&timer);
  }

  return RET_OK;
}

static ret_t init_widget(void* ctx, const void* iter) {
  widget_t* widget = WIDGET(iter);
  (void)ctx;

  if (widget != NULL && widget->name != NULL) {
    widget_t* win = widget_get_window(widget);
    if (strstr(widget->name, "weather:")) {
      char* name = widget->name + 8;
      if (tk_str_eq(name, "return")) {
        widget_on(widget, EVT_CLICK, on_close_window, win);
      } else if (tk_str_eq(name, "slideview")) {
        // widget_on(widget, EVT_VALUE_CHANGED, on_slide_active_changed, win);
      } else if (tk_str_eq(name, "gesture")) {
        widget_on(widget, EVT_SLIDE_RIGHT, on_close_window, win);
      } else if (tk_str_eq(name, "hour_weather")) {
        widget_on(widget, EVT_CHANGE_SELECT, on_change_select, NULL);
      }
    }
  }
  return RET_OK;
}

static void init_children_widget(widget_t* widget) {
  widget_foreach(widget, init_widget, widget);
}

static int s_window_style = 0;

static ret_t on_update_weather_style(const timer_info_t* timer) {
  widget_t* window = WIDGET(timer->ctx);

  widget_t* slide = widget_lookup(window, "weather:slideview", TRUE);
  if (slide != NULL && SLIDE_VIEW(slide)->animating) {
    char window_style[64];
    memset(window_style, '\0', sizeof(window_style));
    tk_snprintf(window_style, sizeof(window_style), "weather_0%d", s_window_style);
    widget_use_style(window, window_style);

    if (s_window_style == 5) {
      s_window_style = 0;
    } else {
      s_window_style++;
    }
  }

  widget_invalidate(window, NULL);

  return RET_REPEAT;
}

static ret_t on_temperature_update_timer(const timer_info_t* timer) {
  widget_t* widget = WIDGET(timer->ctx);

  uint32_t r = rand() % 10 + 20;
  wstr_t wstr;
  wstr_init(&wstr, 0);
  wstr_from_int(&wstr, r);
  wstr_append(&wstr, L"°");

  widget_set_text(widget, wstr.str);
  wstr_reset(&wstr);

  return RET_REPEAT;
}

/**
 * 点击天气图标打开天气应用
 */
ret_t on_open_weather(void* ctx, event_t* e) {
  widget_t* to_close = WIDGET(ctx);
  (void)e;
  widget_t* win = window_open_and_close(g_window_enum->weather.str, to_close);
  if (win) {
    active_old = 0;
    init_children_widget(win);
    widget_t* slide_view = widget_lookup(win, "weather:slideview", TRUE);
    widget_on(slide_view, EVT_VALUE_CHANGED, on_slide_active_changed, win);

    timer_info_t timer;
    timer.ctx = win;
    on_update_weather(&timer);

    value_t val;
    value_set_uint32(&val, widget_add_timer(win, on_update_weather, 3000));
    widget_set_prop(win, "timer_id", &val);

    // 背景自动变换
    if (FALSE) {
      widget_add_timer(win, on_update_weather_style, 1000);
    }

    widget_t* temperature = widget_lookup(win, "temperature", TRUE);
    if (temperature != NULL) {
      // widget_add_timer(temperature, on_temperature_update_timer, 1000);
    }

    widget_t* hour_weather = widget_lookup(win, "hour_weather", TRUE);
    if (hour_weather != NULL) {
      widget_on(hour_weather, EVT_CHANGE_SELECT, on_change_select, hour_weather);
    }

    return RET_OK;
  }
  return RET_FAIL;
}
