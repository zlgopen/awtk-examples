/**
 * File:   axis_render.h
 * Author: AWTK Develop Team
 * Brief:  axis render
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

#ifndef TK_AXIS_RENDER_H
#define TK_AXIS_RENDER_H

#include "awtk.h"

BEGIN_C_DECLS

/**
 * @class axis_render_t
 * 坐标轴渲染参数。
 */
typedef struct _axis_render_t {
  /**
   * 标尺矩形域
   */
  rect_t ruler;
  /**
   * 刻度线位置
   */
  float_t* tick;
  /**
   * 分割线长度
   */
  wh_t split_line_len;
  /**
   * 0刻度索引
   */
  uint16_t index_of_zero;
  /**
   * 是否显示第1条分割线
   */
  uint16_t show_1st_split_line : 1;
  /**
   * 是否显示最后1条分割线
   */
  uint16_t show_last__split_line : 1;
} axis_render_t;

ret_t axis_render_layout(widget_t* widget, canvas_t* c, wh_t margin);
ret_t axis_render_on_paint(widget_t* widget, canvas_t* c);
axis_render_t* axis_render_create();
ret_t axis_render_destroy(axis_render_t* render);

END_C_DECLS

#endif /*TK_AXIS_RENDER_H*/
