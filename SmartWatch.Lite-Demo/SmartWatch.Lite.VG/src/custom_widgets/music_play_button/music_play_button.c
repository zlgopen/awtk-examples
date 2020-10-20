#include "tkc/mem.h"
#include "base/timer.h"
#include "tkc/utils.h"
#include "music_play_button.h"
#include "base/widget_vtable.h"

static ret_t music_play_button_pointer_up_cleanup(widget_t *widget) {
  music_play_button_t *music_play_button = MUSIC_PLAY_BUTTON(widget);
  return_value_if_fail(music_play_button != NULL && widget != NULL,
                       RET_BAD_PARAMS);

  music_play_button->pressed = FALSE;
  widget_ungrab(widget->parent, widget);
  widget_set_state(widget, WIDGET_STATE_NORMAL);

  return RET_OK;
}

ret_t music_play_set_value(widget_t *widget, uint32_t value) {
  music_play_button_t *music_play_button = MUSIC_PLAY_BUTTON(widget);
  return_value_if_fail(music_play_button != NULL && widget != NULL,
                       RET_BAD_PARAMS);

  music_play_button->value = value;

  widget_invalidate(widget, NULL);

  return RET_OK;
}

ret_t music_play_set_max_value(widget_t *widget, uint32_t max_value) {
  music_play_button_t *music_play_button = MUSIC_PLAY_BUTTON(widget);
  return_value_if_fail(music_play_button != NULL && widget != NULL,
                       RET_BAD_PARAMS);

  music_play_button->max_value = max_value;

  widget_invalidate(widget, NULL);

  return RET_OK;
}

ret_t music_play_set_line_width(widget_t *widget, uint32_t line_width) {
  music_play_button_t *music_play_button = MUSIC_PLAY_BUTTON(widget);
  return_value_if_fail(music_play_button != NULL && widget != NULL,
                       RET_BAD_PARAMS);

  music_play_button->line_width = line_width;

  widget_invalidate(widget, NULL);

  return RET_OK;
}

ret_t music_play_set_start_angle(widget_t *widget, int32_t start_angle) {
  music_play_button_t *music_play_button = MUSIC_PLAY_BUTTON(widget);
  return_value_if_fail(music_play_button != NULL && widget != NULL,
                       RET_BAD_PARAMS);

  music_play_button->start_angle = start_angle;

  widget_invalidate(widget, NULL);

  return RET_OK;
}

ret_t music_play_set_counter_clock_wise(widget_t *widget,
                                        bool_t counter_clock_wise) {
  music_play_button_t *music_play_button = MUSIC_PLAY_BUTTON(widget);
  return_value_if_fail(music_play_button != NULL && widget != NULL,
                       RET_BAD_PARAMS);

  music_play_button->counter_clock_wise = counter_clock_wise;

  widget_invalidate(widget, NULL);

  return RET_OK;
}

ret_t music_play_set_line_color(widget_t *widget, color_t line_color) {
  music_play_button_t *music_play_button = MUSIC_PLAY_BUTTON(widget);
  return_value_if_fail(music_play_button != NULL && widget != NULL,
                       RET_BAD_PARAMS);

  music_play_button->line_color = line_color;

  widget_invalidate(widget, NULL);

  return RET_OK;
}

static ret_t music_play_button_on_event(widget_t *widget, event_t *e) {
  uint16_t type = e->type;
  music_play_button_t *music_play_button = MUSIC_PLAY_BUTTON(widget);
  return_value_if_fail(music_play_button != NULL && widget != NULL,
                       RET_BAD_PARAMS);

  switch (type) {
  case EVT_POINTER_DOWN: {
    music_play_button->pressed = TRUE;
    widget_set_state(widget, WIDGET_STATE_PRESSED);

    widget_grab(widget->parent, widget);
    break;
  }
  case EVT_POINTER_DOWN_ABORT: {
    music_play_button_pointer_up_cleanup(widget);
    break;
  }
  case EVT_POINTER_UP: {
    pointer_event_t evt = *(pointer_event_t *)e;
    if (music_play_button->pressed &&
        widget_is_point_in(widget, evt.x, evt.y, FALSE)) {
      evt.e = event_init(EVT_CLICK, widget);
      evt.e.size = sizeof(evt);
      music_play_button_pointer_up_cleanup(widget);
      widget_dispatch(widget, (event_t *)&evt);
    } else {
      music_play_button_pointer_up_cleanup(widget);
    }

    break;
  }
  case EVT_POINTER_LEAVE:
    widget_set_state(widget, WIDGET_STATE_NORMAL);
    break;
  case EVT_POINTER_ENTER: {
    pointer_event_t *evt = (pointer_event_t *)e;
    if (music_play_button->pressed || !evt->pressed) {
      widget_set_state(widget, WIDGET_STATE_OVER);
    }
    break;
  }
  default:
    break;
  }

  return RET_OK;
}

