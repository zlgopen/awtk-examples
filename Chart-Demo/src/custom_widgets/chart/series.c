/**
 * File:   series.c
 * Author: AWTK Develop Team
 * Brief:  series
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

#include "series.h"

#define _COLOR_BLACK color_init(0, 0, 0, 0xff)
#define _COLOR_TRANS color_init(0, 0, 0, 0)

// 坐标取整加0.5，防止线宽为1时显示2个像素
#define _VG_XY(v) (float_t)((xy_t)(v) + 0.5)
#define _VGCANVAS_MOVE_TO(vg, x, y) vgcanvas_move_to(vg, _VG_XY(x), _VG_XY(y))
#define _VGCANVAS_LINE_TO(vg, x, y) vgcanvas_line_to(vg, _VG_XY(x), _VG_XY(y))
#define _VGCANVAS_RECT(vg, x, y, w, h) vgcanvas_rect(vg, _VG_XY(x), _VG_XY(y), w, h)
#define _VGCANVAS_ARC(vg, x, y, r, start, end, ccw) \
  vgcanvas_arc(vg, _VG_XY(x), _VG_XY(y), r, start, end, ccw)
#define _VACANVAS_BEZIER_TO(vg, cp1x, cp1y, cp2x, cp2y, x, y)                               \
  vgcanvas_bezier_to(vg, _VG_XY(cp1x), _VG_XY(cp1y), _VG_XY(cp2x), _VG_XY(cp2y), _VG_XY(x), \
                     _VG_XY(y))

ret_t series_draw_line(vgcanvas_t* vg, float_t* x, float_t* y, uint32_t nr,
                       series_line_params_t* params) {
  if (params->show) {
    uint32_t i;
    assert(nr > 1);
    vgcanvas_set_stroke_color(vg, params->color);
    vgcanvas_set_line_width(vg, params->width);
    vgcanvas_begin_path(vg);
    _VGCANVAS_MOVE_TO(vg, x[0], y[0]);
    for (i = 1; i < nr; i++) {
      _VGCANVAS_LINE_TO(vg, x[i], y[i]);
    }
    vgcanvas_stroke(vg);
  }

  return RET_OK;
}

ret_t series_draw_line_area(vgcanvas_t* vg, float_t x0, float_t y0, float_t* x, float_t* y,
                            uint32_t nr, series_line_area_params_t* params, bool_t vertical) {
  if (params->show) {
    uint32_t i;
    assert(nr > 1);
    vgcanvas_set_fill_color(vg, params->color);
    vgcanvas_begin_path(vg);
    _VGCANVAS_MOVE_TO(vg, x[0], y[0]);
    for (i = 1; i < nr; i++) {
      _VGCANVAS_LINE_TO(vg, x[i], y[i]);
    }
    if (vertical) {
      _VGCANVAS_LINE_TO(vg, x[nr - 1], y0);
      _VGCANVAS_LINE_TO(vg, x[0], y0);
    } else {
      _VGCANVAS_LINE_TO(vg, x0, y[nr - 1]);
      _VGCANVAS_LINE_TO(vg, x0, y[0]);
    }
    vgcanvas_fill(vg);
  }

  return RET_OK;
}

ret_t series_draw_smooth_line(vgcanvas_t* vg, float_t* x, float_t* y, uint32_t nr,
                              series_line_params_t* params) {
  if (params->show) {
    uint32_t i;
    float_t cp1x, cp1y, cp2x, cp2y;
    assert(nr > 1);
    vgcanvas_set_stroke_color(vg, params->color);
    vgcanvas_set_line_width(vg, params->width);
    vgcanvas_set_line_cap(vg, "round");
    vgcanvas_set_line_join(vg, "round");
    vgcanvas_begin_path(vg);
    _VGCANVAS_MOVE_TO(vg, x[0], y[0]);
    if (nr == 2) {
      _VGCANVAS_LINE_TO(vg, x[1], y[1]);
    } else {
      for (i = 1; i < nr; i++) {
        if (i == 1) {
          cp1x = x[i - 1] + (x[i] - x[i]) / 4;
          cp1y = y[i - 1] + (y[i] - y[i]) / 4;
          cp2x = x[i] - (x[i + 1] - x[i - 1]) / 4;
          cp2y = y[i] - (y[i + 1] - y[i - 1]) / 4;
        } else if (i == nr - 1) {
          cp1x = x[i - 1] + (x[i] - x[i - 2]) / 4;
          cp1y = y[i - 1] + (y[i] - y[i - 2]) / 4;
          cp2x = x[i] - (x[i] - x[i - 1]) / 4;
          cp2y = y[i] - (y[i] - y[i - 1]) / 4;
        } else {
          cp1x = x[i - 1] + (x[i] - x[i - 2]) / 4;
          cp1y = y[i - 1] + (y[i] - y[i - 2]) / 4;
          cp2x = x[i] - (x[i + 1] - x[i - 1]) / 4;
          cp2y = y[i] - (y[i + 1] - y[i - 1]) / 4;
        }
        _VACANVAS_BEZIER_TO(vg, cp1x, cp1y, cp2x, cp2y, x[i], y[i]);
      }
    }
    vgcanvas_stroke(vg);
  }

  return RET_OK;
}

ret_t series_draw_smooth_line_area(vgcanvas_t* vg, float_t x0, float_t y0, float_t* x, float_t* y,
                                   uint32_t nr, series_line_area_params_t* params,
                                   bool_t vertical) {
  if (params->show) {
    uint32_t i;
    float_t cp1x, cp1y, cp2x, cp2y;
    assert(nr > 1);
    vgcanvas_set_fill_color(vg, params->color);
    vgcanvas_set_line_cap(vg, "round");
    vgcanvas_set_line_join(vg, "round");
    vgcanvas_begin_path(vg);
    _VGCANVAS_MOVE_TO(vg, x[0], y[0]);
    if (nr == 2) {
      _VGCANVAS_LINE_TO(vg, x[1], y[1]);
    } else {
      for (i = 1; i < nr; i++) {
        if (i == 1) {
          cp1x = x[i - 1] + (x[i] - x[i]) / 4;
          cp1y = y[i - 1] + (y[i] - y[i]) / 4;
          cp2x = x[i] - (x[i + 1] - x[i - 1]) / 4;
          cp2y = y[i] - (y[i + 1] - y[i - 1]) / 4;
        } else if (i == nr - 1) {
          cp1x = x[i - 1] + (x[i] - x[i - 2]) / 4;
          cp1y = y[i - 1] + (y[i] - y[i - 2]) / 4;
          cp2x = x[i] - (x[i] - x[i - 1]) / 4;
          cp2y = y[i] - (y[i] - y[i - 1]) / 4;
        } else {
          cp1x = x[i - 1] + (x[i] - x[i - 2]) / 4;
          cp1y = y[i - 1] + (y[i] - y[i - 2]) / 4;
          cp2x = x[i] - (x[i + 1] - x[i - 1]) / 4;
          cp2y = y[i] - (y[i + 1] - y[i - 1]) / 4;
        }
        _VACANVAS_BEZIER_TO(vg, cp1x, cp1y, cp2x, cp2y, x[i], y[i]);
      }
    }
    if (vertical) {
      _VGCANVAS_LINE_TO(vg, x[nr - 1], y0);
      _VGCANVAS_LINE_TO(vg, x[0], y0);
    } else {
      _VGCANVAS_LINE_TO(vg, x0, y[nr - 1]);
      _VGCANVAS_LINE_TO(vg, x0, y[0]);
    }
    vgcanvas_fill(vg);
  }

  return RET_OK;
}

ret_t series_draw_symbol(vgcanvas_t* vg, float_t* x, float_t* y, uint32_t nr,
                         series_symbol_params_t* params, bitmap_t* img) {
  if (params->show) {
    uint32_t i;
    vgcanvas_set_fill_color(vg, params->fill_color);
    if (params->stroke_color.color != _COLOR_TRANS.color) {
      vgcanvas_set_stroke_color(vg, params->stroke_color);
    }
    for (i = 0; i < nr; i++) {
      vgcanvas_begin_path(vg);
      _VGCANVAS_ARC(vg, x[i], y[i], params->size, 0, 2 * M_PI, FALSE);
      if (img) {
        vgcanvas_paint(vg, FALSE, img);
      } else {
        vgcanvas_fill(vg);
        if (params->stroke_color.color != _COLOR_TRANS.color) {
          vgcanvas_stroke(vg);
        }
      }
    }
  }

  return RET_OK;
}

ret_t series_draw_bar(vgcanvas_t* vg, float_t x0, float_t y0, float_t* x, float_t* y, uint32_t nr,
                      series_bar_params_t* params, float_t size, bool_t vertical, bitmap_t* img) {
  uint32_t i;
  vgcanvas_set_fill_color(vg, params->color);
  if (vertical) {
    for (i = 0; i < nr; i++) {
      vgcanvas_begin_path(vg);
      _VGCANVAS_RECT(vg, x0 + x[i], y0, size, y[i]);
      if (img) {
        vgcanvas_paint(vg, FALSE, img);
      } else {
        vgcanvas_fill(vg);
      }
    }
  } else {
    for (i = 0; i < nr; i++) {
      vgcanvas_begin_path(vg);
      _VGCANVAS_RECT(vg, x0, y0 + y[i], x[i], size);
      if (img) {
        vgcanvas_paint(vg, FALSE, img);
      } else {
        vgcanvas_fill(vg);
      }
    }
  }

  return RET_OK;
}

ret_t series_draw_bar_c(canvas_t* c, float_t x0, float_t y0, float_t* x, float_t* y, uint32_t nr,
                        series_bar_params_t* params, float_t size, bool_t vertical) {
  uint32_t i;
  canvas_set_fill_color(c, params->color);
  if (vertical) {
    for (i = 0; i < nr; i++) {
      canvas_fill_rect(c, x0 + x[i], y0, size, y[i]);
    }
  } else {
    for (i = 0; i < nr; i++) {
      canvas_fill_rect(c, x0, y0 + y[i], x[i], size);
    }
  }

  return RET_OK;
}

ret_t series_on_window_open(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  if (widget) {
    widget_start_animator(widget, NULL);
  }
  return RET_REMOVE;
}

ret_t series_set_data_float(float_t* buffer, uint32_t* buffer_size, uint32_t buffer_capacity,
                            const float_t* src, uint32_t src_size) {
  assert(buffer != NULL && buffer_size != NULL && src != NULL);

  uint32_t nr = tk_min(src_size, buffer_capacity);
  memcpy(buffer, src, sizeof(float_t) * nr);
  *buffer_size = nr;

  return RET_OK;
}

ret_t series_fifo_float(float_t* fifo, uint32_t* fifo_size, uint32_t fifo_capacity,
                        const float_t* src, uint32_t src_size) {
  assert(fifo != NULL && fifo_size != NULL && src != NULL);

  if (src_size >= fifo_capacity) {
    memcpy(fifo, &(src[src_size - fifo_capacity]), sizeof(float_t) * fifo_capacity);
    *fifo_size = fifo_capacity;
  } else if (src_size + *fifo_size > fifo_capacity) {
    uint32_t over = src_size + *fifo_size - fifo_capacity;
    uint32_t rest = *fifo_size - over;
    memcpy(fifo, &(fifo[over]), sizeof(float_t) * rest);
    memcpy(&(fifo[rest]), src, sizeof(float_t) * src_size);
    *fifo_size = fifo_capacity;
  } else {
    memcpy(&(fifo[*fifo_size]), src, sizeof(float_t) * src_size);
    *fifo_size += src_size;
  }

  return RET_OK;
}

ret_t series_animator_float_set_params(widget_animator_t* animator, float_t from, float_t to,
                                       void* ctx, chart_animator_on_update_t on_update,
                                       chart_animator_on_destroy_t on_destroy) {
  chart_animator_t* chart = (chart_animator_t*)animator;
  return_value_if_fail(chart != NULL, RET_BAD_PARAMS);

  value_set_float(&(chart->to), to);
  value_set_float(&(chart->from), from);
  chart->ctx = ctx;
  chart->on_update = on_update;
  chart->on_destroy = on_destroy;

  return RET_OK;
}

ret_t series_animator_floats_set_params(widget_animator_t* animator, float_t* from, float_t* to,
                                        void* ctx, chart_animator_on_update_t on_update,
                                        chart_animator_on_destroy_t on_destroy) {
  chart_animator_t* chart = (chart_animator_t*)animator;
  return_value_if_fail(chart != NULL, RET_BAD_PARAMS);

  value_set_pointer(&(chart->to), to);
  value_set_pointer(&(chart->from), from);
  chart->ctx = ctx;
  chart->on_update = on_update;
  chart->on_destroy = on_destroy;

  return RET_OK;
}

void series_animator_floats_on_destroy(chart_animator_t* animator) {
  float_t* from = (float_t*)value_pointer(&(animator->from));
  float_t* to = (float_t*)value_pointer(&(animator->to));
  TKMEM_FREE(from);
  TKMEM_FREE(to);
}
