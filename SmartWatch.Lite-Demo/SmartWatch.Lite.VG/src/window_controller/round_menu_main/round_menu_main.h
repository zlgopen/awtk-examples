#ifndef ROUND_MENU_MAIN_H
#define ROUND_MENU_MAIN_H

#include "awtk.h"
#include "../controller_base.h"

BEGIN_C_DECLS

typedef struct _round_menu_main_t {
  controller_base_t base;
  uint32_t window_index;
} round_menu_main_t;

round_menu_main_t *round_menu_main_init(widget_t *gesture_manager,
                                        widget_t *window_anim_menu);

ret_t round_menu_main_set_enable(round_menu_main_t *handle, bool_t enable);

ret_t round_menu_main_dispose(round_menu_main_t *handle);

END_C_DECLS

#endif /*ROUND_MENU_MAIN_H*/
