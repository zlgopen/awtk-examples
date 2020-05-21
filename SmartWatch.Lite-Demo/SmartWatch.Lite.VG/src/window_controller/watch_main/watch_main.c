#include "watch_main.h"
#include "../watch_main_01/watch_main_01.h"
#include "../watch_main_02/watch_main_02.h"

typedef void *(*watch_window_create_t)();
typedef void (*watch_window_init_t)(void *handle, widget_t *load_widget);
typedef void (*watch_window_close_t)(void *handle);
typedef void (*watch_window_anim_end_t)(void *handle);

typedef struct _watch_window_info_t {
  char *window_name;
  void *window_handle;

  watch_window_create_t create;
  watch_window_init_t init;
  watch_window_close_t close;
  watch_window_anim_end_t anim_end;
} watch_window_info_t;

extern void controller_base_send_exit_fun(controller_base_t *handle);

extern ret_t controller_base_set_enable(controller_base_t *handle,
                                        bool_t enable);

static watch_window_info_t watch_window_list[] = {
    {UI_WATCH_MAIN_01, NULL, watch_main_01_create, watch_main_01_init,
     watch_main_01_close, watch_main_01_anim_end},
    {UI_WATCH_MAIN_02, NULL, watch_main_02_create, watch_main_02_init,
     watch_main_02_close, watch_main_02_anim_end}};

void watch_main_switch_window(watch_main_t *handle) {
  uint32_t len = ARRAY_LEN(watch_window_list);

  window_anim_menu_t *window_anim_menu =
      WINDOW_ANIM_MENU(handle->base.window_anim_menu);

  if (window_anim_menu->animating) {
    return;
  }

  handle->window_index++;
  if (len <= handle->window_index) {
    handle->window_index = 0;
  }

  window_anim_menu_play_anim_move_child_for_load(
      handle->base.window_anim_menu,
      watch_window_list[handle->window_index].window_name,
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
        handle->base.window_anim_menu,
        watch_window_list[handle->window_index].window_name,
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

static ret_t watch_main_on_window_anim_load_widget_func(widget_t *load_widget,
                                                        void *ctx) {
  watch_main_t *handle = (watch_main_t *)ctx;
  if (handle != NULL && watch_window_list[handle->window_index].init != NULL &&
      !handle->is_open_message_window) {
    watch_window_list[handle->window_index].init(
        watch_window_list[handle->window_index].window_handle, load_widget);
  }

  return RET_OK;
}

static ret_t
watch_main_on_window_anim_dispose_widget_func(widget_t *load_widget,
                                              void *ctx) {
  watch_main_t *handle = (watch_main_t *)ctx;
  if (handle != NULL &&
      watch_window_list[handle->window_index].anim_end != NULL &&
      !handle->is_open_message_window) {
    watch_window_list[handle->window_index].anim_end(
        watch_window_list[handle->window_index].window_handle);
  }

  return RET_OK;
}

ret_t watch_main_set_enable(watch_main_t *handle, bool_t enable) {
  controller_base_t *base = (controller_base_t *)handle;
  return_value_if_fail(controller_base_set_enable(base, enable) == RET_OK,
                       RET_BAD_PARAMS);

  if (enable) {
    watch_main_set_gesture(handle);

    window_anim_menu_on_dispose_widget(
        handle->base.window_anim_menu,
        watch_main_on_window_anim_dispose_widget_func, handle);
    window_anim_menu_on_load_widget(handle->base.window_anim_menu,
                                    watch_main_on_window_anim_load_widget_func,
                                    handle);
    window_anim_menu_play_anim_move_child_for_load(
        handle->base.window_anim_menu,
        watch_window_list[handle->window_index].window_name,
        MENU_ANIMATING_MOVE_TYPE_RIGTH, TRUE);
  } else {
    return gesture_remove_all(base->gesture_manager);
  }

  return RET_OK;
}

static ret_t watch_main_on_idle_set_init_window(const idle_info_t *idle) {
  uint32_t i = 0;
  uint32_t len = ARRAY_LEN(watch_window_list);
  watch_main_t *handle = (watch_main_t *)(idle->ctx);

  return_value_if_fail(handle != NULL, RET_BAD_PARAMS);

  for (i = 0; i < len; i++) {
    if (watch_window_list[i].window_handle == NULL &&
        watch_window_list[i].create != NULL) {
      watch_window_list[i].window_handle = watch_window_list[i].create();
    }
  }

  window_anim_menu_on_dispose_widget(
      handle->base.window_anim_menu,
      watch_main_on_window_anim_dispose_widget_func, handle);
  window_anim_menu_on_load_widget(handle->base.window_anim_menu,
                                  watch_main_on_window_anim_load_widget_func,
                                  handle);
  window_anim_menu_set_curr_widget(
      handle->base.window_anim_menu,
      watch_window_list[handle->window_index].window_name);

  watch_main_on_window_anim_dispose_widget_func(NULL, handle);

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

ret_t watch_main_dispose(watch_main_t *handle) {
  uint32_t i = 0;
  uint32_t len = ARRAY_LEN(watch_window_list);

  return_value_if_fail(handle != NULL, RET_BAD_PARAMS);

  for (i = 0; i < len; i++) {
    if (watch_window_list[i].window_handle != NULL &&
        watch_window_list[i].close != NULL) {
      watch_window_list[i].close(watch_window_list[i].window_handle);
    }
  }

  TKMEM_FREE(handle);

  return RET_OK;
}
