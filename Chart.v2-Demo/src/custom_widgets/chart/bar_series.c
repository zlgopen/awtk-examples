/**
 * File:   bar_series.c
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
#include "tkc/utils.h"
#include "base/style_factory.h"
#include "series_p.h"
#include "chart_animator.h"
#include "axis.h"

static void bar_series_parse_bar_params(void* ctx, const char* name, const value_t* v) {
  bar_series_t* series = BAR_SERIES(ctx);
  ENSURE(series != NULL && name != NULL && v != NULL);

  if (tk_str_eq(name, "style")) {
    series_subpart_use_style(WIDGET(series), SERIES_SUBPART_BAR, value_str(v));
  } else if (tk_str_eq(name, "overlap")) {
    series->bar.overlap = value_bool(v);
  }
}

static ret_t bar_series_get_prop(widget_t* widget, const char* name, value_t* v) {
  bar_series_t* series = BAR_SERIES(widget);
  return_value_if_fail(series != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  return_value_if_true(series_p_get_prop(widget, name, v) == RET_OK, RET_OK);

  if (tk_str_eq(name, SERIES_PROP_SERIES_AXIS)) {
    value_set_pointer(v, series_p_lookup_series_axis(widget, series->series_axis));
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_VALUE_AXIS)) {
    value_set_pointer(v, series_p_lookup_value_axis(widget, series->value_axis));
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_TITLE)) {
    value_set_wstr(v, series_get_title(widget));
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_BAR_STYLE)) {
    value_set_str(v, series->bar.style);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_BAR_OVERLAP)) {
    value_set_bool(v, series->bar.overlap);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t bar_series_set_prop(widget_t* widget, const char* name, const value_t* v) {
  bar_series_t* series = BAR_SERIES(widget);
  return_value_if_fail(series != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  return_value_if_true(series_p_set_prop(widget, name, v) == RET_OK, RET_OK);

  if (tk_str_eq(name, SERIES_PROP_SERIES_AXIS)) {
    series->series_axis = tk_str_copy(series->series_axis, value_str(v));
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_VALUE_AXIS)) {
    series->value_axis = tk_str_copy(series->value_axis, value_str(v));
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_TITLE)) {
    return series_set_title(widget, value_str(v));
  } else if (tk_str_eq(name, SERIES_PROP_BAR)) {
    return chart_utils_object_parse(bar_series_parse_bar_params, series, value_str(v));
  } else if (tk_str_eq(name, SERIES_PROP_BAR_STYLE)) {
    return series_subpart_use_style(widget, SERIES_SUBPART_BAR, value_str(v));
  } else if (tk_str_eq(name, SERIES_PROP_BAR_OVERLAP)) {
    series->bar.overlap = value_bool(v);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t bar_series_on_destroy(widget_t* widget) {
  bar_series_t* series = BAR_SERIES(widget);
  return_value_if_fail(series != NULL, RET_BAD_PARAMS);

  TKMEM_FREE(series->bar.style);
  style_destroy(series->bar.astyle);
  series->bar.astyle = NULL;

  TKMEM_FREE(series->series_axis);
  TKMEM_FREE(series->value_axis);
  fifo_destroy(series->base.fifo);

  return RET_OK;
}

static ret_t bar_series_calc_layout(widget_t* widget, bool_t vertical, float_t* xoffset,
                                    float_t* yoffset, float_t* bar_width) {
  float_t margins = 0;
  float_t* offset = vertical ? xoffset : yoffset;
  const char* margin1 = vertical ? STYLE_ID_MARGIN_LEFT : STYLE_ID_MARGIN_TOP;
  const char* margin2 = vertical ? STYLE_ID_MARGIN_RIGHT : STYLE_ID_MARGIN_BOTTOM;
  float_t interval;
  widget_t* saxis = widget_get_prop_pointer(widget, SERIES_PROP_SERIES_AXIS);
  bar_series_t* series = BAR_SERIES(widget);
  return_value_if_fail(series != NULL && widget->parent != NULL && saxis != NULL, RET_BAD_PARAMS);
  return_value_if_fail(xoffset != NULL && yoffset != NULL && bar_width != NULL, RET_BAD_PARAMS);

  interval = axis_measure_series_interval(saxis);

  if (series->bar.overlap) {
    *offset = (float_t)style_get_int(widget->astyle, margin1, 0);
    margins += *offset + style_get_int(widget->astyle, margin2, 0);
    *bar_width = interval - margins;
  } else {
    uint32_t cnt = 0;

    WIDGET_FOR_EACH_CHILD_BEGIN(widget->parent, iter, i)
    if (tk_str_eq(iter->vt->type, widget->vt->type)) {
      margins += style_get_int(iter->astyle, margin1, 0);
      margins += style_get_int(iter->astyle, margin2, 0);
      cnt++;
    }
    WIDGET_FOR_EACH_CHILD_END()

    /* 取整以避免柱条之间有空隙 */
    *bar_width = (float_t)tk_roundi((interval - margins) / cnt);

    WIDGET_FOR_EACH_CHILD_BEGIN(widget->parent, iter, i)
    if (tk_str_eq(iter->vt->type, widget->vt->type)) {
      *offset += style_get_int(iter->astyle, margin1, 0);
      break_if_true(iter == widget);

      *offset += *bar_width;
      *offset += style_get_int(iter->astyle, margin2, 0);
    }
    WIDGET_FOR_EACH_CHILD_END()
  }

  // 从刻度的下一个像素点开始计算
  *offset += 1;

  if ((!vertical) ^ AXIS(saxis)->inverse) {
    *bar_width = -(*bar_width);
    *offset = -(*offset);
    *offset += interval / 2;
  } else {
    *offset -= interval / 2;
  }

  return RET_OK;
}

