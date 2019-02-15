/**
 * File:   chart_view.c
 * Author: AWTK Develop Team
 * Brief:  chart_view
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2018-12-05 Xu ChaoZe <xuchaoze@zlg.cn> created
 *
 */

#include "chart_view.h"
#include "line_series.h"
#include "bar_series.h"

#define _VGCANVAS_TRANSLATE(vg, x, y) vgcanvas_translate(vg, _VG_XY(x), _VG_XY(y))

#define CHART_VIEW_FOR_EACH_AXIS_BEGIN(chart_view, iter, i)     \
  if (chart_view->axes != NULL && chart_view->axes->size > 0) { \
    int32_t i = 0;                                              \
    int32_t nr = chart_view->axes->size;                        \
    axis_t** nodes = (axis_t**)(chart_view->axes->elms);        \
    for (i = 0; i < nr; i++) {                                  \
      axis_t* iter = nodes[i];

#define CHART_VIEW_FOR_EACH_AXIS_BEGIN_R(chart_view, iter, i)   \
  if (chart_view->axes != NULL && chart_view->axes->size > 0) { \
    int32_t i = 0;                                              \
    int32_t nr = chart_view->axes->size;                        \
    axis_t** nodes = (axis_t**)(chart_view->axes->elms);        \
    for (i = nr - 1; i >= 0; i--) {                             \
      axis_t* iter = nodes[i];

#define CHART_VIEW_FOR_EACH_AXIS_END() \
  }                                    \
  }

#define CHART_VIEW_FOR_EACH_SERIES_BEGIN(chart_view, iter, i)       \
  if (chart_view->series != NULL && chart_view->series->size > 0) { \
    int32_t i = 0;                                                  \
    int32_t nr = chart_view->series->size;                          \
    series_t** nodes = (series_t**)(chart_view->series->elms);      \
    for (i = 0; i < nr; i++) {                                      \
      series_t* iter = nodes[i];

#define CHART_VIEW_FOR_EACH_SERIES_BEGIN_R(chart_view, iter, i)     \
  if (chart_view->series != NULL && chart_view->series->size > 0) { \
    int32_t i = 0;                                                  \
    int32_t nr = chart_view->series->size;                          \
    series_t** nodes = (series_t**)(chart_view->series->elms);      \
    for (i = nr - 1; i >= 0; i--) {                                 \
      series_t* iter = nodes[i];

#define CHART_VIEW_FOR_EACH_SERIES_END() \
  }                                      \
  }

static ret_t chart_view_get_prop(widget_t* widget, const char* name, value_t* v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, CHART_VIEW_PROP_BAR_MARGIN)) {
    value_set_uint32(v, chart_view_get_bar_margin(widget));
    return RET_OK;
  } else if (tk_str_eq(name, CHART_VIEW_PROP_BAR_SPACING)) {
    value_set_uint32(v, chart_view_get_bar_spacing(widget));
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t chart_view_set_prop(widget_t* widget, const char* name, const value_t* v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, CHART_VIEW_PROP_BAR_MARGIN)) {
    return chart_view_set_bar_margin(widget, value_uint32(v));
  } else if (tk_str_eq(name, CHART_VIEW_PROP_BAR_SPACING)) {
    return chart_view_set_bar_spacing(widget, value_uint32(v));
  }

  return RET_NOT_FOUND;
}

static ret_t chart_view_on_add_child(widget_t* widget, widget_t* child) {
  const char* type = widget_get_type(child);
  if (tk_str_eq(type, WIDGET_TYPE_AXIS)) {
    chart_view_t* chart_view = CHART_VIEW(widget);
    if (chart_view) {
      if (chart_view->axes == NULL) {
        chart_view->axes = darray_create(sizeof(axis_t*), NULL, NULL);
      }
      darray_push(chart_view->axes, child);
    }
  } else if (tk_str_eq(type, WIDGET_TYPE_LINE_SERIES) || tk_str_eq(type, WIDGET_TYPE_BAR_SERIES)) {
    chart_view_t* chart_view = CHART_VIEW(widget);
    if (chart_view) {
      if (chart_view->series == NULL) {
        chart_view->series = darray_create(sizeof(series_t*), NULL, NULL);
      }
      darray_push(chart_view->series, child);
    }
  }
  return RET_CONTINUE;
}

static ret_t chart_view_on_remove_child(widget_t* widget, widget_t* child) {
  const char* type = widget_get_type(child);
  if (tk_str_eq(type, WIDGET_TYPE_AXIS)) {
    chart_view_t* chart_view = CHART_VIEW(widget);
    if (chart_view) {
      if (chart_view->axes) {
        darray_remove(chart_view->axes, child);
      }
    }
  } else if (tk_str_eq(type, WIDGET_TYPE_LINE_SERIES) || tk_str_eq(type, WIDGET_TYPE_BAR_SERIES)) {
    chart_view_t* chart_view = CHART_VIEW(widget);
    if (chart_view) {
      if (chart_view->series) {
        darray_remove(chart_view->series, child);
      }
    }
  }
  return RET_CONTINUE;
}

static ret_t chart_view_resize_children(widget_t* widget) {
  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  widget_resize(iter, widget->w, widget->h);
  WIDGET_FOR_EACH_CHILD_END();

  return RET_OK;
}

static ret_t chart_view_on_layout_children(widget_t* widget) {
  chart_view_resize_children(widget);
  return widget_layout_children_default(widget);
}

static ret_t chart_view_on_paint_children(widget_t* widget, canvas_t* c) {
  chart_view_t* chart_view = CHART_VIEW(widget);

  if (chart_view->axes && chart_view->is_axes_dirty) {
    axis_render_layout(widget, c, 0);
    chart_view->is_axes_dirty = FALSE;
  }

  widget_on_paint_children_default(widget, c);

  return RET_OK;
}

static ret_t chart_view_on_destroy(widget_t* widget) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL, RET_BAD_PARAMS);

  if (chart_view->axes != NULL) {
    darray_destroy(chart_view->axes);
  }

  if (chart_view->series != NULL) {
    darray_destroy(chart_view->series);
  }

  return RET_OK;
}

