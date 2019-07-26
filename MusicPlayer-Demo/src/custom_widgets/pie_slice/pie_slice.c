/**
 * File:   pie_slice.c
 * Author: AWTK Develop Team
 * Brief:  pie_slice
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * * 2018-12-4 TangJunJie created
 *
 */

/*
vgcanvas_save(vg);
vgcanvas_translate(vg, 100, 100);
vgcanvas_move_to(vg, 0, 0);
vgcanvas_begin_path(vg);
vgcanvas_set_line_width(vg, 1);
vgcanvas_set_stroke_color(vg, color_init(0, 0xff, 0, 0xff));
vgcanvas_set_fill_color(vg, color_init( 0xff, 0, 0, 0xff));

//1 顺时针画环形
vgcanvas_arc(vg, 0, 0, 50, 0, 0.5 * 3.15,  FALSE);
vgcanvas_arc(vg, 0, 0, 25, 0.5 * 3.15,  0, TRUE);
//2 逆时针画环形
vgcanvas_arc(vg, 0, 0, 25, (2-0.5) * 3.15, 0, FALSE);
vgcanvas_arc(vg, 0, 0, 50, 0, (2-0.5) * 3.15,  TRUE);
//3 顺时针画扇形
vgcanvas_arc(vg, 0, 0, 50, 0, 0.5 * 3.15, FALSE);
vgcanvas_line_to(vg, 0, 0);
//4 逆时针画扇形
vgcanvas_arc(vg, 0, 0, 50, 0, (2-0.5) * 3.15, TRUE);
vgcanvas_line_to(vg, 0, 0);

vgcanvas_fill(vg);
vgcanvas_close_path(vg);
vgcanvas_restore(vg);
*/

#include "pie_slice.h"
#include <math.h>

static pie_slice_t* pie_slice_get_clicked_pie(widget_t* widget, pie_slice_t* pie_slice, float_t r,
                                              int32_t* x_f, int32_t* y_f) {
  float_t result = 0;
  if (pie_slice->is_semicircle) {
    *y_f = *y_f - r * 0.5;
    if (*y_f > 0) {
      return NULL;
    }
  }
  if (pie_slice->is_exploded) {
    *x_f += pie_slice->x_to;
    *y_f += pie_slice->y_to;
  }
  float_t rad = (float_t)atan((double)abs(*y_f) / (double)abs(*x_f));
  if (*x_f > 0 && *y_f > 0) {
    result = rad;
  }
  if (*x_f < 0 && *y_f > 0) {
    result = M_PI - rad;
  }
  if (*x_f < 0 && *y_f < 0) {
    result = M_PI + rad;
  }
  if (*x_f > 0 && *y_f < 0) {
    result = 2 * M_PI - rad;
  }

  float_t end_angle = 0;
  float_t angle_result = result * 180 / M_PI;
  pie_slice_t* iter_result = NULL;
  WIDGET_FOR_EACH_CHILD_BEGIN(widget->parent, iter, i)
  iter_result = PIE_SLICE(iter);
  float_t start_angle = iter_result->start_angle;
  end_angle = iter_result->start_angle + iter_result->value;
  if (pie_slice->is_semicircle) {
    start_angle = 180 * 2 - (start_angle + iter_result->value) / 2;
    end_angle = start_angle + iter_result->value / 2;
  }
  if ((angle_result >= start_angle) && (angle_result <= end_angle)) {
    break;
  }
  WIDGET_FOR_EACH_CHILD_END();

  return iter_result;
}

static ret_t pie_slice_on_event_4_pointer_down(widget_t* widget, pie_slice_t* pie_slice, float_t r,
                                               int32_t x_f, int32_t y_f, pie_slice_t* iter_result) {
  int space = r / 3;
  if (pie_slice->is_semicircle) {
    space = tk_min((int)(r - pie_slice->inner_radius), space);
  }
  float_t length = sqrt(pow(x_f, 2) + pow(y_f, 2));
  bool_t y_f_flag =
      pie_slice->is_semicircle ? (y_f <= 0 && abs(y_f) <= space) : (abs(y_f) <= space);
  if ((abs(x_f) <= space) && y_f_flag) {
    pie_slice_set_exploded_4_all((widget_t*)iter_result);
  } else {
    int32_t radius = widget->h * 0.5 - iter_result->explode_distancefactor;
    if (iter_result->is_exploded) {
      radius = widget->h * 0.5;
    }
    if (pie_slice->is_semicircle) {
      radius += iter_result->explode_distancefactor * 1.5;
      if ((length <= radius) && (length >= pie_slice->inner_radius)) {
        pie_slice_set_exploded_4_others((widget_t*)iter_result);
      }
    } else {
      if (length <= radius) {
        pie_slice_set_exploded_4_others((widget_t*)iter_result);
      }
    }
  }

  return RET_OK;
}

