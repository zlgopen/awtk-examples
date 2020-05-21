#ifndef CONTROLLER_BASE_H
#define CONTROLLER_BASE_H

#include "awtk.h"
#include "../ui_main_list.h"
#include "../custom_widgets/custom_widgets.h"

#define ARRAY_LEN(a) sizeof(a) / sizeof(a[0])

BEGIN_C_DECLS

struct _controller_base_t;

typedef ret_t (*controller_base_exit_func_t)(struct _controller_base_t *handle,
                                             void *ctx);

typedef ret_t (*controller_base_set_enable_t)(struct _controller_base_t *handle,
                                              bool_t enable);

typedef struct _controller_base_t *(*controller_base_create_t)(
    widget_t *gesture_manager, widget_t *window_anim_menu);

typedef ret_t (*controller_base_dispose_t)(struct _controller_base_t *handle);

typedef struct _controller_base_t {
  bool_t enable;
  widget_t *gesture_manager;
  widget_t *window_anim_menu;
  void *exit_fun_ctx;
  controller_base_exit_func_t exit_fun;
} controller_base_t;

ret_t controller_base_init(controller_base_t *handle, widget_t *gesture_manager,
                           widget_t *window_anim_menu);

ret_t controller_base_set_exit_func(controller_base_t *handle,
                                    controller_base_exit_func_t fun, void *ctx);

ret_t controller_base_dispose(controller_base_t *handle);

END_C_DECLS

#endif /*CONTROLLER_BASE_H*/
