/**
 *
 *
 */

#include "health_circle.h"
#include <tkc/color_parser.h>
#include "../common/log.h"

#ifdef _WIN32
#pragma warning(disable : 4244 4305)
#endif  // _WIN32

ret_t health_circle_set_value_b(widget_t* widget, float_t value) {
  health_circle_t* health_circle = HEALTH_CIRCLE(widget);
  return_value_if_fail(health_circle != NULL, RET_BAD_PARAMS);

  if (health_circle->value_b != value) {
    event_t e = event_init(EVT_VALUE_WILL_CHANGE, widget);
    widget_dispatch(widget, &e);
    health_circle->value_b = value;
    e = event_init(EVT_VALUE_CHANGED, widget);
    widget_dispatch(widget, &e);
    widget_invalidate(widget, NULL);
  }

  return RET_OK;
}

ret_t health_circle_set_value_m(widget_t* widget, float_t value) {
  health_circle_t* health_circle = HEALTH_CIRCLE(widget);
  return_value_if_fail(health_circle != NULL, RET_BAD_PARAMS);

  if (health_circle->value_m != value) {
    event_t e = event_init(EVT_VALUE_WILL_CHANGE, widget);
    widget_dispatch(widget, &e);
    health_circle->value_m = value;
    e = event_init(EVT_VALUE_CHANGED, widget);
    widget_dispatch(widget, &e);
    widget_invalidate(widget, NULL);
  }

  return RET_OK;
}

ret_t health_circle_set_vlaue_s(widget_t* widget, float_t value) {
  health_circle_t* health_circle = HEALTH_CIRCLE(widget);
  return_value_if_fail(health_circle != NULL, RET_BAD_PARAMS);

  if (health_circle->value_s != value) {
    event_t e = event_init(EVT_VALUE_WILL_CHANGE, widget);
    widget_dispatch(widget, &e);
    health_circle->value_s = value;
    e = event_init(EVT_VALUE_CHANGED, widget);
    widget_dispatch(widget, &e);
    widget_invalidate(widget, NULL);
  }

  return RET_OK;
}

ret_t health_circle_set_value_max_b(widget_t* widget, float_t value) {
  health_circle_t* health_circle = HEALTH_CIRCLE(widget);
  return_value_if_fail(health_circle != NULL, RET_BAD_PARAMS);

  health_circle->max_b = value;
  event_t e = event_init(EVT_VALUE_CHANGED, widget);
  widget_dispatch(widget, &e);

  return widget_invalidate(widget, NULL);
}

ret_t health_circle_set_value_max_m(widget_t* widget, float_t value) {
  health_circle_t* health_circle = HEALTH_CIRCLE(widget);
  return_value_if_fail(health_circle != NULL, RET_BAD_PARAMS);

  health_circle->max_m = value;
  event_t e = event_init(EVT_VALUE_CHANGED, widget);
  widget_dispatch(widget, &e);

  return widget_invalidate(widget, NULL);
}

ret_t health_circle_set_value_max_s(widget_t* widget, float_t value) {
  health_circle_t* health_circle = HEALTH_CIRCLE(widget);
  return_value_if_fail(health_circle != NULL, RET_BAD_PARAMS);

  health_circle->max_s = value;
  event_t e = event_init(EVT_VALUE_CHANGED, widget);
  widget_dispatch(widget, &e);

  return widget_invalidate(widget, NULL);
}

ret_t health_circle_set_color_b(widget_t* widget, const char* color) {
  health_circle_t* health_circle = HEALTH_CIRCLE(widget);
  return_value_if_fail(health_circle != NULL, RET_BAD_PARAMS);

  color_from_str(&health_circle->color_b, color);
  health_circle->bg_color_b =
      color_init(color_r(&health_circle->color_b) / 4, color_g(&health_circle->color_b) / 4,
                 color_b(&health_circle->color_b) / 4, color_a(&health_circle->color_b));
  health_circle->mb_color_b =
      color_init(color_r(&health_circle->color_b) / 2, color_g(&health_circle->color_b) / 2,
                 color_b(&health_circle->color_b) / 2, color_a(&health_circle->color_b));
  return widget_invalidate(widget, NULL);
}

