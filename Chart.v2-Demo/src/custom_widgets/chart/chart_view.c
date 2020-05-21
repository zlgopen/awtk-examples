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
#include "axis.h"
#include "series_p.h"
#include "tooltip.h"

static widget_t* chart_view_get_series(widget_t* widget, uint32_t index) {
  uint32_t cnt = 0;
  return_value_if_fail(widget != NULL, NULL);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  if (widget_is_series(iter)) {
    if (cnt == index) {
      return iter;
    }

    cnt++;
  }
  WIDGET_FOR_EACH_CHILD_END()

  return NULL;
}

static ret_t chart_view_get_prop(widget_t* widget, const char* name, value_t* v) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, CHART_VIEW_PROP_TOP_SERIES)) {
    value_set_uint32(v, chart_view->top_series);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t chart_view_set_prop(widget_t* widget, const char* name, const value_t* v) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, CHART_VIEW_PROP_TOP_SERIES)) {
    return chart_view_set_top_series(widget, value_int32(v));
  }

  return RET_NOT_FOUND;
}

static ret_t chart_view_calc_series_rect(widget_t* widget, rect_t* r) {
  style_t* style;
  int32_t margin = 0;
  int32_t margin_left = 0;
  int32_t margin_right = 0;
  int32_t margin_top = 0;
  int32_t margin_bottom = 0;
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL && r != NULL, RET_BAD_PARAMS);

  style = widget->astyle;
  return_value_if_fail(style != NULL, RET_BAD_PARAMS);

  margin = style_get_int(style, STYLE_ID_MARGIN, 0);
  margin_top = style_get_int(style, STYLE_ID_MARGIN_TOP, margin);
  margin_left = style_get_int(style, STYLE_ID_MARGIN_LEFT, margin);
  margin_right = style_get_int(style, STYLE_ID_MARGIN_RIGHT, margin);
  margin_bottom = style_get_int(style, STYLE_ID_MARGIN_BOTTOM, margin);

  r->x = margin_left;
  r->y = margin_top;
  r->w = widget->w - margin_left - margin_right;
  r->h = widget->h - margin_top - margin_bottom;
  return RET_OK;
}

static ret_t chart_view_on_layout_axes(widget_t* widget) {
  rect_t r = {0};
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  return_value_if_fail(chart_view_calc_series_rect(widget, &r) == RET_OK, RET_BAD_PARAMS);
  return_value_if_fail(r.w > 0 && r.h > 0, RET_BAD_PARAMS);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  if (widget_is_axis(iter)) {
    axis_on_self_layout(iter, &r);
    widget_move_resize(iter, 0, 0, widget->w, widget->h);
  }
  WIDGET_FOR_EACH_CHILD_END()

  return RET_OK;
}

static ret_t chart_view_on_layout_series(widget_t* widget) {
  uint32_t cnt = 0;
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL, RET_BAD_PARAMS);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  if (widget_is_series(iter)) {
    widget_move_resize(iter, 0, 0, widget->w, widget->h);
    cnt++;
  }
  WIDGET_FOR_EACH_CHILD_END()

  if (chart_view->series_cnt != cnt) {
    TKMEM_FREE(chart_view->last_series_offset);
    if (cnt > 0) {
      chart_view->last_series_offset = TKMEM_ZALLOCN(uint32_t, cnt);
    }
    chart_view->series_cnt = cnt;
  }

  return RET_OK;
}

static ret_t chart_view_on_layout_tooltip(widget_t* widget) {
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  if (widget_is_tooltip(iter)) {
    widget_move_resize(iter, 0, 0, widget->w, widget->h);
  }
  WIDGET_FOR_EACH_CHILD_END()

  return RET_OK;
}

static ret_t chart_view_on_layout_children(widget_t* widget) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL, RET_BAD_PARAMS);

  widget_layout_children_default(widget);
  chart_view_on_layout_series(widget);
  chart_view_on_layout_tooltip(widget);
  chart_view->need_relayout_axes = TRUE;
  return RET_OK;
}

static ret_t chart_view_on_paint_self(widget_t* widget, canvas_t* c) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL && c != NULL, RET_BAD_PARAMS);

  if (chart_view->need_relayout_axes) {
    chart_view_on_layout_axes(widget);
    chart_view->need_relayout_axes = FALSE;
  }

  return RET_OK;
}

