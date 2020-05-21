#include "tkc/mem.h"
#include "base/timer.h"
#include "tkc/utils.h"
#include "calendar_month.h"
#include "base/widget_vtable.h"
#include "widget_animators/widget_animator_scroll.h"

#define WEEK_DAY_NUMBER 7

static void calendar_month_get_data(calendar_month_t *calendar_month,
                                    int32_t *year, int32_t *month,
                                    int32_t *day) {
  *day = -1;
  if (*month <= 0) {
    if (*year > 0) {
      *month = 12;
      *year = *year - 1;
    } else {
      *month = 0;
    }
  }
  if (*month > 12) {
    *month = 1;
    *year = *year + 1;
  }
  if (*year < 0) {
    *year = 0;
  }

  if (*month == calendar_month->curr_month &&
      *year == calendar_month->curr_year) {
    *day = calendar_month->curr_day;
  }
}

static ret_t calendar_month_get_prop(widget_t *widget, const char *name,
                                     value_t *v) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(widget != NULL && calendar_month != NULL &&
                           name != NULL && v != NULL,
                       RET_BAD_PARAMS);

  if (tk_str_eq(name, CALENDAR_MONTH_PROP_STRING_MONDAY)) {
    value_set_str(v, calendar_month->str_Monday);
    return RET_OK;
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_STRING_TUESDAY)) {
    value_set_str(v, calendar_month->str_Tuesday);
    return RET_OK;
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_STRING_WEDNESDAY)) {
    value_set_str(v, calendar_month->str_Wednesday);
    return RET_OK;
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_STRING_THURSDAY)) {
    value_set_str(v, calendar_month->str_Thursday);
    return RET_OK;
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_STRING_FRIDAY)) {
    value_set_str(v, calendar_month->str_Friday);
    return RET_OK;
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_STRING_SATURDAY)) {
    value_set_str(v, calendar_month->str_Saturday);
    return RET_OK;
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_STRING_SUNDAY)) {
    value_set_str(v, calendar_month->str_Sunday);
    return RET_OK;
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_FONT_NAME)) {
    value_set_str(v, calendar_month->font_name);
    return RET_OK;
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_FONT_SIZE)) {
    value_set_int32(v, calendar_month->font_size);
    return RET_OK;
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_ANIMATING_TIME)) {
    value_set_uint32(v, calendar_month->animating_time);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_XOFFSET)) {
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_YOFFSET)) {
    value_set_int(v, calendar_month->yoffset);
    return RET_OK;
  }
  return RET_NOT_FOUND;
}

static ret_t calendar_month_set_prop(widget_t *widget, const char *name,
                                     const value_t *v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL,
                       RET_BAD_PARAMS);

  if (tk_str_eq(name, CALENDAR_MONTH_PROP_STRING_MONDAY)) {
    return calendar_month_set_Monday_string(widget, value_str(v));
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_STRING_TUESDAY)) {
    return calendar_month_set_Tuesday_string(widget, value_str(v));
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_STRING_WEDNESDAY)) {
    return calendar_month_set_Wednesday_string(widget, value_str(v));
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_STRING_THURSDAY)) {
    return calendar_month_set_Thursday_string(widget, value_str(v));
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_STRING_FRIDAY)) {
    return calendar_month_set_Friday_string(widget, value_str(v));
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_STRING_SATURDAY)) {
    return calendar_month_set_Saturday_string(widget, value_str(v));
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_STRING_SUNDAY)) {
    return calendar_month_set_Sunday_string(widget, value_str(v));
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_FONT_NAME)) {
    return calendar_month_set_font_name(widget, value_str(v));
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_FONT_SIZE)) {
    return calendar_month_set_font_size(widget, value_int32(v));
  } else if (tk_str_eq(name, CALENDAR_MONTH_PROP_ANIMATING_TIME)) {
    return calendar_month_set_animating_time(widget, value_uint32(v));
  } else if (tk_str_eq(name, WIDGET_PROP_XOFFSET)) {
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_YOFFSET)) {
    calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
    calendar_month->yoffset = value_int(v);
    widget_invalidate(widget, NULL);
    return RET_OK;
  }
  return RET_NOT_FOUND;
}