static bool_t bar_series_is_point_in_bar(widget_t* widget, uint32_t index, xy_t x, xy_t y) {
  point_t o;
  float_t edge1 = 0;
  float_t edge2 = 0;
  float_t offset = 0;
  float_t xoffset = 0;
  float_t yoffset = 0;
  float_t bar_width = 0;
  float_t interval = series_p_get_series_interval(widget);
  bool_t vertical = series_p_is_vertical(widget);
  widget_t* saxis = widget_get_prop_pointer(widget, SERIES_PROP_SERIES_AXIS);
  widget_t* vaxis = widget_get_prop_pointer(widget, SERIES_PROP_VALUE_AXIS);
  return_value_if_fail(widget != NULL && widget->parent != NULL, FALSE);
  return_value_if_fail(saxis != NULL && vaxis != NULL, FALSE);

  series_p_get_origin_point(widget, saxis, vaxis, FALSE, &o);
  bar_series_calc_layout(widget, vertical, &xoffset, &yoffset, &bar_width);

  if (vertical) {
    interval = (AXIS(saxis)->inverse ? -interval : interval);
    offset = AXIS(saxis)->type == AXIS_TYPE_CATEGORY ? (interval / 2) : 0;
    edge1 = interval * index + offset + xoffset;
    edge2 = edge1 + bar_width;
  } else {
    interval = (AXIS(saxis)->inverse ? interval : -interval);
    offset = AXIS(saxis)->type == AXIS_TYPE_CATEGORY ? (interval / 2) : 0;
    edge1 = interval * index + offset + yoffset;
    edge2 = edge1 + bar_width;
  }

  if (bar_width >= 0) {
    edge2 = edge1 + bar_width;
  } else {
    edge2 = edge1;
    edge1 = edge2 + bar_width;
  }

  if (vertical) {
    return (x >= o.x + edge1 && x < o.x + edge2);
  } else {
    return (y >= o.y + edge1 && y < o.y + edge2);
  }
}