ret_t health_circle_set_color_m(widget_t* widget, const char* color) {
  health_circle_t* health_circle = HEALTH_CIRCLE(widget);
  return_value_if_fail(health_circle != NULL, RET_BAD_PARAMS);

  color_from_str(&health_circle->color_m, color);
  health_circle->bg_color_m =
      color_init(color_r(&health_circle->color_m) / 4, color_g(&health_circle->color_m) / 4,
                 color_b(&health_circle->color_m) / 4, color_a(&health_circle->color_m));
  health_circle->mb_color_m =
      color_init(color_r(&health_circle->color_m) / 2, color_g(&health_circle->color_m) / 2,
                 color_b(&health_circle->color_m) / 2, color_a(&health_circle->color_m));

  return widget_invalidate(widget, NULL);
}

ret_t health_circle_set_color_s(widget_t* widget, const char* color) {
  health_circle_t* health_circle = HEALTH_CIRCLE(widget);
  return_value_if_fail(health_circle != NULL, RET_BAD_PARAMS);

  color_from_str(&health_circle->color_s, color);
  health_circle->bg_color_s =
      color_init(color_r(&health_circle->color_s) / 4, color_g(&health_circle->color_s) / 4,
                 color_b(&health_circle->color_s) / 4, color_a(&health_circle->color_s));
  health_circle->mb_color_s =
      color_init(color_r(&health_circle->color_s) / 2, color_g(&health_circle->color_s) / 2,
                 color_b(&health_circle->color_s) / 2, color_a(&health_circle->color_s));

  return widget_invalidate(widget, NULL);
}

ret_t health_circle_set_width(widget_t* widget, float_t width) {
  health_circle_t* health_circle = HEALTH_CIRCLE(widget);
  return_value_if_fail(health_circle != NULL, RET_BAD_PARAMS);

  health_circle->width = width;

  return widget_invalidate(widget, NULL);
}

ret_t health_circle_set_only_big(widget_t* widget, bool_t v) {
  health_circle_t* health_circle = HEALTH_CIRCLE(widget);
  return_value_if_fail(health_circle != NULL, RET_BAD_PARAMS);

  health_circle->only_big = v;

  return widget_invalidate(widget, NULL);
}

ret_t health_circle_set_draggable(widget_t* widget, bool_t v) {
  health_circle_t* health_circle = HEALTH_CIRCLE(widget);
  return_value_if_fail(health_circle != NULL, RET_BAD_PARAMS);

  health_circle->draggable = v;

  return RET_OK;
}

ret_t health_circle_set_beyond(widget_t* widget, bool_t v) {
  health_circle_t* health_circle = HEALTH_CIRCLE(widget);
  return_value_if_fail(health_circle != NULL, RET_BAD_PARAMS);

  health_circle->no_beyond = v;
  if (health_circle->value_b > health_circle->max_b) {
    health_circle->value_b = health_circle->max_b;
  }
  if (health_circle->value_m > health_circle->max_m) {
    health_circle->value_m = health_circle->max_m;
  }
  if (health_circle->value_s > health_circle->max_s) {
    health_circle->value_s = health_circle->max_s;
  }

  return widget_invalidate(widget, NULL);
}

float_t get_float(float_t f) {
  return f;
}

static void draw_bg_circle(vgcanvas_t* vg, float_t rr, float_t a, color_t color) {
  vgcanvas_save(vg);
  vgcanvas_set_stroke_color(vg, color);
  vgcanvas_begin_path(vg);
  vgcanvas_arc(vg, 0, 0, rr, -0.5 * M_PI, a, FALSE);
  vgcanvas_stroke(vg);
  vgcanvas_restore(vg);
}

