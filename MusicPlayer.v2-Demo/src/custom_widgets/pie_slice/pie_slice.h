/**
 * File:   pie_slice.h
 * Author: AWTK Develop Team
 * Brief:  pie_slice
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
 * 2018-12-4 TangJunJie created
 *
 */

#ifndef TK_pie_slice_H
#define TK_pie_slice_H

#include "awtk.h"

BEGIN_C_DECLS

/**
 * @class pie_slice_t
 * @parent widget_t
 * @annotation ["scriptable"]
 * 进度圆环控件。
 */
typedef struct _pie_slice_t {
  widget_t widget;
  /**
   * @property {float_t} value
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 值(缺省为0)。
   */
  float_t value;
  /**
   * @property {uint32_t} max
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 最大值(缺省为100)。
   */
  uint32_t max;
  /**
   * @property {int32_t} start_angle
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 起始角度(单位为度，缺省-90)。
   */
  int32_t start_angle;
  /**
   * @property {uint32_t} inner_radius
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 环线的厚度(缺省为8)。
   */
  uint32_t inner_radius;
  /**
   * @property {char*} unit
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 单元(缺省无)。
   */
  char* unit;
  /**
   * @property {bool_t} counter_clock_wise
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 是否为逆时针方向(缺省为FALSE)。
   */
  bool_t counter_clock_wise;
  /**
   * @property {bool_t} show_text
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 是否显示文本(缺省为TRUE)。
   */
  bool_t show_text;
  /**
   * @property {bool_t} is_exploded
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 是否扩展扇形(缺省为FALSE)。
   */
  bool_t is_exploded;
  /**
   * @property {uint32_t} explode_distancefactor
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 扩展距离。
   */
  uint32_t explode_distancefactor;
  /**
   * @property {uint32_t} x_to
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 扩展距离后x坐标。
   */
  int32_t x_to;
  /**
   * @property {uint32_t} y_to
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 扩展距离后y坐标。
   */
  int32_t y_to;
  /**
   * @property {bool_t} pressed
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 是否按下鼠标。
   */
  bool_t pressed;
  /**
   * @property {bool_t} is_semicircle
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 是否画拱形。
   */
  bool_t is_semicircle;
} pie_slice_t;

/**
 * @event {event_t} EVT_VALUE_WILL_CHANGE
 * 值即将改变事件。
 */

/**
 * @event {event_t} EVT_VALUE_CHANGED
 * 值改变事件。
 */

/**
 * @method pie_slice_create
 * @annotation ["constructor", "scriptable"]
 * 创建pie_slice对象
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* pie_slice_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method pie_slice_cast
 * 转换为pie_slice对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget pie_slice对象。
 *
 * @return {widget_t*} pie_slice对象。
 */
widget_t* pie_slice_cast(widget_t* widget);

/**
 * @method pie_slice_set_value
 * 设置值。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {float_t}  value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t pie_slice_set_value(widget_t* widget, float_t value);

/**
 * @method pie_slice_set_max
 * 设置最大值。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {uint32_t}  max 最大值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t pie_slice_set_max(widget_t* widget, uint32_t max);

/**
 * @method pie_slice_set_inner_radius
 * 设置环线的厚度。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {uint32_t}  inner_radius 环线的厚度。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t pie_slice_set_inner_radius(widget_t* widget, uint32_t inner_radius);

/**
 * @method pie_slice_set_start_angle
 * 设置起始角度。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {int32_t}  start_angle 起始角度。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t pie_slice_set_start_angle(widget_t* widget, int32_t start_angle);

/**
 * @method pie_slice_set_unit
 * 设置单位。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {const char*}  unit 单位。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t pie_slice_set_unit(widget_t* widget, const char* unit);

/**
 * @method pie_slice_set_show_text
 * 设置是否显示文本。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {bool_t}  show_text 是否显示文本。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t pie_slice_set_show_text(widget_t* widget, bool_t show_text);
/**
 * @method pie_slice_set_is_exploded
 * 设置是否显示扩展。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {bool_t}  is_exploded 是否扩展。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t pie_slice_set_is_exploded(widget_t* widget, bool_t is_exploded);

/**
 * @method pie_slice_set_counter_clock_wise
 * 设置是否为逆时针方向。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {bool_t}  counter_clock_wise 是否为逆时针方向。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t pie_slice_set_counter_clock_wise(widget_t* widget, bool_t counter_clock_wise);

/**
 * @method pie_slice_set_semicircle
 * 设置是否为拱形。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {bool_t}  is_semicircle 是否为逆时针方向。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t pie_slice_set_semicircle(widget_t* widget, bool_t is_semicircle);

/**
 * @method pie_slice_set_exploded
 * 扩展扇形。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t pie_slice_set_exploded(widget_t* widget);

/**
 * @method pie_slice_set_exploded_4_others
 * 点击该扇形扩展，其他扇形回归到原点。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t pie_slice_set_exploded_4_others(widget_t* widget);

/**
 * @method pie_slice_set_exploded_4_all
 * 扩展或者还原全部扇形
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t pie_slice_set_exploded_4_all(widget_t* widget);

#define PIE_SLICE_PROP_UNIT "unit"
#define PIE_SLICE_PROP_inner_radius "inner_radius"
#define PIE_SLICE_PROP_START_ANGLE "start_angle"
#define PIE_SLICE_PROP_COUNTER_CLOCK_WISE "counter_clock_wise"
#define PIE_SLICE_PROP_IS_EXPLODED "is_exploded"

#define WIDGET_TYPE_PIE_SLICE "pie_slice"
#define PIE_SLICE(widget) ((pie_slice_t*)(widget))

END_C_DECLS

#endif /*TK_pie_slice_H*/
