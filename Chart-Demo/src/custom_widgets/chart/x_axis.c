/**
 * File:   x_axis.c
 * Author: AWTK Develop Team
 * Brief:  x axis
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is dirich_text_nodeibuted in the hope that it will be useful,
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

#include "x_axis.h"
#include "axis_p.h"
#include "base/style.h"
#include "base/self_layouter.h"

static ret_t x_axis_update_data_tick(widget_t* widget) {
  uint32_t i;
  uint32_t divnr;
  float_t range;
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && axis->data != NULL, RET_BAD_PARAMS);

  divnr = axis->type == AXIS_TYPE_VALUE ? (axis->data->size - 1) : axis->data->size;
  range = axis->draw_rect.w - 1;

  for (i = 0; i < axis->data->size; i++) {
    axis_data_t* d = (axis_data_t*)(axis->data->elms[i]);
    d->tick = range * i / divnr;
  }

  return RET_OK;
}

static ret_t x_axis_on_paint_begin(widget_t* widget, canvas_t* c) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL, RET_BAD_PARAMS);

  if (axis->need_update_data) {
    ret_t ret;
    if (axis->data_from_series != NULL) {
      ret = axis_update_data_from_series(widget);
    } else {
      ret = x_axis_update_data_tick(widget);
    }

    axis->need_update_data = ret != RET_OK;
  }

  return axis_on_paint_begin(widget, c);
}

static ret_t x_axis_on_self_layout(widget_t* widget, rect_t* r) {
  float_t x_layout, w_layout;
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && r != NULL, 0);

  x_layout = self_layouter_get_param_float(widget->self_layout, "x", 0);
  w_layout = self_layouter_get_param_float(widget->self_layout, "w", 0);
  axis->draw_rect.x = x_layout != 0 ? widget->x : r->x;
  axis->draw_rect.w = w_layout != 0 ? widget->w : r->w;
  if (axis->at == AXIS_AT_TOP) {
    axis->draw_rect.y = r->y;
    axis->draw_rect.h = -(widget->h == 0 ? 1 : widget->h);
    axis->split_line.line_len = r->h;
  } else {
    axis->draw_rect.y = r->y + r->h - 1;
    axis->draw_rect.h = widget->h == 0 ? 1 : widget->h;
    axis->split_line.line_len = -r->h;
  }

  return RET_OK;
}

static float_t x_axis_measure_series_interval(widget_t* widget) {
  float_t vrange = 0;
  float_t prange = 0;
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL, 0);

  vrange = axis_get_range(widget);
  prange = axis->draw_rect.w;

  if (axis->type == AXIS_TYPE_CATEGORY) {
    assert(vrange > 0);
    return prange / vrange;
  } else {
    assert(vrange > 1);
    return (prange - 1) / (vrange - 1);
  }
}

ret_t x_axis_measure_series(widget_t* widget, void* measure_params, fifo_t* src, fifo_t* dst) {
  int32_t i;
  int32_t nr;
  int32_t index;
  float_t sinterval, soffset;
  float_t y, prange, vrange, vmin, srange;
  axis_measure_series_params_t* params = (axis_measure_series_params_t*)measure_params;
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && params != NULL, RET_BAD_PARAMS);
  return_value_if_fail(src != NULL && dst != NULL, RET_BAD_PARAMS);
  return_value_if_fail(dst->unit_size > 0, RET_BAD_PARAMS);

  nr = tk_min(dst->capacity, src->size - params->index_of_fifo);
  return_value_if_true(nr == 0, RET_OK);

  vmin = axis->max * axis->min > 0 ? axis->min : 0;
  vrange = axis_get_range(widget);
  prange = axis->draw_rect.w;
  srange = axis_get_range(params->series_axis);
  sinterval = axis_measure_series_interval(params->series_axis);
  sinterval = AXIS(params->series_axis)->inverse ? sinterval : -sinterval;
  soffset = AXIS(params->series_axis)->type == AXIS_TYPE_CATEGORY ? (sinterval / 2) : 0;

  if (tk_abs(sinterval) >= 1.0) {
    for (i = 0; i < nr; i++) {
      index = src->size - params->index_of_fifo - nr + i;
      y = sinterval * ((float_t)i - params->index_of_fold);
      if (i < params->index_of_fold) {
        y = sinterval * (srange - params->index_of_fold + i) + soffset;
      } else {
        y = sinterval * (i - params->index_of_fold) + soffset;
      }

      fifo_push(dst, NULL);
      params->draw_data_set(fifo_at(dst, dst->size - 1), y, src, index, vmin, vrange, prange,
                            axis->inverse);
    }
  } else {
    void* d = TKMEM_CALLOC(1, dst->unit_size);
    void* dmin = TKMEM_CALLOC(1, dst->unit_size);
    void* dmax = TKMEM_CALLOC(1, dst->unit_size);
    bool_t increase = TRUE;

    // 根据分辨率过滤采样点，每个像素点上最多绘制2个采样点
    for (i = 0; i < nr; i++) {
      index = src->size - params->index_of_fifo - nr + i;
      y = sinterval * ((float_t)i - params->index_of_fold);
      if (i < params->index_of_fold) {
        y = sinterval * (srange - params->index_of_fold + i) + soffset;
      } else {
        y = sinterval * (i - params->index_of_fold) + soffset;
      }

      params->draw_data_set(d, y, src, index, vmin, vrange, prange, axis->inverse);

      if (i == 0) {
        memcpy(dmin, d, dst->unit_size);
        memcpy(dmax, d, dst->unit_size);
      } else {
        if (params->draw_data_compare_series(d, dmin) == 0 && index + 1 < src->size) {
          increase = increase && params->draw_data_compare_value(dmax, d) >= 0;
          params->draw_data_min_value(dmin, d);
          params->draw_data_max_value(dmax, d);
        } else {
          if (params->draw_data_compare_value(dmin, dmax) == 0) {
            break_if_true(dst->size + 1 > nr);
            fifo_push(dst, dmin);
          } else {
            break_if_true(dst->size + 2 > nr);
            if (increase) {
              fifo_push(dst, dmax);
              fifo_push(dst, dmin);
            } else {
              fifo_push(dst, dmin);
              fifo_push(dst, dmax);
            }
          }

          if (i == nr - 1) {
            fifo_push(dst, d);
          } else {
            memcpy(dmin, d, dst->unit_size);
            memcpy(dmax, d, dst->unit_size);
          }

          increase = TRUE;
        }
      }
    }

    TKMEM_FREE(d);
    TKMEM_FREE(dmin);
    TKMEM_FREE(dmax);
  }

  return RET_OK;
}

ret_t x_axis_draw_tick(axis_t* axis, canvas_t* c) {
  style_t* style = axis->tick.astyle;
  bitmap_t img;
  color_t trans = color_init(0, 0, 0, 0);
  color_t color = style_get_color(style, STYLE_ID_FG_COLOR, trans);
  const char* image_name = style_get_str(style, STYLE_ID_FG_IMAGE, NULL);
  image_draw_type_t draw_type =
      (image_draw_type_t)style_get_int(style, STYLE_ID_FG_IMAGE_DRAW_TYPE, IMAGE_DRAW_PATCH3_X);
  int32_t axis_offset = widget_get_prop_int(WIDGET(axis), AXIS_PROP_OFFSET, 0);
  rect_t r = rect_init(0, axis->draw_rect.y + axis_offset, 1, AXIS_DEFAULT_TICK_LEN);
  const axis_data_t** labels = (const axis_data_t**)(axis->data->elms);
  float_t offset = 0;
  uint32_t i;
  uint32_t nr = axis->data->size;

  if (axis->type == AXIS_TYPE_CATEGORY && axis->tick.align_with_label) {
    offset = (nr > 1 ? (labels[1]->tick - labels[0]->tick) : axis->draw_rect.w) / 2.0;
    nr--;
  }

  if ((axis->at == AXIS_AT_TOP && !axis->tick.inside) ||
      ((axis->at == AXIS_AT_BOTTOM || axis->at == AXIS_AT_AUTO) && axis->tick.inside)) {
    r.h = -r.h;
  }

  if (image_name != NULL) {
    if (widget_load_image(WIDGET(axis), image_name, &img) != RET_OK) {
      image_name = NULL;
    }
  }

  if (color.rgba.a || image_name != NULL) {
    canvas_set_stroke_color(c, color);

    for (i = 0; i < axis->data->size; i++) {
      if (axis->inverse) {
        r.x = axis->draw_rect.x + axis->draw_rect.w - 1 - (labels[i]->tick + offset);
      } else {
        r.x = axis->draw_rect.x + labels[i]->tick + offset;
      }
      continue_if_true(r.x < axis->draw_rect.x);
      continue_if_true(r.x >= axis->draw_rect.x + axis->draw_rect.w);

      if (color.rgba.a) {
        canvas_draw_vline(c, r.x, r.y, r.h);
      }

      if (image_name != NULL) {
        canvas_draw_image_ex(c, &img, draw_type, &r);
      }
    }

    if (axis->type == AXIS_TYPE_CATEGORY && !axis->tick.align_with_label) {
      if (axis->inverse) {
        r.x = axis->draw_rect.x;
      } else {
        r.x = axis->draw_rect.x + axis->draw_rect.w - 1;
      }

      if (color.rgba.a) {
        canvas_draw_vline(c, r.x, r.y, r.h);
      }

      if (image_name != NULL) {
        canvas_draw_image_ex(c, &img, draw_type, &r);
      }
    }
  }

  return RET_OK;
}

ret_t x_axis_draw_line(axis_t* axis, canvas_t* c) {
  style_t* style = axis->line.astyle;
  bitmap_t img;
  color_t trans = color_init(0, 0, 0, 0);
  color_t color = style_get_color(style, STYLE_ID_FG_COLOR, trans);
  const char* image_name = style_get_str(style, STYLE_ID_FG_IMAGE, NULL);
  image_draw_type_t draw_type =
      (image_draw_type_t)style_get_int(style, STYLE_ID_FG_IMAGE_DRAW_TYPE, IMAGE_DRAW_PATCH3_X);
  int32_t axis_offset = widget_get_prop_int(WIDGET(axis), AXIS_PROP_OFFSET, 0);
  rect_t r = rect_init(axis->draw_rect.x, axis->draw_rect.y + axis_offset, axis->draw_rect.w, 1);

  r.x -= axis->line.lengthen;
  r.w += axis->line.lengthen * 2;

  if (color.rgba.a) {
    canvas_set_stroke_color(c, color);
    canvas_draw_hline(c, r.x, r.y, r.w);
  }

  if (image_name != NULL) {
    if (widget_load_image(WIDGET(axis), image_name, &img) == RET_OK) {
      canvas_draw_image_ex(c, &img, draw_type, &r);
    }
  }

  return RET_OK;
}

ret_t x_axis_draw_split_line(axis_t* axis, canvas_t* c) {
  style_t* style = axis->split_line.astyle;
  bitmap_t img;
  color_t trans = color_init(0, 0, 0, 0);
  color_t color = style_get_color(style, STYLE_ID_FG_COLOR, trans);
  const char* image_name = style_get_str(style, STYLE_ID_FG_IMAGE, NULL);
  image_draw_type_t draw_type =
      (image_draw_type_t)style_get_int(style, STYLE_ID_FG_IMAGE_DRAW_TYPE, IMAGE_DRAW_PATCH3_X);
  rect_t r = rect_init(0, axis->draw_rect.y, 1, axis->split_line.line_len);
  const axis_data_t** labels = (const axis_data_t**)(axis->data->elms);
  uint32_t i;

  if (image_name != NULL) {
    if (widget_load_image(WIDGET(axis), image_name, &img) != RET_OK) {
      image_name = NULL;
    }
  }

  if (color.rgba.a || image_name != NULL) {
    canvas_set_stroke_color(c, color);

    for (i = 0; i < axis->data->size; i++) {
      if (axis->inverse) {
        r.x = axis->draw_rect.x + axis->draw_rect.w - 1 - labels[i]->tick;
      } else {
        r.x = axis->draw_rect.x + labels[i]->tick;
      }
      continue_if_true(r.x < axis->draw_rect.x);
      continue_if_true(r.x >= axis->draw_rect.x + axis->draw_rect.w);

      if (color.rgba.a) {
        canvas_draw_vline(c, r.x, r.y, r.h);
      }

      if (image_name != NULL) {
        canvas_draw_image_ex(c, &img, draw_type, &r);
      }
    }

    if (axis->type == AXIS_TYPE_CATEGORY && !axis->tick.align_with_label) {
      if (axis->inverse) {
        r.x = axis->draw_rect.x;
      } else {
        r.x = axis->draw_rect.x + axis->draw_rect.w - 1;
      }

      if (color.rgba.a) {
        canvas_draw_vline(c, r.x, r.y, r.h);
      }

      if (image_name != NULL) {
        canvas_draw_image_ex(c, &img, draw_type, &r);
      }
    }
  }

  return RET_OK;
}

ret_t x_axis_draw_label(axis_t* axis, canvas_t* c) {
  style_t* style = axis->label.astyle;
  color_t trans = color_init(0, 0, 0, 0);
  color_t color = style_get_color(style, STYLE_ID_TEXT_COLOR, trans);
  const char* font_name = style_get_str(style, STYLE_ID_FONT_NAME, NULL);
  uint16_t font_size = style_get_int(style, STYLE_ID_FONT_SIZE, TK_DEFAULT_FONT_SIZE);
  int32_t margin = style_get_int(style, STYLE_ID_MARGIN, 0);
  float_t spacer = 0;
  float_t offset = 0;
  int32_t axis_offset = widget_get_prop_int(WIDGET(axis), AXIS_PROP_OFFSET, 0);
  rect_t r = rect_init(0, axis->draw_rect.y + axis_offset, axis->draw_rect.w, font_size);
  const axis_data_t** labels = (const axis_data_t**)(axis->data->elms);
  uint32_t i;

  if (axis->data->size > 1) {
    r.w = labels[1]->tick - labels[0]->tick;
  }

  if (axis->type == AXIS_TYPE_VALUE) {
    offset = -r.w / 2;
  } else if (axis->type == AXIS_TYPE_CATEGORY) {
    r.w = axis->inverse ? -r.w : r.w;
  }

  spacer =
      (axis->tick.show && !(axis->tick.inside ^ axis->label.inside) ? AXIS_DEFAULT_TICK_LEN : 0) +
      margin;

  if ((axis->at == AXIS_AT_TOP && !axis->label.inside) ||
      ((axis->at == AXIS_AT_AUTO || axis->at == AXIS_AT_BOTTOM) && axis->label.inside)) {
    canvas_set_text_align(c, ALIGN_H_CENTER, ALIGN_V_BOTTOM);
    r.y -= spacer + r.h + 1;
  } else {
    canvas_set_text_align(c, ALIGN_H_CENTER, ALIGN_V_TOP);
    r.y += spacer;
  }

  canvas_set_text_color(c, color);
  canvas_set_font(c, font_name, font_size);

  if (color.rgba.a) {
    for (i = 0; i < axis->data->size; i++) {
      if (axis->inverse) {
        r.x = axis->draw_rect.x + axis->draw_rect.w - 1 - labels[i]->tick + offset;
      } else {
        r.x = axis->draw_rect.x + labels[i]->tick + offset;
      }

      canvas_draw_text_in_rect(c, labels[i]->text.str, labels[i]->text.size, &r);
    }
  }

  return RET_OK;
}

ret_t x_axis_on_paint_before(widget_t* widget, canvas_t* c) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && c != NULL, RET_BAD_PARAMS);

  if (axis->data != NULL) {
    if (axis->split_line.show) {
      x_axis_draw_split_line(axis, c);
      axis->painted_before = TRUE;
    }
  }

  return RET_OK;
}

ret_t x_axis_on_paint_self(widget_t* widget, canvas_t* c) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && c != NULL, RET_BAD_PARAMS);

  if (axis->data != NULL) {
    if (axis->split_line.show && !axis->painted_before) {
      x_axis_draw_split_line(axis, c);
    }

    if (axis->tick.show) {
      x_axis_draw_tick(axis, c);
    }

    if (axis->label.show) {
      x_axis_draw_label(axis, c);
    }
  }

  if (axis->line.show) {
    x_axis_draw_line(axis, c);
  }

  axis->painted_before = FALSE;

  return RET_OK;
}

static const char* s_x_axis_properties[] = {WIDGET_PROP_MIN,
                                            WIDGET_PROP_MAX,
                                            AXIS_PROP_TYPE,
                                            AXIS_PROP_AT,
                                            AXIS_PROP_OFFSET,
                                            AXIS_PROP_SPLIT_LINE_STYLE,
                                            AXIS_PROP_LINE_STYLE,
                                            AXIS_PROP_TICK_STYLE,
                                            AXIS_PROP_LABEL_STYLE,
                                            AXIS_PROP_TITLE_STYLE,
                                            AXIS_PROP_SPLIT_LINE_SHOW,
                                            AXIS_PROP_LINE_SHOW,
                                            AXIS_PROP_TICK_SHOW,
                                            AXIS_PROP_LABEL_SHOW,
                                            AXIS_PROP_TITLE_SHOW,
                                            NULL};

TK_DECL_VTABLE(x_axis) = {.type = WIDGET_TYPE_X_AXIS,
                          .size = sizeof(x_axis_t),
                          .parent = TK_PARENT_VTABLE(axis),
                          .create = x_axis_create,
                          .clone_properties = s_x_axis_properties,
                          .persistent_properties = s_x_axis_properties,
                          .on_paint_begin = x_axis_on_paint_begin,
                          .on_paint_self = x_axis_on_paint_self,
                          .set_prop = axis_p_set_prop,
                          .get_prop = axis_p_get_prop,
                          .on_destroy = axis_on_destroy};

static const axis_vtable_t s_x_axis_internal_vtable = {
    .on_self_layout = x_axis_on_self_layout,
    .on_paint_before = x_axis_on_paint_before,
    .measure_series_interval = x_axis_measure_series_interval,
    .measure_series = x_axis_measure_series};

widget_t* x_axis_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = axis_create(parent, TK_REF_VTABLE(x_axis), x, y, w, h);
  x_axis_t* x_axis = X_AXIS(widget);
  return_value_if_fail(x_axis != NULL, NULL);

  x_axis->base.vt = &s_x_axis_internal_vtable;
  x_axis->base.draw_rect = rect_init(x, y, w, h);

  return widget;
}
widget_t* x_axis_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, x_axis), NULL);

  return widget;
}
