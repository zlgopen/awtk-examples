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
#include "tkc/utils.h"
#include "base/style_factory.h"
#include "series_p.h"
#include "chart_animator.h"
#include "axis.h"

static void line_series_parse_line_params(void* ctx, const char* name, const value_t* v) {
  line_series_t* series = LINE_SERIES(ctx);
  ENSURE(series != NULL && name != NULL && v != NULL);

  if (tk_str_eq(name, "style")) {
    series_subpart_use_style(WIDGET(series), SERIES_SUBPART_LINE, value_str(v));
  } else if (tk_str_eq(name, "smooth")) {
    series->line.smooth = value_bool(v);
  } else if (tk_str_eq(name, "show")) {
    series->line.show = value_bool(v);
  }
}

static void line_series_parse_line_area_params(void* ctx, const char* name, const value_t* v) {
  line_series_t* series = LINE_SERIES(ctx);
  ENSURE(series != NULL && name != NULL && v != NULL);

  if (tk_str_eq(name, "style")) {
    series_subpart_use_style(WIDGET(series), SERIES_SUBPART_LINE_AREA, value_str(v));
  } else if (tk_str_eq(name, "show")) {
    series->area.show = value_bool(v);
  }
}

static void line_series_parse_symbol_params(void* ctx, const char* name, const value_t* v) {
  line_series_t* series = LINE_SERIES(ctx);
  ENSURE(series != NULL && name != NULL && v != NULL);

  if (tk_str_eq(name, "style")) {
    series_subpart_use_style(WIDGET(series), SERIES_SUBPART_SYMBOL, value_str(v));
  } else if (tk_str_eq(name, "size")) {
    series->symbol.size = value_float(v);
  } else if (tk_str_eq(name, "show")) {
    series->symbol.show = value_bool(v);
  }
}

static ret_t line_series_get_prop(widget_t* widget, const char* name, value_t* v) {
  line_series_t* series = LINE_SERIES(widget);
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
  } else if (tk_str_eq(name, SERIES_PROP_LINE_SHOW)) {
    value_set_bool(v, series->line.show);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_LINE_SMOOTH)) {
    value_set_bool(v, series->line.smooth);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_LINE_STYLE)) {
    value_set_str(v, series->line.style);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_LINE_AREA_SHOW)) {
    value_set_bool(v, series->area.show);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_LINE_AREA_STYLE)) {
    value_set_str(v, series->area.style);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_SHOW)) {
    value_set_bool(v, series->symbol.show);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_STYLE)) {
    value_set_str(v, series->symbol.style);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_SIZE)) {
    value_set_float(v, series->symbol.size);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t line_series_set_prop(widget_t* widget, const char* name, const value_t* v) {
  line_series_t* series = LINE_SERIES(widget);
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
  } else if (tk_str_eq(name, SERIES_PROP_LINE)) {
    return chart_utils_object_parse(line_series_parse_line_params, series, value_str(v));
  } else if (tk_str_eq(name, SERIES_PROP_LINE_AREA)) {
    return chart_utils_object_parse(line_series_parse_line_area_params, series, value_str(v));
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL)) {
    return chart_utils_object_parse(line_series_parse_symbol_params, series, value_str(v));
  } else if (tk_str_eq(name, SERIES_PROP_LINE_SHOW)) {
    series->line.show = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_LINE_SMOOTH)) {
    series->line.smooth = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_LINE_STYLE)) {
    return series_subpart_use_style(widget, SERIES_SUBPART_LINE, value_str(v));
  } else if (tk_str_eq(name, SERIES_PROP_LINE_AREA_SHOW)) {
    series->area.show = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_LINE_AREA_STYLE)) {
    return series_subpart_use_style(widget, SERIES_SUBPART_LINE_AREA, value_str(v));
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_SIZE)) {
    series->symbol.size = value_float(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_SHOW)) {
    series->symbol.show = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, SERIES_PROP_SYMBOL_STYLE)) {
    return series_subpart_use_style(widget, SERIES_SUBPART_SYMBOL, value_str(v));
  }

  return RET_NOT_FOUND;
}

