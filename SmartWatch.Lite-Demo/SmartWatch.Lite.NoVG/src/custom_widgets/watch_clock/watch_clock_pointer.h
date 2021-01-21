#ifndef WATCH_CLOCK_POINTER_H
#define WATCH_CLOCK_POINTER_H

#include "awtk.h"

BEGIN_C_DECLS

typedef enum _watch_clock_pointer_type_t {
  WATCH_CLOCK_POINTER_HOUR = 0x0,
  WATCH_CLOCK_POINTER_MINUTE,
  WATCH_CLOCK_POINTER_SECOND,

  WATCH_CLOCK_POINTER_NONE = 0xff,
} watch_clock_pointer_type_t;

const static char *watch_clock_pointer_type_string_list[] = {"hour", "minute",
                                                             "second"};

typedef struct _watch_clock_pointer_t {
  widget_t widget;
  uint8_t value;

  int32_t anchor_x;
  int32_t anchor_y;

  bitmap_t bitmap;

  watch_clock_pointer_type_t clock_pointer_type;
  uint8_t *point_image_data;
  bool_t is_oom;
  char *pointer_image;

  asset_info_t *asset_image_info;

} watch_clock_pointer_t;

/**
 * @method watch_clock_pointer_create
 * 创建watch_clock_pointer对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t *watch_clock_pointer_create(widget_t *parent, xy_t x, xy_t y, wh_t w,
                                     wh_t h);

/**
 * @method watch_clock_pointer_cast
 * 转换为watch_clock_pointer对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget watch_clock_pointer对象。
 *
 * @return {widget_t*} watch_clock_pointer对象。
 */
widget_t *watch_clock_pointer_cast(widget_t *widget);

ret_t watch_clock_pointer_set_value(widget_t *widget, uint8_t value);

watch_clock_pointer_type_t
watch_clock_pointer_get_clock_pointer_type(widget_t *widget);

ret_t watch_clock_pointer_set_clock_pointer_type(
    widget_t *widget, watch_clock_pointer_type_t type);

#define WIDGET_TYPE_WATCH_CLOCK_POINTER "watch_clock_pointer"

#define WATCH_CLOCK_POINTER_PROP_IMAGE "image"
#define WATCH_CLOCK_POINTER_PROP_ANCHOR_X "anchor_x"
#define WATCH_CLOCK_POINTER_PROP_ANCHOR_L_Y "anchor_l_y"
#define WATCH_CLOCK_POINTER_PROP_CLOCK_POINTER_TYPE "clock_pointer_type"

#define WATCH_CLOCK_POINTER(widget)                                            \
  ((watch_clock_pointer_t *)(watch_clock_pointer_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(watch_clock_pointer);

END_C_DECLS

#endif /*WATCH_CLOCK_POINTER_H*/
