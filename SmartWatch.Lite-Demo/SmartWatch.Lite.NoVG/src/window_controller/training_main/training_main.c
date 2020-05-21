#include <stdio.h>
#include "training_main.h"
#include "../../custom_widgets/custom_widgets.h"

extern void controller_base_send_exit_fun(controller_base_t *handle);
extern ret_t controller_base_set_enable(controller_base_t *handle,
                                        bool_t enable);

static ret_t training_main_on_click_gesture_fun(gesture_type_t gesture_type,
                                                point_t *start_point,
                                                point_t *curr_point,
                                                void *ctx) {
  training_main_t *handle = (training_main_t *)(ctx);
  controller_base_t *base = (controller_base_t *)ctx;

  return_value_if_fail(base->enable, RET_BAD_PARAMS);

  if (gesture_type == GESTURE_LEFT_TO_RIGHT_TYPE) {
    training_main_set_enable(handle, FALSE);
    controller_base_send_exit_fun(base);
  }

  return RET_OK;
}

ret_t training_main_set_enable(training_main_t *handle, bool_t enable) {
  controller_base_t *base = (controller_base_t *)handle;
  return_value_if_fail(controller_base_set_enable(base, enable) == RET_OK,
                       RET_BAD_PARAMS);

  if (enable) {
    gesture_on_click_event(base->gesture_manager,
                           training_main_on_click_gesture_fun,
                           GESTURE_LEFT_TO_RIGHT_TYPE, handle);

    window_anim_menu_on_load_widget(base->window_anim_menu, NULL, NULL);
    window_anim_menu_on_dispose_widget(base->window_anim_menu, NULL, NULL);
  } else {
    return gesture_remove_all(base->gesture_manager);
  }
  return RET_OK;
}

training_main_t *training_main_init(widget_t *gesture_manager,
                                    widget_t *window_anim_menu) {
  training_main_t *handle = TKMEM_ZALLOC(training_main_t);

  return_value_if_fail(handle != NULL, NULL);

  controller_base_init((controller_base_t *)handle, gesture_manager,
                       window_anim_menu);

  handle->base.enable = FALSE;

  return handle;
}
