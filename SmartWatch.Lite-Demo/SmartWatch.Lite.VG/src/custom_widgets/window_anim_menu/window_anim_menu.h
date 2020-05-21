#ifndef TK_WINDOW_ANIM_MENU_H
#define TK_WINDOW_ANIM_MENU_H

#include "base/widget.h"
#include "base/velocity.h"

BEGIN_C_DECLS

typedef ret_t (*event_load_widget_func_t)(widget_t *load_widget, void *ctx);

typedef ret_t (*event_dispose_widget_func_t)(widget_t *dispose_widget,
                                             void *ctx);

typedef enum _animating_move_type_t {
  MENU_ANIMATING_MOVE_TYPE_TOP,
  MENU_ANIMATING_MOVE_TYPE_BOTTOM,
  MENU_ANIMATING_MOVE_TYPE_LETF,
  MENU_ANIMATING_MOVE_TYPE_RIGTH,
} animating_move_type_t;

typedef struct _window_anim_menu_t {
  widget_t widget;

  bool_t is_move_back;

  /**
   * @property {char*} animating_time
   * @annotation
   *["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 动画播放时间
   *
   */
  uint32_t animating_time;

  animating_move_type_t animating_move_type;

  /*private*/
  int32_t xoffset;
  int32_t yoffset;
  bool_t animating;
  widget_t *curr_widget;
  widget_t *next_widget;
  void *on_load_widget_fun_ctx;
  void *on_dispose_widget_fun_ctx;
  event_load_widget_func_t on_load_widget_fun;
  event_dispose_widget_func_t on_dispose_widget_fun;
} window_anim_menu_t;

/**
 * @event {event_t} EVT_VALUE_WILL_CHANGE
 * 值(当前页)即将改变事件。
 */

/**
 * @event {event_t} EVT_VALUE_CHANGED
 * 值(当前页)改变事件。
 */

/**
 * @method window_anim_menu_create
 * 创建window_anim_menu对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t *window_anim_menu_create(widget_t *parent, xy_t x, xy_t y, wh_t w,
                                  wh_t h);

/**
 * @method window_anim_menu_cast
 * 转换为window_anim_menu对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget window_anim_menu对象。
 *
 * @return {widget_t*} window_anim_menu对象。
 */
widget_t *window_anim_menu_cast(widget_t *widget);

/**
* @method window_anim_menu_set_animating_time
* 设置动画播放时间
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget window_anim_menu对象。
* @param {uint32_t} animating_time 动画播放时间。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t window_anim_menu_set_animating_time(widget_t *widget,
                                          uint32_t animating_time);

/**
* @method window_anim_menu_set_curr_widget
* 设置当前显示的widget（是动态加载widget，而且不播放动画，自动释放后台的widget）
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget window_anim_menu对象。
* @param {const char*} widget_name 动态加载的widget的名字。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t window_anim_menu_set_curr_widget(widget_t *widget,
                                       const char *widget_name);

/**
* @method window_anim_menu_set_curr_widget
* 设置当前显示的widget（在动画播放完后自动释放后台的widget）
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget window_anim_menu对象。
* @param {widget_t*} next_widget
*动态加载的widget（该widget会被设置为window_anim_menu对象的子集）。
* @param {animating_move_type_t} animating_move_type 动画的类型。
* @param {bool_t} is_move_back 是否是返回类型动画。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t window_anim_menu_play_anim_move_child_for_widget(
    widget_t *widget, widget_t *next_widget,
    animating_move_type_t animating_move_type, bool_t is_move_back);

/**
* @method window_anim_menu_play_anim_move_child_for_load
* 设置当前显示的widget（在动画播放完后自动释放后台的widget）
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget window_anim_menu对象。
* @param {const char*} widget_name 动态加载的widget的名字。
* @param {animating_move_type_t} animating_move_type 动画的类型。
* @param {bool_t} is_move_back 是否是返回类型动画。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t window_anim_menu_play_anim_move_child_for_load(
    widget_t *widget, const char *widget_name,
    animating_move_type_t animating_move_type, bool_t is_move_back);

/**
* @method window_anim_menu_on_load_widget
* 设置成功加载当前显示widget后触发的事件
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget window_anim_menu对象。
* @param {event_load_widget_func_t} load_widget_func 触发函数。
* @param {void*} ctx 上下文。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t window_anim_menu_on_load_widget(widget_t *widget,
                                      event_load_widget_func_t load_widget_func,
                                      void *ctx);

/**
* @method window_anim_menu_on_dispose_widget
* 设置释放之前显示widget后触发的事件
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget window_anim_menu对象。
* @param {event_load_widget_func_t} load_widget_func 触发函数。
* @param {void*} ctx 上下文。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t window_anim_menu_on_dispose_widget(
    widget_t *widget, event_dispose_widget_func_t dispose_widget, void *ctx);

#define WIDGET_TYPE_WINDOW_ANIM_MENU "window_anim_menu"

#define WINDOW_ANIM_MENU(widget)                                               \
  ((window_anim_menu_t *)(window_anim_menu_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(window_anim_menu);

END_C_DECLS

#endif /*TK_WINDOW_ANIM_MENU_H*/
