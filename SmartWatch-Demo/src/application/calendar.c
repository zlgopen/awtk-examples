/**
 *
 *
 *
 */

#include <awtk.h>
#include "tkc/date_time.h"
#include "../custom_widgets/gesture.h"

#include "../common/animator.h"
#include "../common/global.h"
#include "../common/log.h"
#include "../common/global.h"
#include "../window_main.h"
#include "../slide_appliction/app.h"

static ret_t on_calendar_down(void* ctx, event_t* e);
static ret_t on_calendar_up(void* ctx, event_t* e);

static date_time_t date;                                                         // 当前时间
static date_time_t date_current;                                                 // 展示时间
static int day_of_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};  // 每个月的天数

static const char* s_calendar_labels = "calendar_labels";
static const char* s_calendar_date = "calendar_date";

/**
 * 计算日历
 */
static void calendar_cal(widget_t* widget, date_time_t* date_toshow) {
  if(date_toshow->year <= 0){
    date_toshow->year = 1;
  }
  if(date_toshow->month <= 0){
    date_toshow->month = 1;
  }
  if(date_toshow->day <= 0){
    date_toshow->day = 1;
  }
  // 更新年份/月份标签
  widget_t* label = widget_lookup(widget, s_calendar_date, TRUE);
  if (label) {
    char tmp[12] = {0};
    tk_snprintf(tmp, sizeof(tmp), "%4d/%02d", date_toshow->year, date_toshow->month);
    widget_set_text_utf8(label, tmp);
  }

  // 计算平闰年
  if (((date_toshow->year % 4 == 0) && date_toshow->year % 100 != 0) ||
      date_toshow->year % 400 == 0) {
    day_of_month[1] = 29;
  } else {
    day_of_month[1] = 28;
  }

  // 计算当月1日为星期几
  int32_t month = date_toshow->month;
  int32_t year = date_toshow->year;
  int32_t offset =
      (1 + 1 + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
  if (month == 1 || month == 2) {
    date_toshow->month += 12;
    date_toshow->year--;
    month = date_toshow->month;
    year = date_toshow->year;
    offset =
        (1 + 1 + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
    date_toshow->month -= 12;
    date_toshow->year++;
  }

  // 初始化日历标签
  widget_t* labels = widget_lookup(widget, s_calendar_labels, TRUE);
  if (labels) {
    WIDGET_FOR_EACH_CHILD_BEGIN(labels, iter, i)
    if (iter->name != NULL) {
      widget_set_text_utf8(iter, "");
    }
    widget_use_style(iter, "calendar");
    WIDGET_FOR_EACH_CHILD_END();
  }

  // 更改日历标签显示的日期
  char widget_name[16] = {0};
  char tmp_8[3] = {0};
  label = NULL;
  int i = 0;
  for (i = 0; i < day_of_month[date_toshow->month - 1]; i++) {
    memset(widget_name, 0, 16);
    tk_snprintf(widget_name, 16, "calendar_day%02d", i + offset);
    label = widget_lookup(widget, widget_name, TRUE);
    if (label != NULL) {
      tk_snprintf(tmp_8, 3, "%d", i + 1);
      widget_set_text_utf8(label, tmp_8);
      widget_use_style(label, "calendar");
      if (i + 1 == date_toshow->day && date_toshow->month == date.month &&
          date_toshow->year == date.year) {
        widget_use_style(label, "calendar_today");
      }
    }
  }
}

/**
 * 点击返回按钮关闭窗口
 */
static ret_t on_return(void* ctx, event_t* e) {
  (void)e;
  widget_t* win = WIDGET(ctx);

  open_and_close(win);

  return RET_OK;
}

static ret_t on_calendar_up_prev(void* ctx, event_t* e) {
  (void)e;
  widget_t* window = widget_get_window(WIDGET(ctx));
  widget_t* widget = widget_lookup(window, s_calendar_labels, TRUE);
  if (widget != NULL) {
    widget_t* calendar_date = widget_lookup(window, s_calendar_date, TRUE);
    wh_t h = widget->h;
    (void)h;
    if (calendar_date != NULL) {
      const char* name = "up";
      widget_create_animator_with(widget, "y(name=%s, from=%d, to=%d, duration=%d)", name,
                                  calendar_date->h, -window->h, 500);
      widget_animator_t* animator = widget_find_animator(widget, name);
      if (animator != NULL) {
        widget_animator_on(animator, EVT_ANIM_END, on_calendar_up, window);
      }
    }
  }
  widget_invalidate_force(window, NULL);
  return RET_OK;
}

ret_t on_widget_invalidate(void* ctx, event_t* e) {
  (void)e;
  widget_t* widget = WIDGET(ctx);
  widget_invalidate(widget, NULL);
  return RET_OK;
}

/**
 * 点击上翻按钮更新为上个月日历
 */
static ret_t on_calendar_up(void* ctx, event_t* e) {
  (void)e;
  if (date_current.month == 1) {
    date_current.month = 12;
    date_current.year--;
		if(date_current.year <= 0){
      date_current.year = 1;
    }
  } else {
    date_current.month -= 1;
		if(date_current.month <= 0){
      date_current.month = 1;
    }
  }

  widget_t* window = widget_get_window(WIDGET(ctx));
  calendar_cal(ctx, &date_current);

  widget_t* widget = widget_lookup(window, s_calendar_labels, TRUE);
  if (widget != NULL) {
    widget_t* calendar_date = widget_lookup(window, s_calendar_date, TRUE);
    if (calendar_date != NULL) {
      const char* name = "after_up";
      widget_create_animator_with(widget, "y(name=%s, from=%d, to=%d, duration=%d)", name,
                                  window->h, calendar_date->h, 500);
      widget_animator_t* animator = widget_find_animator(widget, name);
      if (animator != NULL) {
        widget_animator_on(animator, EVT_ANIM_END, on_widget_invalidate, window);
      }
    }
  }

  return RET_OK;
}

static ret_t on_calendar_down_prev(void* ctx, event_t* e) {
  (void)e;
  widget_t* window = widget_get_window(WIDGET(ctx));
  widget_t* widget = widget_lookup(window, s_calendar_labels, TRUE);
  if (widget != NULL) {
    widget_t* calendar_date = widget_lookup(window, s_calendar_date, TRUE);
    wh_t h = widget->h;
    (void)h;
    if (calendar_date != NULL) {
      const char* name = "down";
      widget_create_animator_with(widget, "y(name=%s, from=%d, to=%d, duration=%d)", name,
                                  widget->y - 10, window->h, 500);
      widget_animator_t* animator = widget_find_animator(widget, name);
      if (animator != NULL) {
        widget_animator_on(animator, EVT_ANIM_END, on_calendar_down, window);
      }
    }
  }
  widget_invalidate_force(window, NULL);
  return RET_OK;
}

/**
 * 点击下翻按钮更新为下个月日历
 */
static ret_t on_calendar_down(void* ctx, event_t* e) {
  (void)e;
  if (date_current.month == 12) {
    date_current.month = 1;
    date_current.year++;
  } else {
    date_current.month += 1;
  }
  calendar_cal(ctx, &date_current);

  widget_t* window = widget_get_window(WIDGET(ctx));
  widget_t* widget = widget_lookup(window, s_calendar_labels, TRUE);
  if (widget != NULL) {
    widget_t* calendar_date = widget_lookup(window, s_calendar_date, TRUE);
    wh_t h = widget->h;
    if (calendar_date != NULL) {
      const char* name = "after_down";
      widget_create_animator_with(widget, "y(name=%s,from=%d, to=%d, duration=%d)", name, -h,
                                  calendar_date->h, 500);
      widget_animator_t* animator = widget_find_animator(widget, name);
      if (animator != NULL) {
        widget_animator_on(animator, EVT_ANIM_END, on_widget_invalidate, window);
      }
    }
  }

  return RET_OK;
}

/**
 * 点击当前按钮更新日历为当前月日历
 */
static ret_t on_calendar_current(void* ctx, event_t* e) {
  (void)e;
  widget_t* win = WIDGET(ctx);
  (void)win;

  date_time_init(&date);
  date_time_init(&date_current);

  calendar_cal(ctx, &date);
  return RET_OK;
}

/**
 * 打开日历窗口
 */
ret_t on_open_calendar(void* ctx, event_t* e) {
  (void)e;
  widget_t* to_close = WIDGET(ctx);

  widget_t* win = window_open_and_close(g_window_enum->calendar.str, to_close);
  if (win != NULL) {
    // widget_on(win, EVT_WINDOW_CLOSE, on_window_close, to_close);

    on_calendar_current(win, NULL);
    // init button
    widget_t* buttons = widget_lookup(win, "calendar_ctrl", TRUE);
    if (buttons) {
      WIDGET_FOR_EACH_CHILD_BEGIN(buttons, iter, i)
      if (tk_str_eq(iter->name, "calendar:return")) {
        widget_on(iter, EVT_CLICK, on_return, win);
      } else if (tk_str_eq(iter->name, "calendar:up")) {
        // widget_on(iter, EVT_CLICK, on_calendar_up, win);
        widget_on(iter, EVT_CLICK, on_calendar_up_prev, win);
      } else if (tk_str_eq(iter->name, "calendar:current")) {
        widget_on(iter, EVT_CLICK, on_calendar_current, win);
      } else if (tk_str_eq(iter->name, "calendar:down")) {
        // widget_on(iter, EVT_CLICK, on_calendar_down, win);
        widget_on(iter, EVT_CLICK, on_calendar_down_prev, win);
      }
      WIDGET_FOR_EACH_CHILD_END();
    }

    // init gesture
    widget_t* gesture = widget_lookup(win, "calendar:gesture", TRUE);
    if (gesture) {
      // widget_on(gesture, EVT_SLIDE_DOWN, on_calendar_up, win);
      // widget_on(gesture, EVT_SLIDE_UP, on_calendar_down, win);
      widget_on(gesture, EVT_SLIDE_UP, on_calendar_up_prev, win);
      widget_on(gesture, EVT_SLIDE_DOWN, on_calendar_down_prev, win);
      widget_on(gesture, EVT_SLIDE_RIGHT, on_return, win);
    }

    return RET_OK;
  }

  return RET_FAIL;
}
