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

#include "series_types.h"

BEGIN_C_DECLS

/**
 * @class series_t
 * @parent widget_t
 * @annotation ["scriptable","widget"]
 * 序列控件。
 */
typedef struct _series_t {
  widget_t widget;

  /**
   * @property {fifo_t*} fifo
   * @annotation ["readable"]
   * 数据fifo。
   */
  fifo_t* fifo;
  /**
   * @property {uint32_t} capacity
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * fifo的容量。
   */
  uint32_t capacity;
  /**
   * @property {uint32_t} unit_size
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * fifo元素的大小。
   */
  uint32_t unit_size;
  /**
   * @property {uint32_t} offset
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * fifo的偏移量。
   */
  uint32_t offset;
  /**
   * @property {series_dispaly_mode_t} display_mode
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 显示模式。
   */
  series_dispaly_mode_t display_mode;
  /**
   * @property {bool_t} animation
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 播放值更新动画。0表示禁止，非0表示动画持续事件。
   */
  uint32_t animation;
  /**
   * @property {series_tooltip_format_t} tooltip_format
   * @annotation ["readable"]
   * 提示信息格式化。
   */
  series_tooltip_format_t tooltip_format;
  /**
   * @property {void*} tooltip_format_ctx
   * @annotation ["readable"]
   * 提示信息格式化的上下文。
   */
  void* tooltip_format_ctx;
  /**
   * @property {series_vtable_t} vt
   * @annotation ["readable"]
   * 虚函数表。
   */
  const series_vtable_t* vt;
  /**
   * @property {bool_t} need_reset_fifo
   * @annotation ["readable"]
   * 是否需要重置数据fifo。
   */
  uint8_t need_reset_fifo : 1;
  /**
   * @property {bool_t} need_reset_fifo
   * @annotation ["readable"]
   * 是否已初始化。
   */
  uint8_t inited : 1;
  /**
   * @property {uint32_t} new_period
   * @annotation ["readable"]
   * 序列的新周期点数。
   */
  uint32_t new_period;

  /* private */
  float_t clip_range;
  uint32_t coverage;
} series_t;

/**
 * @method series_create
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
widget_t* series_create(widget_t* parent, const widget_vtable_t* vt, xy_t x, xy_t y, wh_t w,
                        wh_t h);

/**
 * @method series_count
 * 获取series的当前点数。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 *
 * @return {uint32_t} 当前点数。
 */
uint32_t series_count(widget_t* widget);

/**
 * @method series_set
 * 设置指定位置的序列点。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t} index 序列点在fifo中的位置。
 * @param {const void*} data 序列点数据。
 * @param {uint32_t} nr 序列点数量。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_set(widget_t* widget, uint32_t index, const void* data, uint32_t nr);

/**
 * @method series_rset
 * 设置指定位置（反向）的序列点。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t} index 序列点在fifo中的位置。
 * @param {const void*} data 序列点数据。
 * @param {uint32_t} nr 序列点数量。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_rset(widget_t* widget, uint32_t index, const void* data, uint32_t nr);

/**
 * @method series_push
 * 在尾巴追加多个序列点。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const void*} data 序列点数据。
 * @param {uint32_t} nr 序列点数量。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_push(widget_t* widget, const void* data, uint32_t nr);

/**
 * @method series_at
 * 返回特定位置的序列点数据。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t} index 序列点在fifo中的位置。
 *
 * @return {void*} 如果找到，返回特定位置的序列点数据，否则返回NULL。
 */
void* series_at(widget_t* widget, uint32_t index);

/**
 * @method series_get_current
 * 返回当前显示序列点的位置。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t*} begin 起始点fifo中的位置。
 * @param {uint32_t*} end 结束点fifo中的位置。
 * @param {uint32_t*} middle 中间点fifo中的位置。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_get_current(widget_t* widget, uint32_t* begin, uint32_t* end, uint32_t* middle);

/**
 * @method series_is_point_in
 * 判断一个点是否在series显示区域内。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {bool_t} is_local TRUE表示是相对与控件左上角的坐标，否则表示全局坐标。
 *
 * @return {bool_t} 返回是否在series显示区域内。
 */
bool_t series_is_point_in(widget_t* widget, xy_t x, xy_t y, bool_t is_local);

