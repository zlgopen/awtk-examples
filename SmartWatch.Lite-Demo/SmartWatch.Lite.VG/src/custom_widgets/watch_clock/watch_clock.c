/**
 * File:   watch_clock.h
 * Author: AWTK Develop Team
 * Brief:  watch_clock
 *
 * Copyright (c) 2018 - 2020  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2018-08-29 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "base/timer.h"
#include "tkc/utils.h"
#include "tkc/matrix.h"
#include "watch_clock.h"
#include "tkc/date_time.h"
#include "base/image_manager.h"
#include "widget_animators/widget_animator_prop.h"

#define START_ANIMATOR_TIME 1000

static ret_t watch_clock_update_time(watch_clock_t *watch_clock);

static bool_t watch_clock_value_is_anchor_px(const char *value) {
  const char *tmp = NULL;
  size_t len = strlen(value);
  return_value_if_fail(len > 2, FALSE);

  tmp = value + len - 2;
  if (tk_str_eq(tmp, "px") != 0 || tk_str_eq(tmp, "PX") != 0) {
    return TRUE;
  }
  return FALSE;
}

ret_t watch_clock_set_anchor_for_str(float_t max_size, const char *anchor,
                                     float_t *image_anchor) {
  float_t anchor_tmp = 0.0f;
  bool_t is_anchor_px = TRUE;

  return_value_if_fail(anchor != NULL, RET_BAD_PARAMS);

  anchor_tmp = (float_t)tk_atof(anchor);
  is_anchor_px = watch_clock_value_is_anchor_px(anchor);

  if (is_anchor_px) {
    return_value_if_fail(0 <= anchor_tmp && anchor_tmp <= max_size,
                         RET_BAD_PARAMS);
  } else {
    return_value_if_fail(0 <= anchor_tmp && anchor_tmp <= 1.0f, RET_BAD_PARAMS);
  }

  *image_anchor = is_anchor_px ? anchor_tmp : anchor_tmp * max_size;

  return RET_OK;
}

ret_t watch_clock_set_hour_anchor(widget_t *widget, const char *anchor_x,
                                  const char *anchor_y) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);

  return_value_if_fail(watch_clock != NULL, RET_BAD_PARAMS);

  if (anchor_x != NULL) {
    watch_clock->hour_anchor_x =
        tk_str_copy(watch_clock->hour_anchor_x, anchor_x);
  }
  if (anchor_y != NULL) {
    watch_clock->hour_anchor_y =
        tk_str_copy(watch_clock->hour_anchor_y, anchor_y);
  }

  return RET_OK;
}

ret_t watch_clock_set_minute_anchor(widget_t *widget, const char *anchor_x,
                                    const char *anchor_y) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);

  return_value_if_fail(watch_clock != NULL, RET_BAD_PARAMS);

  if (anchor_x != NULL) {
    watch_clock->minute_anchor_x =
        tk_str_copy(watch_clock->minute_anchor_x, anchor_x);
  }
  if (anchor_y != NULL) {
    watch_clock->minute_anchor_y =
        tk_str_copy(watch_clock->minute_anchor_y, anchor_y);
  }

  return RET_OK;
}

ret_t watch_clock_set_second_anchor(widget_t *widget, const char *anchor_x,
                                    const char *anchor_y) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);

  return_value_if_fail(watch_clock != NULL, RET_BAD_PARAMS);

  if (anchor_x != NULL) {
    watch_clock->second_anchor_x =
        tk_str_copy(watch_clock->second_anchor_x, anchor_x);
  }
  if (anchor_y != NULL) {
    watch_clock->second_anchor_y =
        tk_str_copy(watch_clock->second_anchor_y, anchor_y);
  }

  return RET_OK;
}

ret_t watch_clock_set_smooth(widget_t *widget, bool_t smooth) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL, RET_BAD_PARAMS);

  watch_clock->is_smooth = smooth;
  return RET_OK;
}

ret_t watch_clock_set_hour(widget_t *widget, float_t hour) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL, RET_BAD_PARAMS);

  watch_clock->hour = hour;
  widget_invalidate(widget, NULL);
  return RET_OK;
}

ret_t watch_clock_set_minute(widget_t *widget, float_t minute) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL, RET_BAD_PARAMS);

  watch_clock->minute = minute;
  widget_invalidate(widget, NULL);
  return RET_OK;
}

ret_t watch_clock_set_second(widget_t *widget, float_t second) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL, RET_BAD_PARAMS);

  watch_clock->second = second;
  widget_invalidate(widget, NULL);
  return RET_OK;
}

ret_t watch_clock_set_hour_image(widget_t *widget, const char *hour_image) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL, RET_BAD_PARAMS);

  watch_clock->hour_image = tk_str_copy(watch_clock->hour_image, hour_image);

  return RET_OK;
}

ret_t watch_clock_set_minute_image(widget_t *widget, const char *minute_image) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL, RET_BAD_PARAMS);

  watch_clock->minute_image =
      tk_str_copy(watch_clock->minute_image, minute_image);

  return RET_OK;
}

ret_t watch_clock_set_second_image(widget_t *widget, const char *second_image) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL, RET_BAD_PARAMS);

  watch_clock->second_image =
      tk_str_copy(watch_clock->second_image, second_image);

  return RET_OK;
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

static ret_t watch_clock_get_prop(widget_t *widget, const char *name,
                                  value_t *v) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL && name != NULL && v != NULL,
                       RET_BAD_PARAMS);

  if (tk_str_eq(name, WATCH_CLOCK_PROP_HOUR)) {
    value_set_float(v, watch_clock->hour);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_MINUTE)) {
    value_set_float(v, watch_clock->minute);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_SECOND)) {
    value_set_float(v, watch_clock->second);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_HOUR_IMAGE)) {
    value_set_str(v, watch_clock->hour_image);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_SECOND_IMAGE)) {
    value_set_str(v, watch_clock->second_image);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_MINUTE_IMAGE)) {
    value_set_str(v, watch_clock->minute_image);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_BG_IMAGE)) {
    value_set_str(v, watch_clock->bg_image);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_IMAGE)) {
    value_set_str(v, watch_clock->image);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_HOUR_ANCHOR_X)) {
    value_set_str(v, watch_clock->hour_anchor_x);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_HOUR_ANCHOR_Y)) {
    value_set_str(v, watch_clock->hour_anchor_y);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_MINUTE_ANCHOR_X)) {
    value_set_str(v, watch_clock->minute_anchor_x);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_MINUTE_ANCHOR_Y)) {
    value_set_str(v, watch_clock->minute_anchor_y);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_SECOND_ANCHOR_X)) {
    value_set_str(v, watch_clock->second_anchor_x);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_SECOND_ANCHOR_Y)) {
    value_set_str(v, watch_clock->second_anchor_y);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_CLOCK_SMOOTH)) {
    value_set_bool(v, watch_clock->is_smooth);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_START_ANIMATOR)) {
    value_set_bool(v, watch_clock->is_animator);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t watch_clock_set_prop(widget_t *widget, const char *name,
                                  const value_t *v) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(widget != NULL && name != NULL && v != NULL,
                       RET_BAD_PARAMS);

  if (tk_str_eq(name, WATCH_CLOCK_PROP_HOUR)) {
    return watch_clock_set_hour(widget, value_float(v));
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_MINUTE)) {
    return watch_clock_set_minute(widget, value_float(v));
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_SECOND)) {
    return watch_clock_set_second(widget, value_float(v));
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_HOUR_IMAGE)) {
    return watch_clock_set_hour_image(widget, value_str(v));
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_SECOND_IMAGE)) {
    return watch_clock_set_second_image(widget, value_str(v));
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_MINUTE_IMAGE)) {
    return watch_clock_set_minute_image(widget, value_str(v));
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_BG_IMAGE)) {
    return watch_clock_set_bg_image(widget, value_str(v));
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_IMAGE)) {
    return watch_clock_set_image(widget, value_str(v));
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_HOUR_ANCHOR_X)) {
    return watch_clock_set_hour_anchor(widget, value_str(v), NULL);
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_HOUR_ANCHOR_Y)) {
    return watch_clock_set_hour_anchor(widget, NULL, value_str(v));
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_MINUTE_ANCHOR_X)) {
    return watch_clock_set_minute_anchor(widget, value_str(v), NULL);
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_MINUTE_ANCHOR_Y)) {
    return watch_clock_set_minute_anchor(widget, NULL, value_str(v));
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_SECOND_ANCHOR_X)) {
    return watch_clock_set_second_anchor(widget, value_str(v), NULL);
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_SECOND_ANCHOR_Y)) {
    return watch_clock_set_second_anchor(widget, NULL, value_str(v));
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_CLOCK_SMOOTH)) {
    return watch_clock_set_smooth(widget, value_bool(v));
  } else if (tk_str_eq(name, WATCH_CLOCK_PROP_START_ANIMATOR)) {
    watch_clock->is_animator = value_bool(v);
    watch_clock->hour = 0.0f;
    watch_clock->minute = 0.0f;
    watch_clock->second = 0.0f;
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t watch_clock_on_timer(const timer_info_t *info) {
  widget_t *widget = NULL;
  watch_clock_t *watch_clock = NULL;
  return_value_if_fail(info != NULL, RET_REMOVE);

  widget = WIDGET(info->ctx);
  watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(widget != NULL && watch_clock != NULL, RET_BAD_PARAMS);

  if (watch_clock->is_animator && !watch_clock->is_animator_end) {
    return RET_REPEAT;
  }

  if (watch_clock_update_time(watch_clock) != RET_OK) {
    watch_clock->second++;
    if (watch_clock->second >= 60.0f) {
      watch_clock->second = 0.0f;
      watch_clock->minute++;
      if (watch_clock->minute >= 60.0f) {
        watch_clock->minute = 0.0f;
        watch_clock->hour++;
        if (watch_clock->hour >= 12.0f) {
          watch_clock->hour = 0.0f;
        }
      }
    }
  }

  widget_invalidate_force(widget, NULL);

  return RET_REPEAT;
}

static ret_t watch_clock_on_destroy(widget_t *widget) {
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(widget != NULL && watch_clock != NULL, RET_BAD_PARAMS);

  TKMEM_FREE(watch_clock->image);
  TKMEM_FREE(watch_clock->bg_image);
  TKMEM_FREE(watch_clock->hour_image);
  TKMEM_FREE(watch_clock->minute_image);
  TKMEM_FREE(watch_clock->second_image);

  TKMEM_FREE(watch_clock->hour_anchor_x);
  TKMEM_FREE(watch_clock->hour_anchor_y);
  TKMEM_FREE(watch_clock->minute_anchor_x);
  TKMEM_FREE(watch_clock->minute_anchor_y);
  TKMEM_FREE(watch_clock->second_anchor_x);
  TKMEM_FREE(watch_clock->second_anchor_y);

  return RET_OK;
}

static ret_t watch_clock_load_image(widget_t *widget, const char *name,
                                    bitmap_t *bitmap) {
  if (name != NULL && bitmap != NULL) {
    return widget_load_image(widget, name, bitmap);
  }

  return RET_FAIL;
}

static ret_t watch_clock_draw_image(widget_t *widget, canvas_t *c,
                                    bitmap_t *img, float_t dx, float_t dy,
                                    float_t anchor_x, float_t anchor_y,
                                    float_t rotation) {
  vgcanvas_t *vg = canvas_get_vgcanvas(c);

  vgcanvas_save(vg);
  vgcanvas_translate(vg, c->ox + dx, c->oy + dy);
  vgcanvas_translate(vg, anchor_x, anchor_y);
  vgcanvas_rotate(vg, rotation);
  vgcanvas_translate(vg, -anchor_x, -anchor_y);
  vgcanvas_draw_image(vg, img, 0.0f, 0.0f, (float_t)img->w, (float_t)img->h,
                      0.0f, 0.0f, (float_t)img->w, (float_t)img->h);
  vgcanvas_restore(vg);

  return RET_OK;
}

static ret_t watch_clock_on_paint_background(widget_t *widget, canvas_t *c) {
  bitmap_t bitmap;
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  rect_t dst = rect_init(0, 0, widget->w, widget->h);
  if (watch_clock_load_image(widget, watch_clock->bg_image, &bitmap) ==
      RET_OK) {
    return canvas_draw_image_ex(c, &bitmap, IMAGE_DRAW_CENTER, &dst);
  }
  return RET_OK;
}

static ret_t watch_clock_on_paint_end(widget_t *widget, canvas_t *c) {
  bitmap_t bitmap;
  float_t rotation = 0;
  float_t anchor_x = 0;
  float_t anchor_y = 0;
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  rect_t dst = rect_init(0, 0, widget->w, widget->h);

  if (watch_clock_load_image(widget, watch_clock->hour_image, &bitmap) ==
          RET_OK &&
      watch_clock_set_anchor_for_str(
          (float_t)bitmap.w, watch_clock->hour_anchor_x, &anchor_x) == RET_OK &&
      watch_clock_set_anchor_for_str(
          (float_t)bitmap.h, watch_clock->hour_anchor_y, &anchor_y) == RET_OK) {
    float_t dx = dst.w / 2 - anchor_x;
    float_t dy = dst.h / 2 - anchor_y;
    float_t hour = watch_clock->hour + watch_clock->minute / 60.0f;

    rotation = (2 * M_PI * hour) / 12.0f;

    watch_clock_draw_image(widget, c, &bitmap, dx, dy, anchor_x, anchor_y,
                           rotation);
  }

  if (watch_clock_load_image(widget, watch_clock->minute_image, &bitmap) ==
          RET_OK &&
      watch_clock_set_anchor_for_str((float_t)bitmap.w,
                                     watch_clock->minute_anchor_x,
                                     &anchor_x) == RET_OK &&
      watch_clock_set_anchor_for_str((float_t)bitmap.h,
                                     watch_clock->minute_anchor_y,
                                     &anchor_y) == RET_OK) {
    float_t dx = dst.w / 2 - anchor_x;
    float_t dy = dst.h / 2 - anchor_y;
    float_t minute = watch_clock->minute + watch_clock->second / 60.0f;

    rotation = (2 * M_PI * minute) / 60.0f;

    watch_clock_draw_image(widget, c, &bitmap, dx, dy, anchor_x, anchor_y,
                           rotation);
  }

  if (watch_clock_load_image(widget, watch_clock->second_image, &bitmap) ==
          RET_OK &&
      watch_clock_set_anchor_for_str((float_t)bitmap.w,
                                     watch_clock->second_anchor_x,
                                     &anchor_x) == RET_OK &&
      watch_clock_set_anchor_for_str((float_t)bitmap.h,
                                     watch_clock->second_anchor_y,
                                     &anchor_y) == RET_OK) {
    float_t dx = dst.w / 2 - anchor_x;
    float_t dy = dst.h / 2 - anchor_y;

    rotation = (2 * M_PI * watch_clock->second) / 60.0f;

    watch_clock_draw_image(widget, c, &bitmap, dx, dy, anchor_x, anchor_y,
                           rotation);
  }

  if (watch_clock_load_image(widget, watch_clock->image, &bitmap) == RET_OK) {
    canvas_draw_image_ex(c, &bitmap, IMAGE_DRAW_CENTER, &dst);
  }

  return RET_OK;
}

static const char *s_watch_clock_properties[] = {
    WATCH_CLOCK_PROP_HOUR, WATCH_CLOCK_PROP_MINUTE, WATCH_CLOCK_PROP_SECOND,
    WATCH_CLOCK_PROP_IMAGE, WATCH_CLOCK_PROP_BG_IMAGE,
    WATCH_CLOCK_PROP_HOUR_IMAGE, WATCH_CLOCK_PROP_MINUTE_IMAGE,
    WATCH_CLOCK_PROP_SECOND_IMAGE,
    WATCH_CLOCK_PROP_HOUR_ANCHOR_X WATCH_CLOCK_PROP_HOUR_ANCHOR_Y,
    WATCH_CLOCK_PROP_MINUTE_ANCHOR_X,
    WATCH_CLOCK_PROP_MINUTE_ANCHOR_Y WATCH_CLOCK_PROP_SECOND_ANCHOR_X,
    WATCH_CLOCK_PROP_SECOND_ANCHOR_Y, WATCH_CLOCK_PROP_START_ANIMATOR,
    WATCH_CLOCK_PROP_CLOCK_SMOOTH, NULL};

TK_DECL_VTABLE(watch_clock) = {
    .size = sizeof(watch_clock_t),
    .type = WIDGET_TYPE_WATCH_CLOCK,
    .clone_properties = s_watch_clock_properties,
    .persistent_properties = s_watch_clock_properties,
    .parent = TK_PARENT_VTABLE(widget),
    .create = watch_clock_create,
    .on_paint_background = watch_clock_on_paint_background,
    .on_paint_end = watch_clock_on_paint_end,
    .set_prop = watch_clock_set_prop,
    .get_prop = watch_clock_get_prop,
    .on_destroy = watch_clock_on_destroy};

static ret_t watch_clock_update_time(watch_clock_t *watch_clock) {
  date_time_t dt;

  if (date_time_init(&dt) != NULL) {
    watch_clock->hour = (float_t)(dt.hour % 12);
    watch_clock->minute = (float_t)dt.minute;
    watch_clock->second = (float_t)dt.second;

    return RET_OK;
  } else {
    return RET_FAIL;
  }
}

static ret_t watch_clock_start_animator_on_done(void *ctx, event_t *e) {
  watch_clock_t *watch_clock = WATCH_CLOCK(ctx);
  watch_clock->is_animator_end = TRUE;
  watch_clock_set_smooth(WIDGET(ctx), watch_clock->is_last_smooth);
  return RET_OK;
}

ret_t watch_clock_start_animator(widget_t *widget) {
  date_time_t dt;
  widget_animator_t *animator_hour = NULL;
  widget_animator_t *animator_minute = NULL;
  widget_animator_t *animator_second = NULL;
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL && watch_clock->is_animator,
                       RET_BAD_PARAMS);

  if (date_time_init(&dt) == NULL) {
    watch_clock->is_animator = FALSE;
    watch_clock->is_animator_end = FALSE;
    return RET_BAD_PARAMS;
  }

  animator_hour =
      widget_animator_prop_create(widget, START_ANIMATOR_TIME, 0,
                                  EASING_QUADRATIC_IN, WATCH_CLOCK_PROP_HOUR);
  animator_minute =
      widget_animator_prop_create(widget, START_ANIMATOR_TIME, 0,
                                  EASING_QUADRATIC_IN, WATCH_CLOCK_PROP_MINUTE);
  animator_second =
      widget_animator_prop_create(widget, START_ANIMATOR_TIME, 0,
                                  EASING_QUADRATIC_IN, WATCH_CLOCK_PROP_SECOND);

  return_value_if_fail(animator_hour != NULL && animator_minute != NULL &&
                           animator_second != NULL,
                       RET_OOM);

  widget_animator_prop_set_params(animator_hour, 0.0f, (float_t)(dt.hour % 12));
  widget_animator_prop_set_params(animator_minute, 0.0f, (float_t)dt.minute);
  widget_animator_prop_set_params(animator_second, 0.0f, (float_t)dt.second);

  widget_animator_on(animator_hour, EVT_ANIM_END,
                     watch_clock_start_animator_on_done, widget);

  widget_animator_start(animator_hour);
  widget_animator_start(animator_minute);
  widget_animator_start(animator_second);

  if (!watch_clock->is_smooth) {
    watch_clock->is_last_smooth = watch_clock->is_smooth;
    watch_clock_set_smooth(widget, TRUE);
  }

  return RET_OK;
}

widget_t *watch_clock_create(widget_t *parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t *widget =
      widget_create(parent, TK_REF_VTABLE(watch_clock), x, y, w, h);
  watch_clock_t *watch_clock = WATCH_CLOCK(widget);
  return_value_if_fail(watch_clock != NULL, NULL);

  watch_clock_update_time(watch_clock);
  widget_add_timer(widget, watch_clock_on_timer, 1000);

  watch_clock->is_smooth = FALSE;
  watch_clock->is_animator = FALSE;
  watch_clock->is_animator_end = FALSE;

  watch_clock->hour_anchor_x = tk_str_copy(NULL, "0.5");
  watch_clock->hour_anchor_y = tk_str_copy(NULL, "0.5");
  watch_clock->minute_anchor_x = tk_str_copy(NULL, "0.5");
  watch_clock->minute_anchor_y = tk_str_copy(NULL, "0.5");
  watch_clock->second_anchor_x = tk_str_copy(NULL, "0.5");
  watch_clock->second_anchor_y = tk_str_copy(NULL, "0.5");

  return widget;
}

widget_t *watch_clock_cast(widget_t *widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, watch_clock), NULL);

  return widget;
}
