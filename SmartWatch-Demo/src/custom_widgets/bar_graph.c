#include "bar_graph.h"
#include <time.h>
#ifdef _WIN32
#pragma warning(disable : 4244)
#endif  // _WIN32

widget_t* bar_graph_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);
widget_t* bar_graph_cast(widget_t* widget);
static ret_t bar_graph_on_paint_self(widget_t* widget, canvas_t* c);
static ret_t on_destroy_bar_graph(widget_t* widget);
static ret_t get_prop(widget_t* widget, const char* name, value_t* v);
static ret_t set_prop(widget_t* widget, const char* name, const value_t* v);

static const char* s_bar_graph_properties[] = {NULL};

static const widget_vtable_t s_bar_graph_vtable = {.size = sizeof(bar_graph_t),
                                                   .type = WIDGET_TYPE_BAR_GRAPH,
                                                   .clone_properties = s_bar_graph_properties,
                                                   .persistent_properties = s_bar_graph_properties,
                                                   .create = bar_graph_create,
                                                   .on_paint_self = bar_graph_on_paint_self,
                                                   .on_destroy = on_destroy_bar_graph,
                                                   .get_prop = get_prop,
                                                   .set_prop = set_prop};

static ret_t get_prop(widget_t* widget, const char* name, value_t* v) {
  bar_graph_t* bar_graph = BAR_GRAPH(widget);
  ret_t ret = RET_OK;
  if (tk_str_eq(name, "size")) {
    value_set_uint32(v, bar_graph->size);
  } else if (tk_str_eq(name, "spacing")) {
    value_set_uint32(v, bar_graph->size);
  } else if (tk_str_eq(name, "color")) {
  } else {
    ret = RET_NOT_FOUND;
  }
  return ret;
}

static ret_t set_prop(widget_t* widget, const char* name, const value_t* v) {
  bar_graph_t* bar_graph = BAR_GRAPH(widget);
  if (tk_str_eq(name, "size")) {
    bar_graph->size = value_uint32(v);
  } else if (tk_str_eq(name, "spacing")) {
    bar_graph->spacing = value_uint32(v);
  } else if (tk_str_eq(name, "color")) {
    const char* str = value_str(v);
    bar_graph->color = color_parse(str);
  } else if (tk_str_eq(name, "arc")) {
    const char* str = value_str(v);
    if (tk_str_eq(str, "true")) {
      bar_graph->is_arc_endpoint = TRUE;
    }
  } else if (tk_str_eq(name, "zero_color")) {
    const char* str = value_str(v);
    bar_graph->zero_color = color_parse(str);
  } else {
    return RET_NOT_FOUND;
  }
  return RET_OK;
}

widget_t* bar_graph_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, &s_bar_graph_vtable, x, y, w, h);
  assert(widget != NULL);
  bar_graph_t* graph = BAR_GRAPH(widget);
  graph->is_arc_endpoint = FALSE;
  graph->spacing = 0;
  graph->separator = 0;

  graph->color = color_parse("#FFFFFF");
  graph->zero_color = color_parse("#FFFFFF");

  graph->data = NULL;
  // srand(time(NULL));
  return widget;
}

widget_t* bar_graph_cast(widget_t* widget) {
  return widget;
}

static ret_t bar_graph_on_paint_self(widget_t* widget, canvas_t* c) {
  return_value_if_fail(widget != NULL && c != NULL, RET_BAD_PARAMS);
  vgcanvas_t* vg = canvas_get_vgcanvas(c);
  bar_graph_t* bar_graph = BAR_GRAPH(widget);
  wh_t w = widget->w;
  wh_t h = widget->h;

  vgcanvas_save(vg);

  vgcanvas_set_line_width(vg, 0);

  darray_t* array = bar_graph->data;
  if (array == NULL) {
    return RET_OK;
  }

  float_t b_w = (float_t)widget->w / bar_graph->size;  // 单个区域的宽
  float_t b_x = (float_t)b_w * array->size;            // 绘制所有的宽度
  (void)b_x;

  bar_t** elems = (bar_t**)array->elms;
  uint32_t size = array->size;

  uint32_t space = bar_graph->spacing;
  size_t i = 0;
  for (i = 0; i < size; i++) {
    vgcanvas_save(vg);

    b_x = (float)w / bar_graph->size * i + 2 * space;  // 线条所在的矩形的x
    b_w = (float)w / bar_graph->size - 2 * space;      // 线条所在的矩形的w
    (void)b_x;
    (void)b_w;

    float_t high = h * elems[i]->high;  // 高点所在位置
    float_t low = h * elems[i]->low;    // 低点所在位置
    float_t b_y = h - high;             // 线条所在矩形的y
    float_t b_h = high - low;           // 线条所在矩形的h

    if (elems[i]->high == elems[i]->low) {
      vgcanvas_set_fill_color(vg, bar_graph->zero_color);
    } else {
      vgcanvas_set_fill_color(vg, bar_graph->color);
    }

    if (bar_graph->is_arc_endpoint) {
      if (low != high) {
        {
          vgcanvas_save(vg);
          vgcanvas_translate(vg, c->ox, c->oy);
          vgcanvas_translate(vg, b_x + b_w / 2, b_y + b_w / 2);
          vgcanvas_ellipse(vg, 0, 0, b_w / 2, b_w / 2);
          vgcanvas_fill(vg);
          vgcanvas_restore(vg);
        }
      }

      // 线条中间矩形
      if (b_h >= b_w) {
        vgcanvas_save(vg);
        vgcanvas_translate(vg, c->ox, c->oy);

        vgcanvas_rect(vg, b_x, b_y + b_w / 2, b_w, b_h - b_w);
        vgcanvas_fill(vg);

        vgcanvas_restore(vg);
      }

      vgcanvas_save(vg);
      vgcanvas_translate(vg, c->ox, c->oy);
      vgcanvas_translate(vg, b_x + b_w / 2, h - low - b_w / 2);
      vgcanvas_ellipse(vg, 0, 0, b_w / 2, b_w / 2);
      vgcanvas_fill(vg);
      vgcanvas_restore(vg);
    } else {
      vgcanvas_save(vg);
      // vgcanvas_rect_global(vg, b_x, b_y, b_w, b_h, widget);
      vgcanvas_fill(vg);
      vgcanvas_restore(vg);
    }

    vgcanvas_restore(vg);
  }

  vgcanvas_restore(vg);

  return RET_OK;
}

// 自己不保存数据
static ret_t on_destroy_bar_graph(widget_t* widget) {
  bar_graph_t* graph = BAR_GRAPH(widget);
  (void)graph;
  return RET_OK;
}