static ret_t health_circle_on_paint_self(widget_t* widget, canvas_t* c) {
  ret_t ret = RET_OK;
  (void)ret;
  vgcanvas_t* vg = canvas_get_vgcanvas(c);
  health_circle_t* health_circle = HEALTH_CIRCLE(widget);
  float_t rr = widget->w / 2.0 > widget->h / 2.0 ? widget->h / 2.0 : widget->w / 2.0;

  vgcanvas_save(vg);

  vgcanvas_translate(vg, c->ox, c->oy);
  vgcanvas_translate(vg, widget->w / 2.0, widget->h / 2.0);

  vgcanvas_set_line_width(vg, health_circle->width);

  vgcanvas_set_line_cap(vg, "round");
  // vgcanvas_set_antialias(vg, TRUE);
  // draw big circle
  draw_bg_circle(vg, rr - health_circle->width / 2.0, 1.5 * M_PI, health_circle->bg_color_b);

  draw_bg_circle(vg, rr - health_circle->width / 2.0,
                 (health_circle->value_b / health_circle->max_b) * 2.0 * M_PI - 0.5 * M_PI,
                 health_circle->color_b);


  if (!health_circle->only_big) {
    // draw middle circle
    vgcanvas_save(vg);
    draw_bg_circle(vg, rr - health_circle->width * 1.5 - 2, 1.5 * M_PI, health_circle->bg_color_m);

    vgcanvas_set_stroke_color(vg, health_circle->color_m);
    vgcanvas_begin_path(vg);
    vgcanvas_arc(vg, 0, 0, rr - health_circle->width * 1.5 - 2, -0.5 * M_PI,
                 (health_circle->value_m / health_circle->max_m) * 2 * M_PI - 0.5 * M_PI, FALSE);

    vgcanvas_stroke(vg);

    vgcanvas_restore(vg);
  }
  vgcanvas_restore(vg);

  return RET_OK;
}

static float_t C2R(xy_t ox, xy_t oy, xy_t x, xy_t y) {
  float_t result = 0;  // 记录弧度
  float_t x_f = x - ox;
  float_t y_f = y - oy;
  float_t rad = atan(fabs((float)y_f) / fabs((float)x_f));
  if (x_f > 0 && y_f > 0) {
    result = rad;
  }
  if (x_f < 0 && y_f > 0) {
    result = M_PI - rad;
  }
  if (x_f < 0 && y_f < 0) {
    result = M_PI + rad;
  }
  if (x_f > 0 && y_f < 0) {
    result = 2 * M_PI - rad;
  }
  return result;
}

