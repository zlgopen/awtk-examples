#include "watch_main.h"
#include "../../custom_widgets/music_play_button/music_play_button.h"

#define POW_BUTTON "pow_button"
#define TRAINING_BUTTON "training_button"

#define POW_LABEL "pow_label"
#define WEATHER_LABEL "weather_label"
#define TRAINING_LABEL "training_label"

#define UPDATE_TIME 1000

const static char *watch_window_name[] = {UI_WATCH_MAIN_01};

extern void controller_base_send_exit_fun(controller_base_t *handle);

extern ret_t controller_base_set_enable(controller_base_t *handle,
                                        bool_t enable);

void watch_main_switch_window(watch_main_t *handle) {
  uint32_t len = ARRAY_LEN(watch_window_name);

  window_anim_menu_t *window_anim_menu =
      WINDOW_ANIM_MENU(handle->base.window_anim_menu);

  if (window_anim_menu->animating) {
    return;
  }

  if (len <= 1) {
    return;
  }

  handle->window_index++;
  if (len <= handle->window_index) {
    handle->window_index = 0;
  }

  window_anim_menu_play_anim_move_child_for_load(
      handle->base.window_anim_menu, watch_window_name[handle->window_index],
      MENU_ANIMATING_MOVE_TYPE_LETF, FALSE);
}

static ret_t watch_main_on_click_gesture_fun(gesture_type_t gesture_type,
                                             point_t *start_point,
                                             point_t *curr_point, void *ctx) {
  watch_main_t *handle = (watch_main_t *)(ctx);
  controller_base_t *base = (controller_base_t *)ctx;

  return_value_if_fail(base->enable, RET_BAD_PARAMS);

  if (gesture_type == GESTURE_LEFT_TO_RIGHT_TYPE &&
      !handle->is_open_message_window) {
    watch_main_switch_window(handle);
  } else if (gesture_type == GESTURE_RIGHT_TO_LEFT_TYPE &&
             !handle->is_open_message_window) {
    watch_main_set_enable(handle, FALSE);
    controller_base_send_exit_fun(base);
  } else if (gesture_type == GESTURE_TOP_TO_BOTTOM_TYPE &&
             !handle->is_open_message_window) {
    handle->is_open_message_window = TRUE;
    window_anim_menu_play_anim_move_child_for_load(
        handle->base.window_anim_menu, UI_WATCH_MAIN_MESSAGE,
        MENU_ANIMATING_MOVE_TYPE_TOP, FALSE);
  } else if (gesture_type == GESTURE_BOTTOM_TO_TOP_TYPE &&
             handle->is_open_message_window) {
    handle->is_open_message_window = FALSE;
    window_anim_menu_play_anim_move_child_for_load(
        handle->base.window_anim_menu, watch_window_name[handle->window_index],
        MENU_ANIMATING_MOVE_TYPE_TOP, TRUE);
  }

  return RET_OK;
}

static void watch_main_set_gesture(watch_main_t *handle) {
  controller_base_t *base = (controller_base_t *)handle;

  gesture_on_click_event(base->gesture_manager, watch_main_on_click_gesture_fun,
                         GESTURE_LEFT_TO_RIGHT_TYPE, handle);
  gesture_on_click_event(base->gesture_manager, watch_main_on_click_gesture_fun,
                         GESTURE_RIGHT_TO_LEFT_TYPE, handle);
  gesture_on_click_event(base->gesture_manager, watch_main_on_click_gesture_fun,
                         GESTURE_TOP_TO_BOTTOM_TYPE, handle);
  gesture_on_click_event(base->gesture_manager, watch_main_on_click_gesture_fun,
                         GESTURE_BOTTOM_TO_TOP_TYPE, handle);
}

static void watch_main_update_data(watch_main_t *handle) {
  char tmp[10];

  handle->pow += 5;
  if (handle->pow >= 100) {
    handle->pow = 0;
  }
  memset(tmp, 0, sizeof(tmp));
  tk_snprintf(tmp, 10, "%d", handle->pow);
  widget_set_text_utf8(handle->pow_label, tmp);
  music_play_set_value(handle->pow_button, handle->pow);

  handle->training += 20;
  if (handle->training >= 1000) {
    handle->training = 0;
  }
  memset(tmp, 0, sizeof(tmp));
  tk_snprintf(tmp, 10, "%d", handle->training);
  widget_set_text_utf8(handle->training_label, tmp);
  music_play_set_value(handle->training_button, handle->training);
}

