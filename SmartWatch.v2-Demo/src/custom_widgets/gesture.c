

#include "gesture.h"
#include "base/timer.h"
#include "base/widget_vtable.h"
#include "../common/log.h"
#include "ext_widgets/slide_view/slide_view.h"
#include "slide_menu_v.h"
//#define LOG(_fmt, ...)
#ifdef _WIN32
#pragma warning(disable : 4244)
#endif  // _WIN32

static ret_t gesture_remove_timer(widget_t* widget) {
  gesture_t* gesture = GESTURE(widget);
  if (gesture->timer_id != TK_INVALID_ID) {
    timer_remove(gesture->timer_id);
    gesture->timer_id = TK_INVALID_ID;
  }

  return RET_OK;
}

static ret_t gesture_pointer_up_cleanup(widget_t* widget) {
  gesture_t* gesture = GESTURE(widget);

  gesture->pressed = FALSE;
  gesture_remove_timer(widget);
  widget_ungrab(widget->parent, widget);
  widget_set_state(widget, WIDGET_STATE_NORMAL);

  return RET_OK;
}

static ret_t gesture_on_long_press(const timer_info_t* info) {
  LOG("");
  pointer_event_t evt;
  widget_t* widget = WIDGET(info->ctx);
  gesture_t* gesture = GESTURE(widget);

  evt.x = 0;
  evt.y = 0;
  widget_t* window = widget_get_window(widget);

  if (gesture->pressed) {
    evt.e = event_init(EVT_LONG_PRESS, widget);
    LOG("loog press enable");

    gesture_pointer_up_cleanup(widget);
    widget_dispatch(widget, (event_t*)&evt);

    gesture->long_pressed = TRUE;

    event_t event = event_init(EVT_POINTER_DOWN_ABORT, window);
    widget_dispatch(window, &event);
  }

  return RET_REMOVE;
}

static widget_t* gesture_find_grabed_target(widget_t* widget) {
  widget_t* grabed = NULL;
  return_value_if_fail(widget != NULL, NULL);

  while (widget->grab_widget != NULL) {
    grabed = widget->grab_widget;
    widget = widget->grab_widget;
  }

  return grabed;
}

static ret_t gesture_on_event(widget_t* widget, event_t* e) {
  uint16_t type = e->type;
  gesture_t* gesture = GESTURE(widget);

  switch (type) {
    case EVT_POINTER_DOWN: {
      LOG("down");
      gesture->grad_widget = gesture_find_grabed_target(widget);
      gesture->child_graded = FALSE;
      gesture->pressed = TRUE;
      gesture->no_move = TRUE;
      pointer_event_t* pointer_event = (pointer_event_t*)e;
      LOG("down:(%d, %d)", pointer_event->x, pointer_event->y);
      gesture->press_point.x = pointer_event->x;
      gesture->press_point.y = pointer_event->y;
      gesture_remove_timer(widget);
      gesture->timer_id = widget_add_timer(widget, gesture_on_long_press, TK_LONG_PRESS_TIME);
      LOG("timer id:%d", gesture->timer_id);
      widget_grab(widget->parent, widget);
      break;
    }
    case EVT_POINTER_DOWN_ABORT: {
      LOG("down_abort");
      if (gesture->pressed) {
        LOG("EVT_POINTER_DOWN_ABORT");
        gesture_pointer_up_cleanup(widget);
      }
      break;
    }
    case EVT_POINTER_MOVE: {
      LOG("move");
      point_t* press_point = &(gesture->press_point);
      pointer_event_t* pointer_event = (pointer_event_t*)e;

      float_t distance = sqrt(pow(press_point->x - pointer_event->x, 2) +
                              pow(press_point->y - pointer_event->y, 2));
      LOG("distance:%f", distance);

      system_info_t* info = system_info();
      uint32_t lcd_w = info->lcd_w;
      float_t percent = 0;
      if (lcd_w == 800) {
        percent = 0.01F;
      } else if (lcd_w == 480) {
        percent = 0.05F;
      }

      if (gesture->pressed && distance > tk_min(widget->w, widget->h) * percent) {
        gesture_remove_timer(widget);
        gesture->no_move = FALSE;
      }
      break;
    }
    case EVT_POINTER_UP: {
      LOG("up");
      pointer_event_t* pointer_event = (pointer_event_t*)e;
      LOG("up:(%d, %d)", pointer_event->x, pointer_event->y);
      (void)pointer_event;
      if (gesture->child_graded) {
        gesture->long_pressed = FALSE;
        gesture_pointer_up_cleanup(widget);
        return RET_OK;
      }
      if (gesture->pressed) {
        pointer_event_t evt = *(pointer_event_t*)e;
        if (evt.x != gesture->press_point.x || evt.y != gesture->press_point.y) {
          float_t distance_x = gesture->press_point.x - evt.x;
          float_t distance_y = gesture->press_point.y - evt.y;
          distance_x = distance_x > 0 ? distance_x : -distance_x;
          distance_y = distance_y > 0 ? distance_y : -distance_y;
          if (distance_x > distance_y) {
            if (distance_x < 40 && distance_x < widget->w * 0.2f) {
              gesture_pointer_up_cleanup(widget);
              break;
            }
            if (evt.x > gesture->press_point.x) {
              log_debug("slide right\n");
              evt.e = event_init(EVT_SLIDE_RIGHT, widget);
              widget_dispatch(widget, (event_t*)&evt);
            } else {
              log_debug("slide left\n");
              evt.e = event_init(EVT_SLIDE_LEFT, widget);
              widget_dispatch(widget, (event_t*)&evt);
            }
          } else {
            if (distance_y < 40 && distance_y < widget->h * 0.2f) {
              gesture_pointer_up_cleanup(widget);
              break;
            }
            if (evt.y > gesture->press_point.y) {
              log_debug("slide down\n");
              evt.e = event_init(EVT_SLIDE_DOWN, widget);
              widget_dispatch(widget, (event_t*)&evt);
            } else {
              log_debug("slide up\n");
              evt.e = event_init(EVT_SLIDE_UP, widget);
              widget_dispatch(widget, (event_t*)&evt);
            }
          }
        }

        if (gesture->long_pressed) {
          gesture->long_pressed = FALSE;
          gesture_pointer_up_cleanup(widget);
          return RET_STOP;
        }
      }
      gesture_pointer_up_cleanup(widget);
    }
  }

  return RET_OK;
}

