/**
 * File:   axis.h
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
#include "chart_utils.h"

#define LABEL_FOR_EACH_DATA_BEGIN(tlabel, iter, i)      \
  if (tlabel->data != NULL && tlabel->data->size > 0) { \
    int32_t i = 0;                                      \
    int32_t nr = tlabel->data->size;                    \
    wstr_t** children = (wstr_t**)(tlabel->data->elms); \
    for (i = 0; i < nr; i++) {                          \
      wstr_t* iter = children[i];

#define LABEL_FOR_EACH_DATA_BEGIN_R(tlabel, iter, i)    \
  if (tlabel->data != NULL && tlabel->data->size > 0) { \
    int32_t i = 0;                                      \
    int32_t nr = tlabel->data->size;                    \
    wstr_t** children = (wstr_t**)(tlabel->data->elms); \
    for (i = nr - 1; i >= 0; i--) {                     \
      wstr_t* iter = children[i];

#define LABEL_FOR_EACH_DATA_END() \
  }                               \
  }

static ret_t axis_on_paint_self(widget_t* widget, canvas_t* c) {
  axis_t* axis = AXIS(axis_cast(widget));
  return_value_if_fail(axis != NULL && axis->render != NULL, RET_BAD_PARAMS);

  return axis_render_on_paint(widget, c);
}

static ret_t axis_get_prop(widget_t* widget, const char* name, value_t* v) {
  axis_t* axis = AXIS(axis_cast(widget));
  return_value_if_fail(axis != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, AXIS_PROP_ORIENTATION)) {
    value_set_int(v, axis->orientation);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TYPE)) {
    value_set_int(v, axis->type);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_AT)) {
    value_set_int(v, axis->at);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_MIN)) {
    value_set_float(v, axis->label.val_min);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_MAX)) {
    value_set_float(v, axis->label.val_max);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_OFFSET)) {
    value_set_int(v, axis->offset);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_SPLIT_LINE_COLOR)) {
    value_set_uint32(v, axis->split_line.style.color.color);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_SPLIT_LINE_SHOW)) {
    value_set_bool(v, axis->split_line.show);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_LINE_COLOR)) {
    value_set_uint32(v, axis->line.style.color.color);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_SPLIT_LINE_SHOW)) {
    value_set_bool(v, axis->split_line.show);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TICK_COLOR)) {
    value_set_uint32(v, axis->tick.style.color.color);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TICK_SHOW)) {
    value_set_bool(v, axis->tick.show);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_LABEL_FONT_SIZE)) {
    value_set_int(v, axis->label.font.size);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_LABEL_COLOR)) {
    value_set_uint32(v, axis->label.font.color.color);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_LABEL_SHOW)) {
    value_set_bool(v, axis->label.show);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TITLE_FONT_SZIE)) {
    value_set_int(v, axis->title.font.size);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TITLE_COLOR)) {
    value_set_uint32(v, axis->title.font.color.color);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TITLE_SHOW)) {
    value_set_bool(v, axis->title.show);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t axis_set_orientation(axis_t* axis, const value_t* v) {
  ENSURE(axis != NULL && v != NULL);

  if (v->type == VALUE_TYPE_STRING) {
    const char* str = value_str(v);
    if (tk_str_eq(str, "y")) {
      axis->orientation = AXIS_ORIENTATION_Y;
    } else {
      axis->orientation = AXIS_ORIENTATION_X;
    }
  } else {
    axis->orientation = (axis_orientation_t)value_int(v);
  }

  return RET_OK;
}

static ret_t axis_set_type(axis_t* axis, const value_t* v) {
  ENSURE(axis != NULL && v != NULL);

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
  ENSURE(axis != NULL && v != NULL);

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

static ret_t axis_set_data(axis_t* axis, const value_t* v) {
  ENSURE(axis != NULL && v != NULL);

  wstr_t* str;
  const char* value = value_str(v);
  const char* token = NULL;
  tokenizer_t tokenizer;
  tokenizer_init(&tokenizer, value, strlen(value), "[,]");
  if (axis->label.data == NULL) {
    axis->label.data = darray_create(4, NULL, NULL);
  }
  while (tokenizer_has_more(&tokenizer)) {
    token = tokenizer_next(&tokenizer);
    str = (wstr_t*)TKMEM_ALLOC(sizeof(wstr_t));
    wstr_init(str, 0);
    wstr_set_utf8(str, token);
    darray_push(axis->label.data, str);
  }
  tokenizer_deinit(&tokenizer);

  return RET_OK;
}

static void axis_parse_split_line(void* ctx, const char* name, const value_t* v) {
  axis_t* axis = AXIS(ctx);
  ENSURE(axis != NULL && name != NULL && v != NULL);
  if (tk_str_eq(name, "color")) {
    axis->split_line.style.color = chart_utils_value_color(v);
  } else if (tk_str_eq(name, "show")) {
    axis->split_line.show = value_bool(v);
  }
}

static void axis_parse_line(void* ctx, const char* name, const value_t* v) {
  axis_t* axis = AXIS(ctx);
  ENSURE(axis != NULL && name != NULL && v != NULL);
  if (tk_str_eq(name, "color")) {
    axis->line.style.color = chart_utils_value_color(v);
  } else if (tk_str_eq(name, "show")) {
    axis->line.show = value_bool(v);
  }
}

static void axis_parse_tick(void* ctx, const char* name, const value_t* v) {
  axis_t* axis = AXIS(ctx);
  ENSURE(axis != NULL && name != NULL && v != NULL);
  if (tk_str_eq(name, "color")) {
    axis->tick.style.color = chart_utils_value_color(v);
  } else if (tk_str_eq(name, "show")) {
    axis->tick.show = value_bool(v);
  }
}

static void axis_parse_label(void* ctx, const char* name, const value_t* v) {
  axis_t* axis = AXIS(ctx);
  ENSURE(axis != NULL && name != NULL && v != NULL);
  if (tk_str_eq(name, "color")) {
    axis->label.font.color = chart_utils_value_color(v);
  } else if (tk_str_eq(name, "font_size")) {
    axis->label.font.size = value_uint16(v);
  } else if (tk_str_eq(name, "show")) {
    axis->label.show = value_bool(v);
  }
}

static void axis_parse_title(void* ctx, const char* name, const value_t* v) {
  axis_t* axis = AXIS(ctx);
  ENSURE(axis != NULL && name != NULL && v != NULL);
  if (tk_str_eq(name, "color")) {
    axis->title.font.color = chart_utils_value_color(v);
  } else if (tk_str_eq(name, "font_size")) {
    axis->title.font.size = value_uint16(v);
  } else if (tk_str_eq(name, "show")) {
    axis->title.show = value_bool(v);
  }
}

static ret_t axis_set_prop(widget_t* widget, const char* name, const value_t* v) {
  axis_t* axis = AXIS(axis_cast(widget));
  return_value_if_fail(axis != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, AXIS_PROP_ORIENTATION)) {
    return axis_set_orientation(axis, v);
  } else if (tk_str_eq(name, AXIS_PROP_TYPE)) {
    return axis_set_type(axis, v);
  } else if (tk_str_eq(name, AXIS_PROP_AT)) {
    return axis_set_at(axis, v);
  } else if (tk_str_eq(name, WIDGET_PROP_MIN)) {
    axis->label.val_min = value_float(v);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_MAX)) {
    axis->label.val_max = value_float(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_OFFSET)) {
    axis->offset = value_int(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_DATA)) {
    return axis_set_data(axis, v);
  } else if (tk_str_eq(name, AXIS_PROP_SPLIT_LINE)) {
    return chart_utils_object_parse(axis_parse_split_line, axis, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_LINE)) {
    return chart_utils_object_parse(axis_parse_line, axis, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_TICK)) {
    return chart_utils_object_parse(axis_parse_tick, axis, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_LABEL)) {
    return chart_utils_object_parse(axis_parse_label, axis, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_TITLE)) {
    return chart_utils_object_parse(axis_parse_title, axis, value_str(v));
  } else if (tk_str_eq(name, AXIS_PROP_SPLIT_LINE_COLOR)) {
    axis->split_line.style.color = chart_utils_value_color(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_SPLIT_LINE_SHOW)) {
    axis->split_line.show = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_LINE_COLOR)) {
    axis->line.style.color = chart_utils_value_color(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_LINE_SHOW)) {
    axis->line.show = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TICK_COLOR)) {
    axis->tick.style.color = chart_utils_value_color(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TICK_SHOW)) {
    axis->tick.show = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_LABEL_FONT_SIZE)) {
    axis->label.font.size = value_uint16(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_LABEL_COLOR)) {
    axis->label.font.color = chart_utils_value_color(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_LABEL_SHOW)) {
    axis->label.show = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TITLE_FONT_SZIE)) {
    axis->title.font.size = value_uint16(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TITLE_COLOR)) {
    axis->title.font.color = chart_utils_value_color(v);
    return RET_OK;
  } else if (tk_str_eq(name, AXIS_PROP_TITLE_SHOW)) {
    axis->title.show = value_bool(v);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static void axis_label_destroy(axis_label_params_t* label) {
  LABEL_FOR_EACH_DATA_BEGIN(label, iter, i)
  if (iter) {
    wstr_reset(iter);
    TKMEM_FREE(iter);
  }
  LABEL_FOR_EACH_DATA_END();
  label->data->size = 0;
}

static ret_t axis_on_destroy(widget_t* widget) {
  axis_t* axis = AXIS(axis_cast(widget));
  return_value_if_fail(axis != NULL, RET_BAD_PARAMS);

  if (axis->label.data != NULL) {
    axis_label_destroy(&(axis->label));
    darray_destroy(axis->label.data);
  }

  axis_render_destroy(axis->render);

  TKMEM_FREE(axis->label.font.name);
  TKMEM_FREE(axis->label.formater);
  TKMEM_FREE(axis->title.font.name);
  TKMEM_FREE(axis->title.text);

  return RET_OK;
}

static const char* s_axis_properties[] = {
    WIDGET_PROP_MIN, WIDGET_PROP_MAX,  AXIS_PROP_ORIENTATION, AXIS_PROP_TYPE,
    AXIS_PROP_AT,    AXIS_PROP_OFFSET, AXIS_PROP_SPLIT_LINE,  AXIS_PROP_LINE,
    AXIS_PROP_TICK,  AXIS_PROP_LABEL,  AXIS_PROP_TITLE,       NULL};

static const widget_vtable_t s_axis_vtable = {.size = sizeof(axis_t),
                                              .type = WIDGET_TYPE_AXIS,
                                              .clone_properties = s_axis_properties,
                                              .persistent_properties = s_axis_properties,
                                              .create = axis_create,
                                              .on_paint_self = axis_on_paint_self,
                                              .set_prop = axis_set_prop,
                                              .get_prop = axis_get_prop,
                                              .on_destroy = axis_on_destroy};

widget_t* axis_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  axis_t* axis = TKMEM_ZALLOC(axis_t);
  widget_t* widget = WIDGET(axis);
  return_value_if_fail(axis != NULL, NULL);

  widget_init(widget, parent, &s_axis_vtable, x, y, w, h);

  axis->label.show = TRUE;
  axis->label.font.color = color_init(0, 0, 0, 0xff);
  axis->label.font.size = 18;
  axis->tick.show = TRUE;
  axis->tick.style.color = color_init(0, 0, 0, 0xff);
  axis->line.show = TRUE;
  axis->line.style.color = color_init(0, 0, 0, 0xff);
  axis->split_line.show = TRUE;
  axis->split_line.style.color = color_init(0x80, 0x80, 0x80, 0xff);

  return widget;
}
widget_t* axis_cast(widget_t* widget) {
  return_value_if_fail(widget != NULL && widget->vt == &s_axis_vtable, NULL);

  return widget;
}

float_t axis_measure_label(widget_t* widget, canvas_t* c) {
  float_t ret = 0.0;
  axis_t* axis = AXIS(axis_cast(widget));
  return_value_if_fail(axis != NULL, ret);

  axis_label_params_t* params = &(axis->label);
  LABEL_FOR_EACH_DATA_BEGIN(params, iter, i)
  if (iter) {
    canvas_set_font(c, params->font.name ? params->font.name : TK_DEFAULT_FONT, params->font.size);
    ret = tk_max(ret, canvas_measure_text(c, iter->str, iter->size));
  }
  LABEL_FOR_EACH_DATA_END();

  return ret;
}
