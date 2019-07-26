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

#include "base/widget.h"
#include "axis_types.h"

BEGIN_C_DECLS

/**
 * @class axis_t
 * @parent widget_t
 * @annotation ["scriptable","widget"]
 * 坐标轴控件。
 */
typedef struct _axis_t {
  widget_t widget;
  /**
   * @property {axis_type_t} type
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 类型。
   */
  axis_type_t type;
  /**
   * @property {axis_at_type_t} at
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 位置。
   */
  axis_at_type_t at;
  /**
   * @property {float_t} min
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 最小刻度。
   */
  float_t min;
  /**
   * @property {float_t} max
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 最大刻度。
   */
  float_t max;
  /**
   * @property {darray_t*} data
   * @annotation ["readable"]
   * 刻度数据。
   */
  darray_t* data;
  /**
   * @property {axis_data_from_series_t} data_from_series
   * @annotation ["readable"]
   * 刻度数据的生成器。
   */
  axis_data_from_series_t data_from_series;
  /**
   * @property {void*} data_from_series_ctx
   * @annotation ["readable"]
   * 刻度数据生成时的上下文。
   */
  void* data_from_series_ctx;
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
   * 刻度显示值
   */
  axis_label_params_t label;
  /**
   * 标题
   */
  axis_title_params_t title;
  /**
   * @property {uint32_t} offset
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 偏移位置。
   */
  float_t offset;
  /**
   * @property {bool_t} offset_percent
   * @annotation ["readable"]
   * 偏移位置是否为百分比值。
   */
  uint8_t offset_percent : 1;
  /**
   * @property {bool_t} inverse
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 是否反向
   */
  uint8_t inverse : 1;
  /**
   * @property {bool_t} data_fixed
   * @annotation ["readable"]
   * 刻度数据是否固定。
   */
  uint8_t data_fixed : 1;
  /**
   * @property {bool_t} need_update_data
   * @annotation ["readable"]
   * 刻度数据是否需要更新。
   */
  uint8_t need_update_data : 1;
  /**
   * @property {bool_t} painted_before
   * @annotation ["readable"]
   * 是否需要在绘制前做额外处理。
   */
  uint8_t painted_before : 1;
  /**
   * @property {bool_t} inited
   * @annotation ["readable"]
   * 是否已初始化。
   */
  uint8_t inited : 1;

  /**
   * @property {series_vtable_t} vt
   * @annotation ["readable"]
   * 虚函数表。
   */
  const axis_vtable_t* vt;

  /* private */
  rect_t draw_rect;
} axis_t;

/**
 * @method axis_create
 * 创建对象。
 *
 * > 仅供子类调用。
 *
 * @param {widget_t*} parent widget的父控件。
 * @param {const widget_vtable_t*} vt 虚函数表。
 * @param {xy_t}   x x坐标。
 * @param {xy_t}   y y坐标。
 * @param {wh_t}   w 宽度。
 * @param {wh_t}   h 高度。
 *
 * @return {widget_t*} widget对象本身。
 */
