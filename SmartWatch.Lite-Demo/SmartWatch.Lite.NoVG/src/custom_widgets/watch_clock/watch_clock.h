#ifndef WATCH_CLOCK_H
#define WATCH_CLOCK_H

#include "awtk.h"

BEGIN_C_DECLS

typedef struct _watch_clock_t {
  widget_t widget;

  int32_t hour;
  int32_t minute;
  int32_t second;

  char *image;
  char *bg_image;

  widget_t *hour_widget;
  widget_t *minute_widget;
  widget_t *second_widget;
} watch_clock_t;

/**
 * @method watch_clock_create
 * 创建watch_clock对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t *watch_clock_create(widget_t *parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method watch_clock_cast
 * 转换为watch_clock对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget watch_clock对象。
 *
 * @return {widget_t*} watch_clock对象。
 */
widget_t *watch_clock_cast(widget_t *widget);

ret_t watch_clock_set_bg_image(widget_t *widget, const char *bg_image);

ret_t watch_clock_set_image(widget_t *widget, const char *image);

#define WIDGET_TYPE_WATCH_CLOCK "watch_clock"

#define WATCH_CLOCK_PROP_HOUR "hour"
#define WATCH_CLOCK_PROP_MINUTE "minute"
#define WATCH_CLOCK_PROP_SECOND "second"
#define WATCH_CLOCK_PROP_IMAGE "image"
#define WATCH_CLOCK_PROP_BG_IMAGE "bg_image"

#define WATCH_CLOCK(widget)                                                    \
  ((watch_clock_t *)(watch_clock_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(watch_clock);

END_C_DECLS

#endif /*WATCH_CLOCK_H*/