static ret_t bar_series_on_paint_internal(widget_t* widget, canvas_t* c, float_t ox, float_t oy,
                                          fifo_t* fifo, uint32_t index, uint32_t size,
                                          rect_t* clip_rect, bool_t minmax) {
  float_t xoffset = 0;
  float_t yoffset = 0;
  float_t bar_width;
  vgcanvas_t* vg;
  rect_t r_save;
  rect_t r = *clip_rect;
  bool_t vertical = series_p_is_vertical(widget);
  bar_series_t* series = BAR_SERIES(widget);
  return_value_if_fail(series != NULL, RET_BAD_PARAMS);

  bar_series_calc_layout(widget, vertical, &xoffset, &yoffset, &bar_width);
  if (vertical) {
    r.x += tk_roundi(xoffset) - 1;
    r.w += tk_roundi(bar_width) + 2;
  } else {
    r.y += tk_roundi(yoffset) - 1;
    r.h += tk_roundi(bar_width) + 2;
  }

  canvas_get_clip_rect(c, &r_save);
  canvas_set_clip_rect(c, &r);

  vg = canvas_get_vgcanvas(c);
  assert(vg != NULL);
  vgcanvas_save(vg);
  series_p_draw_bar(widget, c, vg, series->bar.astyle, ox + xoffset, oy + yoffset, fifo, index,
                    size, bar_width, vertical, minmax);
  vgcanvas_restore(vg);

  canvas_set_clip_rect(c, &r_save);

  return RET_OK;
}

static ret_t bar_series_on_paint(widget_t* widget, canvas_t* c, float_t ox, float_t oy,
                                 fifo_t* fifo, uint32_t index, uint32_t size, rect_t* clip_rect) {
  return bar_series_on_paint_internal(widget, c, ox, oy, fifo, index, size, clip_rect, FALSE);
}

static ret_t bar_series_minmax_on_paint(widget_t* widget, canvas_t* c, float_t ox, float_t oy,
                                        fifo_t* fifo, uint32_t index, uint32_t size,
                                        rect_t* clip_rect) {
  return bar_series_on_paint_internal(widget, c, ox, oy, fifo, index, size, clip_rect, TRUE);
}

static ret_t bar_series_start_init_if_not_inited(widget_t* widget) {
  bar_series_t* series = BAR_SERIES(widget);
  return_value_if_fail(series != NULL, RET_BAD_PARAMS);

  if (!series->base.inited) {
    assert(series->bar.astyle != NULL);
    widget_subpart_update_style(series->bar.astyle, widget, SERIES_SUBPART_BAR, series->bar.style);

    series_p_start_animator_when_inited(widget);

    series->base.inited = TRUE;
  }

  return RET_OK;
}

static ret_t bar_series_on_paint_self(widget_t* widget, canvas_t* c) {
  bar_series_t* series = BAR_SERIES(widget);
  return_value_if_fail(series != NULL, RET_BAD_PARAMS);

  bar_series_start_init_if_not_inited(widget);
  series_p_reset_fifo(widget);

  if (series->base.display_mode == SERIES_DISPLAY_COVER) {
    return series_p_on_paint_self_cover(widget, c);
  } else {
    return series_p_on_paint_self_push(widget, c);
  }
}

static ret_t bar_series_subpart_set_style_name(widget_t* widget, const char* subpart,
                                               const char* value) {
  bar_series_t* series = BAR_SERIES(widget);
  return_value_if_fail(series != NULL && subpart != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(subpart, SERIES_SUBPART_BAR)) {
    series->bar.style = tk_str_copy(series->bar.style, value);
    return RET_OK;
  } else {
    return RET_NOT_FOUND;
  }

  return RET_OK;
}

static style_t** bar_series_subpart_get_style_obj(widget_t* widget, const char* subpart) {
  bar_series_t* series = BAR_SERIES(widget);
  return_value_if_fail(series != NULL && subpart != NULL, NULL);

  if (tk_str_eq(subpart, SERIES_SUBPART_BAR)) {
    return &(series->bar.astyle);
  } else {
    return NULL;
  }
}

