/**
 * File:   tooltip.h
 * Author: AWTK Develop Team
 * Brief:  tooltip
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

#ifndef TK_TOOLTIP_H
#define TK_TOOLTIP_H

#include "tooltip_types.h"

BEGIN_C_DECLS

/**
 * @class tooltip_t
 * @parent widget_t
 * @annotation ["scriptable","design","widget"]
 * 提示信息控件。
 */
typedef struct _tooltip_t {
  widget_t widget;
  /**
   * 线参数。
   */
  tooltip_line_params_t line;
  /**
   * 点参数。
   */
  tooltip_symbol_params_t symbol;
  /**
   * 提示文本参数。
   */
  tooltip_tip_params_t tip;

  /* private */
  point_t down;
  bool_t vertival;
  widget_animator_t* wa_move;
  uint32_t inited : 1;
  uint32_t moved : 1;
} tooltip_t;

/**
 * @method tooltip_create
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
widget_t* tooltip_create(widget_t* parent, const widget_vtable_t* vt, xy_t x, xy_t y, wh_t w,
                         wh_t h);

/**
 * @method tooltip_create_default
 * 创建tooltip对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* tooltip_create_default(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method tooltip_move
 * 移动tooltip。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {xy_t}   x x坐标
 * @param {xy_t}   y y坐标
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t tooltip_move(widget_t* widget, xy_t x, xy_t y);

/**
 * @method tooltip_subpart_use_style
 * 启用指定子部件的主题。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} subpart 子部件。
 * @param {const char*} style style的名称。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t tooltip_subpart_use_style(widget_t* widget, const char* subpart, const char* style);

/**
 * @method tooltip_subpart_set_style
 * 设置指定子部件的style。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} subpart 子部件。
 * @param {const char*} state_and_name 状态和名字，用英文的冒号分隔。
 * @param {const value_t*} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t tooltip_subpart_set_style(widget_t* widget, const char* subpart, const char* state_and_name,
                                const value_t* value);

/**
 * @method tooltip_subpart_set_style_int
 * 设置指定子部件的整数类型style。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} subpart 子部件。
 * @param {const char*} state_and_name 状态和名字，用英文的冒号分隔。
 * @param {int32_t} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t tooltip_subpart_set_style_int(widget_t* widget, const char* subpart,
                                    const char* state_and_name, int32_t value);

/**
 * @method tooltip_subpart_set_style_color
 * 设置指定子部件的颜色类型style。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} subpart 子部件。
 * @param {const char*} state_and_name 状态和名字，用英文的冒号分隔。
 * @param {uint32_t} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t tooltip_subpart_set_style_color(widget_t* widget, const char* subpart,
                                      const char* state_and_name, uint32_t value);

/**
 * @method tooltip_subpart_set_style_str
 * 设置指定子部件的字符串类型style。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {const char*} subpart 子部件。
 * @param {const char*} state_and_name 状态和名字，用英文的冒号分隔。
 * @param {const char*} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t tooltip_subpart_set_style_str(widget_t* widget, const char* subpart,
                                    const char* state_and_name, const char* value);

/**
 * @method tooltip_cast
 * 转换为tooltip_base对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget tooltip_base对象。
 *
 * @return {widget_t*} tooltip_base对象。
 */
widget_t* tooltip_cast(widget_t* widget);

#define TOOLTIP(widget) ((tooltip_t*)(tooltip_cast(WIDGET(widget))))

/**
 * @method widget_is_tooltip
 * 判断当前控件是否为tooltip控件。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 *
 * @return {bool_t} 返回当前控件是否为tooltip控件。
 */
bool_t widget_is_tooltip(widget_t* widget);

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(tooltip);

END_C_DECLS

#endif /*TK_TOOLTIP_H*/
