/**
 *
 *
 *
 */

#ifndef TK_HOUR_WEATHER_H
#define TK_HOUR_WEATHER_H

#include <base/widget.h>
#include <tkc/date_time.h>

typedef struct _hour_weather_t {
  widget_t widget;

  int32_t time;
  uint8_t selected;

  float_t width;
  color_t bar_color;
  color_t point_color;

  char* weather_info;

  // private
  int32_t temp[11];    // 温度
  int8_t weather[11];  // 天气

  bool_t pressed;
  float_t point_rad;  // 指示点的弧度
} hour_weather_t;

widget_t* hour_weather_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

#define WIDGET_TYPE_HOUR_WEATHER "hour_weather"
#define HOUR_WEATHER(widget) ((hour_weather_t*)(widget))

#define WEATHER_NO_DATA 0
#define WEATHER_QINGTIAN 1
#define WEATHER_DUOYUN 2
#define WEATHER_YINTIAN 3
#define WEATHER_ZHENYU 4
#define WEATHER_XIAOYU 5
#define WEATHER_ZHONGYU 6
#define WEATHER_DAYU 7
#define WEATHER_XIAOXUE 8
#define WEATHER_DAXUE 9
#define WEATHER_WUMAI 10
#define WEATHER_YEQING 11

typedef enum _hour_weather_event_type_t {
  EVT_CHANGE_SELECT = EVT_USER_START
} hour_weather_event_type_t;

#endif
