/**
 * File:   bar series.c
 * Author: AWTK Develop Team
 * Brief:  bar series
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
 * 2018-12-05 Xu ChaoZe <xuchaoze@zlg.cn> created
 *
 */

#include "bar_series.h"
#include "chart_utils.h"
#include "chart_view.h"
#include "chart_animator.h"

#define SERIES_ANIMATION_TIME 500
#define SERIES_ANIMATION_EASING EASING_SIN_INOUT

#define _COLOR_BLACK color_init(0, 0, 0, 0xff)
#define _COLOR_TRANS color_init(0, 0, 0, 0)

// 坐标取整加0.5，防止线宽为1时显示2个像素
#define _VG_XY(v) (float_t)((xy_t)(v) + 0.5)
#define _VGCANVAS_TRANSLATE(vg, x, y) vgcanvas_translate(vg, _VG_XY(x), _VG_XY(y))

static void bar_series_animator_on_update(chart_animator_t* animator, float_t percent) {
  float_t* from = (float_t*)value_pointer(&(animator->from));
  float_t* to = (float_t*)value_pointer(&(animator->to));
  bar_series_t* series = BAR_SERIES(bar_series_cast(animator->ctx));
  if (from && to && series) {
    uint32_t i;
    for (i = 0; i < series->fifo_size; i++) {
      series->fifo[i] = from[i] + (to[i] - from[i]) * percent;
    }
  }
}

static ret_t bar_series_generate_points(bar_series_t* series, float_t* x0, float_t* y0, float_t** x,
                                        float_t** y, uint32_t* nr, float_t* size,
                                        bool_t* vertical) {
  widget_t* parent = series->base.widget.parent;
  axis_t* x_axis = AXIS(chart_view_get_axis(parent, AXIS_ORIENTATION_X, series->x_axis_index));
  axis_t* y_axis = AXIS(chart_view_get_axis(parent, AXIS_ORIENTATION_Y, series->y_axis_index));
  float_t margin = chart_view_get_bar_margin(parent);
  float_t spacing = chart_view_get_bar_spacing(parent);
  int32_t total = chart_view_count_series(parent, WIDGET_TYPE_BAR_SERIES);
  int32_t index = chart_view_index_of_series(parent, WIDGET_TYPE_BAR_SERIES, WIDGET(series));
  axis_render_t* x_render;
  axis_render_t* y_render;
  float_t v_range;
  float_t p_range;
  uint32_t i;

  if (x_axis == NULL || x_axis->render == NULL || y_axis == NULL || y_axis->render == NULL) {
    return RET_FAIL;
  }
  x_render = x_axis->render;
  y_render = y_axis->render;

  if (x_axis->type == AXIS_TYPE_VALUE && y_axis->type != AXIS_TYPE_VALUE) {
    *vertical = FALSE;
  } else {
    *vertical = TRUE;
  }

  if (vertical) {
    if (y_axis->at == AXIS_AT_RIGHT) {
      *x0 = y_render->ruler.x - x_render->ruler.w + 1;
    } else {
      *x0 = y_render->ruler.x + y_render->ruler.w - 1;
    }
    *y0 = y_render->ruler.y + y_render->ruler.h - 1 - y_render->tick[y_render->index_of_zero];
    *nr = tk_min(x_axis->label.data->size, series->fifo_size);

    if (*nr < 1) {
      return RET_FAIL;
    }

    *x = (float_t*)TKMEM_ZALLOCN(float_t, *nr);
    *y = (float_t*)TKMEM_ZALLOCN(float_t, *nr);
    assert(*x && *y);

    /* 取整以避免柱条之间有空隙 */
    *size = (wh_t)(
        (x_render->tick[1] - x_render->tick[0] - 2 * margin - (total - 1) * spacing + 2) / total);

    v_range = y_axis->label.val_max - y_axis->label.val_min;
    p_range = y_render->ruler.h;
    for (i = 0; i < *nr; i++) {
      (*x)[i] = x_render->tick[i] + margin + (*size) * index + spacing * index;
      (*y)[i] = -series->fifo[i] / v_range * p_range;
    }
  } else {
    if (x_axis->at == AXIS_AT_TOP) {
      *y0 = x_render->ruler.y + x_render->ruler.h - 1;
    } else {
      *y0 = x_render->ruler.y - y_render->ruler.h + 1;
    }
    *x0 = x_render->ruler.x + x_render->tick[x_render->index_of_zero] - 1;
    *nr = tk_min(y_axis->label.data->size, series->fifo_size);

    if (*nr < 1) {
      return RET_FAIL;
    }

    *x = (float_t*)TKMEM_ZALLOCN(float_t, *nr);
    *y = (float_t*)TKMEM_ZALLOCN(float_t, *nr);
    assert(*x && *y);

    /* 取整以避免柱条之间有空隙 */
    *size = (wh_t)(
        (y_render->tick[1] - y_render->tick[0] - 2 * margin - (total - 1) * spacing + 2) / total);

    v_range = x_axis->label.val_max - x_axis->label.val_min;
    p_range = x_render->ruler.w;
    for (i = 0; i < *nr; i++) {
      (*x)[i] = series->fifo[i] / v_range * p_range;
      (*y)[i] = y_render->tick[i] + margin + (*size) * index + spacing * index;
    }
  }

  return RET_OK;
}