static ret_t line_series_on_destroy(widget_t* widget) {
  line_series_t* series = LINE_SERIES(widget);
  return_value_if_fail(series != NULL, RET_BAD_PARAMS);

  TKMEM_FREE(series->line.style);
  style_destroy(series->line.astyle);
  series->line.astyle = NULL;

  TKMEM_FREE(series->area.style);
  style_destroy(series->area.astyle);
  series->area.astyle = NULL;

  TKMEM_FREE(series->symbol.style);
  style_destroy(series->symbol.astyle);
  series->symbol.astyle = NULL;

  TKMEM_FREE(series->series_axis);
  TKMEM_FREE(series->value_axis);
  fifo_destroy(series->base.fifo);

  return RET_OK;
}

static ret_t line_series_draw_one_series(widget_t* widget, canvas_t* c, float_t ox, float_t oy,
                                         fifo_t* fifo, uint32_t index, uint32_t size,
                                         rect_t* clip_rect, series_p_draw_line_t draw_line,
                                         series_p_draw_line_area_t draw_area,
                                         series_p_draw_smooth_line_t draw_smooth_line,
                                         series_p_draw_smooth_line_area_t draw_smooth_area,
                                         series_p_draw_symbol_t draw_symbol) {
  rect_t r_save;
  rect_t r = *clip_rect;
  vgcanvas_t* vg;
  line_series_t* series = LINE_SERIES(widget);
  bool_t vertical = series_p_is_vertical(widget);
  return_value_if_fail(series != NULL, RET_BAD_PARAMS);

  canvas_get_clip_rect(c, &r_save);

  if (series->line.show || series->area.show) {
    canvas_set_clip_rect(c, &r);

    vg = canvas_get_vgcanvas(c);
    assert(vg != NULL);
    vgcanvas_save(vg);
    vgcanvas_translate(vg, c->ox, c->oy);

    // 加0.5, 避免色块边缘出现虚化（注意, 色块的区域为rect(x + 0.5, y + 0.5, w, h)）
    vgcanvas_translate(vg, 0.5, 0.5);

    if (series->line.show) {
      if (series->line.smooth) {
        draw_smooth_line(widget, vg, series->line.astyle, ox, oy, fifo, index, size, vertical);
      } else {
        draw_line(widget, c, vg, series->line.astyle, ox, oy, fifo, index, size);
      }
    }

    if (series->area.show) {
      if (series->line.smooth) {
        draw_smooth_area(widget, vg, series->area.astyle, ox, oy, fifo, index, size, vertical);
      } else {
        draw_area(widget, vg, series->area.astyle, ox, oy, fifo, index, size, vertical);
      }
    }

    vgcanvas_restore(vg);
  }

  if (series->symbol.show) {
    widget_t* axis = widget_get_prop_pointer(widget, SERIES_PROP_SERIES_AXIS);
    float_t range = axis_get_range(axis);

    // 默认symbol的border_with不超过symbol.size
    if (vertical) {
      r.x -= (r.w > 0 ? 1 : -1) * series->symbol.size * 2;
      r.w += (r.w > 0 ? 1 : -1) * series->symbol.size * 4;
    } else {
      r.y -= (r.h > 0 ? 1 : -1) * series->symbol.size * 2;
      r.h += (r.h > 0 ? 1 : -1) * series->symbol.size * 4;
    }

    // caover类型时，旧波形减少一个sysmbol, 避免symbol的间距很小时，显示残余
    if (fifo->size > range && index + size < fifo->size) {
      index++;
      size--;
    }

    canvas_set_clip_rect(c, &r);

    vg = canvas_get_vgcanvas(c);
    assert(vg != NULL);
    vgcanvas_save(vg);
    vgcanvas_translate(vg, c->ox, c->oy);
    draw_symbol(widget, vg, series->symbol.astyle, ox, oy, fifo, index, size, series->symbol.size);
    vgcanvas_restore(vg);
  }

  canvas_set_clip_rect(c, &r_save);

  return RET_OK;
}

static ret_t line_series_on_paint(widget_t* widget, canvas_t* c, float_t ox, float_t oy,
                                  fifo_t* fifo, uint32_t index, uint32_t size, rect_t* clip_rect) {
  return line_series_draw_one_series(
      widget, c, ox, oy, fifo, index, size, clip_rect, series_p_draw_line, series_p_draw_line_area,
      series_p_draw_smooth_line, series_p_draw_smooth_line_area, series_p_draw_symbol);
}

