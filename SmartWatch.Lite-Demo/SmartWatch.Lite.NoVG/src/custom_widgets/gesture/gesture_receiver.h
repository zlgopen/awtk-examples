#ifndef TK_GESTURE_RECEIVER_H
#define TK_GESTURE_RECEIVER_H

#include "base/widget.h"
#include "gesture.h"

BEGIN_C_DECLS

typedef struct _gesture_receiver_t {
  widget_t widget;

  widget_t *gesture;
} gesture_receiver_t;

/**
 * @method gesture_receiver_create
 * 创建gesture_receiver对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t *gesture_receiver_create(widget_t *parent, xy_t x, xy_t y, wh_t w,
                                  wh_t h);

/**
 * @method gesture_receiver_cast
 * 转换为gesture_receiver对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget gesture_receiver对象。
 *
 * @return {widget_t*} gesture_receiver对象。
 */
widget_t *gesture_receiver_cast(widget_t *widget);

#define WIDGET_TYPE_GESTURE_RECEIVER "gesture_receiver"

#define GESTURE_RECEIVER(widget)                                               \
  ((gesture_receiver_t *)(gesture_receiver_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(gesture_receiver);

END_C_DECLS

#endif /*GESTURE_RECEIVER*/
