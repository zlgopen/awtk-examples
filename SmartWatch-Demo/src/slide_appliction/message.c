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

static ret_t window_message_item_on_drag(void* ctx, event_t* e);
static ret_t window_message_item_on_drag_end(void* ctx, event_t* e);

static ret_t widget_delay_destroy(widget_t* widget) {
  if (widget) {
    widget_ref(widget);
    widget_destroy(widget);
    widget_unref(widget);
  }
  return RET_OK;
}

static ret_t window_message_init_dragger(widget_t* widget, wh_t w, wh_t h) {
  widget_t* dragger = dragger_create(widget, -w, -h, w, h);
  return_value_if_fail(dragger != NULL, RET_BAD_PARAMS);
  dragger_set_range(dragger, 0, 0, 0, 0);
  return RET_OK;
}

static ret_t window_message_deinit_dragger(widget_t* widget) {
  widget_t* win = widget_get_window(widget);
  widget_t* dragger = widget_lookup_by_type(win, WIDGET_TYPE_DRAGGER, FALSE);
  if (dragger) {
    widget_move(dragger, -win->w, -win->h);
    widget_off_by_func(dragger, EVT_DRAG, window_message_item_on_drag, widget);
    widget_off_by_func(dragger, EVT_DRAG_END, window_message_item_on_drag_end, widget);
  }
  return RET_OK;
}

static ret_t window_message_item_move_animate_end_destroy(void* ctx, event_t* e) {
  (void)e;
  widget_t* widget = WIDGET(ctx);
  widget_t* window = widget_get_window(widget);

  window_message_deinit_dragger(widget);

  // 延迟删除，由于关联widget的can_not_destroy计时器未加1，这里widget_destroy会直接销毁，从而引起动画异常
  widget_delay_destroy(widget);

  widget_invalidate(window, NULL);

  return RET_REMOVE;
}

static ret_t window_message_item_move_animate_end(void* ctx, event_t* e) {
  (void)e;
  widget_t* widget = WIDGET(ctx);
  widget_t* window = WIDGET(ctx);
  window_message_deinit_dragger(widget);
  widget_invalidate(window, NULL);
  return RET_REMOVE;
}

ret_t window_message_item_move_animator_start(widget_t* widget, uint32_t duration, xy_t x_from,
                                              xy_t x_to, bool_t destroy_end) {
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

static ret_t window_message_item_on_drag(void* ctx, event_t* e) {
  point_t p = {0, 0};
  dragger_t* dragger = DRAGGER(e->target);
  widget_t* widget = WIDGET(ctx);
  return_value_if_fail(widget != NULL && dragger != NULL, RET_BAD_PARAMS);

  widget_to_screen(WIDGET(dragger), &p);
  widget_to_local(widget->parent, &p);
  widget_move(widget, p.x, p.y);

  return RET_OK;
}

static ret_t window_message_item_on_drag_end(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  dragger_t* dragger = DRAGGER(e->target);
  return_value_if_fail(widget != NULL && dragger != NULL, RET_BAD_PARAMS);

  if (dragger->save_x - WIDGET(dragger)->x > WINDOW_MESSAGE_REMOVE_THRESHOLD) {
    window_message_item_move_animator_start(widget, 500, widget->x, -widget->w, TRUE);
  } else {
    window_message_item_move_animator_start(widget, 500, widget->x, 0, FALSE);
  }

  return RET_OK;
}

static ret_t window_message_item_dragger_start(widget_t* widget, xy_t x, xy_t y) {
  point_t p = {0, 0};
  pointer_event_t abort, down;
  widget_t* win = widget_get_window(widget);
  widget_t* dragger = widget_lookup_by_type(win, WIDGET_TYPE_DRAGGER, FALSE);
  return_value_if_fail(widget != NULL && win != NULL && dragger != NULL, RET_BAD_PARAMS);

  widget_to_screen(widget, &p);
  widget_to_local(win, &p);
  widget_move(dragger, p.x, p.y);
  dragger_set_range(dragger, -widget->w, p.y, widget->w, p.y);
  widget_on(dragger, EVT_DRAG, window_message_item_on_drag, widget);
  widget_on(dragger, EVT_DRAG_END, window_message_item_on_drag_end, widget);

  pointer_event_init(&abort, EVT_POINTER_DOWN_ABORT, win, x, y);
  widget_dispatch_event_to_target_recursive(win, (event_t*)(&abort));

  pointer_event_init(&down, EVT_POINTER_DOWN, dragger, x, y);
  widget_dispatch(WIDGET(dragger), (event_t*)(&down));

  return RET_OK;
}

static ret_t window_message_item_on_pointer_down(void* ctx, event_t* e) {
  (void)e;
  widget_t* widget = WIDGET(ctx);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);
  return widget_set_prop_bool(widget, LIST_ITEM_PROP_PRESSED, TRUE);
}

static ret_t window_message_item_on_pointer_down_abort(void* ctx, event_t* e) {
  (void)e;
  widget_t* widget = WIDGET(ctx);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);
  return widget_set_prop_bool(widget, LIST_ITEM_PROP_PRESSED, FALSE);
}

static ret_t window_message_item_on_pointer_move(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  list_item_t* list_item = LIST_ITEM(ctx);
  pointer_event_t* evt = (pointer_event_t*)e;
  bool_t pressed = FALSE;
  return_value_if_fail(list_item != NULL && evt != NULL, RET_BAD_PARAMS);

  pressed = widget_get_prop_bool(widget, LIST_ITEM_PROP_PRESSED, FALSE);
  if (pressed && !list_item->dragged) {
    int32_t dx = list_item->down.x - evt->x;
    if (evt->pressed && dx > TK_DRAG_THRESHOLD) {
      window_message_item_dragger_start(widget, evt->x, evt->y);
    }
  }

  return RET_OK;
}

static ret_t window_message_item_on_pointer_up(void* ctx, event_t* e) {
  (void)e;
  widget_t* widget = WIDGET(ctx);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);
  return widget_set_prop_bool(widget, LIST_ITEM_PROP_PRESSED, FALSE);
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
  window_message_init_dragger(win, 10, 10);

  widget_t* gesture = widget_lookup(win, "message_window", TRUE);
  widget_on(gesture, EVT_SLIDE_RIGHT, window_message_on_return, win);

  widget_t* view = widget_lookup(win, "view", TRUE);
  uint32_t nr = widget_count_children(view);
  uint32_t i;
  widget_t* child;
  for (i = 1; i < nr; i++) {
    child = widget_get_child(view, i);
    widget_set_prop_bool(child, LIST_ITEM_PROP_PRESSED, FALSE);
    widget_on(child, EVT_POINTER_DOWN, window_message_item_on_pointer_down, child);
    widget_on(child, EVT_POINTER_DOWN_ABORT, window_message_item_on_pointer_down_abort, child);
    widget_on(child, EVT_POINTER_MOVE, window_message_item_on_pointer_move, child);
    widget_on(child, EVT_POINTER_UP, window_message_item_on_pointer_up, child);
  }

  return RET_OK;
}
