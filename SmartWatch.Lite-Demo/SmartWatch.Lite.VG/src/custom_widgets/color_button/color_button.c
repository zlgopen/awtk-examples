#include "tkc/mem.h"
#include "base/timer.h"
#include "tkc/utils.h"
#include "color_button.h"
#include "base/widget_vtable.h"

static ret_t color_button_pointer_up_cleanup(widget_t *widget) {
  color_button_t *color_button = COLOR_BUTTON(widget);
  return_value_if_fail(color_button != NULL && widget != NULL, RET_BAD_PARAMS);

  color_button->pressed = FALSE;
  widget_ungrab(widget->parent, widget);
  widget_set_state(widget, WIDGET_STATE_NORMAL);

  return RET_OK;
}

static ret_t color_button_on_event(widget_t *widget, event_t *e) {
  uint16_t type = e->type;
  color_button_t *color_button = COLOR_BUTTON(widget);
  return_value_if_fail(color_button != NULL && widget != NULL, RET_BAD_PARAMS);

  switch (type) {
  case EVT_POINTER_DOWN: {
    color_button->pressed = TRUE;
    widget_set_state(widget, WIDGET_STATE_PRESSED);

    widget_grab(widget->parent, widget);
    break;
  }
  case EVT_POINTER_DOWN_ABORT: {
    color_button_pointer_up_cleanup(widget);
    break;
  }
  case EVT_POINTER_UP: {
    pointer_event_t evt = *(pointer_event_t *)e;
    if (color_button->pressed &&
        widget_is_point_in(widget, evt.x, evt.y, FALSE)) {
      evt.e = event_init(EVT_CLICK, widget);
      evt.e.size = sizeof(evt);
      color_button_pointer_up_cleanup(widget);
      widget_dispatch(widget, (event_t *)&evt);
    } else {
      color_button_pointer_up_cleanup(widget);
    }

    break;
  }
  case EVT_POINTER_LEAVE:
    widget_set_state(widget, WIDGET_STATE_NORMAL);
    break;
  case EVT_POINTER_ENTER: {
    pointer_event_t *evt = (pointer_event_t *)e;
    if (color_button->pressed || !evt->pressed) {
      widget_set_state(widget, WIDGET_STATE_OVER);
    }
    break;
  }
  default:
    break;
  }

  return RET_OK;
}

static ret_t color_button_get_prop(widget_t *widget, const char *name,
                                   value_t *v) {
  color_button_t *color_button = COLOR_BUTTON(widget);
  return_value_if_fail(color_button != NULL && widget != NULL, RET_BAD_PARAMS);
  if (tk_str_eq(name, COLOR_BUTTON_PROP_BUTTON)) {
    value_set_bool(v, color_button->is_button);
    return RET_OK;
  } else if (tk_str_eq(name, COLOR_BUTTON_PROP_HEIGHT)) {
    value_set_uint32(v, color_button->height);
    return RET_OK;
  } else if (tk_str_eq(name, COLOR_BUTTON_PROP_ORIENTATION_TOP)) {
    value_set_bool(v, color_button->is_orientation_top);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_YOFFSET)) {
    value_set_int32(v, color_button->yoffset);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t color_button_set_prop(widget_t *widget, const char *name,
                                   const value_t *v) {
  color_button_t *color_button = COLOR_BUTTON(widget);
  return_value_if_fail(color_button != NULL && widget != NULL, RET_BAD_PARAMS);
  if (tk_str_eq(name, COLOR_BUTTON_PROP_BUTTON)) {
    color_button->is_button = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, COLOR_BUTTON_PROP_HEIGHT)) {
    color_button->height = value_uint32(v);
    return RET_OK;
  } else if (tk_str_eq(name, COLOR_BUTTON_PROP_ORIENTATION_TOP)) {
    color_button->is_orientation_top = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_YOFFSET)) {
    color_button->yoffset = value_int32(v);
    return RET_OK;
  }
  return RET_NOT_FOUND;
}

