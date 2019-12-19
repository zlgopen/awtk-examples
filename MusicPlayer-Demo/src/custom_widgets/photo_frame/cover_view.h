/**
 * File:   cover_view.h
 * Author: AWTK Develop Team
 * Brief:  cover view
 *
 * Copyright (c) 2018 - 2019  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2018-04-05 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef TK_COVER_VIEW_H
#define TK_COVER_VIEW_H

#include "base/widget.h"
#include "base/widget_vtable.h"

BEGIN_C_DECLS

typedef struct _cover_view_t {
  widget_t widget;
} cover_view_t;

/**
 * @method cover_view_create
 * 创建cover_view对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* cover_view_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method cover_view_cast
 * 转换为cover_view对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget cover_view对象。
 *
 * @return {widget_t*} cover_view对象。
 */
widget_t* cover_view_cast(widget_t* widget);

#define WIDGET_TYPE_COVER_VIEW "cover_view"

#define COVER_VIEW(widget) ((cover_view_t*)(widget))

END_C_DECLS

#endif /*TK_COVER_VIEW_H*/
