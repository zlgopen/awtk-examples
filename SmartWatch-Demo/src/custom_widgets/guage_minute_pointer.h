#pragma once
#include <awtk.h>

typedef struct _guage_minute_pointer_t {
  widget_t widget;

  str_t image;
  float_t minute;

} guage_minute_pointer_t;

widget_t* guage_minute_pointer_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);
widget_t* guage_minute_pointer_cast(widget_t* widget);
ret_t guage_minute_pointer_set_image(widget_t* widget, const char* name);

#define WIDGET_TYPE_GUAGE_MINUTE_POINTER "guage_minute_pointer"
#define GUAGE_MINUTE_POINTER(widget) ((guage_minute_pointer_t*)(widget))