static ret_t line_series_start_init_if_not_inited(widget_t* widget) {
  line_series_t* series = LINE_SERIES(widget);
  return_value_if_fail(series != NULL, RET_BAD_PARAMS);

  if (!series->base.inited) {
    assert(series->line.astyle != NULL);
    widget_subpart_update_style(series->line.astyle, widget, SERIES_SUBPART_LINE,
                                series->line.style);

    assert(series->area.astyle != NULL);
    widget_subpart_update_style(series->area.astyle, widget, SERIES_SUBPART_LINE_AREA,
                                series->area.style);

    assert(series->symbol.astyle != NULL);
    widget_subpart_update_style(series->symbol.astyle, widget, SERIES_SUBPART_SYMBOL,
                                series->symbol.style);

    series_p_start_animator_when_inited(widget);

    series->base.inited = TRUE;
  }

  return RET_OK;
}

static ret_t line_series_on_paint_self(widget_t* widget, canvas_t* c) {
  line_series_t* series = LINE_SERIES(widget);
  return_value_if_fail(series != NULL, RET_BAD_PARAMS);

  line_series_start_init_if_not_inited(widget);
  series_p_reset_fifo(widget);

  if (series->base.display_mode == SERIES_DISPLAY_COVER) {
    return series_p_on_paint_self_cover(widget, c);
  } else {
    return series_p_on_paint_self_push(widget, c);
  }
}

static ret_t line_series_subpart_set_style_name(widget_t* widget, const char* subpart,
                                                const char* value) {
  line_series_t* series = LINE_SERIES(widget);
  return_value_if_fail(series != NULL && subpart != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(subpart, SERIES_SUBPART_LINE)) {
    series->line.style = tk_str_copy(series->line.style, value);
  } else if (tk_str_eq(subpart, SERIES_SUBPART_LINE_AREA)) {
    series->area.style = tk_str_copy(series->area.style, value);
  } else if (tk_str_eq(subpart, SERIES_SUBPART_SYMBOL)) {
    series->symbol.style = tk_str_copy(series->symbol.style, value);
  } else {
    return RET_NOT_FOUND;
  }

  return RET_OK;
}

static style_t** line_series_subpart_get_style_obj(widget_t* widget, const char* subpart) {
  line_series_t* series = LINE_SERIES(widget);
  return_value_if_fail(series != NULL && subpart != NULL, NULL);

  if (tk_str_eq(subpart, SERIES_SUBPART_LINE)) {
    return &(series->line.astyle);
  } else if (tk_str_eq(subpart, SERIES_SUBPART_LINE_AREA)) {
    return &(series->area.astyle);
  } else if (tk_str_eq(subpart, SERIES_SUBPART_SYMBOL)) {
    return &(series->symbol.astyle);
  } else {
    return NULL;
  }
}

static ret_t line_series_colorful_on_paint(widget_t* widget, canvas_t* c, float_t ox, float_t oy,
                                           fifo_t* fifo, uint32_t index, uint32_t size,
                                           rect_t* clip_rect) {
  return line_series_draw_one_series(
      widget, c, ox, oy, fifo, index, size, clip_rect, series_p_draw_line_colorful,
      series_p_draw_line_area_colorful, series_p_draw_smooth_line_colorful,
      series_p_draw_smooth_line_area_colorful, series_p_draw_symbol_colorful);
}

ret_t line_series_colorful_set(widget_t* widget, uint32_t index, const void* data, uint32_t nr) {
  return series_p_set_with_animator(widget, index, data, nr,
                                    chart_animator_fifo_colorful_value_create);
}

static const char* s_line_series_properties[] = {
    SERIES_PROP_CAPACITY,        SERIES_PROP_UNIT_SIZE,      SERIES_PROP_COVERAGE,
    SERIES_PROP_DISPLAY_MODE,    SERIES_PROP_VALUE_ANIMATION,      SERIES_PROP_TITLE,
    SERIES_PROP_LINE_STYLE,      SERIES_PROP_LINE_SHOW,      SERIES_PROP_LINE_SMOOTH,
    SERIES_PROP_LINE_AREA_STYLE, SERIES_PROP_LINE_AREA_SHOW, SERIES_PROP_SYMBOL_STYLE,
    SERIES_PROP_SYMBOL_SIZE,     SERIES_PROP_SYMBOL_SHOW,    NULL};

#define LINE_SERIES_WIDGET_VT                                                               \
  .count = series_p_count, .rset = series_p_rset, .push = series_p_push, .at = series_p_at, \
  .get_current = series_p_get_current, .is_point_in = series_p_is_point_in,                 \
  .index_of_point_in = series_p_index_of_point_in, .to_local = series_p_to_local,           \
  .subpart_set_style_name = line_series_subpart_set_style_name,                             \
  .subpart_get_style_obj = line_series_subpart_get_style_obj