static ret_t health_circle_on_event(widget_t* widget, event_t* e) {
  uint16_t type = e->type;
  health_circle_t* hc = HEALTH_CIRCLE(widget);
  switch (type) {
    case EVT_POINTER_LEAVE: {
      // hc->pressed = FALSE;
      // hc->dragging = 0;
      pointer_event_t evt = *(pointer_event_t*)e;
      if (hc->pressed == TRUE && widget_is_point_in(widget, evt.x, evt.y, FALSE) &&
          widget_get_prop_bool(widget, "_no_move", FALSE)) {
        evt.e = event_init(EVT_CLICK, widget->parent);
        widget_dispatch(widget, (event_t*)&evt);
        hc->pressed = FALSE;
        return RET_OK;
      }

      if (hc->draggable) {
        hc->dragging = 0;
        widget_ungrab(widget_get_window(widget), widget);
      }
      break;
    }
    case EVT_POINTER_DOWN: {
      pointer_event_t* evt = (pointer_event_t*)e;
      hc->pressed = TRUE;
      widget_set_prop_int(widget, "_dowm_x", evt->x);
      widget_set_prop_int(widget, "_dowm_y", evt->y);
      widget_set_prop_bool(widget, "_no_move", TRUE);
      if (hc->draggable) {
        point_t point = {evt->x, evt->y};
        widget_to_local(widget, &point);
        float_t rr = widget->w / 2 > widget->h / 2 ? widget->h / 2 : widget->w / 2;
        float_t dist_x = point.x - widget->w / 2;
        float_t dist_y = point.y - widget->h / 2;
        value_t v;
        value_set_float(&v, C2R(widget->w / 2, widget->h / 2, point.x, point.y));
        widget_set_prop(widget, "_p_r", &v);
        if ((dist_x * dist_x + dist_y * dist_y < rr * rr) &&
            (dist_x * dist_x + dist_y * dist_y) > (rr - hc->width) * (rr - hc->width)) {
          hc->dragging = 'b';
          widget_grab(widget_get_window(widget), widget);
        } else if (!hc->only_big &&
                   (dist_x * dist_x + dist_y * dist_y < (rr - hc->width) * (rr - hc->width)) &&
                   (dist_x * dist_x + dist_y * dist_y) >
                       (rr - hc->width * 2 - 2) * (rr - hc->width * 2 - 2)) {
          hc->dragging = 'm';
        } else if (!hc->only_big &&
                   (dist_x * dist_x + dist_y * dist_y < (rr - hc->width) * (rr - hc->width)) &&
                   (dist_x * dist_x + dist_y * dist_y) >
                       (rr - hc->width * 3 - 4) * (rr - hc->width * 3 - 4)) {
          hc->dragging = 's';
        }
      }
      break;
    }
    case EVT_POINTER_UP: {
      pointer_event_t evt = *(pointer_event_t*)e;
      if (hc->pressed == TRUE && widget_is_point_in(widget, evt.x, evt.y, FALSE) &&
          widget_get_prop_bool(widget, "_no_move", FALSE)) {
        evt.e = event_init(EVT_CLICK, widget->parent);
        widget_dispatch(widget, (event_t*)&evt);
        hc->pressed = FALSE;
        return RET_OK;
      }

      if (hc->draggable) {
        hc->dragging = 0;
        widget_ungrab(widget_get_window(widget), widget);
      }

      //// 防止滚动时页面被拖动
      // if (!hc->pressed && widget_is_point_in(widget, evt.x, evt.y, FALSE)) {
      //  return RET_STOP;
      //}

      break;
    }
    case EVT_POINTER_MOVE: {
      pointer_event_t evt = *(pointer_event_t*)e;
      widget_set_prop_bool(widget, "_no_move", FALSE);
      if (hc->draggable && hc->dragging != 0) {
        hc->pressed = FALSE;  // 当指针移动时不视为点击事件
        point_t point = {evt.x, evt.y};
        widget_to_local(widget, &point);
        float_t result = C2R(widget->w / 2, widget->h / 2, point.x, point.y);
        // result = result>1.5*M_PI ? result-1.5*M_PI : result+0.5*M_PI;
        value_t v;
        widget_get_prop(widget, "_p_r", &v);
        float_t pre_r = value_float(&v);

        if (result < 0.5) {
          if (pre_r > 5.5) {
            result = 6.2831852;
          } else if (pre_r > 4) {
            result = 4.7123889;
          } else if (pre_r > 3) {
            result = M_PI;
          } else if (pre_r > 1) {
            result += 1.5707963;
          }
        }

        if (pre_r < 0.5) {
          if (result > 5.5) {
            pre_r = 6.2831853;
          } else if (result > 4) {
            pre_r = 4.7123889;
          } else if (result > 3) {
            pre_r = M_PI;
          } else if (result > 1) {
            pre_r = 1.5707963;
          }
        }

        value_reset(&v);
        value_set_float(&v, C2R(widget->w / 2, widget->h / 2, point.x, point.y));
        widget_set_prop(widget, "_p_r", &v);

        if (hc->dragging == 'b') {
          // hc->value_b = hc->max_b * (result / (2.0*M_PI));
          result = (result - pre_r) / 6.2831852 * hc->max_b;
          hc->value_b += result;
          if (hc->value_b < 0) {
            hc->value_b = 0;
          }
          if (hc->no_beyond && hc->value_b > hc->max_b) {
            hc->value_b = hc->max_b;
          }
          log_debug("value_b -> %f\n", hc->value_b);
        } else if (hc->dragging == 'm') {
          result = (result - pre_r) / 6.2831852 * hc->max_m;
          hc->value_m += result;
          if (hc->value_m < 0) {
            hc->value_m = 0;
          }
          if (hc->no_beyond && hc->value_m > hc->max_m) {
            hc->value_m = hc->max_m;
          }
          log_debug("value_m -> %f\n", hc->value_m);
          // hc->value_m = hc->max_m * (result / (2.0*M_PI));
        } else {
          result = (result - pre_r) / 6.2831852 * hc->max_s;
          hc->value_s += result;
          if (hc->value_s < 0) {
            hc->value_s = 0;
          }
          if (hc->no_beyond && hc->value_s > hc->max_s) {
            hc->value_s = hc->max_s;
          }
          log_debug("value_s -> %f\n", hc->value_s);
          // hc->value_s = hc->max_s * (result / (2.0*M_PI));
        }
        evt.e = event_init(EVT_VALUE_CHANGED, widget);
        widget_dispatch(widget, (event_t*)&evt);
        widget_invalidate_force(widget, NULL);
      }
      break;
    }
    default:
      break;
  }
  return RET_OK;
}

