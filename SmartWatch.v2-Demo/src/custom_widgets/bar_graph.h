#pragma once
#include "awtk.h"

typedef struct _bar_t {
  float_t high;
  float_t low;
} bar_t;

typedef struct _bar_graph_t {
  widget_t widget;
  darray_t* data;
  uint32_t size;  // 条数
  uint32_t time;
  bool_t is_arc_endpoint;
  uint32_t spacing;
  color_t color;
  uint32_t separator;

  color_t zero_color;  // 数值为0时的颜色
} bar_graph_t;

widget_t* bar_graph_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);
widget_t* bar_graph_cast(widget_t* widget);
#define WIDGET_TYPE_BAR_GRAPH "bar_graph"
#define BAR_GRAPH(widget) ((bar_graph_t*)(widget))