/**
 * @method series_index_of_point_in
 * 返回距离一个点最近的序列点的索引。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {bool_t} is_local TRUE表示是相对与控件左上角的坐标，否则表示全局坐标。
 *
 * @return {int32_t} 返回fifo中的位置。
 */
int32_t series_index_of_point_in(widget_t* widget, xy_t x, xy_t y, bool_t is_local);

/**
 * @method series_to_local
 * 返回指定序列点在控件内的本地坐标，即相对于控件左上角的坐标。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t} index 序列点在fifo中的位置。
 * @param {point_t*} p 坐标点。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_to_local(widget_t* widget, uint32_t index, point_t* p);

/**
 * @method series_set_tooltip_format
 * 设置提示信息格式化。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {series_tooltip_format_t} format 提示信息格式化回调。
 * @param {void*} ctx 格式化时的上下文。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_set_tooltip_format(widget_t* widget, series_tooltip_format_t format, void* ctx);

/**
 * @method series_get_tooltip
 * 获取指定序列点的提示信息。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t} index 序列点在fifo中的位置。
 * @param {wstr_t*} v 提示信息。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_get_tooltip(widget_t* widget, uint32_t index, wstr_t* v);

/**
 * @method series_set_capacity
 * 设置fifo容量。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t} capacity 容量。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_set_capacity(widget_t* widget, uint32_t capacity);

/**
 * @method series_set_unit_size
 * 设置单个序列点数据的大小。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t} unit_size 单个序列点数据的大小。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_set_unit_size(widget_t* widget, uint32_t unit_size);

/**
 * @method series_set_new_period
 * 设置序列的新周期点数。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {uint32_t} new_period 新周期点数。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_set_new_period(widget_t* widget, uint32_t new_period);

/**
 * @method series_set_title
 * 设置标题。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} title 标题。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_set_title(widget_t* widget, const char* title);

/**
 * @method series_get_title
 * 获取标题。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 *
 * @return {wchar_t*} 返回文本。
 */
const wchar_t* series_get_title(widget_t* widget);

/**
 * @method series_subpart_use_style
 * 启用指定子部件的主题。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} subpart 子部件。
 * @param {const char*} style style的名称。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_subpart_use_style(widget_t* widget, const char* subpart, const char* style);

/**
 * @method series_subpart_set_style
 * 设置指定子部件的style。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} subpart 子部件。
 * @param {const char*} state_and_name 状态和名字，用英文的冒号分隔。
 * @param {const value_t*} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_subpart_set_style(widget_t* widget, const char* subpart, const char* state_and_name,
                               const value_t* value);

/**
 * @method series_subpart_set_style_int
 * 设置指定子部件的整数类型style。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} subpart 子部件。
 * @param {const char*} state_and_name 状态和名字，用英文的冒号分隔。
 * @param {int32_t} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_subpart_set_style_int(widget_t* widget, const char* subpart,
                                   const char* state_and_name, int32_t value);

/**
 * @method series_subpart_set_style_color
 * 设置指定子部件的颜色类型style。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} subpart 子部件。
 * @param {const char*} state_and_name 状态和名字，用英文的冒号分隔。
 * @param {uint32_t} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_subpart_set_style_color(widget_t* widget, const char* subpart,
                                     const char* state_and_name, uint32_t value);

/**
 * @method series_subpart_set_style_str
 * 设置指定子部件的字符串类型style。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} subpart 子部件。
 * @param {const char*} state_and_name 状态和名字，用英文的冒号分隔。
 * @param {const char*} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t series_subpart_set_style_str(widget_t* widget, const char* subpart,
                                   const char* state_and_name, const char* value);

/**
 * @method series_cast
 * 转换为series_cast对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget series_cast对象。
 *
 * @return {widget_t*} series_cast对象。
 */
widget_t* series_cast(widget_t* widget);

#define SERIES(widget) ((series_t*)(series_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(series);

/**
 * @method widget_is_series
 * 判断当前控件是否为series控件。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 *
 * @return {bool_t} 返回当前控件是否为series控件。
 */
bool_t widget_is_series(widget_t* widget);

END_C_DECLS

#endif /*TK_SERIES_H*/