static int32_t calendar_month_get_week_number_for_month_first(uint32_t year,
                                                              uint32_t month) {
  uint32_t y = year;
  uint32_t m = month;

  if (m == 1 || m == 2) {
    year--;
    if (year <= 0) {
      assert(!"not find year number");
    }
    m += 12;
  }
  /* 这里加一是为了对称周日在第一的位置 */
  return ((1 + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) + 1) %
         7;
}

static bool_t calendar_month_is_leap_year(uint32_t year) {
  if (year % 100 == 0) {
    if (year % 400 == 0) {
      return TRUE;
    }
  } else {
    if (year % 4 == 0) {
      return TRUE;
    }
  }
  return FALSE;
}

static ret_t calendar_month_darw_one_month(calendar_month_t *calendar_month,
                                           canvas_t *c, int32_t day,
                                           int32_t week_number,
                                           int32_t max_day) {
  int32_t x = 0;
  int32_t y = 0;
  int32_t w = 0;
  int32_t h = 0;
  int32_t number = 1;
  char *week_list[7];
  char str_number[3];
  ret_t ret = RET_FAIL;
  rect_t r = rect_init(0, 0, 0, 0);

  w = calendar_month->widget.w / WEEK_DAY_NUMBER;
  h = calendar_month->widget.h / WEEK_DAY_NUMBER;

  r.w = w;
  r.h = h;

  week_list[0] = calendar_month->str_Sunday;
  week_list[1] = calendar_month->str_Monday;
  week_list[2] = calendar_month->str_Tuesday;
  week_list[3] = calendar_month->str_Wednesday;
  week_list[4] = calendar_month->str_Thursday;
  week_list[5] = calendar_month->str_Friday;
  week_list[6] = calendar_month->str_Saturday;

  canvas_set_text_align(c, ALIGN_H_CENTER, ALIGN_V_MIDDLE);
  canvas_set_fill_color(c, color_init(0x00, 0x00, 0xff, 0xff));
  canvas_set_font(c, calendar_month->font_name, calendar_month->font_size);

  for (x = 0; x < WEEK_DAY_NUMBER; x++) {
    r.x = w * x;
    r.y = h * y;
    if (x == 0 || x == 6) {
      canvas_set_text_color(c, color_init(0xff, 0x00, 0x00, 0xff));
    } else {
      canvas_set_text_color(c, color_init(0xff, 0xff, 0xff, 0xff));
    }
    ret = canvas_draw_utf8_in_rect(c, week_list[x], &r);
  }

  x = week_number;
  for (y = 1; y < WEEK_DAY_NUMBER; y++) {
    for (; x < WEEK_DAY_NUMBER; x++) {
      r.x = w * x;
      r.y = h * y;
      tk_itoa(str_number, sizeof(str_number), number);

      if (number == day) {
        canvas_fill_rect(c, r.x, r.y, r.w, r.h);
      }
      if (x == 0 || x == 6) {
        canvas_set_text_color(c, color_init(0xff, 0x00, 0x00, 0xff));
      } else {
        canvas_set_text_color(c, color_init(0xff, 0xff, 0xff, 0xff));
      }

      ret = canvas_draw_utf8_in_rect(c, str_number, &r);
      number++;
      if (number > max_day) {
        break;
      }
    }
    x = 0;
    if (number > max_day) {
      break;
    }
  }

  return ret;
}

static ret_t calendar_month_darw_month(calendar_month_t *calendar_month,
                                       canvas_t *c, int32_t year, int32_t month,
                                       int32_t day, int32_t yoffset) {
  ret_t ret = RET_OK;
  int32_t max_day = 30;
  int32_t week_number =
      calendar_month_get_week_number_for_month_first(year, month);

  if (month == 2) {
    if (calendar_month_is_leap_year(year)) {
      max_day = 29;
    } else {
      max_day = 28;
    }
  } else if (month == 1 || month == 3 || month == 5 || month == 7 ||
             month == 8 || month == 10 || month == 12) {
    max_day = 31;
  }
  canvas_translate(c, 0, yoffset);
  ret = calendar_month_darw_one_month(calendar_month, c, day, week_number,
                                      max_day);
  canvas_untranslate(c, 0, yoffset);

  return ret;
}