static void bar_series_release_points(float_t* x, float_t* y, bool_t vertical) {
  TKMEM_FREE(x);
  TKMEM_FREE(y);
}

static ret_t bar_series_on_paint_self(widget_t* widget, canvas_t* c) {
  float_t x0, y0, size;
  float_t* x;
  float_t* y;
  uint32_t nr;
  bitmap_t img;
  bool_t has_image;
  bool_t vertical = TRUE;
  bar_series_t* series = BAR_SERIES(bar_series_cast(widget));
  vgcanvas_t* vg = canvas_get_vgcanvas(c);

  return_value_if_fail(series != NULL, RET_BAD_PARAMS);

  if (bar_series_generate_points(series, &x0, &y0, &x, &y, &nr, &size, &vertical) != RET_OK) {
    return RET_OK;
  }

  has_image = series->bar.image && widget_load_image(widget, series->bar.image, &img) == RET_OK;

  if (has_image) {
    vgcanvas_save(vg);
    vgcanvas_translate(vg, c->ox, c->oy);
    vgcanvas_translate(vg, x0, y0);
    series_draw_bar(vg, 0, 0, x, y, nr, &(series->bar), size, vertical, has_image ? &img : NULL);
    vgcanvas_restore(vg);
  } else {
    series_draw_bar_c(c, x0, y0, x, y, nr, &(series->bar), size, vertical);
  }

  bar_series_release_points(x, y, vertical);

  return RET_OK;
}

static ret_t bar_series_get_prop(widget_t* widget, const char* name, value_t* v) {
  bar_series_t* series = BAR_SERIES(bar_series_cast(widget));
  return_value_if_fail(series != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, SERIES_PROP_TITLE)) {
    value_set_wstr(v, series->base.title);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_ANIMATIC)) {
    value_set_bool(v, series->base.animatic);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_BAR_COLOR)) {
    value_set_uint32(v, series->bar.color.color);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_BAR_IMAGE)) {
    value_set_str(v, series->bar.image);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static void bar_series_parse_bar(void* ctx, const char* name, const value_t* v) {
  bar_series_t* series = BAR_SERIES(ctx);
  ENSURE(series != NULL && name != NULL && v != NULL);
  if (tk_str_eq(name, "color")) {
    series->bar.color = chart_utils_value_color(v);
  } else if (tk_str_eq(name, "image")) {
    const char* image = value_str(v);
    if (image) {
      TKMEM_FREE(series->bar.image);
      series->bar.image = tk_strdup(image);
    }
  }
}

static ret_t bar_series_set_prop(widget_t* widget, const char* name, const value_t* v) {
  bar_series_t* series = BAR_SERIES(bar_series_cast(widget));
  return_value_if_fail(series != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, SERIES_PROP_TITLE)) {
    const char* title = value_str(v);
    if (title) {
      uint32_t len = strlen(title);
      TKMEM_FREE(series->base.title);
      series->base.title = (wchar_t*)TKMEM_ALLOC(sizeof(wchar_t) * (len + 1));
      utf8_to_utf16(title, series->base.title, len + 1);
    }
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_ANIMATIC)) {
    series->base.animatic = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_CAPACITY)) {
    TKMEM_FREE(series->fifo)
    series->fifo_capacity = value_uint32(v);
    if (series->fifo_capacity) {
      series->fifo = TKMEM_ZALLOCN(float_t, series->fifo_capacity);
    } else {
      series->fifo = NULL;
    }
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_BAR)) {
    return chart_utils_object_parse(bar_series_parse_bar, series, value_str(v));
  } else if (tk_str_eq(name, SERIES_PROP_BAR_COLOR)) {
    series->bar.color = chart_utils_value_color(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_BAR_IMAGE)) {
    const char* image = value_str(v);
    if (image) {
      TKMEM_FREE(series->bar.image);
      series->bar.image = tk_strdup(image);
    }
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t bar_series_on_destroy(widget_t* widget) {
  bar_series_t* series = BAR_SERIES(bar_series_cast(widget));
  return_value_if_fail(series != NULL, RET_BAD_PARAMS);

  TKMEM_FREE(series->base.title);
  TKMEM_FREE(series->bar.image);
  TKMEM_FREE(series->fifo);

  return RET_OK;
}

static const char* s_bar_series_properties[] = {SERIES_PROP_TITLE, SERIES_PROP_ANIMATIC,
                                                SERIES_PROP_BAR_COLOR, SERIES_PROP_BAR_IMAGE, NULL};

static const widget_vtable_t s_bar_series_vtable = {
    .size = sizeof(bar_series_t),
    .type = WIDGET_TYPE_BAR_SERIES,
    .clone_properties = s_bar_series_properties,
    .persistent_properties = s_bar_series_properties,
    .create = bar_series_create,
    .on_paint_self = bar_series_on_paint_self,
    .set_prop = bar_series_set_prop,
    .get_prop = bar_series_get_prop,
    .on_destroy = bar_series_on_destroy};

widget_t* bar_series_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  bar_series_t* series = TKMEM_ZALLOC(bar_series_t);
  widget_t* widget = WIDGET(series);
  return_value_if_fail(series != NULL, NULL);

  widget_init(widget, parent, &s_bar_series_vtable, x, y, w, h);

  series->base.animatic = TRUE;
  series->bar.color = _COLOR_BLACK;

  widget_t* win = widget_get_window(widget);
  if (win) {
    widget_on(win, EVT_WINDOW_OPEN, series_on_window_open, widget);
  }

  return widget;
}

