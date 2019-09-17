/**
 * File:   tooltip_types.h
 * Author: AWTK Develop Team
 * Brief:  tooltip types
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

#ifndef TK_TOOLTIP_TYPES_H
#define TK_TOOLTIP_TYPES_H

#include "base/widget.h"
#include "chart_utils.h"

BEGIN_C_DECLS

/**
 * @enum tooltip_line_params_t
 * 指示序列点位置的线。
 */
typedef struct _tooltip_line_params_t {
  /**
   * style的名称。
   */
  char* style;
  /**
   * Style对象。
   */
  style_t* astyle;
  /**
   * 是否显示。
   */
  uint8_t show : 1;
} tooltip_line_params_t;

/**
 * @enum tooltip_symbol_params_t
 * 指示序列点位置的点。
 */
typedef struct _tooltip_symbol_params_t {
  /**
   * style的名称。
   */
  char* style;
  /**
   * Style对象。
   */
  style_t* astyle;
  /**
   * 是否显示。
   */
  uint8_t show : 1;
} tooltip_symbol_params_t;

/**
 * @enum tooltip_tip_params_t
 * 提示文本。
 */
typedef struct _tooltip_tip_params_t {
  /**
   * style的名称。
   */
  char* style;
  /**
   * Style对象。
   */
  style_t* astyle;
  /**
   * 是否显示。
   */
  uint8_t show : 1;
} tooltip_tip_params_t;

/**
 * @enum widget_subpart_t
 * @annotation ["scriptable", "string"]
 * @prefix TOOLTIP_SUBPART_
 * 控件的子部件。
 */

/**
 * @const TOOLTIP_SUBPART_LINE
 * 指示序列点位置的线
 */
#define TOOLTIP_SUBPART_LINE "tooltip_line"

/**
 * @const TOOLTIP_SUBPART_SYMBOL
 * 指示序列点位置的点
 */
#define TOOLTIP_SUBPART_SYMBOL "tooltip_symbol"

/**
 * @const TOOLTIP_SUBPART_TIP
 * 提示信息
 */
#define TOOLTIP_SUBPART_TIP "tooltip_tip"

/**
 * @enum widget_prop_t
 * @annotation ["scriptable", "string"]
 * @prefix TOOLTIP_PROP_
 * 控件的属性。
 */

/**
 * @const TOOLTIP_PROP_DOWN_X
 * pointer按下时的x坐标
 */
#define TOOLTIP_PROP_DOWN_X "down_x"

/**
 * @const TOOLTIP_PROP_DOWN_Y
 * pointer按下时的y坐标
 */
#define TOOLTIP_PROP_DOWN_Y "down_y"

/**
 * @const TOOLTIP_PROP_LINE
 * 指示序列点位置的线
 */
#define TOOLTIP_PROP_LINE "line"

/**
 * @const TOOLTIP_PROP_LINE_STYLE
 * 指示序列点位置的线的样式
 */
#define TOOLTIP_PROP_LINE_STYLE "line.style"

/**
 * @const TOOLTIP_PROP_LINE_SHOW
 * 指示序列点位置的线是否显示
 */
#define TOOLTIP_PROP_LINE_SHOW "line.show"

/**
 * @const TOOLTIP_PROP_SYMBOL
 * 指示序列点位置的点
 */
#define TOOLTIP_PROP_SYMBOL "symbol"

/**
 * @const TOOLTIP_PROP_SYMBOL_STYLE
 * 指示序列点位置的点的样式
 */
#define TOOLTIP_PROP_SYMBOL_STYLE "symbol.style"

/**
 * @const TOOLTIP_PROP_SYMBOL_SHOW
 * 指示序列点位置的点是否显示
 */
#define TOOLTIP_PROP_SYMBOL_SHOW "symbol.show"

/**
 * @const TOOLTIP_PROP_TIP
 * 提示文本
 */
#define TOOLTIP_PROP_TIP "tip"

/**
 * @const TOOLTIP_PROP_TIP_STYLE
 * 提示文本的样式
 */
#define TOOLTIP_PROP_TIP_STYLE "tip.style"

/**
 * @const TOOLTIP_PROP_TIP_SHOW
 * 提示文本是否显示
 */
#define TOOLTIP_PROP_TIP_SHOW "tip.show"

/**
 * @enum widget_type_t
 * @annotation ["scriptable", "string"]
 * @prefix WIDGET_TYPE_
 * 控件的类型。
 */

/**
 * @const WIDGET_TYPE_TOOLTIP
 * 提示信息控件。
 */
#define WIDGET_TYPE_TOOLTIP "tooltip"

END_C_DECLS

#endif /*TK_TOOLTIP_TYPES_H*/