static ret_t calendar_month_on_paint_self(widget_t *widget, canvas_t *c) {
  rect_t r;
  rect_t r_save;
  int32_t day = 0;
  int32_t year = 0;
  int32_t month = 0;
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(calendar_month != NULL, RET_BAD_PARAMS);

  day = calendar_month->day;
  year = calendar_month->year;
  month = calendar_month->month;

  canvas_get_clip_rect(c, &r_save);

  canvas_save(c);
  r = rect_init(c->ox, c->oy, widget->w, widget->h);
  r = rect_intersect(&r, &r_save);
  canvas_set_clip_rect(c, &r);

  if (calendar_month->yoffset == 0) {
    calendar_month_darw_month(calendar_month, c, year, month, day, 0);
  } else if (calendar_month->yoffset > 0) {
    calendar_month_darw_month(calendar_month, c, year, month, day,
                              -widget->h + calendar_month->yoffset);
    month--;
    calendar_month_get_data(calendar_month, &year, &month, &day);
    calendar_month_darw_month(calendar_month, c, year, month, day,
                              calendar_month->yoffset);
  } else {
    calendar_month_darw_month(calendar_month, c, year, month, day,
                              widget->h + calendar_month->yoffset);
    month++;
    calendar_month_get_data(calendar_month, &year, &month, &day);
    calendar_month_darw_month(calendar_month, c, year, month, day,
                              calendar_month->yoffset);
  }

  canvas_set_clip_rect(c, &r_save);
  canvas_restore(c);

  return RET_OK;
}

static void calendar_month_update_data(calendar_month_t *calendar_month) {
  date_time_t dt;
  return_if_fail(calendar_month != NULL);
  if (date_time_init(&dt) != NULL) {
    calendar_month->curr_day = dt.day;
    calendar_month->curr_month = dt.month;
    calendar_month->curr_year = dt.year;
  }
}

static ret_t calendar_month_update_data_timer_func(const timer_info_t *timer) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(timer->ctx);
  return_value_if_fail(calendar_month != NULL, RET_REMOVE);
  calendar_month_update_data(calendar_month);
  return RET_REPEAT;
}

static ret_t calendar_month_on_destroy(widget_t *widget) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(widget != NULL && calendar_month != NULL,
                       RET_BAD_PARAMS);

  if (calendar_month->font_name != NULL) {
    TKMEM_FREE(calendar_month->font_name);
  }

  if (calendar_month->str_Monday != NULL) {
    TKMEM_FREE(calendar_month->str_Monday);
  }
  if (calendar_month->str_Tuesday != NULL) {
    TKMEM_FREE(calendar_month->str_Tuesday);
  }
  if (calendar_month->str_Wednesday != NULL) {
    TKMEM_FREE(calendar_month->str_Wednesday);
  }
  if (calendar_month->str_Thursday != NULL) {
    TKMEM_FREE(calendar_month->str_Thursday);
  }
  if (calendar_month->str_Friday != NULL) {
    TKMEM_FREE(calendar_month->str_Friday);
  }
  if (calendar_month->str_Saturday != NULL) {
    TKMEM_FREE(calendar_month->str_Saturday);
  }
  if (calendar_month->str_Sunday != NULL) {
    TKMEM_FREE(calendar_month->str_Sunday);
  }

  return RET_OK;
}

static const char *const s_calendar_month_properties[] = {
    CALENDAR_MONTH_PROP_STRING_MONDAY, CALENDAR_MONTH_PROP_STRING_TUESDAY,
    CALENDAR_MONTH_PROP_STRING_WEDNESDAY, CALENDAR_MONTH_PROP_STRING_THURSDAY,
    CALENDAR_MONTH_PROP_STRING_FRIDAY, CALENDAR_MONTH_PROP_STRING_SATURDAY,
    CALENDAR_MONTH_PROP_STRING_SUNDAY, CALENDAR_MONTH_PROP_FONT_SIZE,
    CALENDAR_MONTH_PROP_FONT_NAME, WIDGET_PROP_YOFFSET, WIDGET_PROP_XOFFSET,
    CALENDAR_MONTH_PROP_ANIMATING_TIME, NULL};

