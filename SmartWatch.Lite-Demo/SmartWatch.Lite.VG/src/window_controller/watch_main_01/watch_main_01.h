#ifndef WATCH_MAIN_01_H
#define WATCH_MAIN_01_H

#include "awtk.h"
#include "../controller_base.h"

BEGIN_C_DECLS

void *watch_main_01_create();

void watch_main_01_init(void *handle, widget_t *load_widget);

void watch_main_01_close(void *handle);

void watch_main_01_anim_end(void *handle);

END_C_DECLS

#endif /*WATCH_MAIN_01_H*/
