#include <stdio.h>
#include "weather_main.h"
#include "../../custom_widgets/custom_widgets.h"

#define AQI_MIN 10
#define AQI_MAX 255
#define TEMP_MIN 0
#define TEMP_MAX 40
#define AQI_LABEL "aqi"
#define NOW_MIN_TEMP_LABEL "now_min_temp"
#define NOW_MAX_TEMP_LABEL "now_max_temp"
#define TOM_MIN_TEMP_LABEL "tom_min_temp"
#define TOM_MAX_TEMP_LABEL "tom_max_temp"
#define WEATHER_DEGREE_LABEL "weather_degree"
#define AFTER_TOM_MIN_TEMP_LABEL "after_tom_min_temp"
#define AFTER_TOM_MAX_TEMP_LABEL "after_tom_max_temp"

#define WINDY_MIN 0
#define WINDY_MAX 10
#define HUMIDITY_MIN 0
#define HUMIDITY_MAX 10
#define PRESSURE_MIN 900
#define PRESSURE_MAX 1500
#define ULTRAVIOLET_RAY_MIN 0
#define ULTRAVIOLET_RAY_MAX 10
#define SOMATONSENSORY_TEMP_MIN 10
#define SOMATONSENSORY_TEMP_MAX 30
#define WINDY_LABEL "windy"
#define HUMIDITY_LABEL "humidity"
#define PRESSURE_LABEL "pressure"
#define ULTRAVIOLET_RAY_LABEL "ultraviolet_rays"
#define SOMATONSENSORY_TEMP_LABEL "somatonsensory_temp"

extern void controller_base_send_exit_fun(controller_base_t *handle);
extern ret_t controller_base_set_enable(controller_base_t *handle,
                                        bool_t enable);

static ret_t weather_main_on_click_gesture_fun(gesture_type_t gesture_type,
                                               point_t *start_point,
                                               point_t *curr_point, void *ctx) {
  weather_main_t *handle = (weather_main_t *)(ctx);
  controller_base_t *base = (controller_base_t *)ctx;

  return_value_if_fail(base->enable, RET_BAD_PARAMS);

  if (gesture_type == GESTURE_LEFT_TO_RIGHT_TYPE) {
    weather_main_set_enable(handle, FALSE);
    controller_base_send_exit_fun(base);
  }

  return RET_OK;
}

static bool_t weather_main_update_temp_ex(widget_t *min_label,
                                          widget_t *max_label, int32_t min,
                                          int32_t max, int32_t *new_min,
                                          int32_t *new_max) {

  char str_tmp[5];
  int32_t new_min_tmp = 0;
  int32_t new_max_tmp = 0;
  bool_t updated = FALSE;
  bool_t is_update = rand() % 2;

  if (is_update) {
    new_min_tmp = min + rand() % (max - min - 1);
    tk_itoa(str_tmp, sizeof(str_tmp), new_min_tmp);
    widget_set_text_utf8(min_label, str_tmp);
    updated = TRUE;
  } else {
    wstr_get_utf8(&(min_label->text), str_tmp, sizeof(str_tmp));
    new_min_tmp = tk_atoi(str_tmp);
  }

  is_update = rand() % 2;
  if (is_update || updated) {
    wstr_get_utf8(&(max_label->text), str_tmp, sizeof(str_tmp));
    new_max_tmp = tk_atoi(str_tmp);

    if ((new_max_tmp <= new_min_tmp && updated) || is_update) {
      new_max_tmp = new_min_tmp + 1 + rand() % (max - new_min_tmp + 1);

      tk_itoa(str_tmp, sizeof(str_tmp), new_max_tmp);
      widget_set_text_utf8(max_label, str_tmp);
      updated = TRUE;
    }
  } else {
    wstr_get_utf8(&(max_label->text), str_tmp, sizeof(str_tmp));
    new_max_tmp = tk_atoi(str_tmp);
  }

  *new_min = new_min_tmp;
  *new_max = new_max_tmp;

  return updated;
}

static int32_t weather_main_update_temp(widget_t *label, int32_t min,
                                        int32_t max) {
  char str_tmp[5];
  int32_t new_tmp = 0;
  bool_t is_update = rand() % 2;

  wstr_get_utf8(&(label->text), str_tmp, sizeof(str_tmp));
  new_tmp = tk_atoi(str_tmp);

  if (is_update || new_tmp < min || new_tmp > max) {
    new_tmp = min + rand() % (max - min);
    tk_itoa(str_tmp, sizeof(str_tmp), new_tmp);
    widget_set_text_utf8(label, str_tmp);
  }

  return new_tmp;
}