static ret_t gesture_on_destroy(widget_t* widget) {
  return gesture_remove_timer(widget);
}

static ret_t gesture_on_pointer_up_before_children(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  gesture_t* gesture = GESTURE(widget);
  widget_t* grad_widget = gesture_find_grabed_target(widget);

  if (gesture->grad_widget != NULL && gesture->grad_widget == grad_widget) {
    pointer_event_t* evt = (pointer_event_t*)e;
    bool_t point_in = widget_is_point_in(grad_widget, evt->x, evt->y, FALSE);

    if (!point_in) {
      return RET_OK;
    }

    if (!grad_widget->vt->inputable &&
        !tk_str_eq(widget_get_type(grad_widget), WIDGET_TYPE_BUTTON)) {
      return RET_OK;
    }

    if (tk_str_eq(widget_get_type(grad_widget), WIDGET_TYPE_SLIDE_VIEW)) {
      slide_view_t* slide_view = SLIDE_VIEW(grad_widget);
      if (!slide_view->dragged) {
        return RET_OK;
      }
    }

    if (tk_str_eq(widget_get_type(grad_widget), WIDGET_TYPE_SLIDE_MENU_V)) {
      slide_menu_v_t* slide_menu = SLIDE_MENU_V(grad_widget);
      if (!slide_menu->dragged) {
        return RET_OK;
      }
    }

    gesture->child_graded = TRUE;
  }

  return RET_OK;
}

static const widget_vtable_t s_gesture_vtable = {.size = sizeof(gesture_t),
                                                 .type = WIDGET_TYPE_GESTURE,
                                                 .create = gesture_create,
                                                 .on_paint_self = widget_on_paint_self_default,
                                                 .on_event = gesture_on_event,
                                                 .on_destroy = gesture_on_destroy};

widget_t* gesture_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  gesture_t* gesture = TKMEM_ZALLOC(gesture_t);
  widget_t* widget = WIDGET(gesture);
  return_value_if_fail(gesture != NULL, NULL);

  widget_init(widget, parent, &s_gesture_vtable, x, y, w, h);

  gesture->pressed = FALSE;
  gesture->enable_dt = TRUE;
  gesture->dt_flag = FALSE;
  gesture->dt_timer_id = TK_INVALID_ID;
  gesture->long_pressed = FALSE;

  widget_on(widget, EVT_POINTER_UP_BEFORE_CHILDREN, gesture_on_pointer_up_before_children, widget);

  return widget;
}
