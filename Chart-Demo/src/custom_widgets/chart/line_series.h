/**
 * File:   line series.h
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

#ifndef TK_LINE_SERIES_H
#define TK_LINE_SERIES_H

#include "series.h"

BEGIN_C_DECLS

/**
 * @class line_series_t
 * @parent widget_t
 * @annotation ["scriptable"]
 * 线形序列。
 */
typedef struct _line_series_t {
  series_t base;
  /**
   * 线。
   */
  series_line_params_t line;
  /**
   * 面。
   */
  series_line_area_params_t area;
  /**
   * 点。
   */
  series_symbol_params_t symbol;
  /**
   * X轴索引。
   */
  uint32_t x_axis_index;
  /**
   * y轴索引。
   */
  uint32_t y_axis_index;
  /**
   * 裁剪显示的采样点。
   */
  float_t clip_sample;
  /**
   * 数据fifo。
   */
  float_t* fifo;
  uint32_t fifo_capacity;
  uint32_t fifo_size;
} line_series_t;

/**
 * @method line_series_create
 * 创建line_series对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* line_series_create(widget_t* widget, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method line_series_set_data
 * 设置点的值(从第0点开始)
 * @annotation ["scriptable"]
 * @param {widget_t*} series line_series对象
 * @param {float_t*} data 值缓存
 * @param {uint32_t} nr 值数量
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t line_series_set_data(widget_t* series, const float_t* data, uint32_t nr);

/**
 * @method line_series_append
 * 向后追加点的值(超出容量则移除旧值)
 * @annotation ["scriptable"]
 * @param {widget_t*} series line_series对象
 * @param {float_t*} data 值缓存
 * @param {uint32_t} nr 值数量
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t line_series_append(widget_t* series, const float_t* data, uint32_t nr);

/**
 * @method line_series_clear
 * 清空点的值
 * @annotation ["scriptable"]
 * @param {widget_t*} series bar_series对象
 *
 * @return {void}
 */
void line_series_clear(widget_t* series);

/**
 * @method line_series_cast
 * 转换为line_series对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget line_series对象。
 *
 * @return {widget_t*} line_series对象。
 */
widget_t* line_series_cast(widget_t* series);

#define WIDGET_TYPE_LINE_SERIES "line_series"
#define LINE_SERIES(widget) ((line_series_t*)(widget))

END_C_DECLS

#endif /*TK_LINE_SERIES_H*/
