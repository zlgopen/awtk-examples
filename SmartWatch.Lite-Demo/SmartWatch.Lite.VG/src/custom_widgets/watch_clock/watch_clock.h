/**
 * File:   watch_clock.h
 * Author: AWTK Develop Team
 * Brief:  watch_clock
 *
 * Copyright (c) 2018 - 2020  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2018-08-30 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef TK_WATCH_CLOCK_H
#define TK_WATCH_CLOCK_H

#include "base/widget.h"

BEGIN_C_DECLS

typedef struct _watch_clock_t {
  widget_t widget;

  /**
   * @property {int32_t} hour
   * @annotation
   * ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 小时。
   */
  float_t hour;
  /**
   * @property {int32_t} minute
   * @annotation
   * ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 分钟。
   */
  float_t minute;
  /**
   * @property {int32_t} second
   * @annotation
   * ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 秒。
   */
  float_t second;

  /**
   * @property {char*} image
   * @annotation
   * ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 中心图片。
   */
  char *image;
  /**
   * @property {char*} bg_image
   * @annotation
   * ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 背景图片。
   */
  char *bg_image;
  /**
   * @property {char*} hour_image
   * @annotation
   * ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 时针图片。
   */
  char *hour_image;
  /**
   * @property {char*} minute_image
   * @annotation
   * ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 分针图片。
   */
  char *minute_image;
  /**
   * @property {char*} second_image
   * @annotation
   * ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 秒针图片。
   */
  char *second_image;

  /**
   * @property {char*} hour_anchor_x
   * @annotation
   * ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 时针图片旋转锚点x坐标。(后面加上px为像素点，不加px为相对百分比坐标0.0f到1.0f)
   */
  char *hour_anchor_x;

  /**
   * @property {char*} hour_anchor_y
   * @annotation
   * ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 时针图片旋转锚点y坐标。(后面加上px为像素点，不加px为相对百分比坐标0.0f到1.0f)
   */
  char *hour_anchor_y;

  /**
   * @property {char*} minute_anchor_x
   * @annotation
   * ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 分针图片旋转锚点x坐标。(后面加上px为像素点，不加px为相对百分比坐标0.0f到1.0f)
   */
  char *minute_anchor_x;

  /**
   * @property {char*} minute_anchor_y
   * @annotation
   * ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 分针图片旋转锚点y坐标。(后面加上px为像素点，不加px为相对百分比坐标0.0f到1.0f)
   */
  char *minute_anchor_y;

  /**
   * @property {char*} second_anchor_x
   * @annotation
   * ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 秒针图片旋转锚点x坐标。(后面加上px为像素点，不加px为相对百分比坐标0.0f到1.0f)
   */
  char *second_anchor_x;

  /**
   * @property {char*} second_anchor_y
   * @annotation
   * ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 秒针图片旋转锚点y坐标。(后面加上px为像素点，不加px为相对百分比坐标0.0f到1.0f)
   */
  char *second_anchor_y;

  bool_t is_smooth;
  bool_t is_last_smooth;

  bool_t is_animator;
  bool_t is_animator_end;
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