TK_DECL_VTABLE(calendar_month) = {
    .size = sizeof(calendar_month_t),
    .type = WIDGET_TYPE_CALENDAR_MONTH,
    .space_key_to_activate = TRUE,
    .return_key_to_activate = TRUE,
    .parent = TK_PARENT_VTABLE(widget),
    .create = calendar_month_create,
    .clone_properties = s_calendar_month_properties,
    .persistent_properties = s_calendar_month_properties,
    .set_prop = calendar_month_set_prop,
    .get_prop = calendar_month_get_prop,
    .on_destroy = calendar_month_on_destroy,
    .on_paint_self = calendar_month_on_paint_self};

widget_t *calendar_month_create(widget_t *parent, xy_t x, xy_t y, wh_t w,
                                wh_t h) {
  widget_t *widget =
      widget_create(parent, TK_REF_VTABLE(calendar_month), x, y, w, h);
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(calendar_month != NULL && widget != NULL, NULL);

  calendar_month->yoffset = 0;
  calendar_month->font_size = 18;
  calendar_month->font_name = NULL;

  calendar_month_update_data(calendar_month);
  widget_add_timer(widget, calendar_month_update_data_timer_func, 1000);

  calendar_month->day = calendar_month->curr_day;
  calendar_month->month = calendar_month->curr_month;
  calendar_month->year = calendar_month->curr_year;

  calendar_month->animating_time = TK_ANIMATING_TIME;
  return widget;
}

widget_t *calendar_month_cast(widget_t *widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, calendar_month), NULL);

  return widget;
}

ret_t calendar_month_set_Monday_string(widget_t *widget,
                                       const char *str_Monday) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(calendar_month != NULL && widget != NULL &&
                           str_Monday != NULL,
                       RET_BAD_PARAMS);

  calendar_month->str_Monday =
      tk_str_copy(calendar_month->str_Monday, str_Monday);

  return RET_OK;
}

ret_t calendar_month_set_Tuesday_string(widget_t *widget,
                                        const char *str_Tuesday) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(calendar_month != NULL && widget != NULL &&
                           str_Tuesday != NULL,
                       RET_BAD_PARAMS);

  calendar_month->str_Tuesday =
      tk_str_copy(calendar_month->str_Tuesday, str_Tuesday);

  return RET_OK;
}

ret_t calendar_month_set_Wednesday_string(widget_t *widget,
                                          const char *str_Wednesday) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(calendar_month != NULL && widget != NULL &&
                           str_Wednesday != NULL,
                       RET_BAD_PARAMS);

  calendar_month->str_Wednesday =
      tk_str_copy(calendar_month->str_Wednesday, str_Wednesday);

  return RET_OK;
}

ret_t calendar_month_set_Thursday_string(widget_t *widget,
                                         const char *str_Thursday) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(calendar_month != NULL && widget != NULL &&
                           str_Thursday != NULL,
                       RET_BAD_PARAMS);

  calendar_month->str_Thursday =
      tk_str_copy(calendar_month->str_Thursday, str_Thursday);

  return RET_OK;
}

ret_t calendar_month_set_Friday_string(widget_t *widget,
                                       const char *str_Friday) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(calendar_month != NULL && widget != NULL &&
                           str_Friday != NULL,
                       RET_BAD_PARAMS);

  calendar_month->str_Friday =
      tk_str_copy(calendar_month->str_Friday, str_Friday);

  return RET_OK;
}

ret_t calendar_month_set_Saturday_string(widget_t *widget,
                                         const char *str_Saturday) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(calendar_month != NULL && widget != NULL &&
                           str_Saturday != NULL,
                       RET_BAD_PARAMS);

  calendar_month->str_Saturday =
      tk_str_copy(calendar_month->str_Saturday, str_Saturday);

  return RET_OK;
}

ret_t calendar_month_set_Sunday_string(widget_t *widget,
                                       const char *str_Sunday) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(calendar_month != NULL && widget != NULL &&
                           str_Sunday != NULL,
                       RET_BAD_PARAMS);

  calendar_month->str_Sunday =
      tk_str_copy(calendar_month->str_Sunday, str_Sunday);

  return RET_OK;
}

