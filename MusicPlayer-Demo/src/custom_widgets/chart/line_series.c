/**
 * File:   line_series.c
 * Author: AWTK Develop Team
 * Brief:  line series
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

#include "line_series.h"
#include "chart_utils.h"
#include "chart_view.h"

#define SERIES_ANIMATION_TIME 500
#define SERIES_ANIMATION_EASING EASING_SIN_INOUT

#define _COLOR_BLACK color_init(0, 0, 0, 0xff)
#define _COLOR_TRANS color_init(0, 0, 0, 0)

// 坐标取整加0.5，防止线宽为1时显示2个像素
#define _VG_XY(v) (float_t)((xy_t)(v) + 0.5)
#define _VGCANVAS_TRANSLATE(vg, x, y) vgcanvas_translate(vg, _VG_XY(x), _VG_XY(y))

static void line_series_animator_move_on_update(chart_animator_t* animator, float_t percent) {
  float_t from = value_float(&(animator->from));
  float_t to = value_float(&(animator->to));
  line_series_t* series = LINE_SERIES(line_series_cast(animator->ctx));
  if (series) {
    series->clip_sample = from + (to - from) * percent;
  }
}

static void line_series_animator_value_on_update(chart_animator_t* animator, float_t percent) {
  float_t* from = (float_t*)value_pointer(&(animator->from));
  float_t* to = (float_t*)value_pointer(&(animator->to));
  line_series_t* series = LINE_SERIES(line_series_cast(animator->ctx));
  if (from && to && series) {
    uint32_t i;
    for (i = 0; i < series->fifo_size; i++) {
      series->fifo[i] = from[i] + (to[i] - from[i]) * percent;
    }
  }
}

static ret_t line_series_generate_points(line_series_t* series, float_t* x0, float_t* y0,
                                         float_t** x, float_t** y, uint32_t* nr, bool_t* vertical) {
  widget_t* parent = series->base.widget.parent;
  axis_t* x_axis = AXIS(chart_view_get_axis(parent, AXIS_ORIENTATION_X, series->x_axis_index));
  axis_t* y_axis = AXIS(chart_view_get_axis(parent, AXIS_ORIENTATION_Y, series->y_axis_index));
  axis_render_t* x_render;
  axis_render_t* y_render;
  bool_t boundary;
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
    boundary = y_axis->type == AXIS_TYPE_CATEGORY;
  } else {
    *vertical = TRUE;
    boundary = x_axis->type == AXIS_TYPE_CATEGORY;
  }

  if (vertical) {
    float_t offset = boundary ? (x_render->tick[1] - x_render->tick[0]) / 2 : 0;

    if (y_axis->at == AXIS_AT_RIGHT) {
      *x0 = y_render->ruler.x - x_render->ruler.w + 1 + offset;
    } else {
      *x0 = y_render->ruler.x + y_render->ruler.w - 1 + offset;
    }
    *y0 = y_render->ruler.y + y_render->ruler.h - 1 - y_render->tick[y_render->index_of_zero];
    *nr = tk_min(x_axis->label.data->size, series->fifo_size);

    if (*nr < 1) {
      return RET_FAIL;
    }

    *x = x_render->tick;
    *y = (float_t*)TKMEM_ZALLOCN(float_t, *nr);
    assert(*y);

    v_range = y_axis->label.val_max - y_axis->label.val_min;
    p_range = y_render->ruler.h;
    for (i = 0; i < *nr; i++) {
      (*y)[i] = -series->fifo[i] / v_range * p_range;
    }
  } else {
    float_t offset = boundary ? (y_render->tick[1] - y_render->tick[0]) / 2 : 0;

    if (x_axis->at == AXIS_AT_TOP) {
      *y0 = x_render->ruler.y + x_render->ruler.h - 1 + offset;
    } else {
      *y0 = x_render->ruler.y - y_render->ruler.h + 1 + offset;
    }
    *x0 = x_render->ruler.x + x_render->tick[x_render->index_of_zero] - 1;
    *nr = tk_min(y_axis->label.data->size, series->fifo_size);

    if (*nr < 1) {
      return RET_FAIL;
    }

    *x = (float_t*)TKMEM_ZALLOCN(float_t, *nr);
    *y = y_render->tick;
    assert(*x);

    v_range = x_axis->label.val_max - x_axis->label.val_min;
    p_range = x_render->ruler.w;
    for (i = 0; i < *nr; i++) {
      (*x)[i] = series->fifo[i] / v_range * p_range;
    }
  }

  return RET_OK;
}

static void line_series_release_points(float_t* x, float_t* y, bool_t vertical) {
  if (vertical) {
    TKMEM_FREE(y);
  } else {
    TKMEM_FREE(x);
  }
}

static ret_t line_series_on_paint_self(widget_t* widget, canvas_t* c) {
  vgcanvas_t* vg;
  float_t x0, y0;
  float_t* x;
  float_t* y;
  uint32_t nr;
  bitmap_t img;
  bool_t has_image = FALSE;
  bool_t vertical = TRUE;
  rect_t r_save;
  rect_t r = rect_init(c->ox, c->oy, widget->w, widget->h);
  line_series_t* series = LINE_SERIES(line_series_cast(widget));

  return_value_if_fail(series != NULL, RET_BAD_PARAMS);

  if (line_series_generate_points(series, &x0, &y0, &x, &y, &nr, &vertical) != RET_OK) {
    return RET_OK;
  }

  if (series->symbol.image) {
    has_image = widget_load_image(widget, series->symbol.image, &img) == RET_OK;
  }

  canvas_get_clip_rect(c, &r_save);
  r = r_save;
  if (nr > 1) {
    if (vertical) {
      r.x = c->ox;
      r.w = x0 + x[nr - 1] - (x[1] - x[0] + 1) * series->clip_sample + series->symbol.size + 1;
    } else {
      r.y = c->oy;
      r.h = y0 + y[nr - 1] - (y[1] - y[0] + 1) * series->clip_sample + series->symbol.size + 1;
    }
  }
  canvas_set_clip_rect(c, &r);

  if (nr == 1) {
    canvas_fill_rect(c, x0 + x[0], y0 + y[0], 1, 1);
  }

  vg = canvas_get_vgcanvas(c);
  vgcanvas_save(vg);
  vgcanvas_translate(vg, c->ox, c->oy);
  vgcanvas_translate(vg, x0, y0);
  if (nr > 1) {
    if (series->line.smooth) {
      series_draw_smooth_line(vg, x, y, nr, &(series->line));
      series_draw_smooth_line_area(vg, 0, 0, x, y, nr, &(series->area), vertical);
    } else {
      series_draw_line(vg, x, y, nr, &(series->line));
      series_draw_line_area(vg, 0, 0, x, y, nr, &(series->area), vertical);
    }
  }
  series_draw_symbol(vg, x, y, nr, &(series->symbol), has_image ? &img : NULL);
  vgcanvas_restore(vg);

  canvas_set_clip_rect(c, &r_save);

  line_series_release_points(x, y, vertical);

  return RET_OK;
}

static ret_t line_series_get_prop(widget_t* widget, const char* name, value_t* v) {
  line_series_t* series = LINE_SERIES(line_series_cast(widget));
  return_value_if_fail(series != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, SERIES_PROP_TITLE)) {
    value_set_wstr(v, series->base.title);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_ANIMATIC)) {
    value_set_bool(v, series->base.animatic);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_LINE_COLOR)) {
    value_set_uint32(v, series->line.color.color);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_LINE_WIDTH)) {
    value_set_float(v, series->line.width);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_LINE_SHOW)) {
    value_set_bool(v, series->line.show);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_LINE_SMOOTH)) {
    value_set_bool(v, series->line.smooth);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_AREA_COLOR)) {
    value_set_uint32(v, series->area.color.color);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_AREA_SHOW)) {
    value_set_bool(v, series->area.show);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_FILL_COLOR)) {
    value_set_uint32(v, series->symbol.fill_color.color);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_STROKE_COLOR)) {
    value_set_uint32(v, series->symbol.stroke_color.color);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_IMAGE)) {
    value_set_str(v, series->symbol.image);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_SIZE)) {
    value_set_float(v, series->symbol.size);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_SHOW)) {
    value_set_bool(v, series->symbol.show);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static void line_series_parse_line(void* ctx, const char* name, const value_t* v) {
  line_series_t* series = LINE_SERIES(ctx);
  ENSURE(series != NULL && name != NULL && v != NULL);
  if (tk_str_eq(name, "color")) {
    series->line.color = chart_utils_value_color(v);
  } else if (tk_str_eq(name, "width")) {
    series->line.width = value_float(v);
  } else if (tk_str_eq(name, "smooth")) {
    series->line.smooth = value_bool(v);
  } else if (tk_str_eq(name, "show")) {
    series->line.show = value_bool(v);
  }
}

static void line_series_parse_area(void* ctx, const char* name, const value_t* v) {
  line_series_t* series = LINE_SERIES(ctx);
  ENSURE(series != NULL && name != NULL && v != NULL);
  if (tk_str_eq(name, "color")) {
    series->area.color = chart_utils_value_color(v);
  } else if (tk_str_eq(name, "show")) {
    series->area.show = value_bool(v);
  }
}

static void line_series_parse_symbol(void* ctx, const char* name, const value_t* v) {
  line_series_t* series = LINE_SERIES(ctx);
  ENSURE(series != NULL && name != NULL && v != NULL);
  if (tk_str_eq(name, "fill_color")) {
    series->symbol.fill_color = chart_utils_value_color(v);
  } else if (tk_str_eq(name, "stroke_color")) {
    series->symbol.stroke_color = chart_utils_value_color(v);
  } else if (tk_str_eq(name, "size")) {
    series->symbol.size = value_float(v);
  } else if (tk_str_eq(name, "image")) {
    const char* image = value_str(v);
    if (image) {
      TKMEM_FREE(series->symbol.image);
      series->symbol.image = tk_strdup(image);
    }
  } else if (tk_str_eq(name, "show")) {
    series->symbol.show = value_bool(v);
  }
}

static ret_t line_series_set_prop(widget_t* widget, const char* name, const value_t* v) {
  line_series_t* series = LINE_SERIES(line_series_cast(widget));
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
  } else if (tk_str_eq(name, SERIES_PROP_LINE)) {
    return chart_utils_object_parse(line_series_parse_line, series, value_str(v));
  } else if (tk_str_eq(name, SERIES_PROP_AREA)) {
    return chart_utils_object_parse(line_series_parse_area, series, value_str(v));
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL)) {
    return chart_utils_object_parse(line_series_parse_symbol, series, value_str(v));
  } else if (tk_str_eq(name, SERIES_PROP_LINE_COLOR)) {
    series->line.color = chart_utils_value_color(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_LINE_WIDTH)) {
    series->line.width = value_float(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_LINE_SHOW)) {
    series->line.show = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_LINE_SMOOTH)) {
    series->line.smooth = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_AREA_COLOR)) {
    series->area.color = chart_utils_value_color(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_AREA_SHOW)) {
    series->area.show = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_FILL_COLOR)) {
    series->symbol.fill_color = chart_utils_value_color(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_STROKE_COLOR)) {
    series->symbol.stroke_color = chart_utils_value_color(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_IMAGE)) {
    const char* image = value_str(v);
    if (image) {
      TKMEM_FREE(series->symbol.image);
      series->symbol.image = tk_strdup(image);
    }
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_SIZE)) {
    series->symbol.size = value_float(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_SHOW)) {
    series->symbol.show = value_bool(v);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t line_series_on_destroy(widget_t* widget) {
  line_series_t* series = LINE_SERIES(line_series_cast(widget));
  return_value_if_fail(series != NULL, RET_BAD_PARAMS);

  TKMEM_FREE(series->base.title);
  TKMEM_FREE(series->symbol.image);
  TKMEM_FREE(series->fifo);

  return RET_OK;
}

static const char* s_line_series_properties[] = {SERIES_PROP_TITLE,
                                                 SERIES_PROP_ANIMATIC,
                                                 SERIES_PROP_LINE_COLOR,
                                                 SERIES_PROP_LINE_WIDTH,
                                                 SERIES_PROP_LINE_SHOW,
                                                 SERIES_PROP_LINE_SMOOTH,
                                                 SERIES_PROP_AREA_COLOR,
                                                 SERIES_PROP_AREA_SHOW,
                                                 SERIES_PROP_SYMBOL_FILL_COLOR,
                                                 SERIES_PROP_SYMBOL_STROKE_COLOR,
                                                 SERIES_PROP_SYMBOL_IMAGE,
                                                 SERIES_PROP_SYMBOL_SIZE,
                                                 SERIES_PROP_SYMBOL_SHOW,
                                                 NULL};

static const widget_vtable_t s_line_series_vtable = {
    .size = sizeof(line_series_t),
    .type = WIDGET_TYPE_LINE_SERIES,
    .clone_properties = s_line_series_properties,
    .persistent_properties = s_line_series_properties,
    .create = line_series_create,
    .on_paint_self = line_series_on_paint_self,
    .set_prop = line_series_set_prop,
    .get_prop = line_series_get_prop,
    .on_destroy = line_series_on_destroy};

widget_t* line_series_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  line_series_t* series = TKMEM_ZALLOC(line_series_t);
  widget_t* widget = WIDGET(series);
  return_value_if_fail(series != NULL, NULL);

  widget_init(widget, parent, &s_line_series_vtable, x, y, w, h);

  series->base.animatic = TRUE;
  series->line.color = _COLOR_BLACK;
  series->line.width = 1;
  series->line.show = TRUE;
  series->line.smooth = FALSE;
  series->area.color = _COLOR_BLACK;
  series->symbol.fill_color = _COLOR_BLACK;
  series->symbol.stroke_color = _COLOR_TRANS;
  series->symbol.size = 5;

  widget_t* win = widget_get_window(widget);
  if (win) {
    widget_on(win, EVT_WINDOW_OPEN, series_on_window_open, widget);
  }

  return widget;
}


ret_t line_series_set_data(widget_t* widget, const float_t* data, uint32_t nr) {
  float_t* fifo;
  uint32_t fifo_size;
  uint32_t fifo_capacity;
  line_series_t* series = LINE_SERIES(line_series_cast(widget));
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

    /* 手动复位，防止由于动画未完成而没有复位 */
    series->clip_sample = 0;

    /* 创建新的动画 */
    wa = chart_animator_create(widget, SERIES_ANIMATION_TIME, 0, SERIES_ANIMATION_EASING);
    assert(wa != NULL && from != NULL && to != NULL);
    series_animator_floats_set_params(wa, from, to, (void*)series,
                                      line_series_animator_value_on_update,
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

ret_t line_series_append(widget_t* widget, const float_t* data, uint32_t nr) {
  float_t* fifo;
  uint32_t fifo_capacity;
  line_series_t* series = LINE_SERIES(line_series_cast(widget));
  return_value_if_fail(series != NULL && data != NULL && nr > 0, RET_BAD_PARAMS);

  fifo = series->fifo;
  fifo_capacity = series->fifo_capacity;
  return_value_if_fail(fifo != NULL && fifo_capacity > 0, RET_BAD_PARAMS);

  series_fifo_float(fifo, &(series->fifo_size), fifo_capacity, data, nr);

  if (series->base.animatic && series->fifo_size > 1) {
    widget_animator_t* wa = NULL;
    widget_t* win = widget_get_window(widget);

    series->clip_sample = tk_min(nr, fifo_capacity);

    /* 销毁旧的动画对象*/
    widget_destroy_animator(widget, NULL);

    /* 创建新的动画 */
    wa = chart_animator_create(widget, SERIES_ANIMATION_TIME, 100, SERIES_ANIMATION_EASING);
    assert(wa != NULL);
    series_animator_float_set_params(wa, series->clip_sample, 0, (void*)series,
                                     line_series_animator_move_on_update, NULL);

    if (win && widget_is_window_opened(win)) {
      widget_animator_start(wa);
    }
  }

  widget_invalidate(widget, NULL);

  return RET_OK;
}

void line_series_clear(widget_t* widget) {
  line_series_t* series = LINE_SERIES(line_series_cast(widget));
  return_if_fail(series != NULL);
  series->fifo_size = 0;
}

widget_t* line_series_cast(widget_t* widget) {
  return_value_if_fail(widget != NULL && (widget->vt == &s_line_series_vtable), NULL);

  return widget;
}
