﻿/**
 * File:   slide_menu_v.h
 * Author: AWTK Develop Team
 * Brief:  slide_menu_v
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
 * 2018-12-08 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef TK_SLIDE_MENU_V_H
#define TK_SLIDE_MENU_V_H

#include "base/widget.h"
#include "base/velocity.h"
#include "base/widget_animator.h"

BEGIN_C_DECLS

/**
 * @class slide_menu_v_t
 * @parent widget_t
 * @annotation ["scriptable","design","widget"]
 * 左右滑动菜单控件。
 *
 * 一般用一组按钮作为子控件，通过左右滑动改变当前的项。除了当菜单使用外，也可以用来切换页面。
 *
 * slide\_menu\_t是[widget\_t](widget_t.md)的子类控件，widget\_t的函数均适用于slide\_menu\_t控件。
 *
 * 在xml中使用"slide\_menu"标签创建左右滑动菜单控件。如：
 *
 * ```xml
 * <slide_menu_v style="mask" align_v="top">
 *  <button style="slide_button" text="0"/>
 *   <button style="slide_button" text="1"/>
 *   <button style="slide_button" text="2"/>
 *   <button style="slide_button" text="3"/>
 *   <button style="slide_button" text="4"/>
 * </slide_menu_v>
 * ```
 *
 * > 更多用法请参考：[slide_menu_v.xml](
 * https://github.com/zlgopen/awtk/blob/master/demos/assets/raw/ui/slide_menu_v.xml)
 *
 * 在c代码中使用函数slide\_menu\_create创建左右滑动菜单控件。如：
 *
 * ```c
 * slide_menu_v = slide_menu_v_create(win, 10, 10, 300, 60);
 * b = button_create(slide_menu_v, 0, 0, 0, 0);
 * widget_set_text_utf8(b, "1");
 * b = button_create(slide_menu_v, 0, 0, 0, 0);
 * widget_set_text_utf8(b, "2");
 * b = button_create(slide_menu_v, 0, 0, 0, 0);
 * widget_set_text_utf8(b, "3");
 * b = button_create(slide_menu_v, 0, 0, 0, 0);
 * widget_set_text_utf8(b, "4");
 * ```
 *
 * 可按下面的方法关注当前项改变的事件：
 *
 * ```c
 * widget_on(slide_menu_v, EVT_VALUE_CHANGED, on_current_changed, slide_menu_v);
 * ```
 *
 * 可按下面的方法关注当前按钮被点击的事件：
 *
 * ```c
 * widget_on(b, EVT_CLICK, on_button_click, b);
 * ```
 *
 * > 完整示例请参考：[slide_menu_v demo](
 * https://github.com/zlgopen/awtk-c-demos/blob/master/demos/slide_menu_v.c)
 *
 * 可用通过style来设置控件的显示风格，如背景颜色和蒙版颜色等等。如：
 *
 * ```xml
 * <style name="mask">
 *   <normal     bg_color="#f0f0f0" mask_color="#f0f0f0"/>
 * </style>
 * ```
 *
 * > 更多用法请参考：[theme default](
 * https://github.com/zlgopen/awtk/blob/master/demos/assets/raw/styles/default.xml#L493)
 *
 */
typedef struct _slide_menu_v_t {
  widget_t widget;

  /**
   * @property {int32_t} value
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 值。代表当前选中项的索引。
   */
  int32_t value;

  /**
   * @property {align_h_t} align_h
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 水平对齐方式。
   */
  align_h_t align_h;

  /**
   * @property {int32_t} item_width
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * menu项的宽度。如果 item_height > 0，所有menu项使用固定宽度，否则使用列表项自身的宽度。
   */
  int32_t item_width;

  /**
   * @property {int32_t} item_height
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * menu项的高度。如果 item_height > 0，所有menu项使用固定高度，否则使用列表项自身的高度。
   */
  int32_t item_height;

  int32_t spacing;

  /**
   * @property {float_t} min_scale
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 最小缩放比例。
   */
  float_t min_scale;

  /**
   * @property {float_t} xoffset
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 相邻项目在x方向上的偏移
   */
  int32_t xoffset;

  /*private*/
  int32_t ydown;
  int32_t yoffset;
  bool_t dragged;
  bool_t pressed;
  velocity_t velocity;
  widget_animator_t* wa;
} slide_menu_v_t;

/**
 * @event {event_t} EVT_VALUE_WILL_CHANGE
 * 值(当前项)即将改变事件。
 */

/**
 * @event {event_t} EVT_VALUE_CHANGED
 * 值(当前项)改变事件。
 */

/**
 * @method slide_menu_v_create
 * 创建slide_menu对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* slide_menu_v_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method slide_menu_v_cast
 * 转换slide_menu对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget slide_menu对象。
 *
 * @return {widget_t*} slide_menu对象。
 */
widget_t* slide_menu_v_cast(widget_t* widget);

/**
 * @method slide_menu_v_set_value
 * 设置当前项。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget slide_menu对象。
 * @param {uint32_t} value 当前项的索引。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slide_menu_v_set_value(widget_t* widget, uint32_t value);

/**
 * @method slide_menu_v_set_align_h
 * 设置水平对齐方式。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget slide_menu对象。
 * @param {align_h_t} align_h 对齐方式。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slide_menu_v_set_align_h(widget_t* widget, align_h_t align_h);

/**
 * @method slide_menu_v_set_item_height
 * 设置menu项的高度。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {int32_t} default_item_height menu项的高度。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slide_menu_v_set_item_height(widget_t* widget, int32_t item_height);
ret_t slide_menu_v_set_item_width(widget_t* widget, int32_t item_width);
ret_t slide_menu_v_set_spacing(widget_t* widget, int32_t spacing);

/**
 * @method slide_menu_v_set_min_scale
 * 设置最小缩放比例。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget slide_menu对象。
 * @param {float_t} min_scale 最小缩放比例，范围[0.5-1]。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slide_menu_v_set_min_scale(widget_t* widget, float_t min_scale);

#define SLIDE_MENU_V_PROP_MIN_SCALE "min_scale"

#define WIDGET_TYPE_SLIDE_MENU_V "slide_menu_v"
#define SLIDE_MENU_V(widget) ((slide_menu_v_t*)(slide_menu_v_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(slide_menu_v);

/*public for test*/
int32_t slide_menu_v_fix_index(widget_t* widget, int32_t index);

END_C_DECLS

#endif /*TK_SLIDE_MENU_V_H*/
