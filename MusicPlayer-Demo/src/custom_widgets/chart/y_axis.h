/**
 * File:   y_axis.h
 * Author: AWTK Develop Team
 * Brief:  y axis
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

#ifndef TK_Y_AXIS_H
#define TK_Y_AXIS_H

#include "axis.h"

BEGIN_C_DECLS

/**
 * @class y_axis_t
 * @parent axis_t
 * @annotation ["scriptable","design","widget"]
 * y坐标轴控件。
 */
typedef struct _y_axis_t {
  axis_t base;
  /**
   * @property {bool_t} y_defined
   * @annotation ["readable"]
   * y坐标是否确定。
   */
  uint8_t y_defined : 1;
  /**
   * @property {bool_t} h_defined
   * @annotation ["readable"]
   * h高度是否确定。
   */
  uint8_t h_defined : 1;
} y_axis_t;

/**
 * @method y_axis_create
 * 创建axis对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* y_axis_create(widget_t* widget, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method y_axis_cast
 * 转换为axis对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget axis对象。
 *
 * @return {widget_t*} axis对象。
 */
widget_t* y_axis_cast(widget_t* widget);

#define Y_AXIS(widget) ((y_axis_t*)(y_axis_cast(WIDGET(widget))))

END_C_DECLS

#endif /*TK_Y_AXIS_H*/