static ret_t chart_view_on_paint_children_before(widget_t* widget, canvas_t* c) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL && c != NULL, RET_BAD_PARAMS);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  int32_t left = c->ox + iter->x;
  int32_t top = c->oy + iter->y;
  int32_t bottom = top + iter->h;
  int32_t right = left + iter->w;

  if (!iter->visible) {
    iter->dirty = FALSE;
    continue;
  }

  if (left > c->clip_right || right < c->clip_left || top > c->clip_bottom ||
      bottom < c->clip_top) {
    iter->dirty = FALSE;
    continue;
  }

  if (widget_is_axis(iter)) {
    axis_on_paint_before(iter, c);
  }

  WIDGET_FOR_EACH_CHILD_END();

  return RET_OK;
}

static ret_t chart_view_on_paint_children(widget_t* widget, canvas_t* c) {
  widget_t* top_series = NULL;
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL && c != NULL, RET_BAD_PARAMS);

  if (chart_view->top_series >= 0) {
    top_series = chart_view_get_series(widget, chart_view->top_series);
  }

  chart_view_on_paint_children_before(widget, c);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  int32_t left = c->ox + iter->x;
  int32_t top = c->oy + iter->y;
  int32_t bottom = top + iter->h;
  int32_t right = left + iter->w;

  if (!iter->visible) {
    iter->dirty = FALSE;
    continue;
  }

  if (left > c->clip_right || right < c->clip_left || top > c->clip_bottom ||
      bottom < c->clip_top) {
    iter->dirty = FALSE;
    continue;
  }

  if (top_series != NULL && iter == top_series) {
    continue;
  }

  widget_paint(iter, c);
  WIDGET_FOR_EACH_CHILD_END();

  if (top_series != NULL) {
    widget_paint(top_series, c);
  }

  return RET_OK;
}

ret_t chart_view_set_top_series(widget_t* widget, int32_t index) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL, RET_BAD_PARAMS);

  if (chart_view->top_series != index) {
    chart_view->top_series = index;
    widget_invalidate(widget, NULL);
  }

  return RET_OK;
}

static ret_t chart_view_set_axes_need_update_data(widget_t* widget) {
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  if (widget_is_axis(iter)) {
    axis_set_need_update_data(iter);
  }
  WIDGET_FOR_EACH_CHILD_END()

  return RET_OK;
}

static ret_t chart_view_on_pointer_down(widget_t* widget, pointer_event_t* evt) {
  uint32_t cnt = 0;
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL && evt != NULL, RET_BAD_PARAMS);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  if (widget_is_series(iter)) {
    chart_view->last_series_offset[cnt] = widget_get_prop_int(iter, SERIES_PROP_OFFSET, 0);
    cnt++;
  }
  WIDGET_FOR_EACH_CHILD_END()

  chart_view->pressed = TRUE;
  chart_view->down.x = evt->x;
  chart_view->down.y = evt->y;

  return RET_OK;
}

static ret_t chart_view_on_pointer_move(widget_t* widget, pointer_event_t* evt) {
  uint32_t cnt = 0;
  bool_t vertical;
  uint32_t interval;
  float_t cur_offset;
  float_t offset;
  widget_t* saxis;
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL && evt != NULL, RET_BAD_PARAMS);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  if (widget_is_series(iter)) {
    saxis = widget_get_prop_pointer(iter, SERIES_PROP_SERIES_AXIS);
    return_value_if_fail(saxis != NULL, RET_BAD_PARAMS);

    interval = axis_measure_series_interval(saxis);
    vertical = series_p_is_vertical(iter);

    if (!chart_view->dragged) {
      float_t threshold = tk_min(TK_DRAG_THRESHOLD, interval / 2);
      if (vertical) {
        chart_view->dragged = tk_abs(evt->x - chart_view->down.x) > threshold;
      } else {
        chart_view->dragged = tk_abs(evt->y - chart_view->down.y) > threshold;
      }
    }

    if (chart_view->dragged) {
      offset = vertical ? (evt->x - chart_view->down.x) : (chart_view->down.y - evt->y);
      offset =
          chart_view->last_series_offset[cnt] + (AXIS(saxis)->inverse ? -1 : 1) * offset / interval;
      offset = tk_max(0, offset);

      cur_offset = widget_get_prop_int(iter, SERIES_PROP_OFFSET, 0);
      if (offset >= 0 && offset != cur_offset) {
        widget_set_prop_int(iter, SERIES_PROP_OFFSET, offset);
        chart_view_set_axes_need_update_data(widget);
      }
    }

    cnt++;
  }
  WIDGET_FOR_EACH_CHILD_END()

  return RET_OK;
}

