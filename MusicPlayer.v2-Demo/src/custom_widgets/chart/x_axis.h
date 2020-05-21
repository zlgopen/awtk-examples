/**
 * File:   x_axis.h
 * Author: AWTK Develop Team
 * Brief:  x axis
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

#ifndef TK_X_AXIS_H
#define TK_X_AXIS_H

#include "axis.h"

BEGIN_C_DECLS

/**
 * @class x_axis_t
 * @parent axis_t
 * @annotation ["scriptable","design","widget"]
 * x坐标轴控件。
 */
typedef struct _x_axis_t {
  axis_t base;
  /**
   * @property {bool_t} x_defined
   * @annotation ["readable"]
   * x坐标是否确定。
   */
  uint8_t x_defined : 1;
  /**
   * @property {bool_t} w_defined
   * @annotation ["readable"]
   * w宽度是否确定。
   */
  uint8_t w_defined : 1;
} x_axis_t;

/**
 * @method x_axis_create
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
widget_t* x_axis_create(widget_t* widget, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method x_axis_cast
 * 转换为axis对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget axis对象。
 *
 * @return {widget_t*} axis对象。
 */
widget_t* x_axis_cast(widget_t* widget);

#define X_AXIS(widget) ((x_axis_t*)(x_axis_cast(WIDGET(widget))))

END_C_DECLS

#endif /*TK_X_AXIS_H*/
