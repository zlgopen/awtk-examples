#include "calendar_main.h"
#include "../../custom_widgets/custom_widgets.h"

#define PREV_BUTTON "prev"
#define NEXT_BUTTON "next"
#define CURR_BUTTON "curr"
#define DATE_LABEL "date_label"

extern void controller_base_send_exit_fun(controller_base_t *handle);
extern ret_t controller_base_set_enable(controller_base_t *handle,
                                        bool_t enable);

static ret_t calendar_main_on_click_gesture_fun(gesture_type_t gesture_type,
                                                point_t *start_point,
                                                point_t *curr_point,
                                                void *ctx) {
  calendar_main_t *handle = (calendar_main_t *)(ctx);
  controller_base_t *base = (controller_base_t *)ctx;

  return_value_if_fail(base->enable, RET_BAD_PARAMS);

  if (gesture_type == GESTURE_LEFT_TO_RIGHT_TYPE) {
    calendar_main_set_enable(handle, FALSE);
    controller_base_send_exit_fun(base);
  }

  return RET_OK;
}

static ret_t calendar_main_set_date_label(widget_t *date_label, int32_t year,
                                          int32_t month, int32_t day) {
  char date[20];
  if (day > 0) {
    tk_snprintf(date, sizeof(date), "%4d/%02d/%02d", year, month, day);
  } else {
    tk_snprintf(date, sizeof(date), "%4d/%02d", year, month);
  }

  return widget_set_text_utf8(date_label, date);
}

static ret_t calendar_main_on_next_month(void *ctx, event_t *e) {
  ret_t ret = RET_OK;
  calendar_main_t *handle = (calendar_main_t *)ctx;
  calendar_month_t *calendar_month = CALENDAR_MONTH(handle->calendar_month);

  ret = calendar_month_set_next_month(handle->calendar_month);
  if (ret == RET_OK) {
    ret = calendar_main_set_date_label(handle->date_label, calendar_month->year,
                                       calendar_month->month,
                                       calendar_month->day);
  }

  return ret;
}

static ret_t calendar_main_on_prev_month(void *ctx, event_t *e) {
  ret_t ret = RET_OK;
  calendar_main_t *handle = (calendar_main_t *)ctx;
  calendar_month_t *calendar_month = CALENDAR_MONTH(handle->calendar_month);

  ret = calendar_month_set_prev_month(handle->calendar_month);
  if (ret == RET_OK) {
    ret = calendar_main_set_date_label(handle->date_label, calendar_month->year,
                                       calendar_month->month,
                                       calendar_month->day);
  }

  return ret;
}

static ret_t calendar_main_on_curr_month(void *ctx, event_t *e) {
  ret_t ret = RET_OK;
  calendar_main_t *handle = (calendar_main_t *)ctx;
  calendar_month_t *calendar_month = CALENDAR_MONTH(handle->calendar_month);

  ret = calendar_month_set_curr_month(handle->calendar_month);
  if (ret == RET_OK) {
    ret = calendar_main_set_date_label(handle->date_label, calendar_month->year,
                                       calendar_month->month,
                                       calendar_month->day);
  }

  return ret;
}

static ret_t
calendar_main_on_window_anim_load_widget_func(widget_t *load_widget,
                                              void *ctx) {
  calendar_month_t *calendar_month = NULL;
  calendar_main_t *handle = (calendar_main_t *)ctx;
  widget_t *prev_button = widget_lookup(load_widget, PREV_BUTTON, TRUE);
  widget_t *curr_button = widget_lookup(load_widget, CURR_BUTTON, TRUE);
  widget_t *next_button = widget_lookup(load_widget, NEXT_BUTTON, TRUE);

  return_value_if_fail(prev_button != NULL && curr_button != NULL &&
                           next_button != NULL,
                       RET_FAIL);

  handle->date_label = widget_lookup(load_widget, DATE_LABEL, TRUE);
  handle->calendar_month =
      widget_lookup_by_type(load_widget, WIDGET_TYPE_CALENDAR_MONTH, TRUE);

  return_value_if_fail(
      handle->date_label != NULL && handle->calendar_month != NULL, RET_FAIL);

  calendar_month = CALENDAR_MONTH(handle->calendar_month);

  calendar_main_set_date_label(handle->date_label, calendar_month->year,
                               calendar_month->month, calendar_month->day);
  widget_on(prev_button, EVT_CLICK, calendar_main_on_prev_month, handle);
  widget_on(curr_button, EVT_CLICK, calendar_main_on_curr_month, handle);
  widget_on(next_button, EVT_CLICK, calendar_main_on_next_month, handle);

  return RET_OK;
}

ret_t calendar_main_set_enable(calendar_main_t *handle, bool_t enable) {
  controller_base_t *base = (controller_base_t *)handle;
  return_value_if_fail(controller_base_set_enable(base, enable) == RET_OK,
                       RET_BAD_PARAMS);

  if (enable) {
    gesture_on_click_event(base->gesture_manager,
                           calendar_main_on_click_gesture_fun,
                           GESTURE_LEFT_TO_RIGHT_TYPE, handle);

    window_anim_menu_on_load_widget(
        base->window_anim_menu, calendar_main_on_window_anim_load_widget_func,
        handle);
    window_anim_menu_on_dispose_widget(base->window_anim_menu, NULL, NULL);
  } else {
    return gesture_remove_all(base->gesture_manager);
  }
  return RET_OK;
}

calendar_main_t *calendar_main_init(widget_t *gesture_manager,
                                    widget_t *window_anim_menu) {
  calendar_main_t *handle = TKMEM_ZALLOC(calendar_main_t);

  return_value_if_fail(handle != NULL, NULL);

  controller_base_init((controller_base_t *)handle, gesture_manager,
                       window_anim_menu);

  handle->base.enable = FALSE;

  return handle;
}