static ret_t pie_slice_on_event(widget_t* widget, event_t* e) {
  uint16_t type = e->type;
  pie_slice_t* pie_slice = PIE_SLICE(widget);
  switch (type) {
    case EVT_POINTER_DOWN: {
      pie_slice->pressed = TRUE;
      xy_t cx = widget->w / 2;
      xy_t cy = widget->h / 2;
      float_t r = tk_min(cx, cy) - pie_slice->explode_distancefactor;
      pointer_event_t* pointer_event = (pointer_event_t*)e;
      point_t p = {pointer_event->x, pointer_event->y};
      widget_to_local(widget, &p);
      int32_t x_f = p.x - widget->w * 0.5;
      int32_t y_f = p.y - widget->h * 0.5;
      pie_slice_t* iter_result = pie_slice_get_clicked_pie(widget, pie_slice, r, &x_f, &y_f);
      if (iter_result == NULL) {
        break;
      }
      pie_slice_on_event_4_pointer_down(widget, pie_slice, r, x_f, y_f, iter_result);
      break;
    }
    case EVT_POINTER_UP: {
      pointer_event_t evt = *(pointer_event_t*)e;
      if (pie_slice->pressed && widget_is_point_in(widget, evt.x, evt.y, FALSE)) {
        evt.e = event_init(EVT_CLICK, widget->parent);
        widget_dispatch(widget, (event_t*)&evt);
        pie_slice->pressed = FALSE;
      }
      break;
    }
    default:
      break;
  }

  return RET_OK;
}

ret_t pie_slice_set_exploded(widget_t* widget) {
  pie_slice_t* pieslice = PIE_SLICE(widget);
  return_value_if_fail(pieslice != NULL, RET_BAD_PARAMS);

  char param[100];
  int32_t x_to = 0;
  int32_t y_to = 0;

  if (pieslice->is_exploded == FALSE) {
    pieslice->is_exploded = TRUE;
    int32_t angle_move = pieslice->start_angle + pieslice->value * 0.5;
    if (pieslice->is_semicircle) {
      uint32_t start_angle = 180 * 2 - (pieslice->start_angle + pieslice->value) / 2;
      angle_move = start_angle + pieslice->value * 0.25;
    }
    if (angle_move <= 90) {
      x_to = cos(angle_move * (M_PI / 180)) * pieslice->explode_distancefactor;
      y_to = sin(angle_move * (M_PI / 180)) * pieslice->explode_distancefactor;
    } else if (angle_move <= 180) {
      angle_move = 180 - angle_move;
      x_to = -cos(angle_move * (M_PI / 180)) * pieslice->explode_distancefactor;
      y_to = sin(angle_move * (M_PI / 180)) * pieslice->explode_distancefactor;
    } else if (angle_move <= 270) {
      angle_move = 270 - angle_move;
      x_to = -sin(angle_move * (M_PI / 180)) * pieslice->explode_distancefactor;
      y_to = -cos(angle_move * (M_PI / 180)) * pieslice->explode_distancefactor;
    } else {
      angle_move = 360 - angle_move;
      x_to = cos(angle_move * (M_PI / 180)) * pieslice->explode_distancefactor;
      y_to = -sin(angle_move * (M_PI / 180)) * pieslice->explode_distancefactor;
    }
  } else {
    pieslice->is_exploded = FALSE;
  }

  pieslice->x_to = x_to;
  pieslice->y_to = y_to;

  tk_snprintf(param, sizeof(param), "move(x_to=%d, y_to=%d, duration=200, easing=linear)", x_to,
              y_to);
  widget_create_animator((widget_t*)pieslice, param);

  return RET_OK;
}

