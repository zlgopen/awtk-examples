#include "controller_base.h"

ret_t controller_base_init(controller_base_t *handle, widget_t *gesture_manager,
                           widget_t *window_anim_menu) {
  return_value_if_fail(handle != NULL && gesture_manager != NULL &&
                           window_anim_menu != NULL,
                       RET_OK);

  handle->enable = TRUE;
  handle->gesture_manager = gesture_manager;
  handle->window_anim_menu = window_anim_menu;

  return RET_OK;
}

ret_t controller_base_set_enable(controller_base_t *handle, bool_t enable) {
  return_value_if_fail(handle != NULL, RET_BAD_PARAMS);
  return_value_if_fail(handle->enable != enable, RET_OK);

  handle->enable = enable;

  return RET_OK;
}

ret_t controller_base_set_exit_func(controller_base_t *handle,
                                    controller_base_exit_func_t fun,
                                    void *ctx) {
  return_value_if_fail(handle != NULL && fun != NULL, RET_BAD_PARAMS);

  handle->exit_fun = fun;
  handle->exit_fun_ctx = ctx;

  return RET_OK;
}

void controller_base_send_exit_fun(controller_base_t *handle) {
  if (handle->exit_fun != NULL) {
    handle->exit_fun(handle, handle->exit_fun_ctx);
    handle->enable = FALSE;
  }
}

ret_t controller_base_dispose(controller_base_t *handle) {
  return_value_if_fail(handle != NULL, RET_BAD_PARAMS);
  TKMEM_FREE(handle);

  return RET_OK;
}
