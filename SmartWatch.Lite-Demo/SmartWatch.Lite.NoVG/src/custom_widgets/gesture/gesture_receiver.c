#include "gesture_receiver.h"
#include "base/window_manager.h"

static widget_t *gesture_receiver_find_target(widget_t *widget, xy_t x,
                                              xy_t y) {
  point_t p = {x, y};
  widget_t *iter = NULL;
  gesture_receiver_t *gesture_receiver = GESTURE_RECEIVER(widget);
  return_value_if_fail(widget != NULL && gesture_receiver != NULL, NULL);

  if (widget->grab_widget != NULL) {
    return widget->grab_widget;
  }

  iter = gesture_receiver->gesture;
  widget_to_local(window_manager(), &p);

  if (iter != NULL) {
    int32_t top = iter->y;
    int32_t left = iter->x;
    int32_t rigth = iter->x + iter->w;
    int32_t bottom = iter->y + iter->h;

    if (left <= p.x && p.x <= rigth && top <= p.y && p.y <= bottom) {
      return iter;
    }
  }
  return NULL;
}

TK_DECL_VTABLE(gesture_receiver) = {
    .size = sizeof(gesture_receiver_t),
    .type = WIDGET_TYPE_GESTURE_RECEIVER,
    .parent = TK_PARENT_VTABLE(widget),
    .create = gesture_receiver_create,
    .find_target = gesture_receiver_find_target};

ret_t gesture_receiver_idle_get_gesture_func(const idle_info_t *idle) {
  gesture_receiver_t *gesture_receiver = GESTURE_RECEIVER(idle->ctx);
  gesture_receiver->gesture = widget_lookup_by_type(
      gesture_receiver->widget.parent, WIDGET_TYPE_GESTURE, TRUE);

  return RET_OK;
}

widget_t *gesture_receiver_create(widget_t *parent, xy_t x, xy_t y, wh_t w,
                                  wh_t h) {
  widget_t *widget =
      widget_create(parent, TK_REF_VTABLE(gesture_receiver), x, y, w, h);
  gesture_receiver_t *gesture_receiver = GESTURE_RECEIVER(widget);

  gesture_receiver->gesture = NULL;

  return_value_if_fail(
      idle_add(gesture_receiver_idle_get_gesture_func, widget) > 0, NULL);

  return widget;
}

widget_t *gesture_receiver_cast(widget_t *widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, gesture_receiver), NULL);

  return widget;
}
