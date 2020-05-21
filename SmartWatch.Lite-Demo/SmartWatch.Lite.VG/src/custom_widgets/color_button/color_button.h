#ifndef COLOR_BUTTON_H
#define COLOR_BUTTON_H

#include "awtk.h"

BEGIN_C_DECLS

typedef struct _color_button_t {
  widget_t widget;

  int32_t yoffset;
  uint32_t height;
  bool_t is_button;
  bool_t is_orientation_top;
  /*private*/
  bool_t pressed;
} color_button_t;

/**
 * @method color_button_create
 * 创建color_button对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t *color_button_create(widget_t *parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method color_button_cast
 * 转换为color_button对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget color_button对象。
 *
 * @return {widget_t*} color_button对象。
 */
widget_t *color_button_cast(widget_t *widget);

#define WIDGET_TYPE_COLOR_BUTTON "color_button"

#define COLOR_BUTTON_PROP_BUTTON "button"
#define COLOR_BUTTON_PROP_HEIGHT "l"
#define COLOR_BUTTON_PROP_ORIENTATION_TOP "orientation_top"

#define COLOR_BUTTON(widget)                                                   \
  ((color_button_t *)(color_button_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(color_button);

END_C_DECLS

#endif /*COLOR_BUTTON_H*/
