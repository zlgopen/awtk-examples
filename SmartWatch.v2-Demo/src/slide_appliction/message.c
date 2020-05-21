#include <awtk.h>
#include "../custom_widgets/gesture.h"
#include <widget_animators/widget_animator_move.h>
#include "../common/global.h"
#include "../window_main.h"
#include "../common/log.h"

#ifdef _WIN32
#pragma warning(disable : 4244)
#endif  // _WIN32

#define LIST_ITEM_PROP_PRESSED "pressed"
#define WINDOW_MESSAGE_REMOVE_THRESHOLD 50

static ret_t window_message_item_move_animate_end_destroy(void* ctx, event_t* e) {
  (void)e;
  widget_t* widget = WIDGET(ctx);
  widget_t* window = widget_get_window(widget);

  widget_destroy(widget->parent);
  widget_invalidate(window, NULL);

  return RET_REMOVE;
}

static ret_t window_message_item_move_animate_end(void* ctx, event_t* e) {
  (void)e;
  widget_t* widget = WIDGET(ctx);
  widget_t* window = widget_get_window(widget);

  widget_invalidate(window, NULL);

  return RET_REMOVE;
}

ret_t window_message_item_move_animator_start(widget_t* widget, uint32_t duration, xy_t x_from,
                                              xy_t x_to, bool_t destroy_end) {
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  widget_animator_t* wa = widget_animator_move_create(widget, duration, 0, EASING_SIN_INOUT);
  widget_animator_move_set_params(wa, x_from, 0, x_to, 0);
  if (destroy_end) {
    widget_animator_on(wa, EVT_ANIM_END, window_message_item_move_animate_end_destroy, widget);
  } else {
    widget_animator_on(wa, EVT_ANIM_END, window_message_item_move_animate_end, widget);
  }
  widget_animator_start(wa);

  return RET_OK;
}

static ret_t window_message_on_item_move(void* ctx, event_t* e) {
  (void)e;
  widget_t* widget = WIDGET(ctx);
  widget_set_prop_bool(widget, LIST_ITEM_PROP_PRESSED, TRUE);

  return RET_OK;
}

static ret_t window_message_item_on_pointer_down(void* ctx, event_t* e) {
  (void)e;
  widget_t* widget = WIDGET(ctx);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  return widget_set_prop_bool(widget, LIST_ITEM_PROP_PRESSED, FALSE);
}

static ret_t window_message_item_on_pointer_move(void* ctx, event_t* e) {
  (void)e;
  widget_t* widget = WIDGET(ctx);
  if (widget_get_prop_bool(widget, LIST_ITEM_PROP_PRESSED, FALSE)) {
    return RET_STOP;
  }

  return RET_OK;
}

static ret_t window_message_item_on_pointer_up(void* ctx, event_t* e) {
  pointer_event_t* evt = (pointer_event_t*)e;
  int32_t dx;
  draggable_t* draggable;
  bool_t pressed = FALSE;
  widget_t* widget = WIDGET(ctx);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  draggable = DRAGGABLE(widget_lookup_by_type(widget, WIDGET_TYPE_DRAGGABLE, TRUE));
  return_value_if_fail(draggable != NULL, RET_BAD_PARAMS);

  pressed = widget_get_prop_bool(widget, LIST_ITEM_PROP_PRESSED, FALSE);
  dx = draggable->down.x - evt->x;
  if (pressed && dx > WINDOW_MESSAGE_REMOVE_THRESHOLD) {
    window_message_item_move_animator_start(widget, 500, widget->x, -widget->w, TRUE);
  } else {
    window_message_item_move_animator_start(widget, 500, widget->x, 0, FALSE);
  }

  return widget_set_prop_bool(widget, LIST_ITEM_PROP_PRESSED, FALSE);
}

static ret_t init_widget(void* ctx, const void* iter) {
  (void)ctx;
  widget_t* widget = WIDGET(iter);

  const char* type = widget_get_type(widget);
  if (tk_str_eq(type, WIDGET_TYPE_LIST_ITEM)) {
    widget_t* view = widget_lookup_by_type(widget, WIDGET_TYPE_VIEW, TRUE);
    if (view != NULL) {
      widget_on(view, EVT_MOVE, window_message_on_item_move, view);
      widget_on(view, EVT_POINTER_DOWN, window_message_item_on_pointer_down, view);
      widget_on(view, EVT_POINTER_MOVE, window_message_item_on_pointer_move, view);
      widget_on(view, EVT_POINTER_UP, window_message_item_on_pointer_up, view);
    }
  }

  return RET_OK;
}

static void init_children_widget(widget_t* widget) {
  widget_foreach(widget, init_widget, widget);
}

static ret_t window_message_on_return(void* ctx, event_t* e) {
  (void)e;
  widget_t* win = WIDGET(ctx);
  widget_set_prop_str(win, WIDGET_PROP_CLOSE_ANIM_HINT, "slide_left");

  open_and_close(win);

  return RET_OK;
}

/**
 * 从表盘界面下滑打开消息界面
 */
ret_t open_message(void* ctx, event_t* e) {
  (void)e;
  widget_t* window = WIDGET(ctx);
  widget_t* win = window_open_and_close(g_window_enum->message.str, window);

  if (win) {
    widget_t* gesture = widget_lookup(win, "message_window", TRUE);
    widget_on(gesture, EVT_SLIDE_RIGHT, window_message_on_return, win);
    init_children_widget(win);
  }

  return RET_OK;
}
