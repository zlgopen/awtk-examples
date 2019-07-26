#include "quick.h"
#include <awtk.h>
#include "../custom_widgets/gesture.h"
#include "../common/global.h"
#include "../common/log.h"
#include "../window_main.h"

static ret_t on_return_window(void* ctx, event_t* e) {
  (void)e;
  widget_t* win = WIDGET(ctx);

  open_and_close(win);

  return RET_OK;
}

static ret_t init_quick(widget_t* win) {
  assert(win != NULL);

  widget_t* gesture = widget_lookup(win, "gesture", TRUE);
  assert(gesture != NULL);

  widget_on(gesture, EVT_SLIDE_DOWN, on_return_window, win);

  return RET_OK;
}

ret_t on_open_quick(void* ctx, event_t* e) {
  (void)e;
  widget_t* to_close = WIDGET(ctx);

  widget_t* win = window_open_and_close(g_window_enum->quick.str, to_close);
  if (win) {
    init_quick(win);
    return RET_OK;
  }

  return RET_FAIL;
}