static ret_t watch_main_on_update_data(const timer_info_t *timer) {
  watch_main_t *handle = (watch_main_t *)timer->ctx;
  if (handle->timer_id != TK_INVALID_ID) {
    watch_main_update_data(handle);
  }

  return RET_REPEAT;
}

static ret_t watch_main_widget_remove_timer_on_destroy(void *ctx, event_t *e) {
  watch_main_t *handle = (watch_main_t *)ctx;

  handle->pow_label = NULL;
  handle->pow_button = NULL;
  handle->training_button = NULL;
  handle->training_label = NULL;

  timer_remove(handle->timer_id);
  handle->timer_id = TK_INVALID_ID;

  return RET_REMOVE;
}

static ret_t watch_main_on_window_anim_load_widget_func(widget_t *load_widget,
                                                        void *ctx) {
  watch_main_t *handle = (watch_main_t *)ctx;
  if (load_widget->name != NULL &&
      tk_str_cmp(load_widget->name, UI_WATCH_MAIN_01) == 0) {
    handle->pow_button = widget_lookup(load_widget, POW_BUTTON, TRUE);
    handle->training_button = widget_lookup(load_widget, TRAINING_BUTTON, TRUE);
    handle->pow_label = widget_lookup(load_widget, POW_LABEL, TRUE);
    handle->training_label = widget_lookup(load_widget, TRAINING_LABEL, TRUE);
    return_value_if_fail(
        handle->pow_button != NULL && handle->training_button != NULL &&
            handle->pow_label != NULL && handle->training_label != NULL,
        RET_FAIL);

    handle->timer_id =
        timer_add(watch_main_on_update_data, handle, UPDATE_TIME);

    widget_on(load_widget, EVT_DESTROY,
              watch_main_widget_remove_timer_on_destroy, handle);

    watch_main_update_data(handle);
  }

  return RET_OK;
}

ret_t watch_main_set_enable(watch_main_t *handle, bool_t enable) {
  controller_base_t *base = (controller_base_t *)handle;
  return_value_if_fail(controller_base_set_enable(base, enable) == RET_OK,
                       RET_BAD_PARAMS);

  if (enable) {
    watch_main_set_gesture(handle);
    image_manager_unload_unused(image_manager(), 0);
    window_anim_menu_on_load_widget(handle->base.window_anim_menu,
                                    watch_main_on_window_anim_load_widget_func,
                                    handle);
    window_anim_menu_on_dispose_widget(handle->base.window_anim_menu, NULL,
                                       NULL);
    window_anim_menu_play_anim_move_child_for_load(
        handle->base.window_anim_menu, watch_window_name[handle->window_index],
        MENU_ANIMATING_MOVE_TYPE_RIGTH, TRUE);
  } else {
    return gesture_remove_all(base->gesture_manager);
  }

  return RET_OK;
}

static ret_t watch_main_on_idle_set_init_window(const idle_info_t *idle) {
  watch_main_t *handle = (watch_main_t *)(idle->ctx);

  window_anim_menu_on_load_widget(handle->base.window_anim_menu,
                                  watch_main_on_window_anim_load_widget_func,
                                  handle);
  window_anim_menu_on_dispose_widget(handle->base.window_anim_menu, NULL, NULL);
  window_anim_menu_set_curr_widget(handle->base.window_anim_menu,
                                   watch_window_name[handle->window_index]);

  watch_main_set_gesture(handle);

  return RET_OK;
}

watch_main_t *watch_main_init(widget_t *gesture_manager,
                              widget_t *window_anim_menu) {
  watch_main_t *handle = TKMEM_ZALLOC(watch_main_t);

  return_value_if_fail(handle != NULL, NULL);

  controller_base_init((controller_base_t *)handle, gesture_manager,
                       window_anim_menu);

  handle->window_index = 0;
  handle->is_open_message_window = FALSE;
  if (idle_add(watch_main_on_idle_set_init_window, handle) == 0) {
    TKMEM_FREE(handle);
    handle = NULL;
  }

  return handle;
}
