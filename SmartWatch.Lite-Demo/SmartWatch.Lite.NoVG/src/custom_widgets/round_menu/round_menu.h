#ifndef TK_ROUND_MENU_H
#define TK_ROUND_MENU_H

#include "base/widget.h"
#include "base/velocity.h"
#include "base/widget_animator.h"

BEGIN_C_DECLS

#define ROUND_MENU_CURR_WIDGET_LIST_NUMBER 4

typedef struct _round_menu_t {
  widget_t widget;

  /**
   * @property {int32_t} value
   * @annotation
   * ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 值。代表当前选中项的索引。
   */
  int32_t value;

  int32_t item_h;
  int32_t interval_yoffset;

  /*private*/
  widget_t *paint_widgets[ROUND_MENU_CURR_WIDGET_LIST_NUMBER];

  int32_t ydown;
  int32_t yoffset;
  bool_t dragged;
  bool_t pressed;
  velocity_t velocity;
  widget_animator_t *wa;
} round_menu_t;

/**
 * @event {event_t} EVT_VALUE_WILL_CHANGE
 * 值(当前项)即将改变事件。
 */

/**
 * @event {event_t} EVT_VALUE_CHANGED
 * 值(当前项)改变事件。
 */

/**
 * @method round_menu_create
 * 创建round_menu对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t *round_menu_create(widget_t *parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method round_menu_cast
 * 转换round_menu对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget round_menu对象。
 *
 * @return {widget_t*} round_menu对象。
 */
widget_t *round_menu_cast(widget_t *widget);

/**
 * @method round_menu_set_value
 * 设置当前项。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget round_menu对象。
 * @param {uint32_t} value 当前项的索引。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t round_menu_set_value(widget_t *widget, uint32_t value);

/**
* @method round_menu_set_item_h
* 设置每一项的高度。
* @annotation ["scriptable"]
* @param {widget_t*} widget round_menu对象。
* @param {uint32_t} item_h 每一项的高度。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t round_menu_set_item_h(widget_t *widget, int32_t item_h);

/**
* @method round_menu_set_interval_yoffset
* 设置菜单的Y轴的偏移值。
* @annotation ["scriptable"]
* @param {widget_t*} widget round_menu对象。
* @param {uint32_t} interval_yoffset Y轴的偏移值。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t round_menu_set_interval_yoffset(widget_t *widget,
                                      int32_t interval_yoffset);

#define WIDGET_TYPE_ROUND_MENU "round_menu"
#define WIDGET_PROP_ROUND_MENU_ITEM_H "item_h"
#define WIDGET_PROP_ROUND_MENU_INTERVAL_YOFFSET "interval_yoffset"

#define ROUND_MENU(widget) ((round_menu_t *)(round_menu_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(round_menu);

/*public for test*/
int32_t round_menu_fix_index(widget_t *widget, int32_t index);

END_C_DECLS

#endif /*TK_ROUND_MENU_H*/
