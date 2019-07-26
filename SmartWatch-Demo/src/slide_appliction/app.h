#pragma once
#include <awtk.h>

ret_t on_child_visit(void* ctx, const void* data);

void app_init(widget_t* window, const char* open_anim);
ret_t open_app(void* ctx, event_t* e);
// 记住当前滑到项
void app_remember(widget_t* window);