ret_t pie_slice_set_exploded_4_others(widget_t* widget) {
  pie_slice_t* pieslice = PIE_SLICE(widget);
  return_value_if_fail(pieslice != NULL, RET_BAD_PARAMS);

  pie_slice_t* iter_result = NULL;

  WIDGET_FOR_EACH_CHILD_BEGIN(widget->parent, iter, i)
  iter_result = PIE_SLICE(iter);
  if (iter_result != pieslice) {
    iter_result->is_exploded = TRUE;
    pie_slice_set_exploded((widget_t*)iter_result);
  }
  WIDGET_FOR_EACH_CHILD_END();

  pie_slice_set_exploded((widget_t*)pieslice);

  return RET_OK;
}

ret_t pie_slice_set_exploded_4_all(widget_t* widget) {
  pie_slice_t* iter_result = NULL;
  static bool_t is_exploded = TRUE;
  if (is_exploded) {
    is_exploded = FALSE;
  } else {
    is_exploded = TRUE;
  }

  WIDGET_FOR_EACH_CHILD_BEGIN(widget->parent, iter, i)
  iter_result = PIE_SLICE(iter);
  iter_result->is_exploded = is_exploded;
  pie_slice_set_exploded((widget_t*)iter_result);
  WIDGET_FOR_EACH_CHILD_END();

  return RET_OK;
}

static ret_t pie_slice_on_paint_self(widget_t* widget, canvas_t* c) {
  bitmap_t img;
  style_t* style = widget->astyle;
  color_t trans = color_init(0, 0, 0, 0);
  vgcanvas_t* vg = canvas_get_vgcanvas(c);
  pie_slice_t* pie_slice = PIE_SLICE(widget);
  color_t color = style_get_color(style, STYLE_ID_FG_COLOR, trans);
  const char* image_name = style_get_str(style, STYLE_ID_FG_IMAGE, NULL);
  bool_t has_image = image_name && widget_load_image(widget, image_name, &img) == RET_OK;

  if (pie_slice->value > pie_slice->max) {
    pie_slice->value = pie_slice->max;
  }

  if (vg != NULL && (has_image || color.rgba.a)) {
    xy_t cx = widget->w / 2;
    xy_t cy = widget->h / 2;
    float_t end_angle = 0;
    float_t r = 0;
    bool_t ccw = pie_slice->counter_clock_wise;
    float_t start_angle = TK_D2R(pie_slice->start_angle);
    float_t angle = (M_PI * 2 * pie_slice->value) / pie_slice->max;

    if (ccw) {
      end_angle = start_angle - angle + M_PI * 2;
    } else {
      end_angle = start_angle + angle;
    }

    vgcanvas_save(vg);
    vgcanvas_translate(vg, c->ox, c->oy);
    if (end_angle > start_angle) {
      vgcanvas_set_fill_color(vg, color);
      vgcanvas_begin_path(vg);
      r = tk_min(cx, cy);
      r -= pie_slice->explode_distancefactor;

      if (pie_slice->is_semicircle) {
        if (ccw) {
          start_angle = M_PI * 2 - (start_angle + angle) / 2;
          end_angle = start_angle + angle / 2;
          cy = cy + r * 0.5;
          r += pie_slice->explode_distancefactor * 1.5;
          vgcanvas_arc(vg, cx, cy, r, start_angle, end_angle, !ccw);
          r -= pie_slice->inner_radius;
          vgcanvas_arc(vg, cx, cy, r, end_angle, start_angle, ccw);
        }
      } else {
        vgcanvas_arc(vg, cx, cy, r, start_angle, end_angle, ccw);
        if (r - pie_slice->inner_radius <= 0) {
          vgcanvas_line_to(vg, cx, cy);
        } else {
          r -= pie_slice->inner_radius;
          vgcanvas_arc(vg, cx, cy, r, end_angle, start_angle, !ccw);
        }
      }

      vgcanvas_close_path(vg);
      if (has_image) {
        vgcanvas_paint(vg, FALSE, &img);
      } else {
        vgcanvas_fill(vg);
      }
    }
    vgcanvas_restore(vg);
  }

  color = style_get_color(style, STYLE_ID_TEXT_COLOR, trans);
  if (pie_slice->show_text && color.rgba.a) {
    char s[TK_NUM_MAX_LEN + TK_NUM_MAX_LEN + 1];
    const char* unit = pie_slice->unit != NULL ? pie_slice->unit : "";
    tk_snprintf(s, sizeof(s), "%u%s", (uint32_t)(pie_slice->value), unit);

    widget_set_text_utf8(widget, s);
    widget_paint_helper(widget, c, NULL, NULL);
  }

  return RET_OK;
}