static ret_t weather_main_update_weather(weather_main_t *handle) {

  int32_t min = 0;
  int32_t max = 0;

  weather_main_update_temp(handle->aqi_label, AQI_MIN, AQI_MAX);

  weather_main_update_temp_ex(handle->now_min_temp_label,
                              handle->now_max_temp_label, TEMP_MIN, TEMP_MAX,
                              &min, &max);
  weather_main_update_temp(handle->weather_degree_label, min, max);

  weather_main_update_temp_ex(handle->tom_min_temp_label,
                              handle->tom_max_temp_label, TEMP_MIN, TEMP_MAX,
                              &min, &max);
  weather_main_update_temp_ex(handle->after_tom_min_temp_label,
                              handle->after_tom_max_temp_label, TEMP_MIN,
                              TEMP_MAX, &min, &max);
  return RET_OK;
}

static ret_t weather_main_on_timer_update_weather(const timer_info_t *timer) {
  weather_main_t *handle = (weather_main_t *)(timer->ctx);
  weather_main_update_weather(handle);

  return RET_REPEAT;
}

static ret_t weather_main_remove_timer_on_destroy(void *ctx, event_t *e) {

  weather_main_t *handle = (weather_main_t *)ctx;

  if (handle != NULL && handle->timer_id != 0) {
    timer_remove(handle->timer_id);
    handle->timer_id = 0;
  }
  return RET_REMOVE;
}

static ret_t weather_main_on_window_anim_load_widget_func(widget_t *load_widget,
                                                          void *ctx) {
  weather_main_t *handle = (weather_main_t *)ctx;
  controller_base_t *base = (controller_base_t *)handle;

  return_value_if_fail(handle != NULL && base != NULL, RET_FAIL);

  handle->aqi_label = widget_lookup(load_widget, AQI_LABEL, TRUE);
  handle->now_min_temp_label =
      widget_lookup(load_widget, NOW_MIN_TEMP_LABEL, TRUE);
  handle->now_max_temp_label =
      widget_lookup(load_widget, NOW_MAX_TEMP_LABEL, TRUE);
  handle->tom_min_temp_label =
      widget_lookup(load_widget, TOM_MIN_TEMP_LABEL, TRUE);
  handle->tom_max_temp_label =
      widget_lookup(load_widget, TOM_MAX_TEMP_LABEL, TRUE);
  handle->weather_degree_label =
      widget_lookup(load_widget, WEATHER_DEGREE_LABEL, TRUE);
  handle->after_tom_min_temp_label =
      widget_lookup(load_widget, AFTER_TOM_MIN_TEMP_LABEL, TRUE);
  handle->after_tom_max_temp_label =
      widget_lookup(load_widget, AFTER_TOM_MAX_TEMP_LABEL, TRUE);

  return_value_if_fail(handle->aqi_label != NULL, RET_FAIL);
  return_value_if_fail(handle->now_min_temp_label != NULL, RET_FAIL);
  return_value_if_fail(handle->now_max_temp_label != NULL, RET_FAIL);
  return_value_if_fail(handle->tom_min_temp_label != NULL, RET_FAIL);
  return_value_if_fail(handle->tom_max_temp_label != NULL, RET_FAIL);
  return_value_if_fail(handle->weather_degree_label != NULL, RET_FAIL);
  return_value_if_fail(handle->after_tom_min_temp_label != NULL, RET_FAIL);
  return_value_if_fail(handle->after_tom_max_temp_label != NULL, RET_FAIL);

  handle->timer_id =
      timer_add(weather_main_on_timer_update_weather, handle, 1000);
  widget_on(load_widget, EVT_DESTROY, weather_main_remove_timer_on_destroy,
            handle);

  weather_main_update_weather(handle);

  return RET_OK;
}

ret_t weather_main_set_enable(weather_main_t *handle, bool_t enable) {
  controller_base_t *base = (controller_base_t *)handle;
  return_value_if_fail(controller_base_set_enable(base, enable) == RET_OK,
                       RET_BAD_PARAMS);

  if (enable) {
    gesture_on_click_event(base->gesture_manager,
                           weather_main_on_click_gesture_fun,
                           GESTURE_LEFT_TO_RIGHT_TYPE, handle);
    image_manager_unload_unused(image_manager(), 0);
    window_anim_menu_on_load_widget(
        base->window_anim_menu, weather_main_on_window_anim_load_widget_func,
        handle);
    window_anim_menu_on_dispose_widget(base->window_anim_menu, NULL, NULL);
  } else {
    return gesture_remove_all(base->gesture_manager);
  }
  return RET_OK;
}

weather_main_t *weather_main_init(widget_t *gesture_manager,
                                  widget_t *window_anim_menu) {
  weather_main_t *handle = TKMEM_ZALLOC(weather_main_t);

  return_value_if_fail(handle != NULL, NULL);

  controller_base_init((controller_base_t *)handle, gesture_manager,
                       window_anim_menu);

  handle->timer_id = 0;
  handle->base.enable = FALSE;

  return handle;
}
