#include "tkc/mem.h"
#include "tkc/utils.h"
#include "base/timer.h"
#include "tkc/easing.h"
#include "gesture.h"
#include "widget_animators/widget_animator_scroll.h"

#define GESTURE_TRIGGER_DELAY 500

#define WIDGET_GESTURE_THRESHOLD 30

static ret_t gesture_on_paint_self(widget_t *widget, canvas_t *c) {
  (void)widget;
  (void)c;
  return RET_OK;
}

static ret_t gesture_on_gesture_trigger(const timer_info_t *info) {
  gesture_t *gesture = GESTURE(info->ctx);
  gesture->is_trigger = FALSE;
  return RET_REMOVE;
}

static ret_t gesture_on_visit_send_click_event(void *ctx, const void *data) {
  gesture_t *gesture = GESTURE(ctx);
  gesture_info_t *info = (gesture_info_t *)data;

  int32_t delta_x = gesture->curr_point.x - gesture->start_point.x;
  int32_t delta_y = gesture->curr_point.y - gesture->start_point.y;

  bool_t is_v = tk_abs(delta_y) > tk_abs(delta_x);
  return_value_if_fail(info->threshold > 0, RET_BAD_PARAMS);

  if (!gesture->is_trigger) {
    if (is_v) {
      if (tk_abs(delta_y) > info->threshold) {
        if (delta_y > 0) {
          if (info->gesture_type == GESTURE_TOP_TO_BOTTOM_TYPE) {
            if (info->fun != NULL) {
              info->fun(info->gesture_type, &gesture->start_point,
                        &gesture->curr_point, info->ctx);
            }
            gesture->is_trigger = TRUE;
          }
        } else if (delta_y < 0) {
          if (info->gesture_type == GESTURE_BOTTOM_TO_TOP_TYPE) {
            if (info->fun != NULL) {
              info->fun(info->gesture_type, &gesture->start_point,
                        &gesture->curr_point, info->ctx);
            }
            gesture->is_trigger = TRUE;
          }
        }
      }
    } else {
      if (tk_abs(delta_x) > info->threshold) {
        if (delta_x > 0) {
          if (info->gesture_type == GESTURE_LEFT_TO_RIGHT_TYPE) {
            if (info->fun != NULL) {
              info->fun(info->gesture_type, &gesture->start_point,
                        &gesture->curr_point, info->ctx);
            }
            gesture->is_trigger = TRUE;
          }
        } else if (delta_x < 0) {
          if (info->gesture_type == GESTURE_RIGHT_TO_LEFT_TYPE) {
            if (info->fun != NULL) {
              info->fun(info->gesture_type, &gesture->start_point,
                        &gesture->curr_point, info->ctx);
            }
            gesture->is_trigger = TRUE;
          }
        }
      }
    }

    if (gesture->is_trigger) {
      uint32_t timer_id = timer_add(gesture_on_gesture_trigger, gesture,
                                    gesture->trigger_delay_time);
      if (timer_id == TK_INVALID_ID) {
        gesture->is_trigger = FALSE;
      }
    }
  }

  return RET_OK;
}

static void gesture_on_gesture_event_send_click_event(gesture_t *gesture) {
  darray_foreach(gesture->gesture_fun_click_list,
                 gesture_on_visit_send_click_event, gesture);
}

static ret_t gesture_on_event(widget_t *widget, event_t *e) {
  ret_t ret = RET_OK;
  uint16_t type = e->type;
  gesture_t *gesture = GESTURE(widget);
  return_value_if_fail(widget != NULL && gesture != NULL, RET_BAD_PARAMS);

  switch (type) {
  case EVT_POINTER_DOWN: {
    pointer_event_t *evt = (pointer_event_t *)e;

    gesture->curr_point.x = evt->x;
    gesture->curr_point.y = evt->y;
    gesture->start_point.x = evt->x;
    gesture->start_point.y = evt->y;

    gesture->pressed = TRUE;
    widget_grab(widget->parent, widget);
    break;
  }
  case EVT_POINTER_UP: {
    if (gesture->pressed) {
      if (gesture->dragged) {
        pointer_event_t *evt = (pointer_event_t *)e;
        gesture->curr_point.x = evt->x;
        gesture->curr_point.y = evt->y;

        gesture_on_gesture_event_send_click_event(gesture);
      }
      gesture->dragged = FALSE;
      gesture->pressed = FALSE;
      widget_ungrab(widget->parent, widget);
    }
    break;
  }
  case EVT_POINTER_DOWN_ABORT: {
    if (gesture->pressed) {
      gesture->pressed = FALSE;
      gesture->dragged = FALSE;
      widget_ungrab(widget->parent, widget);
    }
    break;
  }
  case EVT_POINTER_MOVE: {
    pointer_event_t *evt = (pointer_event_t *)e;
    if (evt->pressed && gesture->pressed) {
      int32_t delta_x = evt->x - gesture->start_point.x;
      int32_t delta_y = evt->y - gesture->start_point.y;

      gesture->curr_point.x = evt->x;
      gesture->curr_point.y = evt->y;
      if (tk_abs(delta_x) > TK_DRAG_THRESHOLD ||
          tk_abs(delta_y) > TK_DRAG_THRESHOLD) {
        pointer_event_t abort;
        pointer_event_init(&abort, EVT_POINTER_DOWN_ABORT, widget, evt->x,
                           evt->y);
        widget_dispatch_event_to_target_recursive(widget, (event_t *)(&abort));
        gesture->dragged = TRUE;
      }
    }
    break;
  }
  default:
    break;
  }

  if (ret == RET_OK) {
    if (gesture->dragged) {
      ret = RET_STOP;
    }
  }

  return ret;
}