/**
* @method watch_clock_set_smooth
* 设置指针是否平滑移动。
* @annotation ["scriptable"]
* @param {widget_t*} widget 控件对象。
* @param {bool_t} smooth 是否平滑。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t watch_clock_set_smooth(widget_t *widget, bool_t smooth);

/**
 * @method watch_clock_set_hour
 * 设置小时的值。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {int32_t} hour 小时的值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t watch_clock_set_hour(widget_t *widget, float_t hour);

/**
 * @method watch_clock_set_minute
 * 设置分钟的值。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {int32_t} minute 分钟的值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t watch_clock_set_minute(widget_t *widget, float_t minute);

/**
 * @method watch_clock_set_second
 * 设置秒的值。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {int32_t} second 秒的值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t watch_clock_set_second(widget_t *widget, float_t second);

/**
 * @method watch_clock_set_hour_image
 * 设置小时的图片。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {const char*} hour 小时的图片。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t watch_clock_set_hour_image(widget_t *widget, const char *hour);

/**
 * @method watch_clock_set_minute_image
 * 设置分钟的图片。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {const char*} minute_image 分钟的图片。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t watch_clock_set_minute_image(widget_t *widget, const char *minute_image);

/**
 * @method watch_clock_set_second_image
 * 设置秒的图片。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {const char*} second_image 秒的图片。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t watch_clock_set_second_image(widget_t *widget, const char *second_image);

/**
 * @method watch_clock_set_bg_image
 * 设置背景图片。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {const char*} bg_image 背景图片。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t watch_clock_set_bg_image(widget_t *widget, const char *bg_image);

/**
 * @method watch_clock_set_image
 * 设置中心图片。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {const char*} image 图片。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t watch_clock_set_image(widget_t *widget, const char *image);

/**
 * @method watch_clock_set_hour_anchor
 * 设置小时指针的旋转锚点。
 * > 后面加上px为像素点，不加px为相对百分比坐标0.0f到1.0f
 *
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {const char*} anchor_x 指针的锚点坐标x。
 * @param {const char*} anchor_y 指针的锚点坐标y。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t watch_clock_set_hour_anchor(widget_t *widget, const char *anchor_x,
                                  const char *anchor_y);

/**
 * @method watch_clock_set_minute_anchor
 * 设置分钟指针的旋转锚点。
 * > 后面加上px为像素点，不加px为相对百分比坐标0.0f到1.0f
 *
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {const char*} anchor_x 指针的锚点坐标x。
 * @param {const char*} anchor_y 指针的锚点坐标y。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t watch_clock_set_minute_anchor(widget_t *widget, const char *anchor_x,
                                    const char *anchor_y);

/**
* @method watch_clock_start_animator
* 开启开始动画。
* > 需要 WATCH_CLOCK_PROP_START_ANIMATOR 属性设为 true 才有效果。
*
* @annotation ["scriptable"]
* @param {widget_t*} widget 控件对象。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t watch_clock_start_animator(widget_t *widget);

/**
 * @method watch_clock_set_second_anchor
 * 设置秒钟指针的旋转锚点。
 * > 后面加上px为像素点，不加px为相对百分比坐标0.0f到1.0f
 *
 * @annotation ["scriptable"]
 * @param {widget_t*} widget 控件对象。
 * @param {const char*} anchor_x 指针的锚点坐标x。
 * @param {const char*} anchor_y 指针的锚点坐标y。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t watch_clock_set_second_anchor(widget_t *widget, const char *anchor_x,
                                    const char *anchor_y);

#define WATCH_CLOCK_PROP_HOUR "hour"
#define WATCH_CLOCK_PROP_MINUTE "minute"
#define WATCH_CLOCK_PROP_SECOND "second"

#define WATCH_CLOCK_PROP_IMAGE "image"
#define WATCH_CLOCK_PROP_BG_IMAGE "bg_image"
#define WATCH_CLOCK_PROP_HOUR_IMAGE "hour_image"
#define WATCH_CLOCK_PROP_MINUTE_IMAGE "minute_image"
#define WATCH_CLOCK_PROP_SECOND_IMAGE "second_image"

#define WATCH_CLOCK_PROP_HOUR_ANCHOR_X "hour_anchor_x"
#define WATCH_CLOCK_PROP_HOUR_ANCHOR_Y "hour_anchor_y"

#define WATCH_CLOCK_PROP_MINUTE_ANCHOR_X "minute_anchor_x"
#define WATCH_CLOCK_PROP_MINUTE_ANCHOR_Y "minute_anchor_y"

#define WATCH_CLOCK_PROP_SECOND_ANCHOR_X "second_anchor_x"
#define WATCH_CLOCK_PROP_SECOND_ANCHOR_Y "second_anchor_y"

#define WATCH_CLOCK_PROP_START_ANIMATOR "start_animator"

#define WATCH_CLOCK_PROP_CLOCK_SMOOTH "smooth"

#define WIDGET_TYPE_WATCH_CLOCK "watch_clock"

#define WATCH_CLOCK(widget)                                                    \
  ((watch_clock_t *)(watch_clock_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(watch_clock);

/*public for test*/
ret_t watch_clock_set_anchor_for_str(float_t max_size, const char *anchor,
                                     float_t *image_anchor);

END_C_DECLS

#endif /*TK_WATCH_CLOCK_H*/
