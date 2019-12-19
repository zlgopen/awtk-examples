#include "guage_hour_pointer.h"
#ifdef _WIN32
#pragma warning(disable : 4244)
#endif  // _WIN32

widget_t* guage_hour_pointer_cast(widget_t* widget) {
  return_value_if_fail(widget != NULL, NULL);
  return widget;
}

ret_t guage_hour_pointer_set_image(widget_t* widget, const char* name) {
  return_value_if_fail(widget != NULL && name != NULL && strlen(name) != 0, RET_BAD_PARAMS);
  guage_hour_pointer_t* pointer = GUAGE_HOUR_POINTER(widget);
  str_set(&(pointer->image), name);
  return RET_OK;
}

ret_t guage_hour_pointer_set_hour(widget_t* widget) {
  date_time_t dt;
  guage_hour_pointer_t* pointer = GUAGE_HOUR_POINTER(widget);
  return_value_if_fail(pointer != NULL, RET_BAD_PARAMS);

  date_time_init(&dt);
  pointer->hour = (dt.minute / 60.0 + dt.hour % 12) * (360 / 12);

  return RET_OK;
}

static ret_t guage_hour_pointer_on_paint_self(widget_t* widget, canvas_t* c) {
  return_value_if_fail(widget != NULL && c != NULL, RET_BAD_PARAMS);
  vgcanvas_t* vg = canvas_get_vgcanvas(c);
  guage_hour_pointer_t* pointer = GUAGE_HOUR_POINTER(widget);

  if (pointer->image.size > 0) {
    bitmap_t image;
    widget_load_image(widget, pointer->image.str, &image);
    float_t i_w = image.w;
    float_t i_h = image.h;
    float_t w = widget->w;
    float_t h = widget->h;

    vgcanvas_save(vg);
    vgcanvas_translate(vg, c->ox, c->oy);
    vgcanvas_translate(vg, w / 2.0, h / 2.0);
    vgcanvas_rotate(vg, TK_D2R(pointer->hour));
    vgcanvas_transform(vg, 1, 0, 0, 1, -(i_w / 2), -(i_h / 2));
    vgcanvas_draw_image(vg, &image, 0, 0, i_w, i_h, 0, 0, i_w, i_h);
    vgcanvas_restore(vg);
  }

  return RET_OK;
}

static ret_t guage_hour_pointer_on_destroy(widget_t* widget) {
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);
  guage_hour_pointer_t* pointer = GUAGE_HOUR_POINTER(widget);
  str_reset(&(pointer->image));
  return RET_OK;
}

static ret_t guage_hour_pointer_get_prop(widget_t* widget, const char* name, value_t* v) {
  (void)name;
  (void)v;
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);
  return RET_NOT_FOUND;
}

static ret_t guage_hour_pointer_set_prop(widget_t* widget, const char* name, const value_t* v) {
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);
  if (tk_str_eq(name, WIDGET_PROP_IMAGE)) {
    return guage_hour_pointer_set_image(widget, value_str(v));
  }

  return RET_NOT_FOUND;
}

static const char* s_guage_hour_pointer_properties[] = {NULL};

static const widget_vtable_t s_guage_hour_pointer_vtable = {
    .size = sizeof(guage_hour_pointer_t),
    .type = WIDGET_TYPE_GUAGE_HOUR_POINTER,
    .clone_properties = s_guage_hour_pointer_properties,
    .persistent_properties = s_guage_hour_pointer_properties,
    .create = guage_hour_pointer_create,
    .on_paint_self = guage_hour_pointer_on_paint_self,
    .on_destroy = guage_hour_pointer_on_destroy,
    .get_prop = guage_hour_pointer_get_prop,
    .set_prop = guage_hour_pointer_set_prop};

static ret_t on_timer(const timer_info_t* timer) {
  widget_t* widget = WIDGET(timer->ctx);
  guage_hour_pointer_set_hour(widget);
  widget_invalidate_force(widget, NULL);
  return RET_REPEAT;
}

widget_t* guage_hour_pointer_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  return_value_if_fail(parent != NULL, NULL);
  widget_t* widget = widget_create(parent, &s_guage_hour_pointer_vtable, x, y, w, h);
  widget->sensitive = FALSE;
  guage_hour_pointer_t* pointer = GUAGE_HOUR_POINTER(widget);
  str_init(&(pointer->image), 0);
  guage_hour_pointer_set_hour(widget);
  widget_add_timer(widget, on_timer, 1000);
  return widget;
}