/**
 * File:   axis.h
 * Author: AWTK Develop Team
 * Brief:  axis
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

#ifndef TK_AXIS_H
#define TK_AXIS_H

#include "awtk.h"
#include "axis_render.h"

BEGIN_C_DECLS

/**
 * @enum axis_type_t
 * 坐标轴类型。
 */
typedef enum _axis_type_t {
  /**
   * @const AXIS_TYPE_CATEGORY
   * 种类坐标轴, 用于表示坐标点的类别，坐标点默认在间隔内居中。
   */
  AXIS_TYPE_CATEGORY = 0,
  /**
   * @const AXIS_TYPE_VALUE
   * 值坐标轴, 用于表示坐标点的值, 坐标点默认在间隔的边缘。
   */
  AXIS_TYPE_VALUE
} axis_type_t;

/**
 * @enum axis_orientation_t
 * 坐标轴方向。
 */
typedef enum _axis_orientation_t {
  /**
   * @const AXIS_ORIENTATION_X
   * 未定义。
   */
  AXIS_ORIENTATION_UNDEF = -1,
  /**
   * @const AXIS_ORIENTATION_X
   * x方向。
   */
  AXIS_ORIENTATION_X = 0,
  /**
   * @const AXIS_ORIENTATION_Y
   * y方向。
   */
  AXIS_ORIENTATION_Y
} axis_orientation_t;

/**
 * @enum axis_at_type_t
 * 坐标轴位置。
 */
typedef enum _axis_at_type_t {
  /**
   * @const AXIS_AT_AUTO
   * 自动选择位置。
   */
  AXIS_AT_AUTO = 0,
  /**
   * @const AXIS_AT_LEFT
   * 水平左边。
   */
  AXIS_AT_LEFT = 1,
  /**
   * @const AXIS_AT_RIGHT
   * 水平右边。
   */
  AXIS_AT_RIGHT = 2,
  /**
   * @const AXIS_AT_TOP
   * 垂直顶部。
   */
  AXIS_AT_TOP = 3,
  /**
   * @const AXIS_AT_BOTTOM
   * 垂直低部。
   */
  AXIS_AT_BOTTOM = 4
} axis_at_type_t;

/**
 * @enum axis_font_t
 * 字体。
 */
typedef struct _axis_font_t {
  char* name;
  color_t color;
  uint16_t size;
  uint16_t bold : 1;
  uint16_t italic : 1;
  uint16_t underline : 1;
} axis_font_t;

/**
 * @enum axis_line_style_t
 * 坐标轴的线条样式。
 */
typedef struct _axis_line_style_t {
  color_t color;
} axis_line_style_t;

/**
 * @enum axis_label_params_t
 * 轴上标尺的刻度值。
 */
typedef struct _axis_label_params_t {
  axis_font_t font;
  char* formater;
  darray_t* data;
  float_t val_min;
  float_t val_max;
  uint8_t show : 1;
  uint8_t inside : 1;
  uint8_t from_series : 1;
} axis_label_params_t;

/**
 * @enum axis_title_params_t
 * 轴上的标题。
 */
typedef struct _axis_title_params_t {
  axis_font_t font;
  wchar_t* text;
  uint8_t show : 1;
} axis_title_params_t;

/**
 * @enum axis_tick_params_t
 * 轴上的刻度线。
 */
typedef struct _axis_tick_params_t {
  axis_line_style_t style;
  uint8_t show : 1;
  uint8_t align_with_label : 1;
} axis_tick_params_t;

/**
 * @enum axis_line_params_t
 * 轴线。
 */
typedef struct _axis_line_params_t {
  axis_line_style_t style;
  uint8_t show : 1;
  uint8_t on_zero : 5;
} axis_line_params_t;

/**
 * @enum axis_split_line_params_t
 * 坐标系上的分割线。
 */
typedef struct _axis_split_line_params_t {
  axis_line_style_t style;
  uint8_t show : 1;
} axis_split_line_params_t;

/**
 * @class axis_t
 * @parent widget_t
 * @annotation ["scriptable"]
 * 坐标轴。
 */
typedef struct _axis_t {
  widget_t widget;
  /**
   * @property {axis_orientation_t} orientation
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 方向。
   */
  axis_orientation_t orientation;
  /**
   * @property {axis_at_type_t} at
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 位置。
   */
  axis_at_type_t at;
  /**
   * @property {axis_type_t} type
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 类型。
   */
  axis_type_t type;
  /**
   * @property {wh_t} offset
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 偏移位置。
   */
  wh_t offset;
  /**
   * 分隔线
   */
  axis_split_line_params_t split_line;
  /**
   * 刻度线
   */
  axis_tick_params_t tick;
  /**
   * 轴线
   */
  axis_line_params_t line;
  /**
   * 刻度值
   */
  axis_label_params_t label;
  /**
   * 标题
   */
  axis_title_params_t title;
  /**
   * 采样点是否显示在采样间隔的边缘
   */
  uint8_t boundary_gap : 1;
  /**
   * 是否反向
   */
  uint8_t inverse : 1;

  /* private */
  axis_render_t* render;
} axis_t;

/**
 * @method axis_create
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
widget_t* axis_create(widget_t* widget, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method axis_cast
 * 转换为axis对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget axis对象。
 *
 * @return {widget_t*} axis对象。
 */
widget_t* axis_cast(widget_t* widget);

/**
 * @method axis_measure_label
 * 测量刻度值文本的最大宽度。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget axis对象。
 * @param {canvas_t*} c canvas对象。
 *
 * @return {float_t} 返回刻度值文本的最大宽度。
 */
float_t axis_measure_label(widget_t* widget, canvas_t* c);

#define AXIS_PROP_ORIENTATION "orientation"
#define AXIS_PROP_TYPE "axis_type"
#define AXIS_PROP_AT "at"
#define AXIS_PROP_OFFSET "offset"
#define AXIS_PROP_DATA "data"
#define AXIS_PROP_SPLIT_LINE "split_line"
#define AXIS_PROP_SPLIT_LINE_COLOR "split_line.color"
#define AXIS_PROP_SPLIT_LINE_SHOW "split_line.show"
#define AXIS_PROP_LINE "line"
#define AXIS_PROP_LINE_COLOR "line.color"
#define AXIS_PROP_LINE_SHOW "line.show"
#define AXIS_PROP_TICK "tick"
#define AXIS_PROP_TICK_COLOR "tick.color"
#define AXIS_PROP_TICK_SHOW "tick.show"
#define AXIS_PROP_LABEL "label"
#define AXIS_PROP_LABEL_FONT_SIZE "label.font_size"
#define AXIS_PROP_LABEL_COLOR "label.color"
#define AXIS_PROP_LABEL_SHOW "label.show"
#define AXIS_PROP_TITLE "title"
#define AXIS_PROP_TITLE_FONT_SZIE "title.font_size"
#define AXIS_PROP_TITLE_COLOR "title.color"
#define AXIS_PROP_TITLE_SHOW "title.show"

#define WIDGET_TYPE_AXIS "axis"
#define AXIS(widget) ((axis_t*)(widget))

END_C_DECLS

#endif /*TK_AXIS_H*/
