/**
 * File:   y_axis.c
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

#include "y_axis.h"
#include "axis_p.h"
#include "base/style.h"
#include "base/self_layouter.h"

static ret_t y_axis_update_data_tick(widget_t* widget) {
  uint32_t i;
  uint32_t divnr;
  float_t range;
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && axis->data != NULL, RET_BAD_PARAMS);

  divnr = axis->type == AXIS_TYPE_VALUE ? (axis->data->size - 1) : axis->data->size;
  range = axis->draw_rect.h - 1;

  for (i = 0; i < axis->data->size; i++) {
    axis_data_t* d = (axis_data_t*)(axis->data->elms[i]);
    d->tick = range * i / divnr;
  }

  return RET_OK;
}

static ret_t y_axis_on_paint_begin(widget_t* widget, canvas_t* c) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL, RET_BAD_PARAMS);

  if (axis->need_update_data) {
    ret_t ret;
    if (axis->data_from_series != NULL) {
      ret = axis_update_data_from_series(widget);
    } else {
      ret = y_axis_update_data_tick(widget);
    }

    axis->need_update_data = ret != RET_OK;
  }

  return axis_on_paint_begin(widget, c);
}

static ret_t y_axis_on_self_layout(widget_t* widget, rect_t* r) {
  float_t y_layout, h_layout;
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && r != NULL, 0);

  y_layout = self_layouter_get_param_float(widget->self_layout, "y", 0);
  h_layout = self_layouter_get_param_float(widget->self_layout, "h", 0);
  axis->draw_rect.y = y_layout != 0 ? widget->y : r->y;
  axis->draw_rect.h = h_layout != 0 ? widget->h : r->h;
  if (axis->at == AXIS_AT_RIGHT) {
    axis->draw_rect.x = r->x + r->w - 1;
    axis->draw_rect.w = widget->w == 0 ? 1 : widget->w;
    axis->split_line.line_len = -r->w;
  } else {
    axis->draw_rect.x = r->x;
    axis->draw_rect.w = -(widget->w == 0 ? 1 : widget->w);
    axis->split_line.line_len = r->w;
  }

  return RET_OK;
}

static float_t y_axis_measure_series_interval(widget_t* widget) {
  float_t vrange = 0;
  float_t prange = 0;
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL, 0);

  vrange = axis_get_range(widget);
  prange = axis->draw_rect.h;

  if (axis->type == AXIS_TYPE_CATEGORY) {
    assert(vrange > 0);
    return prange / vrange;
  } else {
    assert(vrange > 1);
    return (prange - 1) / (vrange - 1);
  }
}

ret_t y_axis_measure_series(widget_t* widget, void* measure_params, fifo_t* src, fifo_t* dst) {
  int32_t i;
  int32_t nr;
  int32_t index;
  float_t sinterval, soffset;
  float_t x, prange, vrange, vmin, srange;
  axis_measure_series_params_t* params = (axis_measure_series_params_t*)measure_params;
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && params != NULL, RET_BAD_PARAMS);
  return_value_if_fail(src != NULL && dst != NULL, RET_BAD_PARAMS);
  return_value_if_fail(dst->unit_size > 0, RET_BAD_PARAMS);

  nr = tk_min(dst->capacity, src->size);
  return_value_if_true(nr == 0, RET_OK);

  vmin = axis->max * axis->min > 0 ? axis->min : 0;
  vrange = axis_get_range(widget);
  prange = axis->draw_rect.h;
  srange = axis_get_range(params->series_axis);
  sinterval = axis_measure_series_interval(params->series_axis);
  sinterval = AXIS(params->series_axis)->inverse ? -sinterval : sinterval;
  soffset = AXIS(params->series_axis)->type == AXIS_TYPE_CATEGORY ? (sinterval / 2) : 0;

  if (tk_abs(sinterval) >= 1.0) {
    for (i = 0; i < nr; i++) {
      index = src->size - params->index_of_fifo - nr + i;
      x = sinterval * ((float_t)i - params->index_of_fold);
      if (i < params->index_of_fold) {
        x = sinterval * (srange - params->index_of_fold + i) + soffset;
      } else {
        x = sinterval * (i - params->index_of_fold) + soffset;
      }

      fifo_push(dst, NULL);
      params->draw_data_set(fifo_at(dst, dst->size - 1), x, src, index, vmin, vrange, prange,
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
      x = sinterval * ((float_t)i - params->index_of_fold);
      if (i < params->index_of_fold) {
        x = sinterval * (srange - params->index_of_fold + i) + soffset;
      } else {
        x = sinterval * (i - params->index_of_fold) + soffset;
      }

      params->draw_data_set(d, x, src, index, vmin, vrange, prange, axis->inverse);

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

ret_t y_axis_draw_tick(axis_t* axis, canvas_t* c) {
  style_t* style = axis->tick.astyle;
  bitmap_t img;
  color_t trans = color_init(0, 0, 0, 0);
  color_t color = style_get_color(style, STYLE_ID_FG_COLOR, trans);
  const char* image_name = style_get_str(style, STYLE_ID_FG_IMAGE, NULL);
  image_draw_type_t draw_type =
      (image_draw_type_t)style_get_int(style, STYLE_ID_FG_IMAGE_DRAW_TYPE, IMAGE_DRAW_PATCH3_Y);
  int32_t axis_offset = widget_get_prop_int(WIDGET(axis), AXIS_PROP_OFFSET, 0);
  rect_t r = rect_init(axis->draw_rect.x + axis_offset, 0, AXIS_DEFAULT_TICK_LEN, 1);
  const axis_data_t** labels = (const axis_data_t**)(axis->data->elms);
  float_t offset = 0;
  uint32_t i;
  uint32_t nr = axis->data->size;

  if (axis->type == AXIS_TYPE_CATEGORY && axis->tick.align_with_label) {
    offset = (nr > 1 ? (labels[1]->tick - labels[0]->tick) : axis->draw_rect.h) / 2.0;
    nr--;
  }

  if (((axis->at == AXIS_AT_LEFT || axis->at == AXIS_AT_AUTO) && !axis->tick.inside) ||
      (axis->at == AXIS_AT_RIGHT && axis->tick.inside)) {
    r.w = -r.w;
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
        r.y = axis->draw_rect.y + labels[i]->tick + offset;
      } else {
        r.y = axis->draw_rect.y + axis->draw_rect.h - 1 - (labels[i]->tick + offset);
      }
      continue_if_true(r.y < axis->draw_rect.y);
      continue_if_true(r.y >= axis->draw_rect.y + axis->draw_rect.h);

      if (color.rgba.a) {
        canvas_draw_hline(c, r.x, r.y, r.w);
      }

      if (image_name != NULL) {
        canvas_draw_image_ex(c, &img, draw_type, &r);
      }
    }

    if (axis->type == AXIS_TYPE_CATEGORY && !axis->tick.align_with_label) {
      if (axis->inverse) {
        r.y = axis->draw_rect.y + axis->draw_rect.h - 1;
      } else {
        r.y = axis->draw_rect.y;
      }

      if (color.rgba.a) {
        canvas_draw_hline(c, r.x, r.y, r.w);
      }

      if (image_name != NULL) {
        canvas_draw_image_ex(c, &img, draw_type, &r);
      }
    }
  }

  return RET_OK;
}

ret_t y_axis_draw_line(axis_t* axis, canvas_t* c) {
  style_t* style = axis->line.astyle;
  bitmap_t img;
  color_t trans = color_init(0, 0, 0, 0);
  color_t color = style_get_color(style, STYLE_ID_FG_COLOR, trans);
  const char* image_name = style_get_str(style, STYLE_ID_FG_IMAGE, NULL);
  image_draw_type_t draw_type =
      (image_draw_type_t)style_get_int(style, STYLE_ID_FG_IMAGE_DRAW_TYPE, IMAGE_DRAW_PATCH3_Y);
  int32_t axis_offset = widget_get_prop_int(WIDGET(axis), AXIS_PROP_OFFSET, 0);
  rect_t r = rect_init(axis->draw_rect.x + axis_offset, axis->draw_rect.y, 1, axis->draw_rect.h);

  r.y -= axis->line.lengthen;
  r.h += axis->line.lengthen * 2;

  if (color.rgba.a) {
    canvas_set_stroke_color(c, color);
    canvas_draw_vline(c, r.x, r.y, r.h);
  }

  if (image_name != NULL) {
    if (widget_load_image(WIDGET(axis), image_name, &img) == RET_OK) {
      canvas_draw_image_ex(c, &img, draw_type, &r);
    }
  }

  return RET_OK;
}

ret_t y_axis_draw_split_line(axis_t* axis, canvas_t* c) {
  style_t* style = axis->split_line.astyle;
  bitmap_t img;
  color_t trans = color_init(0, 0, 0, 0);
  color_t color = style_get_color(style, STYLE_ID_FG_COLOR, trans);
  const char* image_name = style_get_str(style, STYLE_ID_FG_IMAGE, NULL);
  image_draw_type_t draw_type =
      (image_draw_type_t)style_get_int(style, STYLE_ID_FG_IMAGE_DRAW_TYPE, IMAGE_DRAW_PATCH3_Y);
  rect_t r = rect_init(axis->draw_rect.x, 0, axis->split_line.line_len, 1);
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
        r.y = axis->draw_rect.y + labels[i]->tick;
      } else {
        r.y = axis->draw_rect.y + axis->draw_rect.h - 1 - labels[i]->tick;
      }
      continue_if_true(r.y < axis->draw_rect.y);
      continue_if_true(r.y >= axis->draw_rect.y + axis->draw_rect.h);

      if (color.rgba.a) {
        canvas_draw_hline(c, r.x, r.y, r.w);
      }

      if (image_name != NULL) {
        canvas_draw_image_ex(c, &img, draw_type, &r);
      }
    }

    if (axis->type == AXIS_TYPE_CATEGORY && !axis->tick.align_with_label) {
      if (axis->inverse) {
        r.y = axis->draw_rect.y + axis->draw_rect.h - 1;
      } else {
        r.y = axis->draw_rect.y;
      }

      if (color.rgba.a) {
        canvas_draw_hline(c, r.x, r.y, r.w);
      }

      if (image_name != NULL) {
        canvas_draw_image_ex(c, &img, draw_type, &r);
      }
    }
  }

  return RET_OK;
}

ret_t y_axis_draw_label(axis_t* axis, canvas_t* c) {
  style_t* style = axis->label.astyle;
  color_t trans = color_init(0, 0, 0, 0);
  color_t color = style_get_color(style, STYLE_ID_TEXT_COLOR, trans);
  const char* font_name = style_get_str(style, STYLE_ID_FONT_NAME, NULL);
  uint16_t font_size = style_get_int(style, STYLE_ID_FONT_SIZE, TK_DEFAULT_FONT_SIZE);
  int32_t margin = style_get_int(style, STYLE_ID_MARGIN, 0);
  float_t spacer = 0;
  float_t offset = 0;
  int32_t axis_offset = widget_get_prop_int(WIDGET(axis), AXIS_PROP_OFFSET, 0);
  rect_t r = rect_init(axis->draw_rect.x + axis_offset, 0, font_size, axis->draw_rect.h);
  const axis_data_t** labels = (const axis_data_t**)(axis->data->elms);
  uint32_t i;

  if (axis->data->size > 1) {
    r.h = labels[0]->tick - labels[1]->tick;
  }

  if (axis->type == AXIS_TYPE_VALUE) {
    offset = -r.h / 2;
  } else if (axis->type == AXIS_TYPE_CATEGORY) {
    r.h = axis->inverse ? -r.h : r.h;
  }

  spacer =
      (axis->tick.show && !(axis->tick.inside ^ axis->label.inside) ? AXIS_DEFAULT_TICK_LEN : 0) +
      margin;

  if ((axis->at == AXIS_AT_RIGHT && !axis->label.inside) ||
      ((axis->at == AXIS_AT_AUTO || axis->at == AXIS_AT_LEFT) && axis->label.inside)) {
    canvas_set_text_align(c, ALIGN_H_LEFT, ALIGN_V_MIDDLE);
    r.x += spacer;
  } else {
    canvas_set_text_align(c, ALIGN_H_RIGHT, ALIGN_V_MIDDLE);
    r.x -= spacer + r.w + 1;
  }

  canvas_set_text_color(c, color);
  canvas_set_font(c, font_name, font_size);

  if (color.rgba.a) {
    for (i = 0; i < axis->data->size; i++) {
      if (axis->inverse) {
        r.y = axis->draw_rect.y + labels[i]->tick + offset;
      } else {
        r.y = axis->draw_rect.y + axis->draw_rect.h - 1 - labels[i]->tick + offset;
      }

      canvas_draw_text_in_rect(c, labels[i]->text.str, labels[i]->text.size, &r);
    }
  }

  return RET_OK;
}

ret_t y_axis_on_paint_before(widget_t* widget, canvas_t* c) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && c != NULL, RET_BAD_PARAMS);

  if (axis->data != NULL) {
    if (axis->split_line.show) {
      y_axis_draw_split_line(axis, c);
      axis->painted_before = TRUE;
    }
  }

  return RET_OK;
}

ret_t y_axis_on_paint_self(widget_t* widget, canvas_t* c) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && c != NULL, RET_BAD_PARAMS);

  if (axis->data != NULL) {
    if (axis->split_line.show && !axis->painted_before) {
      y_axis_draw_split_line(axis, c);
    }

    if (axis->tick.show) {
      y_axis_draw_tick(axis, c);
    }

    if (axis->label.show) {
      y_axis_draw_label(axis, c);
    }
  }

  if (axis->line.show) {
    y_axis_draw_line(axis, c);
  }

  axis->painted_before = FALSE;

  return RET_OK;
}

static const char* s_y_axis_properties[] = {WIDGET_PROP_MIN,
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

TK_DECL_VTABLE(y_axis) = {.type = WIDGET_TYPE_Y_AXIS,
                          .size = sizeof(y_axis_t),
                          .parent = TK_PARENT_VTABLE(axis),
                          .create = y_axis_create,
                          .clone_properties = s_y_axis_properties,
                          .persistent_properties = s_y_axis_properties,
                          .on_paint_begin = y_axis_on_paint_begin,
                          .on_paint_self = y_axis_on_paint_self,
                          .set_prop = axis_p_set_prop,
                          .get_prop = axis_p_get_prop,
                          .on_destroy = axis_on_destroy};

static const axis_vtable_t s_y_axis_internal_vtable = {
    .on_self_layout = y_axis_on_self_layout,
    .on_paint_before = y_axis_on_paint_before,
    .measure_series_interval = y_axis_measure_series_interval,
    .measure_series = y_axis_measure_series};

widget_t* y_axis_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = axis_create(parent, TK_REF_VTABLE(y_axis), x, y, w, h);
  y_axis_t* y_axis = Y_AXIS(widget);
  return_value_if_fail(y_axis != NULL, NULL);

  y_axis->base.vt = &s_y_axis_internal_vtable;
  y_axis->base.draw_rect = rect_init(x, y, w, h);

  return widget;
}
widget_t* y_axis_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, y_axis), NULL);

  return widget;
}