ret_t pie_slice_set_value(widget_t* widget, float_t value) {
  pie_slice_t* pie_slice = PIE_SLICE(widget);
  return_value_if_fail(pie_slice != NULL, RET_BAD_PARAMS);

  if (pie_slice->value != value) {
    event_t e = event_init(EVT_VALUE_WILL_CHANGE, widget);
    widget_dispatch(widget, &e);
    pie_slice->value = value;
    e = event_init(EVT_VALUE_CHANGED, widget);
    widget_dispatch(widget, &e);
    widget_invalidate(widget, NULL);
  }

  return RET_OK;
}

ret_t pie_slice_set_max(widget_t* widget, uint32_t max) {
  pie_slice_t* pie_slice = PIE_SLICE(widget);
  return_value_if_fail(pie_slice != NULL, RET_BAD_PARAMS);

  pie_slice->max = max;

  return widget_invalidate(widget, NULL);
}

ret_t pie_slice_set_inner_radius(widget_t* widget, uint32_t inner_radius) {
  pie_slice_t* pie_slice = PIE_SLICE(widget);
  return_value_if_fail(pie_slice != NULL, RET_BAD_PARAMS);

  pie_slice->inner_radius = inner_radius;

  return widget_invalidate(widget, NULL);
}

ret_t pie_slice_set_start_angle(widget_t* widget, int32_t start_angle) {
  pie_slice_t* pie_slice = PIE_SLICE(widget);
  return_value_if_fail(pie_slice != NULL, RET_BAD_PARAMS);

  pie_slice->start_angle = start_angle;

  return widget_invalidate(widget, NULL);
}

ret_t pie_slice_set_unit(widget_t* widget, const char* unit) {
  pie_slice_t* pie_slice = PIE_SLICE(widget);
  return_value_if_fail(pie_slice != NULL, RET_BAD_PARAMS);

  pie_slice->unit = tk_str_copy(pie_slice->unit, unit);

  return widget_invalidate(widget, NULL);
}

ret_t pie_slice_set_show_text(widget_t* widget, bool_t show_text) {
  pie_slice_t* pie_slice = PIE_SLICE(widget);
  return_value_if_fail(pie_slice != NULL, RET_BAD_PARAMS);

  pie_slice->show_text = show_text;

  return widget_invalidate(widget, NULL);
}

ret_t pie_slice_set_is_exploded(widget_t* widget, bool_t is_exploded) {
  pie_slice_t* pie_slice = PIE_SLICE(widget);
  return_value_if_fail(pie_slice != NULL, RET_BAD_PARAMS);

  pie_slice->is_exploded = is_exploded;

  return widget_invalidate(widget, NULL);
}

ret_t pie_slice_set_counter_clock_wise(widget_t* widget, bool_t counter_clock_wise) {
  pie_slice_t* pie_slice = PIE_SLICE(widget);
  return_value_if_fail(pie_slice != NULL, RET_BAD_PARAMS);

  pie_slice->counter_clock_wise = counter_clock_wise;

  return widget_invalidate(widget, NULL);
}

ret_t pie_slice_set_semicircle(widget_t* widget, bool_t is_semicircle) {
  pie_slice_t* pie_slice = PIE_SLICE(widget);
  return_value_if_fail(pie_slice != NULL, RET_BAD_PARAMS);

  pie_slice->is_semicircle = is_semicircle;

  return widget_invalidate(widget, NULL);
}