static ret_t chart_view_move_tooltip(widget_t* widget, xy_t x, xy_t y) {
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  if (widget_is_tooltip(iter)) {
    tooltip_move(iter, x, y);
  }
  WIDGET_FOR_EACH_CHILD_END()
  return RET_OK;
}

static ret_t chart_view_on_pointer_up(widget_t* widget, pointer_event_t* evt) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL && evt != NULL, RET_BAD_PARAMS);

  if (!chart_view->dragged) {
    point_t p;
    int32_t index;

    WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
    if (widget_is_series(iter)) {
      index = series_index_of_point_in(iter, evt->x, evt->y, FALSE);
      if (index >= 0 && series_to_local(iter, index, &p) == RET_OK) {
        widget_to_global(iter, &p);
        chart_view_move_tooltip(widget, p.x, p.y);
        break;
      }
    }
    WIDGET_FOR_EACH_CHILD_END()
  }

  return RET_OK;
}

static ret_t chart_view_pointer_up_cleanup(widget_t* widget) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL, RET_BAD_PARAMS);

  chart_view->pressed = FALSE;
  chart_view->dragged = FALSE;
  widget_ungrab(widget->parent, widget);

  return RET_OK;
}

static ret_t chart_view_on_event(widget_t* widget, event_t* e) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL, RET_BAD_PARAMS);

  switch (e->type) {
    case EVT_POINTER_DOWN: {
      chart_view_on_pointer_down(widget, (pointer_event_t*)e);
      widget_grab(widget->parent, widget);
      break;
    }
    case EVT_POINTER_DOWN_ABORT: {
      chart_view_pointer_up_cleanup(widget);
      break;
    }
    case EVT_POINTER_MOVE: {
      if (chart_view->pressed) {
        chart_view_on_pointer_move(widget, (pointer_event_t*)e);
      }
      break;
    }
    case EVT_POINTER_UP: {
      chart_view_on_pointer_up(widget, (pointer_event_t*)e);
      chart_view_pointer_up_cleanup(widget);
      break;
    }
    default:
      break;
  }

  return RET_OK;
}

static ret_t chart_view_on_destroy(widget_t* widget) {
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL, RET_BAD_PARAMS);

  TKMEM_FREE(chart_view->last_series_offset);

  return RET_OK;
}

static const char* s_chart_view_properties[] = {CHART_VIEW_PROP_TOP_SERIES, NULL};

TK_DECL_VTABLE(chart_view) = {.size = sizeof(chart_view_t),
                              .inputable = TRUE,
                              .type = WIDGET_TYPE_CHART_VIEW,
                              .clone_properties = s_chart_view_properties,
                              .persistent_properties = s_chart_view_properties,
                              .parent = TK_PARENT_VTABLE(widget),
                              .create = chart_view_create,
                              .on_layout_children = chart_view_on_layout_children,
                              .on_paint_children = chart_view_on_paint_children,
                              .on_paint_self = chart_view_on_paint_self,
                              .get_prop = chart_view_get_prop,
                              .set_prop = chart_view_set_prop,
                              .on_event = chart_view_on_event,
                              .on_destroy = chart_view_on_destroy};

widget_t* chart_view_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(chart_view), x, y, w, h);
  chart_view_t* chart_view = CHART_VIEW(widget);
  return_value_if_fail(chart_view != NULL, NULL);

  chart_view->top_series = -1;
  chart_view->need_relayout_axes = TRUE;

  return widget;
}

widget_t* chart_view_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, chart_view), NULL);

  return widget;
}