static const series_draw_data_info_t s_line_series_draw_data_info = {
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

static const series_vtable_t s_line_series_internal_vtable = {
    LINE_SERIES_WIDGET_VT, .set = series_p_set_default, .on_paint = line_series_on_paint,
    .draw_data_info = &s_line_series_draw_data_info};

TK_DECL_VTABLE(line_series) = {.size = sizeof(line_series_t),
                               .type = WIDGET_TYPE_LINE_SERIES,
                               .enable_pool = TRUE,
                               .parent = TK_PARENT_VTABLE(series),
                               .clone_properties = s_line_series_properties,
                               .persistent_properties = s_line_series_properties,
                               .create = line_series_create,
                               .on_paint_self = line_series_on_paint_self,
                               .set_prop = line_series_set_prop,
                               .get_prop = line_series_get_prop,
                               .on_destroy = line_series_on_destroy};

static const series_draw_data_info_t s_series_p_colorful_draw_data_info = {
    .size = sizeof(series_p_colorful_draw_data_t),
    .compare_in_axis1 = series_p_colorful_draw_data_compare_x,
    .compare_in_axis2 = series_p_colorful_draw_data_compare_y,
    .min_axis1 = series_p_colorful_draw_data_min_x,
    .min_axis2 = series_p_colorful_draw_data_min_y,
    .max_axis1 = series_p_colorful_draw_data_max_x,
    .max_axis2 = series_p_colorful_draw_data_max_y,
    .get_axis1 = series_p_colorful_draw_data_get_x,
    .get_axis2 = series_p_colorful_draw_data_get_y,
    .set_as_axis21 = series_p_colorful_draw_data_set_yx,
    .set_as_axis12 = series_p_colorful_draw_data_set_xy};

static const series_vtable_t s_line_series_colorful_internal_vtable = {
    LINE_SERIES_WIDGET_VT, .set = line_series_colorful_set,
    .on_paint = line_series_colorful_on_paint,
    .draw_data_info = &s_series_p_colorful_draw_data_info};

TK_DECL_VTABLE(line_series_colorful) = {.size = sizeof(line_series_t),
                                        .type = WIDGET_TYPE_LINE_SERIES_COLORFUL,
                                        .enable_pool = TRUE,
                                        .parent = TK_PARENT_VTABLE(series),
                                        .clone_properties = s_line_series_properties,
                                        .persistent_properties = s_line_series_properties,
                                        .create = line_series_colorful_create,
                                        .on_paint_self = line_series_on_paint_self,
                                        .set_prop = line_series_set_prop,
                                        .get_prop = line_series_get_prop,
                                        .on_destroy = line_series_on_destroy};

widget_t* line_series_create_internal(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h,
                                      const widget_vtable_t* wvt, const series_vtable_t* svt) {
  widget_t* widget = series_create(parent, wvt, x, y, w, h);
  line_series_t* series = LINE_SERIES(widget);
  return_value_if_fail(series != NULL, NULL);

  series->base.vt = svt;
  series->line.show = TRUE;
  series->line.smooth = FALSE;
  series->symbol.size = 3;

  if (series->line.astyle == NULL) {
    series->line.astyle = style_factory_create_style(style_factory(), widget);
  }

  if (series->area.astyle == NULL) {
    series->area.astyle = style_factory_create_style(style_factory(), widget);
  }

  if (series->symbol.astyle == NULL) {
    series->symbol.astyle = style_factory_create_style(style_factory(), widget);
  }

  return widget;
}

widget_t* line_series_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  return line_series_create_internal(parent, x, y, w, h, TK_REF_VTABLE(line_series),
                                     &s_line_series_internal_vtable);
}

widget_t* line_series_colorful_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget =
      line_series_create_internal(parent, x, y, w, h, TK_REF_VTABLE(line_series_colorful),
                                  &s_line_series_colorful_internal_vtable);
  series_t* series = SERIES(widget);
  return_value_if_fail(series != NULL, NULL);

  series->unit_size = sizeof(series_colorful_data_t);

  return widget;
}

widget_t* line_series_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, line_series) ||
                           WIDGET_IS_INSTANCE_OF(widget, line_series_colorful),
                       NULL);

  return widget;
}
