/**
 * File:   series.h
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

#ifndef TK_SERIES_H
#define TK_SERIES_H

#include "awtk.h"
#include "chart_animator.h"

BEGIN_C_DECLS

/**
 * 曲线/折线
 */
typedef struct _series_line_params_t {
  /**
   * 线的颜色。
   */
  color_t color;
  /**
   * 线宽。
   */
  float_t width;
  /**
   * 是否显示。
   */
  uint8_t show : 1;
  /**
   * 是否平滑线条。
   */
  uint8_t smooth : 1;
} series_line_params_t;

/**
 * 坐标系中曲线/折线与轴围成的区域
 */
typedef struct _series_line_area_params_t {
  /**
   * 颜色。
   */
  color_t color;
  /**
   * 是否显示。
   */
  uint8_t show : 1;
} series_line_area_params_t;

/**
 * 坐标点
 */
typedef struct _series_symbol_params_t {
  /**
   * 填充颜色
   */
  color_t fill_color;
  /**
   * 边框颜色
   */
  color_t stroke_color;
  /**
   * 填充图形
   */
  char* image;
  /**
   * 大小
   */
  float_t size;
  /**
   * 是否显示
   */
  uint8_t show : 1;
} series_symbol_params_t;

/**
 * 柱条
 */
typedef struct _series_bar_params_t {
  /**
   * 填充颜色
   */
  color_t color;
  /**
   * 填充图形
   */
  char* image;
} series_bar_params_t;

/**
 * series_t
 */
typedef struct _series_t {
  widget_t widget;
  /**
   * 标题
   */
  wchar_t* title;
  /**
   * 值更新时是否启用动画
   */
  uint8_t animatic : 1;
} series_t;

ret_t series_set_data_float(float_t* buffer, uint32_t* buffer_size, uint32_t buffer_capacity,
                            const float_t* src, uint32_t src_size);
ret_t series_draw_line(vgcanvas_t* vg, float_t* x, float_t* y, uint32_t nr,
                       series_line_params_t* params);
ret_t series_draw_line_area(vgcanvas_t* vg, float_t x0, float_t y0, float_t* x, float_t* y,
                            uint32_t nr, series_line_area_params_t* params, bool_t vertical);
ret_t series_draw_smooth_line(vgcanvas_t* vg, float_t* x, float_t* y, uint32_t nr,
                              series_line_params_t* params);
ret_t series_draw_smooth_line_area(vgcanvas_t* vg, float_t x0, float_t y0, float_t* x, float_t* y,
                                   uint32_t nr, series_line_area_params_t* params, bool_t vertical);
ret_t series_draw_symbol(vgcanvas_t* vg, float_t* x, float_t* y, uint32_t nr,
                         series_symbol_params_t* params, bitmap_t* img);
ret_t series_draw_bar(vgcanvas_t* vg, float_t x0, float_t y0, float_t* x, float_t* y, uint32_t nr,
                      series_bar_params_t* params, float_t size, bool_t vertical, bitmap_t* img);
ret_t series_draw_bar_c(canvas_t* c, float_t x0, float_t y0, float_t* x, float_t* y, uint32_t nr,
                        series_bar_params_t* params, float_t size, bool_t vertical);
ret_t series_on_window_open(void* ctx, event_t* e);
ret_t series_fifo_float(float_t* fifo, uint32_t* fifo_size, uint32_t fifo_capacity,
                        const float_t* src, uint32_t src_size);

ret_t series_animator_float_set_params(widget_animator_t* animator, float_t from, float_t to,
                                       void* ctx, chart_animator_on_update_t on_update,
                                       chart_animator_on_destroy_t on_destroy);
ret_t series_animator_floats_set_params(widget_animator_t* animator, float_t* from, float_t* to,
                                        void* ctx, chart_animator_on_update_t on_update,
                                        chart_animator_on_destroy_t on_destroy);
void series_animator_floats_on_destroy(chart_animator_t* animator);

#define SERIES_PROP_TITLE "title"
#define SERIES_PROP_ANIMATIC "animatic"
#define SERIES_PROP_CAPACITY "capacity"
#define SERIES_PROP_LINE "line"
#define SERIES_PROP_LINE_COLOR "line.color"
#define SERIES_PROP_LINE_WIDTH "line.width"
#define SERIES_PROP_LINE_SHOW "line.show"
#define SERIES_PROP_LINE_SMOOTH "line.smooth"
#define SERIES_PROP_AREA "area"
#define SERIES_PROP_AREA_COLOR "area.color"
#define SERIES_PROP_AREA_SHOW "area.show"
#define SERIES_PROP_SYMBOL "symbol"
#define SERIES_PROP_SYMBOL_FILL_COLOR "symbol.fill_color"
#define SERIES_PROP_SYMBOL_STROKE_COLOR "symbol.stroke_color"
#define SERIES_PROP_SYMBOL_IMAGE "symbol.image"
#define SERIES_PROP_SYMBOL_SIZE "symbol.size"
#define SERIES_PROP_SYMBOL_SHOW "symbol.show"
#define SERIES_PROP_BAR "bar"
#define SERIES_PROP_BAR_COLOR "bar.color"
#define SERIES_PROP_BAR_IMAGE "bar.image"

#define SERIES(widget) ((series_t*)(widget))

END_C_DECLS

#endif /*TK_SERIES_H*/
