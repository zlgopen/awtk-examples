#ifndef CALENDAR_MAIN_H
#define CALENDAR_MAIN_H

#include "awtk.h"
#include "../controller_base.h"

BEGIN_C_DECLS

typedef struct _calendar_main_t {
  controller_base_t base;

  widget_t *date_label;
  widget_t *calendar_month;
} calendar_main_t;

calendar_main_t *calendar_main_init(widget_t *gesture_manager,
                                    widget_t *window_anim_menu);

ret_t calendar_main_set_enable(calendar_main_t *handle, bool_t enable);

END_C_DECLS

#endif /*CALENDAR_MAIN_H*/