int32_t bar_series_index_of_point_in(widget_t* widget, xy_t x, xy_t y, bool_t is_local) {
  point_t p = {x, y};
  uint32_t begin, end, middle;
  int32_t index = -1;
  series_t* series = SERIES(widget);
  return_value_if_fail(series != NULL && series->fifo != NULL, -1);
  return_value_if_true(series->fifo->size == 0, -1);

  if (!is_local) {
    widget_to_local(widget, &p);
  }

  series_get_current(widget, &begin, &end, &middle);
  index = series_p_relative_index_of_point_in(widget, p.x, p.y, TRUE);
  return_value_if_true(index < 0, -1);

  if (bar_series_is_point_in_bar(widget, index, p.x, p.y)) {
    if (index <= end - middle) {
      index += middle;
    } else if (index < end - begin + 1) {
      index += begin + middle - end - 1;
    }
    return index;
  }

  return -1;
}

ret_t bar_series_to_local(widget_t* widget, uint32_t index, point_t* p) {
  ret_t ret = series_p_to_local(widget, index, p);

  if (ret == RET_OK) {
    float_t xoffset = 0;
    float_t yoffset = 0;
    float_t bar_width = 0;
    bool_t vertical = series_p_is_vertical(widget);

    bar_series_calc_layout(widget, vertical, &xoffset, &yoffset, &bar_width);
    if (vertical) {
      p->x += tk_roundi(xoffset + bar_width / 2);
    } else {
      p->y += tk_roundi(yoffset + bar_width / 2);
    }
  }

  return ret;
}

ret_t bar_series_minmax_set(widget_t* widget, uint32_t index, const void* data, uint32_t nr) {
  return series_p_set_with_animator(widget, index, data, nr,
                                    chart_animator_fifo_minmax_value_create);
}

static ret_t bar_series_minmax_tooltip_format(void* ctx, const void* data, wstr_t* str) {
  wstr_t temp;
  const wchar_t* title;
  widget_t* widget = WIDGET(ctx);
  series_minmax_data_t* d = (series_minmax_data_t*)(data);
  return_value_if_fail(widget != NULL && d != NULL && str != NULL, RET_BAD_PARAMS);

  wstr_init(str, 0);

  title = series_get_title(widget);
  if (title != NULL && wcslen(title) > 0) {
    wstr_append(str, title);
    wstr_append(str, L": ");
  }

  wstr_init(&temp, 0);
  wstr_from_float(&temp, d->min);
  wstr_append(str, temp.str);
  wstr_append(str, L" ");
  wstr_from_float(&temp, d->max);
  wstr_append(str, temp.str);
  wstr_reset(&temp);

  return RET_OK;
}

static const char* s_bar_series_properties[] = {
    SERIES_PROP_CAPACITY,     SERIES_PROP_UNIT_SIZE,   SERIES_PROP_COVERAGE,
    SERIES_PROP_DISPLAY_MODE, SERIES_PROP_VALUE_ANIMATION,   SERIES_PROP_TITLE,
    SERIES_PROP_BAR_STYLE,    SERIES_PROP_BAR_OVERLAP, NULL};

#define BAR_SERIES_VTABLE                                                                   \
  .count = series_p_count, .rset = series_p_rset, .push = series_p_push, .at = series_p_at, \
  .get_current = series_p_get_current, .is_point_in = series_p_is_point_in,                 \
  .index_of_point_in = bar_series_index_of_point_in, .to_local = bar_series_to_local,       \
  .subpart_set_style_name = bar_series_subpart_set_style_name,                              \
  .subpart_get_style_obj = bar_series_subpart_get_style_obj

static const series_draw_data_info_t s_bar_series_draw_data_info = {
    .size = sizeof(series_p_draw_data_t),
    .compare_in_axis1 = series_p_draw_data_compare_x,
    .compare_in_axis2 = series_p_draw_data_compare_y,
    .min_axis1 = series_p_draw_data_min_x,
    .min_axis2 = series_p_draw_data_min_y,
    .max_axis1 = series_p_draw_data_max_x,
    .max_axis2 = series_p_draw_data_max_y,
    .get_axis1 = series_p_draw_data_get_x,
    .get_axis2 = series_p_draw_data_get_y,
    .set_as_axis21 = series_p_draw_data_set_yx,
    .set_as_axis12 = series_p_draw_data_set_xy};