widget_t* axis_create(widget_t* parent, const widget_vtable_t* vt, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method axis_on_destroy
 * 销毁对象时的处理。
 *
 * > 仅供子类调用。
 *
 * @param {widget_t*} widget widget对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t axis_on_destroy(widget_t* widget);

/**
 * @method axis_on_paint_begin
 * 绘图开始时的处理。
 * @annotation ["private"]
 * @param {widget_t*} widget widget对象。
 * @param {canvas_t*} c 画布对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t axis_on_paint_begin(widget_t* widget, canvas_t* c);

/**
 * @method axis_on_paint_before
 * 绘图前的处理。
 * @annotation ["private"]
 * @param {widget_t*} widget widget对象。
 * @param {canvas_t*} c 画布对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t axis_on_paint_before(widget_t* widget, canvas_t* c);

/**
 * @method axis_on_self_layout
 * 调整坐标轴自身的布局。
 * @annotation ["private"]
 * @param {widget_t*} widget widget对象。
 * @param {rect_t*} r series的显示区域。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t axis_on_self_layout(widget_t* widget, rect_t* r);

/**
 * @method axis_set_range
 * 设置坐标轴的刻度范围。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {float_t} min 最小值。
 * @param {float_t} max 最大值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t axis_set_range(widget_t* widget, float_t min, float_t max);

/**
 * @method axis_get_range
 * 获取坐标轴的刻度范围。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 *
 * @return {float_t} 刻度范围。
 */
float_t axis_get_range(widget_t* widget);

/**
 * @method axis_set_data
 * 设置坐标轴的刻度显示值。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} data 显示值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t axis_set_data(widget_t* widget, const char* data);

/**
 * @method axis_set_data_from_series
 * 设置坐标轴的刻度显示值的生成器。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {axis_data_from_series_t} from_series 生成回调。
 * @param {void*} ctx 上下文。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t axis_set_data_from_series(widget_t* widget, axis_data_from_series_t from_series, void* ctx);

/**
 * @method axis_set_need_update_data
 * 设置需要更新坐标轴的刻度显示值。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t axis_set_need_update_data(widget_t* widget);

/**
 * @method axis_update_data_from_series
 * 更新坐标轴的刻度显示值。
 * @annotation ["private"]
 * @param {widget_t*} widget widget对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t axis_update_data_from_series(widget_t* widget);

/**
 * @method axis_measure_series_interval
 * 测量坐标轴上的序列点之间的间隔。
 * @annotation ["private"]
 * @param {widget_t*} widget widget对象。
 *
 * @return {float_t} 返回序列点之间的间隔。
 */
float_t axis_measure_series_interval(widget_t* widget);

/**
 * @method axis_measure_series_interval
 * 测量坐标轴上的序列点的坐标。
 * @annotation ["private"]
 * @param {widget_t*} widget widget对象。
 * @param {void*} params 测量时需要的参数。
 * @param {fifo_t*} src 原始序列。
 * @param {fifo_t*} dst 坐标序列。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t axis_measure_series(widget_t* widget, void* sample_params, fifo_t* src, fifo_t* dst);

/**
 * @method axis_subpart_use_style
 * 启用指定子部件的主题。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} subpart 子部件。
 * @param {const char*} style style的名称。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t axis_subpart_use_style(widget_t* widget, const char* subpart, const char* style);

/**
 * @method axis_subpart_set_style
 * 设置指定子部件的style。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} subpart 子部件。
 * @param {const char*} state_and_name 状态和名字，用英文的冒号分隔。
 * @param {const value_t*} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t axis_subpart_set_style(widget_t* widget, const char* subpart, const char* state_and_name,
                             const value_t* value);

/**
 * @method axis_subpart_set_style_int
 * 设置指定子部件的整数类型style。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} subpart 子部件。
 * @param {const char*} state_and_name 状态和名字，用英文的冒号分隔。
 * @param {int32_t} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t axis_subpart_set_style_int(widget_t* widget, const char* subpart, const char* state_and_name,
                                 int32_t value);

/**
 * @method axis_subpart_set_style_color
 * 设置指定子部件的颜色类型style。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} subpart 子部件。
 * @param {const char*} state_and_name 状态和名字，用英文的冒号分隔。
 * @param {uint32_t} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t axis_subpart_set_style_color(widget_t* widget, const char* subpart,
                                   const char* state_and_name, uint32_t value);

/**
 * @method axis_subpart_set_style_str
 * 设置指定子部件的字符串类型style。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} subpart 子部件。
 * @param {const char*} state_and_name 状态和名字，用英文的冒号分隔。
 * @param {const char*} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t axis_subpart_set_style_str(widget_t* widget, const char* subpart, const char* state_and_name,
                                 const char* value);

/**
 * @method axis_cast
 * 转换为axis_base对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget axis_base对象。
 *
 * @return {widget_t*} axis_base对象。
 */
widget_t* axis_cast(widget_t* widget);

#define AXIS(widget) ((axis_t*)(axis_cast(WIDGET(widget))))

/**
 * @method widget_is_axis
 * 判断当前控件是否为axis控件。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 *
 * @return {bool_t} 返回当前控件是否为axis控件。
 */
bool_t widget_is_axis(widget_t* widget);

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(axis);

END_C_DECLS

#endif /*TK_AXIS_H*/
