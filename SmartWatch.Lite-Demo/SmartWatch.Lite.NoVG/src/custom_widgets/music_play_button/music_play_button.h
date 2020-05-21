#ifndef MUSIC_PLAY_BUTTON_H
#define MUSIC_PLAY_BUTTON_H

#include "awtk.h"
#include "draw_circle.h"

BEGIN_C_DECLS

typedef struct _music_play_button_t {
  widget_t widget;

  uint32_t value;

  uint32_t max_value;

  uint32_t line_width;

  bool_t counter_clock_wise;

  int32_t start_angle;

  color_t line_color;

  /*private*/
  bool_t pressed;
  draw_circle_t *draw_circle_ctx;
} music_play_button_t;

/**
 * @method music_play_button_create
 * 创建music_play_button对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t *music_play_button_create(widget_t *parent, xy_t x, xy_t y, wh_t w,
                                   wh_t h);

/**
 * @method music_play_button_cast
 * 转换为music_play_button对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget music_play_button对象。
 *
 * @return {widget_t*} music_play_button对象。
 */
widget_t *music_play_button_cast(widget_t *widget);

/**
* @method music_play_set_value
* 设置当前播放进度值
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget music_play_button对象。
* @param {uint32_t} value 进度值。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t music_play_set_value(widget_t *widget, uint32_t value);

/**
* @method music_play_set_counter_clock_wise
* 设置进度条是否为逆时针显示
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget music_play_button对象。
* @param {bool_t} counter_clock_wise 是否逆时针显示。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t music_play_set_counter_clock_wise(widget_t *widget,
                                        bool_t counter_clock_wise);

/**
* @method music_play_set_start_angle
* 设置进度条开始播放角度
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget music_play_button对象。
* @param {int32_t} start_angle 开始播放角度。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t music_play_set_start_angle(widget_t *widget, int32_t start_angle);

/**
* @method music_play_set_line_width
* 设置进度条宽度
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget music_play_button对象。
* @param {uint32_t} line_width 进度条宽度。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t music_play_set_line_width(widget_t *widget, uint32_t line_width);

/**
* @method music_play_set_line_width
* 设置进度条最大值
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget music_play_button对象。
* @param {uint32_t} max_value 进度条最大值。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t music_play_set_max_value(widget_t *widget, uint32_t max_value);

#define WIDGET_TYPE_MUSIC_PLAY_BUTTON "music_play_button"

#define MUSIC_PLAY_BUTTON_PROP_LINE_COLOR "line_color"
#define MUSIC_PLAY_BUTTON_PROP_LINE_WIDTH "line_width"
#define MUSIC_PLAY_BUTTON_PROP_START_ANGLE "start_angle"
#define MUSIC_PLAY_BUTTON_PROP_COUNTER_CLOCK_WISE "counter_clock_wise"
#define MUSIC_PLAY_BUTTON(widget)                                              \
  ((music_play_button_t *)(music_play_button_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(music_play_button);

END_C_DECLS

#endif /*MUSIC_PLAY_BUTTON_H*/
