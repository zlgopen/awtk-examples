#include "tkc/mem.h"
#include "base/timer.h"
#include "tkc/utils.h"
#include "watch_clock.h"
#include "base/widget_vtable.h"
#include "watch_clock_pointer.h"

static ret_t watch_clock_get_prop(widget_t *widget, const char *name,
                                  value_t *v) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL && widget != NULL, RET_BAD_PARAMS);
  if (tk_str_eq(name, WATCH_CLOCK_PROP_BG_IMAGE)) {
    value_set_str(v, watch_clock->bg_image);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_IMAGE)) {
    value_set_str(v, watch_clock->image);
    return RET_OK;
  }
  return RET_NOT_FOUND;
}

static ret_t watch_clock_set_prop(widget_t *widget, const char *name,
                                  const value_t *v) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL && widget != NULL, RET_BAD_PARAMS);
  if (tk_str_eq(name, WATCH_CLOCK_PROP_BG_IMAGE)) {
    return watch_clock_set_bg_image(widget, value_str(v));
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_IMAGE)) {
    return watch_clock_set_image(widget, value_str(v));
  }
  return RET_NOT_FOUND;
}

static ret_t watch_clock_on_paint_end_self(widget_t *widget, canvas_t *c) {
  bitmap_t bitmap;
  rect_t dst = rect_init(0, 0, widget->w, widget->h);
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL, RET_BAD_PARAMS);

  if (watch_clock->image != NULL) {
    if (widget_load_image(widget, watch_clock->image, &bitmap) == RET_OK) {
      return canvas_draw_image_ex(c, &bitmap, IMAGE_DRAW_CENTER, &dst);
    }
  }

  return RET_OK;
}

static ret_t watch_clock_on_paint_background_self(widget_t *widget,
                                                  canvas_t *c) {
  bitmap_t bitmap;
  rect_t dst = rect_init(0, 0, widget->w, widget->h);
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL, RET_BAD_PARAMS);

  if (watch_clock->bg_image != NULL) {
    if (widget_load_image(widget, watch_clock->bg_image, &bitmap) == RET_OK) {
      return canvas_draw_image_ex(c, &bitmap, IMAGE_DRAW_CENTER, &dst);
    }
  }

  return RET_OK;
}

static ret_t watch_clock_on_paint_self(widget_t *widget, canvas_t *c) {
  (void)c;
  (void)widget;
  return RET_OK;
}

ret_t watch_clock_update_child_widget_idle_func(const idle_info_t *idle) {
  widget_t *widget = WIDGET(idle->ctx);
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  if (tk_str_cmp(iter->vt->type, WIDGET_TYPE_WATCH_CLOCK_POINTER) != 0) {
    continue;
  }
  if (watch_clock_pointer_get_clock_pointer_type(iter) ==
      WATCH_CLOCK_POINTER_HOUR) {
    watch_clock->hour_widget = iter;
    watch_clock_pointer_set_value(
        iter, (uint8_t)(watch_clock->hour * 5 + watch_clock->minute / 12));
  } else if (watch_clock_pointer_get_clock_pointer_type(iter) ==
             WATCH_CLOCK_POINTER_MINUTE) {
    watch_clock->minute_widget = iter;
    watch_clock_pointer_set_value(iter, (uint8_t)watch_clock->minute);
  } else if (watch_clock_pointer_get_clock_pointer_type(iter) ==
             WATCH_CLOCK_POINTER_SECOND) {
    watch_clock->second_widget = iter;
    watch_clock_pointer_set_value(iter, (uint8_t)watch_clock->second);
  }
  WIDGET_FOR_EACH_CHILD_END();
  return RET_OK;
}

static ret_t watch_clock_update_time(watch_clock_t *watch_clock) {
  date_time_t dt;

  if (date_time_init(&dt) != NULL) {
    watch_clock->hour = dt.hour % 12;
    watch_clock->minute = dt.minute;
    watch_clock->second = dt.second;

    return RET_OK;
  } else {
    return RET_FAIL;
  }
}