static ret_t gesture_get_prop(widget_t *widget, const char *name, value_t *v) {
  gesture_t *gesture = GESTURE(widget);
  return_value_if_fail(widget != NULL && name != NULL && v != NULL,
                       RET_BAD_PARAMS);

  return RET_NOT_FOUND;
}

static ret_t gesture_set_prop(widget_t *widget, const char *name,
                              const value_t *v) {
  gesture_t *gesture = GESTURE(widget);
  return_value_if_fail(widget != NULL && name != NULL && v != NULL,
                       RET_BAD_PARAMS);

  return RET_NOT_FOUND;
}

static ret_t gesture_on_destroy(widget_t *widget) {
  gesture_t *gesture = GESTURE(widget);
  return_value_if_fail(widget != NULL && gesture != NULL, RET_BAD_PARAMS);

  gesture_remove_all(widget);

  if (gesture->gesture_fun_click_list != NULL) {
    darray_destroy(gesture->gesture_fun_click_list);
    gesture->gesture_fun_click_list = NULL;
  }
  return RET_OK;
}

static const char *s_gesture_properties[] = {NULL};

TK_DECL_VTABLE(gesture) = {.size = sizeof(gesture_t),
                           .inputable = TRUE,
                           .type = WIDGET_TYPE_GESTURE,
                           .only_active_child_visible = TRUE,
                           .clone_properties = s_gesture_properties,
                           .persistent_properties = s_gesture_properties,
                           .parent = TK_PARENT_VTABLE(widget),
                           .create = gesture_create,
                           .on_event = gesture_on_event,
                           .get_prop = gesture_get_prop,
                           .set_prop = gesture_set_prop,
                           .on_destroy = gesture_on_destroy,
                           .on_paint_self = gesture_on_paint_self};
;

ret_t gesture_destroy_click_list(void *data) {
  TKMEM_FREE(data);
  return RET_OK;
}

widget_t *gesture_create(widget_t *parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t *widget = widget_create(parent, TK_REF_VTABLE(gesture), x, y, w, h);
  gesture_t *gesture = GESTURE(widget);
  return_value_if_fail(gesture != NULL, NULL);

  gesture->dragged = FALSE;
  gesture->pressed = FALSE;
  gesture->is_trigger = FALSE;
  gesture->trigger_delay_time = GESTURE_TRIGGER_DELAY;
  gesture->gesture_fun_click_list =
      darray_create(4, gesture_destroy_click_list, NULL);

  return widget;
}

widget_t *gesture_cast(widget_t *widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, gesture), NULL);

  return widget;
}

ret_t gesture_on_click_event_for_threshold(widget_t *widget,
                                           on_gesture_fun_t fun,
                                           gesture_type_t gesture_type,
                                           xy_t threshold, void *ctx) {
  gesture_info_t *gesture_info = NULL;
  gesture_t *gesture = GESTURE(widget);
  return_value_if_fail(gesture != NULL && fun != NULL, RET_BAD_PARAMS);

  gesture_info = TKMEM_ZALLOC(gesture_info_t);
  return_value_if_fail(gesture_info != NULL, RET_OOM);

  gesture_info->ctx = ctx;
  gesture_info->fun = fun;
  gesture_info->threshold = threshold;
  gesture_info->gesture_type = gesture_type;
  darray_push(gesture->gesture_fun_click_list, gesture_info);

  return RET_OK;
}

ret_t gesture_on_click_event(widget_t *widget, on_gesture_fun_t fun,
                             gesture_type_t gesture_type, void *ctx) {
  return gesture_on_click_event_for_threshold(widget, fun, gesture_type,
                                              WIDGET_GESTURE_THRESHOLD, ctx);
}

ret_t gesture_remove_all(widget_t *widget) {
  gesture_t *gesture = GESTURE(widget);
  return_value_if_fail(gesture != NULL, RET_BAD_PARAMS);

  darray_clear(gesture->gesture_fun_click_list);

  return RET_OK;
}
