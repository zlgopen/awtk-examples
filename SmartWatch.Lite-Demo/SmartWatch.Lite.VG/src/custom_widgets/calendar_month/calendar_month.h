#ifndef CALENDAR_MONTH_H
#define CALENDAR_MONTH_H

#include "awtk.h"

BEGIN_C_DECLS

typedef struct _calendar_month_t {
  widget_t widget;

  char *font_name;
  int32_t font_size;

  uint32_t animating_time;

  char *str_Monday;
  char *str_Tuesday;
  char *str_Wednesday;
  char *str_Thursday;
  char *str_Friday;
  char *str_Saturday;
  char *str_Sunday;

  int32_t day;
  int32_t month;
  int32_t year;

  /* private */
  bool_t animating;
  int32_t yoffset;
  int32_t curr_day;
  int32_t curr_month;
  int32_t curr_year;

} calendar_month_t;

/**
 * @method calendar_month_create
 * 创建calendar_month对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t *calendar_month_create(widget_t *parent, xy_t x, xy_t y, wh_t w,
                                wh_t h);

/**
 * @method calendar_month_cast
 * 转换为calendar_month对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget calendar_month对象。
 *
 * @return {widget_t*} calendar_month对象。
 */
widget_t *calendar_month_cast(widget_t *widget);

/**
* @method calendar_month_set_Monday_string
* 设置周一的显示字符串
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget calendar_month对象。
* @param {const char*} str_Monday 显示字符串。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t calendar_month_set_Monday_string(widget_t *widget,
                                       const char *str_Monday);

/**
* @method calendar_month_set_Tuesday_string
* 设置周二的显示字符串
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget calendar_month对象。
* @param {const char*} str_Tuesday 显示字符串。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t calendar_month_set_Tuesday_string(widget_t *widget,
                                        const char *str_Tuesday);

/**
* @method calendar_month_set_Wednesday_string
* 设置周三的显示字符串
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget calendar_month对象。
* @param {const char*} str_Wednesday 显示字符串。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t calendar_month_set_Wednesday_string(widget_t *widget,
                                          const char *str_Wednesday);

/**
* @method calendar_month_set_Thursday_string
* 设置周四的显示字符串
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget calendar_month对象。
* @param {const char*} str_Thursday 显示字符串。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t calendar_month_set_Thursday_string(widget_t *widget,
                                         const char *str_Thursday);

/**
* @method calendar_month_set_Friday_string
* 设置周五的显示字符串
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget calendar_month对象。
* @param {const char*} str_Friday 显示字符串。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t calendar_month_set_Friday_string(widget_t *widget,
                                       const char *str_Friday);

/**
* @method calendar_month_set_Saturday_string
* 设置周六的显示字符串
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget calendar_month对象。
* @param {const char*} str_Saturday 显示字符串。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t calendar_month_set_Saturday_string(widget_t *widget,
                                         const char *str_Saturday);

/**
* @method calendar_month_set_Sunday_string
* 设置周日的显示字符串
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget calendar_month对象。
* @param {const char*} str_Sunday 显示字符串。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t calendar_month_set_Sunday_string(widget_t *widget,
                                       const char *str_Sunday);

/**
* @method calendar_month_set_font_size
* 设置字号
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget calendar_month对象。
* @param {int32_t} font_size 字号。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t calendar_month_set_font_size(widget_t *widget, int32_t font_size);

/**
* @method calendar_month_set_font_name
* 设置字体资源名称
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget calendar_month对象。
* @param {const char*} font_name 字体资源名称。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t calendar_month_set_font_name(widget_t *widget, const char *font_name);

/**
* @method calendar_month_set_next_month
* 显示下一个月历
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget calendar_month对象。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t calendar_month_set_next_month(widget_t *widget);

/**
* @method calendar_month_set_prev_month
* 显示上一个月历
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget calendar_month对象。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t calendar_month_set_prev_month(widget_t *widget);

/**
* @method calendar_month_set_prev_month
* 显示当前的月历
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget calendar_month对象。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t calendar_month_set_curr_month(widget_t *widget);

/**
* @method calendar_month_set_animating_time
* 设置动画播放时间，默认为500毫秒
* @annotation ["cast", "scriptable"]
* @param {widget_t*} widget calendar_month对象。
* @param {uint32_t} animating_time 动画时间。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t calendar_month_set_animating_time(widget_t *widget,
                                        uint32_t animating_time);

#define WIDGET_TYPE_CALENDAR_MONTH "calendar_month"
#define CALENDAR_MONTH_PROP_FONT_SIZE "font_size"
#define CALENDAR_MONTH_PROP_FONT_NAME "font_name"

#define CALENDAR_MONTH_PROP_ANIMATING_TIME "animating_time"

#define CALENDAR_MONTH_PROP_STRING_MONDAY "Monday"
#define CALENDAR_MONTH_PROP_STRING_TUESDAY "Tuesday"
#define CALENDAR_MONTH_PROP_STRING_WEDNESDAY "Wednesday"
#define CALENDAR_MONTH_PROP_STRING_THURSDAY "Thursday"
#define CALENDAR_MONTH_PROP_STRING_FRIDAY "Friday"
#define CALENDAR_MONTH_PROP_STRING_SATURDAY "Saturday"
#define CALENDAR_MONTH_PROP_STRING_SUNDAY "Sunday"

#define CALENDAR_MONTH(widget)                                                 \
  ((calendar_month_t *)(calendar_month_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(calendar_month);

END_C_DECLS

#endif /*CALENDAR_MONTH_H*/