ret_t bar_series_set_data(widget_t* widget, const float_t* data, uint32_t nr) {
  float_t* fifo;
  uint32_t fifo_size;
  uint32_t fifo_capacity;
  bar_series_t* series = BAR_SERIES(bar_series_cast(widget));
  return_value_if_fail(series != NULL && data != NULL && nr > 0, RET_BAD_PARAMS);

  fifo = series->fifo;
  fifo_size = series->fifo_size;
  fifo_capacity = series->fifo_capacity;
  return_value_if_fail(fifo != NULL && fifo_capacity > 0, RET_BAD_PARAMS);

  if (series->base.animatic) {
    widget_animator_t* wa = NULL;
    widget_t* win = widget_get_window(widget);
    uint32_t size = tk_min(fifo_size + nr, fifo_capacity);
    float_t* from = (float_t*)TKMEM_ZALLOCN(float_t, size);
    float_t* to = (float_t*)TKMEM_ZALLOCN(float_t, size);

    /* 销毁旧的动画对象, 避免line_series_animator_on_update时由于fifo_size更新导致from/to数组越界 */
    widget_destroy_animator(widget, NULL);

    /* 创建新的动画 */
    wa = chart_animator_create(widget, SERIES_ANIMATION_TIME, 0, SERIES_ANIMATION_EASING);
    assert(wa != NULL && from != NULL && to != NULL);
    series_animator_floats_set_params(wa, from, to, (void*)series, bar_series_animator_on_update,
                                      series_animator_floats_on_destroy);

    memcpy(from, fifo, sizeof(float_t) * fifo_size);
    memcpy(to, fifo, sizeof(float_t) * fifo_size);
    series_set_data_float(to, &(series->fifo_size), fifo_capacity, data, nr);

    if (win && widget_is_window_opened(win)) {
      widget_animator_start(wa);
    }
  } else {
    series_set_data_float(fifo, &(series->fifo_size), fifo_capacity, data, nr);
    widget_invalidate(widget, NULL);
  }

  return RET_OK;
}

ret_t bar_series_append(widget_t* widget, const float_t* data, uint32_t nr) {
  float_t* fifo;
  uint32_t fifo_size;
  uint32_t fifo_capacity;
  bar_series_t* series = BAR_SERIES(bar_series_cast(widget));
  return_value_if_fail(series != NULL && data != NULL && nr > 0, RET_BAD_PARAMS);

  fifo = series->fifo;
  fifo_size = series->fifo_size;
  fifo_capacity = series->fifo_capacity;
  return_value_if_fail(fifo != NULL && fifo_capacity > 0, RET_BAD_PARAMS);

  if (series->base.animatic) {
    widget_animator_t* wa = NULL;
    widget_t* win = widget_get_window(widget);
    uint32_t size = tk_min(fifo_size + nr, fifo_capacity);
    float_t* from = (float_t*)TKMEM_ZALLOCN(float_t, size);
    float_t* to = (float_t*)TKMEM_ZALLOCN(float_t, size);

    /* 销毁旧的动画对象, 避免line_series_animator_on_update时由于fifo_size更新导致from/to数组越界 */
    widget_destroy_animator(widget, NULL);

    /* 创建新的动画 */
    wa = chart_animator_create(widget, SERIES_ANIMATION_TIME, 0, SERIES_ANIMATION_EASING);
    assert(wa != NULL && from != NULL && to != NULL);
    series_animator_floats_set_params(wa, from, to, (void*)series, bar_series_animator_on_update,
                                      series_animator_floats_on_destroy);

    memcpy(to, fifo, sizeof(float_t) * fifo_size);
    series_fifo_float(to, &(series->fifo_size), fifo_capacity, data, nr);
    if (nr < fifo_capacity) {
      memcpy(from, to, sizeof(float_t) * (series->fifo_size - nr));
      memcpy(fifo, to, sizeof(float_t) * (series->fifo_size - nr));
    }

    if (win && widget_is_window_opened(win)) {
      widget_animator_start(wa);
    }
  } else {
    series_fifo_float(fifo, &(series->fifo_size), fifo_capacity, data, nr);
    widget_invalidate(widget, NULL);
  }

  return RET_OK;
}

void bar_series_clear(widget_t* widget) {
  bar_series_t* series = BAR_SERIES(bar_series_cast(widget));
  return_if_fail(series != NULL);
  series->fifo_size = 0;
}

widget_t* bar_series_cast(widget_t* widget) {
  return_value_if_fail(widget != NULL && (widget->vt == &s_bar_series_vtable), NULL);

  return widget;
}