static ret_t music_play_button_get_prop(widget_t *widget, const char *name,
                                        value_t *v) {
  music_play_button_t *music_play_button = MUSIC_PLAY_BUTTON(widget);
  return_value_if_fail(music_play_button != NULL && name != NULL && v != NULL,
                       RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    value_set_uint32(v, music_play_button->value);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_MAX)) {
    value_set_uint32(v, music_play_button->max_value);
    return RET_OK;
  } else if (tk_str_eq(name, MUSIC_PLAY_BUTTON_PROP_LINE_WIDTH)) {
    value_set_uint32(v, music_play_button->line_width);
    return RET_OK;
  } else if (tk_str_eq(name, MUSIC_PLAY_BUTTON_PROP_LINE_COLOR)) {
    char color_tmp[TK_COLOR_HEX_LEN + 1] = {0};
    value_set_str(v, color_hex_str(music_play_button->line_color, color_tmp));
    return RET_OK;
  } else if (tk_str_eq(name, MUSIC_PLAY_BUTTON_PROP_START_ANGLE)) {
    value_set_int(v, music_play_button->start_angle);
    return RET_OK;
  } else if (tk_str_eq(name, MUSIC_PLAY_BUTTON_PROP_COUNTER_CLOCK_WISE)) {
    value_set_bool(v, music_play_button->counter_clock_wise);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t music_play_button_set_prop(widget_t *widget, const char *name,
                                        const value_t *v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL,
                       RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    return music_play_set_value(widget, value_uint32(v));
  } else if (tk_str_eq(name, WIDGET_PROP_MAX)) {
    return music_play_set_max_value(widget, value_uint32(v));
  } else if (tk_str_eq(name, MUSIC_PLAY_BUTTON_PROP_LINE_WIDTH)) {
    return music_play_set_line_width(widget, value_uint32(v));
  } else if (tk_str_eq(name, MUSIC_PLAY_BUTTON_PROP_LINE_COLOR)) {
    music_play_button_t *music_play_button = MUSIC_PLAY_BUTTON(widget);
    color_from_str(&music_play_button->line_color, value_str(v));
    return RET_OK;
  } else if (tk_str_eq(name, MUSIC_PLAY_BUTTON_PROP_START_ANGLE)) {
    return music_play_set_start_angle(widget, value_int32(v));
  } else if (tk_str_eq(name, MUSIC_PLAY_BUTTON_PROP_COUNTER_CLOCK_WISE)) {
    return music_play_set_counter_clock_wise(widget, value_bool(v));
  }

  return RET_NOT_FOUND;
}

static ret_t music_play_button_on_paint_self(widget_t *widget, canvas_t *c) {
  vgcanvas_t *vg = canvas_get_vgcanvas(c);
  music_play_button_t *music_play_button = MUSIC_PLAY_BUTTON(widget);
  if (music_play_button->value > music_play_button->max_value) {
    music_play_button->value = music_play_button->max_value;
  }

  if (vg != NULL && music_play_button->line_color.rgba.a) {
    xy_t cx = widget->w / 2;
    xy_t cy = widget->h / 2;
    float_t end_angle = 0;
    float_t r = tk_min(cx, cy) - music_play_button->line_width / 2.0f;
    bool_t ccw = music_play_button->counter_clock_wise;
    float_t start_angle = TK_D2R(music_play_button->start_angle);
    float_t angle =
        (M_PI * 2 * music_play_button->value) / music_play_button->max_value;

    if (ccw) {
      end_angle = start_angle - angle + M_PI * 2;
      if (fabs(end_angle - start_angle) < 0.001f) {
        end_angle = start_angle + 0.001f;
      }
    } else {
      end_angle = start_angle + angle;
    }

    vgcanvas_save(vg);
    vgcanvas_translate(vg, (float_t)c->ox, (float_t)c->oy);
    vgcanvas_set_stroke_color(vg, music_play_button->line_color);
    vgcanvas_set_line_width(vg, (float_t)music_play_button->line_width);
    vgcanvas_set_line_cap(vg, "round");
    vgcanvas_begin_path(vg);
    if (end_angle > start_angle) {
      vgcanvas_arc(vg, (float_t)cx, (float_t)cy, (float_t)r, start_angle,
                   end_angle, ccw);
      vgcanvas_stroke(vg);
    }

    vgcanvas_restore(vg);
  }

  return RET_OK;
}

static const char *const s_music_play_button_properties[] = {NULL};

TK_DECL_VTABLE(music_play_button) = {
    .size = sizeof(music_play_button_t),
    .type = WIDGET_TYPE_MUSIC_PLAY_BUTTON,
    .space_key_to_activate = TRUE,
    .return_key_to_activate = TRUE,
    .parent = TK_PARENT_VTABLE(widget),
    .create = music_play_button_create,
    .clone_properties = s_music_play_button_properties,
    .persistent_properties = s_music_play_button_properties,
    .on_event = music_play_button_on_event,
    .set_prop = music_play_button_set_prop,
    .get_prop = music_play_button_get_prop,
    .on_paint_self = music_play_button_on_paint_self};

widget_t *music_play_button_create(widget_t *parent, xy_t x, xy_t y, wh_t w,
                                   wh_t h) {
  widget_t *widget =
      widget_create(parent, TK_REF_VTABLE(music_play_button), x, y, w, h);
  music_play_button_t *music_play_button = MUSIC_PLAY_BUTTON(widget);
  return_value_if_fail(music_play_button != NULL, NULL);

  music_play_button->pressed = FALSE;
  music_play_button->counter_clock_wise = FALSE;
  music_play_button->start_angle = 0;

  return widget;
}

widget_t *music_play_button_cast(widget_t *widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, music_play_button), NULL);

  return widget;
}
