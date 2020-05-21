/**
 * File:   axis_p.c
 * Author: AWTK Develop Team
 * Brief:  axis private
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

#include "axis_p.h"
#include "tkc/utils.h"
#include "tkc/wstr.h"
#include "base/style.h"
#include "series.h"

void axis_p_parse_line_params(void* ctx, const char* name, const value_t* v) {
  axis_t* axis = AXIS(ctx);
  ENSURE(axis != NULL && name != NULL && v != NULL);

  if (tk_str_eq(name, "style")) {
    axis_subpart_use_style(WIDGET(axis), AXIS_SUBPART_LINE, value_str(v));
  } else if (tk_str_eq(name, "show")) {
    axis->line.show = value_bool(v);
  } else if (tk_str_eq(name, "lengthen")) {
    axis->line.lengthen = value_uint32(v);
  }
}

void axis_p_parse_split_line_params(void* ctx, const char* name, const value_t* v) {
  axis_t* axis = AXIS(ctx);
  ENSURE(axis != NULL && name != NULL && v != NULL);

  if (tk_str_eq(name, "style")) {
    axis_subpart_use_style(WIDGET(axis), AXIS_SUBPART_SPLIT_LINE, value_str(v));
  } else if (tk_str_eq(name, "show")) {
    axis->split_line.show = value_bool(v);
  }
}

void axis_p_parse_tick_params(void* ctx, const char* name, const value_t* v) {
  axis_t* axis = AXIS(ctx);
  ENSURE(axis != NULL && name != NULL && v != NULL);

  if (tk_str_eq(name, "style")) {
    axis_subpart_use_style(WIDGET(axis), AXIS_SUBPART_TICK, value_str(v));
  } else if (tk_str_eq(name, "show")) {
    axis->tick.show = value_bool(v);
  } else if (tk_str_eq(name, "inside")) {
    axis->tick.inside = value_bool(v);
  } else if (tk_str_eq(name, "align_with_label")) {
    axis->tick.align_with_label = value_bool(v);
  }
}

void axis_p_parse_label_params(void* ctx, const char* name, const value_t* v) {
  axis_t* axis = AXIS(ctx);
  ENSURE(axis != NULL && name != NULL && v != NULL);

  if (tk_str_eq(name, "style")) {
    axis_subpart_use_style(WIDGET(axis), AXIS_SUBPART_LABEL, value_str(v));
  } else if (tk_str_eq(name, "show")) {
    axis->label.show = value_bool(v);
  } else if (tk_str_eq(name, "inside")) {
    axis->label.inside = value_bool(v);
  }
}

void axis_p_parse_title_params(void* ctx, const char* name, const value_t* v) {
  axis_t* axis = AXIS(ctx);
  ENSURE(axis != NULL && name != NULL && v != NULL);

  if (tk_str_eq(name, "style")) {
    axis_subpart_use_style(WIDGET(axis), AXIS_SUBPART_TITLE, value_str(v));
  } else if (tk_str_eq(name, "show")) {
    axis->title.show = value_bool(v);
  } else if (tk_str_eq(name, "text")) {
    wstr_from_value(&(axis->title.text), v);
  }
}

static ret_t axis_set_type(axis_t* axis, const value_t* v) {
  if (v->type == VALUE_TYPE_STRING) {
    const char* str = value_str(v);
    if (tk_str_eq(str, "value")) {
      axis->type = AXIS_TYPE_VALUE;
    } else if (tk_str_eq(str, "category")) {
      axis->type = AXIS_TYPE_CATEGORY;
    }
  } else {
    axis->type = (axis_type_t)value_int(v);
  }

  return RET_OK;
}

static ret_t axis_set_at(axis_t* axis, const value_t* v) {
  if (v->type == VALUE_TYPE_STRING) {
    const char* str = value_str(v);
    if (tk_str_eq(str, "top")) {
      axis->at = AXIS_AT_TOP;
    } else if (tk_str_eq(str, "bottom")) {
      axis->at = AXIS_AT_BOTTOM;
    } else if (tk_str_eq(str, "left")) {
      axis->at = AXIS_AT_LEFT;
    } else if (tk_str_eq(str, "right")) {
      axis->at = AXIS_AT_RIGHT;
    }
  } else {
    axis->at = (axis_at_type_t)value_int(v);
  }

  return RET_OK;
}

static ret_t axis_calc_series_rect(widget_t* widget, rect_t* r) {
  style_t* style;
  int32_t margin = 0;
  int32_t margin_left = 0;
  int32_t margin_right = 0;
  int32_t margin_top = 0;
  int32_t margin_bottom = 0;
  return_value_if_fail(widget != NULL && r != NULL, RET_BAD_PARAMS);

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

ret_t axis_set_offset(axis_t* axis, const char* v) {
  assert(axis != NULL && v != NULL);

  axis->offset = tk_atof(v);
  axis->offset_percent = strchr(v, '%') == NULL ? FALSE : TRUE;

  return RET_OK;
}

ret_t axis_get_offset(axis_t* axis, value_t* v) {
  assert(axis != NULL && v != NULL);

  if (!axis->offset_percent) {
    value_set_float(v, axis->offset);
  } else {
    rect_t r = rect_init(0, 0, 0, 0);
    axis_calc_series_rect(WIDGET(axis)->parent, &r);
    if (tk_str_eq(widget_get_type(WIDGET(axis)), WIDGET_TYPE_X_AXIS)) {
      value_set_float(v, axis->offset * r.h / 100);
    } else {
      value_set_float(v, axis->offset * r.w / 100);
    }
  }

  return RET_OK;
}

ret_t axis_p_get_prop(widget_t* widget, const char* name, value_t* v) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, AXIS_PROP_TYPE)) {
    value_set_int(v, axis->type);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_AT)) {
    value_set_int(v, axis->at);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_MIN)) {
    value_set_float(v, axis->min);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_MAX)) {
    value_set_float(v, axis->max);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_OFFSET)) {
    return axis_get_offset(axis, v);
  } else if (tk_str_eq(name, AXIS_PROP_INVERSE)) {
    value_set_bool(v, axis->inverse);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_SPLIT_LINE_STYLE)) {
    value_set_str(v, axis->split_line.style);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_SPLIT_LINE_SHOW)) {
    value_set_bool(v, axis->split_line.show);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_LINE_STYLE)) {
    value_set_str(v, axis->line.style);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_LINE_SHOW)) {
    value_set_bool(v, axis->line.show);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TICK_STYLE)) {
    value_set_str(v, axis->tick.style);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TICK_SHOW)) {
    value_set_bool(v, axis->tick.show);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_LABEL_STYLE)) {
    value_set_str(v, axis->label.style);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_LABEL_SHOW)) {
    value_set_bool(v, axis->label.show);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TITLE_STYLE)) {
    value_set_str(v, axis->title.style);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TITLE_SHOW)) {
    value_set_bool(v, axis->title.show);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

ret_t axis_p_set_prop(widget_t* widget, const char* name, const value_t* v) {
  axis_t* axis = AXIS(widget);
  return_value_if_fail(axis != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, AXIS_PROP_TYPE)) {
    return axis_set_type(axis, v);
  } else if (tk_str_eq(name, AXIS_PROP_AT)) {
    return axis_set_at(axis, v);
  } else if (tk_str_eq(name, WIDGET_PROP_MIN)) {
    axis->min = value_float(v);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_MAX)) {
    axis->max = value_float(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_OFFSET)) {
    return axis_set_offset(axis, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_DATA)) {
    return axis_set_data(widget, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_INVERSE)) {
    axis->inverse = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_SPLIT_LINE)) {
    return chart_utils_object_parse(axis_p_parse_split_line_params, axis, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_SPLIT_LINE_STYLE)) {
    return axis_subpart_use_style(widget, AXIS_SUBPART_SPLIT_LINE, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_SPLIT_LINE_SHOW)) {
    axis->split_line.show = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_LINE)) {
    return chart_utils_object_parse(axis_p_parse_line_params, axis, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_LINE_STYLE)) {
    return axis_subpart_use_style(widget, AXIS_SUBPART_LINE, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_LINE_SHOW)) {
    axis->line.show = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TICK)) {
    return chart_utils_object_parse(axis_p_parse_tick_params, axis, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_TICK_STYLE)) {
    return axis_subpart_use_style(widget, AXIS_SUBPART_TICK, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_TICK_SHOW)) {
    axis->tick.show = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_LABEL)) {
    return chart_utils_object_parse(axis_p_parse_label_params, axis, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_LABEL_STYLE)) {
    return axis_subpart_use_style(widget, AXIS_SUBPART_LABEL, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_LABEL_SHOW)) {
    axis->label.show = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TITLE)) {
    return chart_utils_object_parse(axis_p_parse_title_params, axis, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_TITLE_STYLE)) {
    return axis_subpart_use_style(widget, AXIS_SUBPART_TITLE, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_TITLE_SHOW)) {
    axis->title.show = value_bool(v);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}
