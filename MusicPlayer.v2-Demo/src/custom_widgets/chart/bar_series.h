/**
 * File:   bar_series.h
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

#ifndef TK_BAR_SERIES_H
#define TK_BAR_SERIES_H

#include "series.h"

BEGIN_C_DECLS

/**
 * @class bar_series_t
 * @parent series_t
 * @annotation ["scriptable"]
 * 柱条序列。
 */
typedef struct _bar_series_t {
  series_t base;
  /**
   * @property {char*} series_axis
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 标示序列位置的轴。
   */
  char* series_axis;
  /**
   * @property {char*} value_axis
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 标示序列值的轴。
   */
  char* value_axis;
  /**
   * 柱条参数。
   */
  series_bar_params_t bar;
} bar_series_t;

/**
 * @method bar_series_create
 * 创建bar_series对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* bar_series_create(widget_t* widget, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method bar_series_minmax_create
 * 创建bar_series_minmax对象（同时显示最大最小值）
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* bar_series_minmax_create(widget_t* widget, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method bar_series_cast
 * 转换为bar_series对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget bar_series对象。
 *
 * @return {widget_t*} bar_series对象。
 */
widget_t* bar_series_cast(widget_t* series);

#define BAR_SERIES(widget) ((bar_series_t*)(bar_series_cast(WIDGET(widget))))

END_C_DECLS

#endif /*TK_BAR_SERIES_H*/
