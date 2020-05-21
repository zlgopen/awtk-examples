#ifndef WATCH_MAIN_02_H
#define WATCH_MAIN_02_H

#include "awtk.h"
#include "../controller_base.h"

BEGIN_C_DECLS

void *watch_main_02_create();

void watch_main_02_init(void *handle, widget_t *load_widget);

void watch_main_02_close(void *handle);

void watch_main_02_anim_end(void *handle);

END_C_DECLS

#endif /*WATCH_MAIN_02_H*/
