#ifndef TRAINING_MAIN_H
#define TRAINING_MAIN_H

#include "awtk.h"
#include "../controller_base.h"

BEGIN_C_DECLS

typedef struct _training_main_t { controller_base_t base; } training_main_t;

training_main_t *training_main_init(widget_t *gesture_manager,
                                    widget_t *window_anim_menu);

ret_t training_main_set_enable(training_main_t *handle, bool_t enable);

END_C_DECLS

#endif /*TRAINING_MAIN_H*/
