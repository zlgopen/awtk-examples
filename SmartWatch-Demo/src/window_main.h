#pragma once
#include <awtk.h>

typedef ret_t (*callback_t)(widget_t*, void* ctx);

void window_init(widget_t* window);
ret_t application_init(void);

ret_t on_set_window_anim_hint_slide_right(widget_t* window, void* ctx);

ret_t on_set_window_anim_hint_slide_down(widget_t* window, void* ctx);

widget_t* open_watch_and_close(const char* name, widget_t* window);
