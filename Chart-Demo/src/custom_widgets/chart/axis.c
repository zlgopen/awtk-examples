/**
 * File:   axis.c
 * Author: AWTK Develop Team
 * Brief:  axis
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is dirich_text_nodeibuted in the hope that it will be useful,
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

#include "axis.h"
#include "base/style_factory.h"
#include "tkc/utils.h"
#include "tkc/wstr.h"
#include "axis_p.h"
#include "series.h"

axis_data_t* axis_data_create(float_t tick, const char* text) {
  axis_data_t* d = (axis_data_t*)TKMEM_ZALLOC(axis_data_t);
  return_value_if_fail(d != NULL, NULL);

  d->tick = tick;
  wstr_init(&(d->text), 0);
  if (text) {
    wstr_set_utf8(&(d->text), text);
  }

  return d;
}

ret_t axis_data_destroy(axis_data_t* data) {
  return_value_if_fail(data != NULL, RET_BAD_PARAMS);

  wstr_reset(&(data->text));
  TKMEM_FREE(data);

  return RET_OK;
}

static ret_t axis_start_init_if_not_inited(widget_t* widget) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL, RET_BAD_PARAMS);

  if (!axis->inited) {
    assert(axis->split_line.astyle != NULL);
    widget_subpart_update_style(axis->split_line.astyle, widget, AXIS_SUBPART_SPLIT_LINE,
                                axis->split_line.style);

    assert(axis->line.astyle != NULL);
    widget_subpart_update_style(axis->line.astyle, widget, AXIS_SUBPART_LINE, axis->line.style);

    assert(axis->tick.astyle != NULL);
    widget_subpart_update_style(axis->tick.astyle, widget, AXIS_SUBPART_TICK, axis->tick.style);

    assert(axis->label.astyle != NULL);
    widget_subpart_update_style(axis->label.astyle, widget, AXIS_SUBPART_LABEL, axis->label.style);

    assert(axis->title.astyle != NULL);
    widget_subpart_update_style(axis->title.astyle, widget, AXIS_SUBPART_TITLE, axis->title.style);

    axis->inited = TRUE;
  }

  return RET_OK;
}

ret_t axis_on_paint_begin(widget_t* widget, canvas_t* c) {
  (void)c;
  return axis_start_init_if_not_inited(widget);
}

ret_t axis_on_destroy(widget_t* widget) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL, RET_BAD_PARAMS);

  if (axis->data != NULL) {
    darray_destroy(axis->data);
  }

  TKMEM_FREE(axis->split_line.style);
  style_destroy(axis->split_line.astyle);
  axis->split_line.astyle = NULL;

  TKMEM_FREE(axis->line.style);
  style_destroy(axis->line.astyle);
  axis->line.astyle = NULL;

  TKMEM_FREE(axis->tick.style);
  style_destroy(axis->tick.astyle);
  axis->tick.astyle = NULL;

  TKMEM_FREE(axis->label.style);
  style_destroy(axis->label.astyle);
  axis->label.astyle = NULL;

  TKMEM_FREE(axis->title.style);
  style_destroy(axis->title.astyle);
  axis->title.astyle = NULL;

  wstr_reset(&(axis->title.text));

  return RET_OK;
}

static ret_t axis_subpart_set_style_name(widget_t* widget, const char* subpart, const char* value) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && subpart != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(subpart, AXIS_SUBPART_SPLIT_LINE)) {
    axis->split_line.style = tk_str_copy(axis->split_line.style, value);
  } else if (tk_str_eq(subpart, AXIS_SUBPART_LINE)) {
    axis->line.style = tk_str_copy(axis->line.style, value);
  } else if (tk_str_eq(subpart, AXIS_SUBPART_TICK)) {
    axis->tick.style = tk_str_copy(axis->tick.style, value);
  } else if (tk_str_eq(subpart, AXIS_SUBPART_LABEL)) {
    axis->label.style = tk_str_copy(axis->label.style, value);
  } else if (tk_str_eq(subpart, AXIS_SUBPART_TITLE)) {
    axis->title.style = tk_str_copy(axis->title.style, value);
  } else {
    return RET_NOT_FOUND;
  }

  return RET_OK;
}

static style_t** axis_subpart_get_style_obj(widget_t* widget, const char* subpart) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && subpart != NULL, NULL);

  if (tk_str_eq(subpart, AXIS_SUBPART_SPLIT_LINE)) {
    return &(axis->split_line.astyle);
  } else if (tk_str_eq(subpart, AXIS_SUBPART_LINE)) {
    return &(axis->line.astyle);
  } else if (tk_str_eq(subpart, AXIS_SUBPART_TICK)) {
    return &(axis->tick.astyle);
  } else if (tk_str_eq(subpart, AXIS_SUBPART_LABEL)) {
    return &(axis->label.astyle);
  } else if (tk_str_eq(subpart, AXIS_SUBPART_TITLE)) {
    return &(axis->title.astyle);
  } else {
    return NULL;
  }
}

ret_t axis_subpart_use_style(widget_t* widget, const char* subpart, const char* style) {
  return widget_subpart_use_style(widget, subpart, style, axis_subpart_set_style_name,
                                  axis_subpart_get_style_obj);
}

ret_t axis_subpart_set_style(widget_t* widget, const char* subpart, const char* state_and_name,
                             const value_t* value) {
  return widget_subpart_set_style(widget, subpart, state_and_name, value,
                                  axis_subpart_get_style_obj);
}

ret_t axis_subpart_set_style_int(widget_t* widget, const char* subpart, const char* state_and_name,
                                 int32_t value) {
  return widget_subpart_set_style_int(widget, subpart, state_and_name, value,
                                      axis_subpart_get_style_obj);
}

ret_t axis_subpart_set_style_color(widget_t* widget, const char* subpart,
                                   const char* state_and_name, uint32_t value) {
  return widget_subpart_set_style_color(widget, subpart, state_and_name, value,
                                        axis_subpart_get_style_obj);
}

ret_t axis_subpart_set_style_str(widget_t* widget, const char* subpart, const char* state_and_name,
                                 const char* value) {
  return widget_subpart_set_style_str(widget, subpart, state_and_name, value,
                                      axis_subpart_get_style_obj);
}

ret_t axis_set_range(widget_t* widget, float_t min, float_t max) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL, RET_BAD_PARAMS);

  axis->min = tk_min(min, max);
  axis->max = tk_max(min, max);
  axis->need_update_data = !axis->data_fixed;

  return widget_invalidate(widget, NULL);
}

float_t axis_get_range(widget_t* widget) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL, 0.0);

  if (axis->type == AXIS_TYPE_VALUE && axis->max != axis->min) {
    return tk_abs(axis->max - axis->min) + 1;
  } else {
    return (float_t)(axis->data->size);
  }
}

ret_t axis_set_data(widget_t* widget, const char* data) {
  const char* token = NULL;
  tokenizer_t tokenizer;
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && axis->data != NULL && data != NULL, RET_BAD_PARAMS);

  darray_clear(axis->data);

  tokenizer_init(&tokenizer, data, strlen(data), "[,]");
  while (tokenizer_has_more(&tokenizer)) {
    token = tokenizer_next(&tokenizer);
    darray_push(axis->data, axis_data_create(0, token));
  }
  tokenizer_deinit(&tokenizer);

  axis->need_update_data = TRUE;

  return RET_OK;
}

ret_t axis_set_data_from_series(widget_t* widget, axis_data_from_series_t from_series, void* ctx) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL, RET_BAD_PARAMS);

  axis->data_from_series = from_series;
  axis->data_from_series_ctx = ctx;
  axis->need_update_data = TRUE;
  return widget_invalidate(widget, NULL);
}

ret_t axis_set_need_update_data(widget_t* widget) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL, RET_BAD_PARAMS);

  axis->need_update_data = TRUE;
  return widget_invalidate(widget, NULL);
}

static widget_t* axis_lookup_indicated_series(widget_t* widget) {
  return_value_if_fail(widget != NULL && widget->parent != NULL, NULL);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget->parent, iter, i)
  if (widget_is_series(iter)) {
    if (widget_get_prop_pointer(iter, SERIES_PROP_SERIES_AXIS) == widget) {
      return iter;
    }
  }
  WIDGET_FOR_EACH_CHILD_END()

  return NULL;
}

ret_t axis_update_data_from_series(widget_t* widget) {
  uint32_t b = 0;
  uint32_t e = 0;
  uint32_t m = 0;
  float_t interval;
  widget_t* series;
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && axis->data != NULL, RET_BAD_PARAMS);

  series = axis_lookup_indicated_series(widget);
  return_value_if_fail(series != NULL, RET_BAD_PARAMS);

  if (SERIES(series)->fifo && SERIES(series)->fifo->size > 0) {
    return_value_if_fail(series_get_current(series, &b, &e, &m) == RET_OK, RET_FAIL);

    darray_clear(axis->data);

    interval = axis_measure_series_interval(widget);
    axis->data_from_series(axis->data_from_series_ctx, b, e, m, interval, axis->data);
  }

  return RET_OK;
}

ret_t axis_on_paint_before(widget_t* widget, canvas_t* c) {
  ret_t ret = RET_NOT_IMPL;
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && axis->vt != NULL, RET_BAD_PARAMS);

  if (axis->vt->on_paint_before) {
    ret = axis->vt->on_paint_before(widget, c);
  }

  return ret;
}

ret_t axis_on_self_layout(widget_t* widget, rect_t* r) {
  ret_t ret = RET_NOT_IMPL;
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && axis->vt != NULL, RET_BAD_PARAMS);

  if (axis->vt->on_self_layout) {
    ret = axis->vt->on_self_layout(widget, r);
  }

  return ret;
}

float_t axis_measure_series_interval(widget_t* widget) {
  float_t ret = 0;
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && axis->vt != NULL, 0);

  if (axis->vt->measure_series_interval) {
    ret = axis->vt->measure_series_interval(widget);
  }

  return ret;
}

ret_t axis_measure_series(widget_t* widget, void* sample_params, fifo_t* src, fifo_t* dst) {
  ret_t ret = RET_NOT_IMPL;
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && axis->vt != NULL, 0);

  if (axis->vt->measure_series) {
    ret = axis->vt->measure_series(widget, sample_params, src, dst);
  }

  return ret;
}

TK_DECL_VTABLE(axis) = {.size = sizeof(axis_t), .parent = TK_PARENT_VTABLE(widget)};

widget_t* axis_create(widget_t* parent, const widget_vtable_t* vt, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, vt, x, y, w, h);
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL, NULL);

  wstr_init(&(axis->title.text), 0);
  axis->label.show = TRUE;
  axis->tick.show = TRUE;
  axis->line.show = TRUE;
  axis->split_line.show = TRUE;
  axis->data = darray_create(4, (tk_destroy_t)axis_data_destroy, NULL);

  if (axis->split_line.astyle == NULL) {
    axis->split_line.astyle = style_factory_create_style(style_factory(), widget);
  }

  if (axis->line.astyle == NULL) {
    axis->line.astyle = style_factory_create_style(style_factory(), widget);
  }

  if (axis->tick.astyle == NULL) {
    axis->tick.astyle = style_factory_create_style(style_factory(), widget);
  }

  if (axis->label.astyle == NULL) {
    axis->label.astyle = style_factory_create_style(style_factory(), widget);
  }

  if (axis->title.astyle == NULL) {
    axis->title.astyle = style_factory_create_style(style_factory(), widget);
  }

  return widget;
}

widget_t* axis_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, axis), NULL);

  return widget;
}

bool_t widget_is_axis(widget_t* widget) {
#ifdef WITH_WIDGET_TYPE_CHECK
  return WIDGET_IS_INSTANCE_OF(widget, axis);
#else
  return (widget != NULL && strstr(widget->vt->type, "_axis") != NULL);
#endif
}
