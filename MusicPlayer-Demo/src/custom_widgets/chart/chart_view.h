/**
 * File:   chart_view.h
 * Author: AWTK Develop Team
 * Brief:  chart view
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

#ifndef TK_CHART_VIEW_H
#define TK_CHART_VIEW_H

#include "awtk.h"
#include "axis.h"
#include "series.h"

BEGIN_C_DECLS

typedef struct _bar_layout_t {
  /**
   * 柱条在间隔内的边距。
   */
  uint8_t margin;
  /**
   * 间隔内相邻柱条的间距。
   */
  uint8_t spacing;
} bar_layout_t;

/**
 * @class chart_view_t
 * @parent widget_t
 * @annotation ["scriptable"]
 * chart view
 */
typedef struct _chart_view_t {
  widget_t widget;
  bar_layout_t bar_layout;

  /* private */
  bool_t is_axes_dirty;
  darray_t* axes;
  darray_t* series;
} chart_view_t;

/**
 * @method chart_view_create
 * 创建chart_view_对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* chart_view_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method chart_view_count_axis
 * 获取指定方向的axis的个数。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {axis_orientation_t} orientation axis的方向
 *
 * @return {uint32_t} axis的个数。
 */
uint32_t chart_view_count_axis(widget_t* widget, axis_orientation_t orientation);

/**
 * @method chart_view_get_axis
 * 获取指定方向的指定索引的axis对象。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {axis_orientation_t} orientation axis的方向
 * @param {uint32_t} index 索引
 *
 * @return {widget_t*} axis对象。
 */
widget_t* chart_view_get_axis(widget_t* widget, axis_orientation_t orientation, uint32_t index);

/**
 * @method chart_view_count_series
 * 获取指定类型的series的个数。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {const char*} type series对象的类型。
 *
 * @return {uint32_t} series的个数。
 */
uint32_t chart_view_count_series(widget_t* widget, const char* type);

/**
 * @method chart_view_get_series
 * 获取指定类型的指定索引的series对象。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {const char*} type series对象的类型。
 * @param {uint32_t} index 索引
 *
 * @return {widget_t*} series对象。
 */
widget_t* chart_view_get_series(widget_t* widget, const char* type, uint32_t index);

/**
 * @method chart_view_index_of_series
 * 获取series对象在指定类型的series集合中的索引编号。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {const char*} type series对象的类型。
 * @param {widget_t*} series series对象
 *
 * @return {int32_t} 索引编号。
 */
int32_t chart_view_index_of_series(widget_t* widget, const char* type, widget_t* series);

/**
 * @method chart_view_get_bar_margin
 * 获取柱条在间隔内的边距。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 *
 * @return {uint32_t} 返回值。
 */
uint32_t chart_view_get_bar_margin(widget_t* widget);

/**
 * @method chart_view_set_bar_margin
 * 设置柱条在间隔内的边距。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {uint32_t} margin 边距。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t chart_view_set_bar_margin(widget_t* widget, uint32_t margin);

/**
 * @method chart_view_get_bar_spacing
 * 获取同一间隔内的相邻柱条的间距。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 *
 * @return {uint32_t} 返回值。
 */
uint32_t chart_view_get_bar_spacing(widget_t* widget);

/**
 * @method chart_view_set_bar_spacing
 * 设置同一间隔内的相邻柱条的间距。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {uint32_t} spacing 间距。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t chart_view_set_bar_spacing(widget_t* widget, uint32_t spacing);

/**
 * @method chart_view_cast
 * 转换chart_view对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget chart_view对象。
 *
 * @return {widget_t*} chart_view对象。
 */
widget_t* chart_view_cast(widget_t* widget);

#define CHART_VIEW_PROP_BAR_MARGIN "bar.margin"
#define CHART_VIEW_PROP_BAR_SPACING "bar.spacing"

#define WIDGET_TYPE_CHART_VIEW "chart_view"
#define CHART_VIEW(widget) ((chart_view_t*)(widget))

END_C_DECLS

#endif /*TK_CHART_VIEW_H*/