static ret_t health_circle_on_set_prop(widget_t* widget, const char* name, const value_t* v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);
  health_circle_t* circle = HEALTH_CIRCLE(widget);

  if (tk_str_eq(name, HEALTH_CIRCLE_PROP_VALUE_B)) {
    return health_circle_set_value_b(widget, value_float(v));
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_VALUE_M)) {
    return health_circle_set_value_m(widget, value_float(v));
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_VALUE_S)) {
    return health_circle_set_vlaue_s(widget, value_float(v));
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_MAX_B)) {
    return health_circle_set_value_max_b(widget, value_float(v));
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_MAX_M)) {
    return health_circle_set_value_max_m(widget, value_float(v));
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_MAX_S)) {
    return health_circle_set_value_max_s(widget, value_float(v));
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_COLOR_B)) {
    return health_circle_set_color_b(widget, value_str(v));
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_COLOR_M)) {
    return health_circle_set_color_m(widget, value_str(v));
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_COLOR_S)) {
    return health_circle_set_color_s(widget, value_str(v));
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_WIDTH)) {
    return health_circle_set_width(widget, value_float(v));
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_ONLY_BIG)) {
    return health_circle_set_only_big(widget, value_bool(v));
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_DRAGGABLE)) {
    return health_circle_set_draggable(widget, value_bool(v));
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_NO_BEYOND)) {
    return health_circle_set_beyond(widget, value_bool(v));
  } else if (tk_str_eq(name, "smooth_start")) {
    HEALTH_CIRCLE(widget)->smooth_start = value_bool(v);
    LOG("%d", value_bool(v));
    // assert(FALSE);
    return RET_OK;
  } else if (tk_str_eq(name, "smooth_end")) {
    HEALTH_CIRCLE(widget)->smooth_end = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, "end_color_b")) {
    circle->end_color_b = color_parse(value_str(v));
    return RET_OK;
  } else if (tk_str_eq(name, "end_color_m")) {
    circle->end_color_m = color_parse(value_str(v));
    return RET_OK;
  } else if (tk_str_eq(name, "end_color_s")) {
    circle->end_color_s = color_parse(value_str(v));
    return RET_OK;
  } else if (tk_str_eq(name, "draw_end_line_b")) {
    circle->draw_end_line_b = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, "bg_color_b")) {
    circle->bg_color_b = color_parse(value_str(v));
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t health_circle_on_get_prop(widget_t* widget, const char* name, value_t* v) {
  health_circle_t* health_circle = HEALTH_CIRCLE(widget);
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, HEALTH_CIRCLE_PROP_MAX_B)) {
    value_set_float(v, health_circle->max_b);
    return RET_OK;
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_MAX_M)) {
    value_set_float(v, health_circle->max_m);
    return RET_OK;
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_MAX_S)) {
    value_set_float(v, health_circle->max_s);
    return RET_OK;
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_VALUE_B)) {
    value_set_float(v, health_circle->value_b);
    return RET_OK;
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_VALUE_M)) {
    value_set_float(v, health_circle->value_m);
    return RET_OK;
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_VALUE_S)) {
    value_set_float(v, health_circle->value_s);
    return RET_OK;
  } else if (tk_str_eq(name, HEALTH_CIRCLE_PROP_ONLY_BIG)) {
    value_set_bool(v, health_circle->only_big);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static const char* s_health_circle_clone_properties[] = {HEALTH_CIRCLE_PROP_VALUE_B,
                                                         HEALTH_CIRCLE_PROP_VALUE_M,
                                                         HEALTH_CIRCLE_PROP_VALUE_S,
                                                         HEALTH_CIRCLE_PROP_MAX_B,
                                                         HEALTH_CIRCLE_PROP_MAX_M,
                                                         HEALTH_CIRCLE_PROP_MAX_S,
                                                         HEALTH_CIRCLE_PROP_COLOR_B,
                                                         HEALTH_CIRCLE_PROP_COLOR_M,
                                                         HEALTH_CIRCLE_PROP_COLOR_S,
                                                         "smooth_end",
                                                         NULL};
static const widget_vtable_t s_health_circle_vtable = {
    .size = sizeof(health_circle_t),
    .type = WIDGET_TYPE_HEALTH_CIRCLE,
    .clone_properties = s_health_circle_clone_properties,
    .create = health_circle_create,
    .on_paint_self = health_circle_on_paint_self,
    .on_event = health_circle_on_event,
    .get_prop = health_circle_on_get_prop,
    .set_prop = health_circle_on_set_prop};

widget_t* health_circle_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  health_circle_t* health_circle = TKMEM_ZALLOC(health_circle_t);
  widget_t* widget = WIDGET(health_circle);
  return_value_if_fail(health_circle != NULL, NULL);

  widget_init(widget, parent, &s_health_circle_vtable, x, y, w, h);

  // set default value
  health_circle->value_s = 0;
  health_circle->value_m = 0;
  health_circle->value_b = 0;
  health_circle->max_s = 100;
  health_circle->max_m = 100;
  health_circle->max_b = 100;
  health_circle->color_b = color_init(0xEE, 0x11, 0xC2, 0xFF);  // "#EE11C2"
  health_circle->color_m = color_init(0xC2, 0xEE, 0x11, 0xFF);  // "#C2EE11"
  health_circle->color_s = color_init(0x09, 0xC7, 0xF7, 0xFF);  // "#09C7F7"

  health_circle->bg_color_b =
      color_init(color_r(&health_circle->color_b) / 4, color_g(&health_circle->color_b) / 4,
                 color_b(&health_circle->color_b) / 4, color_a(&health_circle->color_b));
  health_circle->bg_color_m =
      color_init(color_r(&health_circle->color_m) / 4, color_g(&health_circle->color_m) / 4,
                 color_b(&health_circle->color_m) / 4, color_a(&health_circle->color_m));
  health_circle->bg_color_s =
      color_init(color_r(&health_circle->color_s) / 4, color_g(&health_circle->color_s) / 4,
                 color_b(&health_circle->color_s) / 4, color_a(&health_circle->color_s));
  health_circle->mb_color_b =
      color_init(color_r(&health_circle->color_b) / 2, color_g(&health_circle->color_b) / 2,
                 color_b(&health_circle->color_b) / 2, color_a(&health_circle->color_b));
  health_circle->mb_color_m =
      color_init(color_r(&health_circle->color_m) / 2, color_g(&health_circle->color_m) / 2,
                 color_b(&health_circle->color_m) / 2, color_a(&health_circle->color_m));
  health_circle->mb_color_s =
      color_init(color_r(&health_circle->color_s) / 2, color_g(&health_circle->color_s) / 2,
                 color_b(&health_circle->color_s) / 2, color_a(&health_circle->color_s));
  health_circle->width = 40;
  health_circle->only_big = FALSE;
  health_circle->pressed = FALSE;
  health_circle->draggable = FALSE;
  health_circle->no_beyond = FALSE;
  health_circle->dragging = 0;

  health_circle->smooth_end = TRUE;
  health_circle->smooth_start = TRUE;

  health_circle->draw_end_line_b = TRUE;

  return widget;
}

widget_t* health_circle_cast(widget_t* widget) {
  return_value_if_fail(widget != NULL && widget->vt == &s_health_circle_vtable, NULL);

  return widget;
}