static ret_t watch_clock_on_timer(const timer_info_t *info) {
  widget_t *widget = NULL;
  watch_clock_t *watch_clock = NULL;
  return_value_if_fail(info != NULL, RET_REMOVE);

  widget = WIDGET(info->ctx);
  watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(widget != NULL && watch_clock != NULL, RET_BAD_PARAMS);

  if (watch_clock_update_time(watch_clock) != RET_OK) {
    watch_clock->second++;
    if (watch_clock->second >= 60) {
      watch_clock->second = 0;
      watch_clock->minute++;
      if (watch_clock->minute >= 60) {
        watch_clock->minute = 0;
        watch_clock->hour++;
        if (watch_clock->hour >= 12) {
          watch_clock->hour = 0;
        }
      }
    }
  }

  if (watch_clock->hour_widget != NULL) {
    watch_clock_pointer_set_value(
        watch_clock->hour_widget,
        (uint8_t)(watch_clock->hour * 5 + watch_clock->minute / 12));
  }
  if (watch_clock->minute_widget != NULL) {
    watch_clock_pointer_set_value(watch_clock->minute_widget,
                                  (uint8_t)watch_clock->minute);
  }
  if (watch_clock->second_widget != NULL) {
    watch_clock_pointer_set_value(watch_clock->second_widget,
                                  (uint8_t)watch_clock->second);
  }

  widget_invalidate_force(widget, NULL);

  return RET_REPEAT;
}

static ret_t watch_clock_on_destroy(widget_t *widget) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(widget != NULL && watch_clock != NULL, RET_BAD_PARAMS);

  if (watch_clock->image != NULL) {
    TKMEM_FREE(watch_clock->image);
  }
  if (watch_clock->bg_image != NULL) {
    TKMEM_FREE(watch_clock->bg_image);
  }
  return RET_OK;
}

static const char *const s_watch_clock_properties[] = {
    WATCH_CLOCK_PROP_HOUR,  WATCH_CLOCK_PROP_MINUTE,   WATCH_CLOCK_PROP_SECOND,
    WATCH_CLOCK_PROP_IMAGE, WATCH_CLOCK_PROP_BG_IMAGE, NULL};

TK_DECL_VTABLE(watch_clock) = {
    .size = sizeof(watch_clock_t),
    .type = WIDGET_TYPE_WATCH_CLOCK,
    .space_key_to_activate = TRUE,
    .return_key_to_activate = TRUE,
    .parent = TK_PARENT_VTABLE(widget),
    .create = watch_clock_create,
    .clone_properties = s_watch_clock_properties,
    .persistent_properties = s_watch_clock_properties,
    .set_prop = watch_clock_set_prop,
    .get_prop = watch_clock_get_prop,
    .on_paint_background = watch_clock_on_paint_background_self,
    .on_paint_end = watch_clock_on_paint_end_self,
    .on_destroy = watch_clock_on_destroy,
    .on_paint_self = watch_clock_on_paint_self};

widget_t *watch_clock_create(widget_t *parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t *widget =
      widget_create(parent, TK_REF_VTABLE(watch_clock), x, y, w, h);
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL, NULL);

  watch_clock->hour = 0;
  watch_clock->minute = 0;
  watch_clock->second = 0;
  watch_clock->image = NULL;
  watch_clock->bg_image = NULL;

  watch_clock_update_time(watch_clock);
  return_value_if_fail(widget_add_timer(widget, watch_clock_on_timer, 1000) > 0,
                       NULL);
  return_value_if_fail(
      idle_add(watch_clock_update_child_widget_idle_func, widget) > 0, NULL);

  return widget;
}

widget_t *watch_clock_cast(widget_t *widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, watch_clock), NULL);

  return widget;
}

ret_t watch_clock_set_bg_image(widget_t *widget, const char *bg_image) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL, RET_BAD_PARAMS);

  watch_clock->bg_image = tk_str_copy(watch_clock->bg_image, bg_image);

  return RET_OK;
}

ret_t watch_clock_set_image(widget_t *widget, const char *image) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL, RET_BAD_PARAMS);

  watch_clock->image = tk_str_copy(watch_clock->image, image);

  return RET_OK;
}
