#include "watch_04.h"
#include "../common/global.h"
#include "../window_main.h"
#include "../custom_widgets/health_circle.h"
#include "../common/log.h"
#include "../custom_widgets/gesture.h"

#ifdef _WIN32
#pragma warning(disable : 4244)
#endif  // _WIN32

ret_t on_open_calendar(void* ctx, event_t* e);
ret_t on_open_weather(void* ctx, event_t* e);
ret_t on_open_music(void* ctx, event_t* e);

static void update_watch_04(widget_t* window) {
  widget_t* week = widget_lookup(window, "week", TRUE);
  widget_t* day = widget_lookup(window, "day", TRUE);

  date_time_t dt;
  date_time_init(&dt);

  str_t str_wday;
  str_init(&str_wday, 0);

  switch (dt.wday) {
    case 0:
      str_append(&str_wday, "SUN");
      break;
    case 1:
      str_append(&str_wday, "MON");
      break;
    case 2:
      str_append(&str_wday, "TUES");
      break;
    case 3:
      str_append(&str_wday, "WED");
      break;
    case 4:
      str_append(&str_wday, "THUR");
      break;
    case 5:
      str_append(&str_wday, "FRI");
      break;
    case 6:
      str_append(&str_wday, "SAT");
      break;
    default:
      break;
  }

  LOG("%d %s", dt.wday, str_wday.str);

  widget_set_text_utf8(week, str_wday.str);
  str_reset(&str_wday);

  char str_day[3];
  memset(str_day, '\0', sizeof(str_day));
  tk_snprintf(str_day, sizeof(str_day), "%02d", dt.day);
  widget_set_text_utf8(day, str_day);
}

static ret_t on_watch_04_timer(const timer_info_t* timer) {
  widget_t* window = WIDGET(timer->ctx);

  update_watch_04(window);

  widget_t* icon_app_wechart = widget_lookup(window, "icon_app_wechart", TRUE);
  assert(icon_app_wechart);

  widget_t* icon_app_phone = widget_lookup(window, "icon_app_phone", TRUE);
  assert(icon_app_phone);

  widget_t* icon_app_run = widget_lookup(window, "icon_app_run", TRUE);
  assert(icon_app_run);

  widget_t* icon_app_training = widget_lookup(window, "icon_app_training", TRUE);
  assert(icon_app_training);

  widget_t* icon_app_heart = widget_lookup(window, "icon_app_heart", TRUE);
  assert(icon_app_heart);

  widget_use_style(icon_app_wechart, icon_app_wechart->name);
  widget_use_style(icon_app_phone, icon_app_phone->name);
  widget_use_style(icon_app_run, icon_app_run->name);
  widget_use_style(icon_app_training, icon_app_training->name);
  widget_use_style(icon_app_heart, icon_app_heart->name);

  date_time_t dt;
  date_time_init(&dt);

  str_t style;
  str_init(&style, 0);
  switch (dt.second % 5) {
    case 0: {
      str_append(&style, icon_app_wechart->name);
      str_append(&style, "_pressed");
      widget_use_style(icon_app_wechart, style.str);
      break;
    }
    case 1: {
      str_append(&style, icon_app_phone->name);
      str_append(&style, "_pressed");
      widget_use_style(icon_app_phone, style.str);
      break;
    }
    case 2: {
      str_append(&style, icon_app_run->name);
      str_append(&style, "_pressed");
      widget_use_style(icon_app_run, style.str);
      break;
    }
    case 3: {
      str_append(&style, icon_app_training->name);
      str_append(&style, "_pressed");
      widget_use_style(icon_app_training, style.str);
      break;
    }
    case 4: {
      str_append(&style, icon_app_heart->name);
      str_append(&style, "_pressed");
      widget_use_style(icon_app_heart, style.str);
      break;
    }
    default:
      break;
  }
  str_reset(&style);

  return RET_REPEAT;
}

static ret_t on_watch_04_music(const timer_info_t* timer) {
  LOG("");
  widget_t* widget = WIDGET(timer->ctx);
  music_state_t* music_state = &(global_data.music_state);
  LOG("play_state:%d", music_state->play_state);
  LOG("play_progress:%d", music_state->play_progress);
  health_circle_set_value_b(widget, music_state->play_progress);
  widget_invalidate(widget, NULL);
  return RET_REPEAT;
}

void watch_04_init(widget_t* window) {
  widget_t* gesture = widget_lookup(window, "gesture", TRUE);
  if (gesture != NULL) {
    global_data.watch_type = WATCH_04;
    window_init(window);
  }

  widget_t* watch_04_day = widget_lookup(window, "watch_04_day", TRUE);
  if (watch_04_day != NULL) {
    widget_on(watch_04_day, EVT_CLICK, on_open_calendar, window);
  }

  widget_t* watch_04_weather = widget_lookup(window, "watch_04_weather", TRUE);
  if (watch_04_weather != NULL) {
    widget_on(watch_04_weather, EVT_CLICK, on_open_weather, window);
  }

  widget_t* watch_04_music = widget_lookup(window, "watch_04_music", TRUE);
  if (watch_04_music != NULL) {
    widget_on(watch_04_music, EVT_CLICK, on_open_music, window);
  }

  update_watch_04(window);

  widget_add_timer(window, on_watch_04_timer, 1000);

  widget_t* music_circle = widget_lookup(window, "music_circle", TRUE);
  if (music_circle != NULL) {
    timer_info_t timer;
    timer.ctx = music_circle;
    on_watch_04_music(&timer);
    widget_add_timer(music_circle, on_watch_04_music, 1000);
  }
}