ret_t calendar_month_set_font_name(widget_t *widget, const char *font_name) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(calendar_month != NULL && widget != NULL &&
                           font_name != NULL,
                       RET_BAD_PARAMS);

  calendar_month->font_name = tk_str_copy(calendar_month->font_name, font_name);

  return RET_OK;
}

ret_t calendar_month_set_font_size(widget_t *widget, int32_t font_size) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(calendar_month != NULL && widget != NULL,
                       RET_BAD_PARAMS);

  calendar_month->font_size = font_size;

  return RET_OK;
}

static ret_t calendar_month_on_scroll_done(void *ctx, event_t *e) {
  widget_t *widget = WIDGET(ctx);
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(widget != NULL && calendar_month != NULL,
                       RET_BAD_PARAMS);

  calendar_month->yoffset = 0;
  calendar_month->animating = FALSE;

  return RET_OK;
}

#ifdef WATCH_ANIMATOR
static ret_t calendar_month_move_animator(calendar_month_t *calendar_month,
                                          bool_t is_up) {
  widget_animator_t *animator = NULL;

  return_value_if_fail(!calendar_month->animating, RET_FAIL);

  animator = widget_animator_scroll_create(WIDGET(calendar_month),
                                           calendar_month->animating_time, 0,
                                           EASING_SIN_INOUT);

  if (is_up) {
    widget_animator_scroll_set_params(animator, 0, 0, 0,
                                      -calendar_month->widget.h);
  } else {
    widget_animator_scroll_set_params(animator, 0, 0, 0,
                                      calendar_month->widget.h);
  }

  calendar_month->animating = TRUE;
  widget_animator_on(animator, EVT_ANIM_END, calendar_month_on_scroll_done,
                     calendar_month);
  widget_animator_start(animator);

  return RET_OK;
}
#else
static ret_t calendar_month_move_animator(calendar_month_t *calendar_month,
                                          bool_t is_up) {
  if (is_up) {
    calendar_month->yoffset = -calendar_month->widget.h;
  } else {
    calendar_month->yoffset = calendar_month->widget.h;
  }
  calendar_month_on_scroll_done(calendar_month, NULL);
  return RET_OK;
}
#endif // WATCH_ANIMATOR

ret_t calendar_month_set_next_month(widget_t *widget) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(calendar_month != NULL && widget != NULL,
                       RET_BAD_PARAMS);
  return_value_if_fail(calendar_month->year >= 0, RET_FAIL);
  return_value_if_fail(!calendar_month->animating, RET_FAIL);

  calendar_month->month++;
  calendar_month_get_data(calendar_month, &(calendar_month->year),
                          &(calendar_month->month), &(calendar_month->day));

  calendar_month_move_animator(calendar_month, FALSE);
  return RET_OK;
}

ret_t calendar_month_set_prev_month(widget_t *widget) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);

  return_value_if_fail(calendar_month != NULL && widget != NULL,
                       RET_BAD_PARAMS);
  return_value_if_fail(calendar_month->year >= 0, RET_FAIL);
  return_value_if_fail(!calendar_month->animating, RET_FAIL);

  calendar_month->month--;
  calendar_month_get_data(calendar_month, &(calendar_month->year),
                          &(calendar_month->month), &(calendar_month->day));
  calendar_month_move_animator(calendar_month, TRUE);
  return RET_OK;
}

ret_t calendar_month_set_curr_month(widget_t *widget) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);

  return_value_if_fail(calendar_month != NULL && widget != NULL,
                       RET_BAD_PARAMS);
  return_value_if_fail(calendar_month->year >= 0, RET_FAIL);
  return_value_if_fail(!calendar_month->animating, RET_FAIL);

  calendar_month->day = calendar_month->curr_day;
  calendar_month->month = calendar_month->curr_month;
  calendar_month->year = calendar_month->curr_year;
  calendar_month_move_animator(calendar_month, FALSE);
  return RET_OK;
}

ret_t calendar_month_set_animating_time(widget_t *widget,
                                        uint32_t animating_time) {
  calendar_month_t *calendar_month = CALENDAR_MONTH(widget);
  return_value_if_fail(calendar_month != NULL && widget != NULL,
                       RET_BAD_PARAMS);

  calendar_month->animating_time = animating_time;

  return RET_OK;
}
