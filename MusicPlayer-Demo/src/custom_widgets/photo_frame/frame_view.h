/**
 * File:   window_record.c
 * Author: AWTK Develop Team
 * Brief:  Frame View
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
 * 2019-2-20  ZhongWei1 created
 *
 */

#ifndef TK_FRAME_VIEW_H
#define TK_FRAME_VIEW_H

#include "awtk.h"

BEGIN_C_DECLS

typedef struct _frame_view_t {
  widget_t widget;
  /**
   * @property {int32_t} value
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 值。代表当前选中项的索引。
   */
  int32_t value;

  /**
   * @property {align_v_t} align_v
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 垂直对齐方式。
   */
  align_v_t align_v;

  /**
   * @property {float_t} min_scale
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 最小缩放比例。
   */
  float_t min_scale;

  /*private*/

  int32_t xoffset;
  widget_animator_t* aim;
} frame_view_t;

/**
 * @event {event_t} EVT_VALUE_WILL_CHANGE
 * 值(当前项)即将改变事件。
 */

/**
 * @event {event_t} EVT_VALUE_CHANGED
 * 值(当前项)改变事件。
 */

/**
 * @method frame_view_create
 * 创建frame_view对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* frame_view_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method frame_view_cast
 * 转换frame_view对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget frame_view对象。
 *
 * @return {widget_t*} frame_view对象。
 */
widget_t* frame_view_cast(widget_t* widget);

/**
 * @method frame_view_set_value
 * 设置当前项。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget frame_view对象。
 * @param {uint32_t} value 当前项的索引。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t frame_view_set_value(widget_t* widget, uint32_t value);

/**
 * @method frame_view_set_align_v
 * 设置垂直对齐方式。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget frame_view对象。
 * @param {align_v_t} align_v 对齐方式。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t frame_view_set_align_v(widget_t* widget, align_v_t align_v);

/**
 * @method frame_view_set_min_scale
 * 移动到上一个或者下一个widget对象
 * @annotation ["scriptable"]
 * @param {widget_t*} widget frame_view对象。
 * @param {bool_t} operate TRUE表示下一个frame_photo对象，FALSE表示上一个frame_photo对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t frame_view_set_then(widget_t* widget, bool_t operate);

int32_t frame_view_fix_index(widget_t* widget, int32_t index);
#define FRAME_VIEW_PROP_MIN_SCALE "min_scale"
#define WIDGET_TYPE_FRAME_VIEW "frame_view"
#define FRAME_VIEW(widget) ((frame_view_t*)(widget))
END_C_DECLS

#endif /*TK_FRAME_VIEW_H*/
