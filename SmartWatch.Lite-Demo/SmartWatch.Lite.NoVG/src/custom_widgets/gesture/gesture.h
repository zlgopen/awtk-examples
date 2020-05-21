#ifndef TK_GESTURE_H
#define TK_GESTURE_H

#include "base/widget.h"
#include "tkc/darray.h"

BEGIN_C_DECLS

typedef enum _gesture_type_t {
  GESTURE_TOP_TO_BOTTOM_TYPE,
  GESTURE_BOTTOM_TO_TOP_TYPE,
  GESTURE_LEFT_TO_RIGHT_TYPE,
  GESTURE_RIGHT_TO_LEFT_TYPE,
  GESTURE_LONG_PRESS_TYPE,
} gesture_type_t;

typedef ret_t (*on_gesture_fun_t)(gesture_type_t gesture_type,
                                  point_t *start_point, point_t *curr_point,
                                  void *ctx);

typedef struct _gesture_info_t {
  void *ctx;
  xy_t threshold;
  on_gesture_fun_t fun;
  gesture_type_t gesture_type;
} gesture_info_t;

typedef struct _gesture_t {
  widget_t widget;

  bool_t pressed;
  bool_t dragged;

  point_t curr_point;
  point_t start_point;

  darray_t *gesture_fun_click_list;

  /* private */
  bool_t is_trigger;
  uint32_t trigger_delay_time;

} gesture_t;

/**
* @method gesture_create
* 创建gesture对象
* @annotation ["constructor", "scriptable"]
* @param {widget_t*} parent 父控件
* @param {xy_t} x x坐标
* @param {xy_t} y y坐标
* @param {wh_t} w 宽度
* @param {wh_t} h 高度
*
* @return {widget_t*} 对象。
*/
widget_t *gesture_create(widget_t *parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
* @method gesture_cast
* 转换为gesture对象(供脚本语言使用)。
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget gesture对象。
*
* @return {widget_t*} gesture对象。
*/
widget_t *gesture_cast(widget_t *widget);

/**
* @method gesture_on_click_event
* 添加手势触发事件
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget gesture对象。
* @param {on_gesture_fun_t} fun 触发事件。
* @param {gesture_type_t} gesture_type 手势类型。
* @param {void*} ctx 上下文。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t gesture_on_click_event(widget_t *widget, on_gesture_fun_t fun,
                             gesture_type_t gesture_type, void *ctx);

/**
* @method gesture_on_click_event_for_threshold
* 添加手势触发事件
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget gesture对象。
* @param {on_gesture_fun_t} fun 触发事件。
* @param {gesture_type_t} gesture_type 手势类型。
* @param {xy_t} threshold 手势触发门槛。
* @param {void*} ctx 上下文。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t gesture_on_click_event_for_threshold(widget_t *widget,
                                           on_gesture_fun_t fun,
                                           gesture_type_t gesture_type,
                                           xy_t threshold, void *ctx);

/**
* @method gesture_remove_all
* 移除所有事件
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget gesture对象。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t gesture_remove_all(widget_t *widget);

#define WIDGET_TYPE_GESTURE "gesture"

#define GESTURE(widget) ((gesture_t *)(gesture_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(gesture);

END_C_DECLS

#endif /*TK_GESTURE_H*/