static ret_t pie_slice_get_prop(widget_t* widget, const char* name, value_t* v) {
  pie_slice_t* pie_slice = PIE_SLICE(widget);
  return_value_if_fail(pie_slice != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    value_set_float(v, pie_slice->value);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_MAX)) {
    value_set_uint32(v, pie_slice->max);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_SHOW_TEXT)) {
    value_set_bool(v, pie_slice->show_text);
    return RET_OK;
  } else if (tk_str_eq(name, PIE_SLICE_PROP_UNIT)) {
    value_set_str(v, pie_slice->unit);
    return RET_OK;
  } else if (tk_str_eq(name, PIE_SLICE_PROP_inner_radius)) {
    value_set_uint32(v, pie_slice->inner_radius);
    return RET_OK;
  } else if (tk_str_eq(name, PIE_SLICE_PROP_START_ANGLE)) {
    value_set_int(v, pie_slice->start_angle);
    return RET_OK;
  } else if (tk_str_eq(name, PIE_SLICE_PROP_COUNTER_CLOCK_WISE)) {
    value_set_bool(v, pie_slice->counter_clock_wise);
    return RET_OK;
  } else if (tk_str_eq(name, PIE_SLICE_PROP_IS_EXPLODED)) {
    value_set_bool(v, pie_slice->is_exploded);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t pie_slice_set_prop(widget_t* widget, const char* name, const value_t* v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    return pie_slice_set_value(widget, value_float(v));
  } else if (tk_str_eq(name, WIDGET_PROP_MAX)) {
    return pie_slice_set_max(widget, value_int(v));
  } else if (tk_str_eq(name, WIDGET_PROP_SHOW_TEXT)) {
    return pie_slice_set_show_text(widget, value_bool(v));
  } else if (tk_str_eq(name, PIE_SLICE_PROP_COUNTER_CLOCK_WISE)) {
    return pie_slice_set_counter_clock_wise(widget, value_bool(v));
  } else if (tk_str_eq(name, PIE_SLICE_PROP_inner_radius)) {
    return pie_slice_set_inner_radius(widget, value_int(v));
  } else if (tk_str_eq(name, PIE_SLICE_PROP_START_ANGLE)) {
    return pie_slice_set_start_angle(widget, value_int(v));
  } else if (tk_str_eq(name, PIE_SLICE_PROP_UNIT)) {
    return pie_slice_set_unit(widget, value_str(v));
  } else if (tk_str_eq(name, PIE_SLICE_PROP_IS_EXPLODED)) {
    return pie_slice_set_is_exploded(widget, value_bool(v));
  }

  return RET_NOT_FOUND;
}

static const char* s_pie_slice_clone_properties[] = {WIDGET_PROP_VALUE,
                                                     WIDGET_PROP_MAX,
                                                     PIE_SLICE_PROP_COUNTER_CLOCK_WISE,
                                                     PIE_SLICE_PROP_inner_radius,
                                                     PIE_SLICE_PROP_START_ANGLE,
                                                     PIE_SLICE_PROP_UNIT,
                                                     WIDGET_PROP_SHOW_TEXT,
                                                     NULL};
static const widget_vtable_t s_pie_slice_vtable = {.size = sizeof(pie_slice_t),
                                                   .type = WIDGET_TYPE_PIE_SLICE,
                                                   .clone_properties = s_pie_slice_clone_properties,
                                                   .create = pie_slice_create,
                                                   .on_paint_self = pie_slice_on_paint_self,
                                                   .on_event = pie_slice_on_event,
                                                   .get_prop = pie_slice_get_prop,
                                                   .set_prop = pie_slice_set_prop};

widget_t* pie_slice_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  pie_slice_t* pie_slice = TKMEM_ZALLOC(pie_slice_t);
  widget_t* widget = WIDGET(pie_slice);
  return_value_if_fail(pie_slice != NULL, NULL);

  widget_init(widget, parent, &s_pie_slice_vtable, x, y, w, h);

  pie_slice->max = 100;
  pie_slice->inner_radius = 8;
  pie_slice->start_angle = 0;
  pie_slice->show_text = FALSE;
  pie_slice->counter_clock_wise = FALSE;
  pie_slice->is_exploded = FALSE;
  pie_slice->explode_distancefactor = 25;
  pie_slice->is_semicircle = FALSE;

  return widget;
}

widget_t* pie_slice_cast(widget_t* widget) {
  return_value_if_fail(widget != NULL && widget->vt == &s_pie_slice_vtable, NULL);

  return widget;
}