static const series_vtable_t s_bar_series_internal_vtable = {
    BAR_SERIES_VTABLE, .set = series_p_set_default, .on_paint = bar_series_on_paint,
    .draw_data_info = &s_bar_series_draw_data_info};

static const series_draw_data_info_t s_series_p_minmax_draw_data_info = {
    .size = sizeof(series_p_minmax_draw_data_t),
    .compare_in_axis1 = series_p_minmax_draw_data_compare_x,
    .compare_in_axis2 = series_p_minmax_draw_data_compare_y,
    .min_axis1 = series_p_minmax_draw_data_min_x,
    .min_axis2 = series_p_minmax_draw_data_min_y,
    .max_axis1 = series_p_minmax_draw_data_max_x,
    .max_axis2 = series_p_minmax_draw_data_max_y,
    .get_axis1 = series_p_minmax_draw_data_get_x,
    .get_axis2 = series_p_minmax_draw_data_get_y,
    .set_as_axis21 = series_p_minmax_draw_data_set_yx,
    .set_as_axis12 = series_p_minmax_draw_data_set_xy};

static const series_vtable_t s_bar_series_minmax_internal_vtable = {
    BAR_SERIES_VTABLE, .set = bar_series_minmax_set, .on_paint = bar_series_minmax_on_paint,
    .draw_data_info = &s_series_p_minmax_draw_data_info};

TK_DECL_VTABLE(bar_series) = {.size = sizeof(bar_series_t),
                              .type = WIDGET_TYPE_BAR_SERIES,
                              .enable_pool = TRUE,
                              .parent = TK_PARENT_VTABLE(series),
                              .clone_properties = s_bar_series_properties,
                              .persistent_properties = s_bar_series_properties,
                              .create = bar_series_create,
                              .on_paint_self = bar_series_on_paint_self,
                              .set_prop = bar_series_set_prop,
                              .get_prop = bar_series_get_prop,
                              .on_destroy = bar_series_on_destroy};

TK_DECL_VTABLE(bar_series_minmax) = {.size = sizeof(bar_series_t),
                                     .type = WIDGET_TYPE_BAR_SERIES_MINMAX,
                                     .enable_pool = TRUE,
                                     .parent = TK_PARENT_VTABLE(series),
                                     .clone_properties = s_bar_series_properties,
                                     .persistent_properties = s_bar_series_properties,
                                     .create = bar_series_minmax_create,
                                     .on_paint_self = bar_series_on_paint_self,
                                     .set_prop = bar_series_set_prop,
                                     .get_prop = bar_series_get_prop,
                                     .on_destroy = bar_series_on_destroy};

widget_t* bar_series_create_internal(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h,
                                     const widget_vtable_t* wvt, const series_vtable_t* svt) {
  widget_t* widget = series_create(parent, wvt, x, y, w, h);
  bar_series_t* series = BAR_SERIES(widget);
  return_value_if_fail(series != NULL, NULL);

  series->base.vt = svt;

  if (series->bar.astyle == NULL) {
    series->bar.astyle = style_factory_create_style(style_factory(), widget);
  }

  return widget;
}

widget_t* bar_series_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  return bar_series_create_internal(parent, x, y, w, h, TK_REF_VTABLE(bar_series),
                                    &s_bar_series_internal_vtable);
}

widget_t* bar_series_minmax_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = bar_series_create_internal(
      parent, x, y, w, h, TK_REF_VTABLE(bar_series_minmax), &s_bar_series_minmax_internal_vtable);
  series_t* series = SERIES(widget);
  return_value_if_fail(series != NULL, NULL);

  series->unit_size = sizeof(series_minmax_data_t);
  series->tooltip_format = bar_series_minmax_tooltip_format;
  series->tooltip_format_ctx = widget;

  return widget;
}

widget_t* bar_series_cast(widget_t* widget) {
  return_value_if_fail(
      WIDGET_IS_INSTANCE_OF(widget, bar_series) || WIDGET_IS_INSTANCE_OF(widget, bar_series_minmax),
      NULL);

  return widget;
}