static const char* s_chart_view_properties[] = {CHART_VIEW_PROP_BAR_MARGIN,
                                                CHART_VIEW_PROP_BAR_SPACING, NULL};

static const widget_vtable_t s_chart_view_vtable = {
    .size = sizeof(chart_view_t),
    .type = WIDGET_TYPE_CHART_VIEW,
    .clone_properties = s_chart_view_properties,
    .persistent_properties = s_chart_view_properties,
    .create = chart_view_create,
    .on_add_child = chart_view_on_add_child,
    .on_remove_child = chart_view_on_remove_child,
    .on_layout_children = chart_view_on_layout_children,
    .on_paint_children = chart_view_on_paint_children,
    .get_prop = chart_view_get_prop,
    .set_prop = chart_view_set_prop,
    .on_destroy = chart_view_on_destroy};

widget_t* chart_view_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  chart_view_t* chart_view = TKMEM_ZALLOC(chart_view_t);
  widget_t* widget = WIDGET(chart_view);
  return_value_if_fail(chart_view != NULL, NULL);

  widget_init(widget, parent, &s_chart_view_vtable, x, y, w, h);

  chart_view->bar_layout.margin = 4;
  chart_view->bar_layout.spacing = 0;
  chart_view->is_axes_dirty = TRUE;

  return widget;
}

uint32_t chart_view_count_axis(widget_t* widget, axis_orientation_t orientation) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL, 0);

  uint32_t cnt = 0;
  if (chart_view->axes != NULL) {
    if (orientation == AXIS_ORIENTATION_UNDEF) {
      cnt = chart_view->axes->size;
    } else {
      CHART_VIEW_FOR_EACH_AXIS_BEGIN(chart_view, iter, i)
      if (iter->orientation == orientation) {
        cnt++;
      }
      CHART_VIEW_FOR_EACH_AXIS_END()
    }
  }
  return cnt;
}

widget_t* chart_view_get_axis(widget_t* widget, axis_orientation_t orientation, uint32_t index) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL && chart_view->axes, NULL);

  if (orientation == AXIS_ORIENTATION_UNDEF) {
    return WIDGET(chart_view->axes->elms[index]);
  } else {
    uint32_t cnt = 0;
    CHART_VIEW_FOR_EACH_AXIS_BEGIN(chart_view, iter, i)
    if (iter->orientation == orientation) {
      if (cnt == index) {
        return WIDGET(iter);
      }
      cnt++;
    }
    CHART_VIEW_FOR_EACH_AXIS_END()
  }
  return NULL;
}

uint32_t chart_view_count_series(widget_t* widget, const char* type) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL, 0);

  uint32_t cnt = 0;
  if (chart_view->series != NULL) {
    if (type == NULL) {
      cnt = chart_view->series->size;
    } else {
      CHART_VIEW_FOR_EACH_SERIES_BEGIN(chart_view, iter, i)
      if (tk_str_eq(widget_get_type(WIDGET(iter)), type)) {
        cnt++;
      }
      CHART_VIEW_FOR_EACH_SERIES_END()
    }
  }
  return cnt;
}

widget_t* chart_view_get_series(widget_t* widget, const char* type, uint32_t index) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL && chart_view->series, NULL);

  if (type == NULL) {
    return WIDGET(chart_view->series->elms[index]);
  } else {
    uint32_t cnt = 0;
    CHART_VIEW_FOR_EACH_SERIES_BEGIN(chart_view, iter, i)
    if (tk_str_eq(widget_get_type(WIDGET(iter)), type)) {
      if (cnt == index) {
        return WIDGET(iter);
      }
      cnt++;
    }
    CHART_VIEW_FOR_EACH_SERIES_END()
  }
  return NULL;
}

int32_t chart_view_index_of_series(widget_t* widget, const char* type, widget_t* series) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL && series != NULL, -1);

  if (type) {
    int32_t cnt = 0;
    CHART_VIEW_FOR_EACH_SERIES_BEGIN(chart_view, iter, i)
    if (tk_str_eq(widget_get_type(WIDGET(iter)), type)) {
      if (iter == SERIES(series)) {
        return cnt;
      }
      cnt++;
    }
    WIDGET_FOR_EACH_CHILD_END();
  } else {
    CHART_VIEW_FOR_EACH_SERIES_BEGIN(chart_view, iter, i)
    if (iter == SERIES(series)) {
      return i;
    }
    WIDGET_FOR_EACH_CHILD_END();
  }

  return -1;
}

uint32_t chart_view_get_bar_margin(widget_t* widget) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL, 0);
  return chart_view->bar_layout.margin;
}

ret_t chart_view_set_bar_margin(widget_t* widget, uint32_t margin) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL, RET_BAD_PARAMS);
  chart_view->bar_layout.margin = (uint8_t)margin;
  return RET_OK;
}

uint32_t chart_view_get_bar_spacing(widget_t* widget) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL, 0);
  return chart_view->bar_layout.spacing;
}

ret_t chart_view_set_bar_spacing(widget_t* widget, uint32_t spacing) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL, RET_BAD_PARAMS);
  chart_view->bar_layout.spacing = (uint8_t)spacing;
  return RET_OK;
}

widget_t* chart_view_cast(widget_t* widget) {
  return_value_if_fail(widget != NULL && (widget->vt == &s_chart_view_vtable), NULL);

  return widget;
}
