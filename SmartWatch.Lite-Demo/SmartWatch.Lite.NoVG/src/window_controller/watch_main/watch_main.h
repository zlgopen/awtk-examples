#ifndef WATCH_MAIN_H
#define WATCH_MAIN_H

#include "awtk.h"
#include "../controller_base.h"

BEGIN_C_DECLS

typedef struct _watch_main_t {
  controller_base_t base;
  uint32_t window_index;
  bool_t is_open_message_window;

  int32_t pow;
  int32_t training;

  widget_t *pow_label;
  widget_t *training_label;

  widget_t *pow_button;
  widget_t *training_button;

  uint32_t timer_id;
} watch_main_t;

typedef ret_t (*watch_main_exit_func_t)(watch_main_t *handle, void *ctx);

watch_main_t *watch_main_init(widget_t *gesture_manager,
                              widget_t *window_anim_menu);

ret_t watch_main_set_enable(watch_main_t *handle, bool_t enable);

#define watch_main_dispose(handle)                                             \
  controller_base_dispose((controller_base_t *)handle)

END_C_DECLS

#endif /*WATCH_MAIN_H*/
