/**
 * File:   series_p.h
 * Author: AWTK Develop Team
 * Brief:  series private
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

#ifndef TK_SERIES_PRIVATE_H
#define TK_SERIES_PRIVATE_H

#include "series.h"

BEGIN_C_DECLS

typedef struct _series_p_draw_data_t {
  float_t x;
  float_t y;
} series_p_draw_data_t;

float_t series_p_draw_data_get_x(const void* data);
float_t series_p_draw_data_get_y(const void* data);
int series_p_draw_data_compare_x(const void* a, const void* b);
int series_p_draw_data_compare_y(const void* a, const void* b);
void* series_p_draw_data_min_x(void* d, const void* data);
void* series_p_draw_data_min_y(void* d, const void* data);
void* series_p_draw_data_max_x(void* d, const void* data);
void* series_p_draw_data_max_y(void* d, const void* data);
ret_t series_p_draw_data_set_yx(void* dst, float_t series, fifo_t* value, uint32_t value_index,
                                float_t value_min, float_t value_range, float_t pixel_range,
                                bool_t inverse);
ret_t series_p_draw_data_set_xy(void* dst, float_t series, fifo_t* value, uint32_t value_index,
                                float_t value_min, float_t value_range, float_t pixel_range,
                                bool_t inverse);

typedef struct _series_p_colorful_draw_data_t {
  float_t x;
  float_t y;
  color_t c;
} series_p_colorful_draw_data_t;

float_t series_p_colorful_draw_data_get_x(const void* data);
float_t series_p_colorful_draw_data_get_y(const void* data);
int series_p_colorful_draw_data_compare_x(const void* a, const void* b);
int series_p_colorful_draw_data_compare_y(const void* a, const void* b);
void* series_p_colorful_draw_data_min_x(void* d, const void* data);
void* series_p_colorful_draw_data_min_y(void* d, const void* data);
void* series_p_colorful_draw_data_max_x(void* d, const void* data);
void* series_p_colorful_draw_data_max_y(void* d, const void* data);
ret_t series_p_colorful_draw_data_set_yx(void* dst, float_t series, fifo_t* value,
                                         uint32_t value_index, float_t value_min,
                                         float_t value_range, float_t pixel_range, bool_t inverse);
ret_t series_p_colorful_draw_data_set_xy(void* dst, float_t series, fifo_t* value,
                                         uint32_t value_index, float_t value_min,
                                         float_t value_range, float_t pixel_range, bool_t inverse);

typedef struct _series_p_minmax_draw_data_t {
  float_t xmin;
  float_t xmax;
  float_t ymin;
  float_t ymax;
} series_p_minmax_draw_data_t;

float_t series_p_minmax_draw_data_get_x(const void* data);
float_t series_p_minmax_draw_data_get_y(const void* data);
int series_p_minmax_draw_data_compare_x(const void* a, const void* b);
int series_p_minmax_draw_data_compare_y(const void* a, const void* b);
void* series_p_minmax_draw_data_min_x(void* d, const void* data);
void* series_p_minmax_draw_data_min_y(void* d, const void* data);
void* series_p_minmax_draw_data_max_x(void* d, const void* data);
void* series_p_minmax_draw_data_max_y(void* d, const void* data);
ret_t series_p_minmax_draw_data_set_yx(void* dst, float_t series, fifo_t* value,
                                       uint32_t value_index, float_t value_min, float_t value_range,
                                       float_t pixel_range, bool_t inverse);
ret_t series_p_minmax_draw_data_set_xy(void* dst, float_t series, fifo_t* value,
                                       uint32_t value_index, float_t value_min, float_t value_range,
                                       float_t pixel_range, bool_t inverse);

typedef ret_t (*series_p_draw_line_t)(widget_t* widget, canvas_t* c, vgcanvas_t* vg, style_t* style,
                                      float_t ox, float_t oy, fifo_t* fifo, uint32_t index,
                                      uint32_t size);
typedef ret_t (*series_p_draw_line_area_t)(widget_t* widget, vgcanvas_t* vg, style_t* style,
                                           float_t ox, float_t oy, fifo_t* fifo, uint32_t index,
                                           uint32_t size, bool_t vertical);
typedef ret_t (*series_p_draw_smooth_line_t)(widget_t* widget, vgcanvas_t* vg, style_t* style,
                                             float_t ox, float_t oy, fifo_t* fifo, uint32_t index,
                                             uint32_t size, bool_t vertical);
typedef ret_t (*series_p_draw_smooth_line_area_t)(widget_t* widget, vgcanvas_t* vg, style_t* style,
                                                  float_t ox, float_t oy, fifo_t* fifo,
                                                  uint32_t index, uint32_t size, bool_t vertical);
typedef ret_t (*series_p_draw_symbol_t)(widget_t* widget, vgcanvas_t* vg, style_t* style,
                                        float_t ox, float_t oy, fifo_t* fifo, uint32_t index,
                                        uint32_t size, float_t symbol_size);

ret_t series_p_draw_line(widget_t* widget, canvas_t* c, vgcanvas_t* vg, style_t* style, float_t ox,
                         float_t oy, fifo_t* fifo, uint32_t index, uint32_t size);
ret_t series_p_draw_line_colorful(widget_t* widget, canvas_t* c, vgcanvas_t* vg, style_t* style,
                                  float_t ox, float_t oy, fifo_t* fifo, uint32_t index,
                                  uint32_t size);
ret_t series_p_draw_line_area(widget_t* widget, vgcanvas_t* vg, style_t* style, float_t ox,
                              float_t oy, fifo_t* fifo, uint32_t index, uint32_t size,
                              bool_t vertical);
ret_t series_p_draw_line_area_colorful(widget_t* widget, vgcanvas_t* vg, style_t* style, float_t ox,
                                       float_t oy, fifo_t* fifo, uint32_t index, uint32_t size,
                                       bool_t vertical);
ret_t series_p_draw_smooth_line(widget_t* widget, vgcanvas_t* vg, style_t* style, float_t ox,
                                float_t oy, fifo_t* fifo, uint32_t index, uint32_t size,
                                bool_t vertical);
ret_t series_p_draw_smooth_line_colorful(widget_t* widget, vgcanvas_t* vg, style_t* style,
                                         float_t ox, float_t oy, fifo_t* fifo, uint32_t index,
                                         uint32_t size, bool_t vertical);
ret_t series_p_draw_smooth_line_area(widget_t* widget, vgcanvas_t* vg, style_t* style, float_t ox,
                                     float_t oy, fifo_t* fifo, uint32_t index, uint32_t size,
                                     bool_t vertical);
ret_t series_p_draw_smooth_line_area_colorful(widget_t* widget, vgcanvas_t* vg, style_t* style,
                                              float_t ox, float_t oy, fifo_t* fifo, uint32_t index,
                                              uint32_t size, bool_t vertical);
ret_t series_p_draw_symbol(widget_t* widget, vgcanvas_t* vg, style_t* style, float_t ox, float_t oy,
                           fifo_t* fifo, uint32_t index, uint32_t size, float_t symbol_size);
ret_t series_p_draw_symbol_colorful(widget_t* widget, vgcanvas_t* vg, style_t* style, float_t ox,
                                    float_t oy, fifo_t* fifo, uint32_t index, uint32_t size,
                                    float_t symbol_size);
ret_t series_p_draw_bar(widget_t* widget, canvas_t* c, vgcanvas_t* vg, style_t* style, float_t ox,
                        float_t oy, fifo_t* fifo, uint32_t index, uint32_t size, float_t bar_size,
                        bool_t vertical, bool_t minmax);

typedef widget_animator_t* (*series_animator_create_t)(widget_t* widget, uint32_t duration,
                                                       uint32_t delay, easing_type_t easing);

ret_t series_p_reset_fifo(widget_t* widget);
uint32_t series_p_count(widget_t* widget);
ret_t series_p_set_with_animator(widget_t* widget, uint32_t index, const void* data, uint32_t nr,
                                 series_animator_create_t create_animator);
ret_t series_p_set_default(widget_t* widget, uint32_t index, const void* data, uint32_t nr);
ret_t series_p_rset(widget_t* widget, uint32_t index, const void* data, uint32_t nr);
ret_t series_p_push(widget_t* widget, const void* data, uint32_t nr);
ret_t series_p_pop(widget_t* widget, uint32_t nr);
void* series_p_at(widget_t* widget, uint32_t index);
ret_t series_p_get_current(widget_t* widget, uint32_t* begin, uint32_t* end, uint32_t* middle);
bool_t series_p_is_point_in(widget_t* widget, xy_t x, xy_t y, bool_t is_local);
int32_t series_p_relative_index_of_point_in(widget_t* widget, xy_t x, xy_t y, bool_t is_local);
int32_t series_p_index_of_point_in(widget_t* widget, xy_t x, xy_t y, bool_t is_local);
ret_t series_p_to_local(widget_t* widget, uint32_t index, point_t* p);
uint32_t series_p_get_offset_max(widget_t* widget);
ret_t series_p_set_offset(widget_t* widget, uint32_t offset);
ret_t series_p_set_new_period(widget_t* widget, uint32_t period);
ret_t series_p_get_prop(widget_t* widget, const char* name, value_t* v);
ret_t series_p_set_prop(widget_t* widget, const char* name, const value_t* v);
widget_t* series_p_lookup_series_axis(widget_t* widget, const char* name);
widget_t* series_p_lookup_value_axis(widget_t* widget, const char* name);
ret_t series_p_get_origin_point(widget_t* widget, widget_t* saxis, widget_t* vaxis, bool_t inverse,
                                point_t* p);
float_t series_p_get_series_interval(widget_t* widget);
bool_t series_p_is_vertical(widget_t* widget);

ret_t series_p_start_animator_when_inited(widget_t* widget);
ret_t series_p_on_paint_self_push(widget_t* widget, canvas_t* c);
ret_t series_p_on_paint_self_cover(widget_t* widget, canvas_t* c);

#define SERIES_ANIMATION_EASING EASING_SIN_INOUT

END_C_DECLS

#endif /*TK_SERIES_PRIVATE_H*/