static ret_t color_button_on_paint_background(widget_t *widget, canvas_t *c) {

  bitmap_t img;
  float_t x = 0.0f;
  float_t y = 0.0f;
  float_t x1 = 0.0f;
  float_t y1 = 0.0f;
  float_t x2 = 0.0f;
  float_t y2 = 0.0f;
  style_t *style = widget->astyle;
  color_t trans = color_init(0, 0, 0, 0);
  vgcanvas_t *vg = canvas_get_vgcanvas(c);
  rect_t r = rect_init(0, 0, widget->w, widget->h);
  color_button_t *color_button = COLOR_BUTTON(widget);
  color_t color = style_get_color(style, STYLE_ID_BG_COLOR, trans);
  const char *image_name = style_get_str(style, STYLE_ID_FG_IMAGE, NULL);

  return_value_if_fail(vg != NULL, RET_FAIL);
  return_value_if_fail(color_button != NULL, RET_BAD_PARAMS);

  x1 = (float_t)(c->ox + widget->x);
  x2 = x1 + widget->w;
  x = (x1 + x2) / 2;

  if (color_button->is_orientation_top) {
    y = (float_t)(c->oy) - (float_t)(color_button->height);
    y2 = y1 = (float_t)c->oy + (float_t)color_button->height;
  } else {
    y = (float_t)c->oy + (float_t)widget->h + (float_t)color_button->height;
    y2 = y1 =
        (float_t)c->oy + (float_t)widget->h - (float_t)color_button->height;
  }

  if (color.rgba.a) {
    vgcanvas_begin_path(vg);
    vgcanvas_move_to(vg, x1, y1);
    vgcanvas_quad_to(vg, x, y, x2, y2);
    vgcanvas_close_path(vg);
    vgcanvas_set_fill_color(vg, color);
    vgcanvas_fill(vg);

    canvas_set_fill_color(c, color);
    if (color_button->is_orientation_top) {
      canvas_fill_rect(c, 0, color_button->height, widget->w,
                       widget->h - color_button->height);
    } else {
      canvas_fill_rect(c, 0, 0, widget->w, widget->h - color_button->height);
    }
  }

  if (image_name != NULL) {
    if (widget_load_image(widget, image_name, &img) == RET_OK) {
      r.y += color_button->yoffset;
      canvas_draw_image_ex(c, &img, IMAGE_DRAW_ICON, &r);
    }
  }

  return RET_OK;
}

static ret_t color_button_on_paint_self(widget_t *widget, canvas_t *c) {
  (void)c;
  (void)widget;
  return RET_OK;
}

static const char *const s_color_button_properties[] = {
    COLOR_BUTTON_PROP_BUTTON, COLOR_BUTTON_PROP_HEIGHT,
    COLOR_BUTTON_PROP_ORIENTATION_TOP, WIDGET_PROP_YOFFSET, NULL};

TK_DECL_VTABLE(color_button) = {
    .size = sizeof(color_button_t),
    .type = WIDGET_TYPE_COLOR_BUTTON,
    .space_key_to_activate = TRUE,
    .return_key_to_activate = TRUE,
    .parent = TK_PARENT_VTABLE(widget),
    .create = color_button_create,
    .clone_properties = s_color_button_properties,
    .persistent_properties = s_color_button_properties,
    .on_event = color_button_on_event,
    .set_prop = color_button_set_prop,
    .get_prop = color_button_get_prop,
    .on_paint_background = color_button_on_paint_background,
    .on_paint_self = color_button_on_paint_self};

widget_t *color_button_create(widget_t *parent, xy_t x, xy_t y, wh_t w,
                              wh_t h) {
  widget_t *widget =
      widget_create(parent, TK_REF_VTABLE(color_button), x, y, w, h);
  color_button_t *color_button = COLOR_BUTTON(widget);
  return_value_if_fail(color_button != NULL, NULL);

  color_button->pressed = FALSE;

  return widget;
}

widget_t *color_button_cast(widget_t *widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, color_button), NULL);

  return widget;
}
