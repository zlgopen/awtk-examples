/**
 * File:   chart_utils.c
 * Author: AWTK Develop Team
 * Brief:  animate widget by change its value
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

#include "chart_utils.h"
#include "tkc/utils.h"
#include "base/style_const.h"
#include "base/style_mutable.h"
#include "base/style_factory.h"

color_t chart_utils_value_color(const value_t* v) {
  if (v->type == VALUE_TYPE_STRING) {
    return color_parse(value_str(v));
  } else {
    color_t color;
    color.color = value_uint32(v);
    return color;
  }
}

ret_t chart_utils_object_parse(chart_utils_on_object_parse_t on_parse, void* ctx,
                               const char* object) {
  ENSURE(on_parse != NULL && object != NULL);

  char name[TK_NAME_LEN + 1];
  value_t v;
  const char* token = NULL;
  tokenizer_t tokenizer;

  tokenizer_init(&tokenizer, object, strlen(object), "{,:} ");

  while (tokenizer_has_more(&tokenizer)) {
    token = tokenizer_next(&tokenizer);

    if (tokenizer_has_more(&tokenizer)) {
      tk_strncpy(name, token, TK_NAME_LEN);
      value_set_str(&v, tokenizer_next(&tokenizer));
      on_parse(ctx, name, &v);
    }
  }

  tokenizer_deinit(&tokenizer);

  return RET_OK;
}

static ret_t _widget_get_window_theme(widget_t* widget, theme_t** win_theme,
                                      theme_t** default_theme) {
  value_t v;
  widget_t* win = widget_get_window(widget);

  return_value_if_fail(win != NULL, RET_BAD_PARAMS);

  if (widget_get_prop(win, WIDGET_PROP_THEME_OBJ, &v) == RET_OK) {
    *win_theme = (theme_t*)value_pointer(&v);
  }

  if (widget_get_prop(win, WIDGET_PROP_DEFAULT_THEME_OBJ, &v) == RET_OK) {
    *default_theme = (theme_t*)value_pointer(&v);
  }

  return RET_OK;
}

static bool_t _is_valid_style_name(const char* str) {
  return str != NULL && *str;
}

static const void* widget_subpart_get_const_style_data(widget_t* widget, const char* subpart,
                                                       const char* style_name) {
  const void* data = NULL;
  theme_t* win_theme = NULL;
  theme_t* default_theme = NULL;
  const char* state = widget_get_prop_str(widget, WIDGET_PROP_STATE_FOR_STYLE, widget->state);

  style_name = _is_valid_style_name(style_name) ? style_name : TK_DEFAULT_STYLE;

  return_value_if_fail(_widget_get_window_theme(widget, &win_theme, &default_theme) == RET_OK,
                       NULL);

  if (win_theme != NULL) {
    data = theme_find_style(win_theme, subpart, style_name, state);
  }

  if (data == NULL) {
    data = theme_find_style(default_theme, subpart, style_name, state);
  }

  return data;
}

static ret_t widget_subpart_style_const_notify_widget_state_changed(style_t* s, widget_t* widget,
                                                                    const char* subpart,
                                                                    const char* style_name) {
  style_const_t* style = (style_const_t*)s;

  style->data = widget_subpart_get_const_style_data(widget, subpart, style_name);

  return RET_OK;
}

static ret_t widget_subpart_style_mutable_notify_widget_state_changed(style_t* s, widget_t* widget,
                                                                      const char* subpart,
                                                                      const char* style_name) {
  style_mutable_t* style = STYLE_MUTABLE(s);

  style->widget = widget;
  return widget_subpart_style_const_notify_widget_state_changed(style->default_style, widget,
                                                                subpart, style_name);
}

ret_t widget_subpart_update_style(style_t* style, widget_t* widget, const char* subpart,
                                  const char* style_name) {
  return_value_if_fail(style != NULL && widget != NULL, RET_BAD_PARAMS);

  if (style->vt->is_mutable) {
    widget_subpart_style_mutable_notify_widget_state_changed(style, widget, subpart, style_name);
  } else {
    widget_subpart_style_const_notify_widget_state_changed(style, widget, subpart, style_name);
  }

  return RET_OK;
}

ret_t widget_subpart_use_style(widget_t* widget, const char* subpart, const char* value,
                               widget_subpart_set_style_name_t set_style_name,
                               widget_subpart_get_style_obj_t get_style_obj) {
  style_t** style;
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);
  return_value_if_fail(set_style_name != NULL && get_style_obj != NULL, RET_BAD_PARAMS);

  return_value_if_fail(set_style_name(widget, subpart, value) == RET_OK, RET_BAD_PARAMS);

  style = get_style_obj(widget, subpart);
  return_value_if_fail(style != NULL && *style != NULL, RET_BAD_PARAMS);

  widget_subpart_update_style((*style), widget, subpart, value);

  return widget_invalidate(widget, NULL);
}

static ret_t widget_subpart_ensure_style_mutable(widget_t* widget, style_t** style) {
  return_value_if_fail(widget != NULL && style != NULL, RET_BAD_PARAMS);

  if (*style == NULL) {
    *style = style_mutable_create(widget, NULL);
    return_value_if_fail(*style != NULL, RET_OOM);
  }

  if (!((*style)->vt->is_mutable)) {
    *style = style_mutable_create(widget, *style);
    return_value_if_fail(*style != NULL, RET_OOM);
  }

  return RET_OK;
}

ret_t widget_subpart_set_style(widget_t* widget, const char* subpart, const char* state_and_name,
                               const value_t* value, widget_subpart_get_style_obj_t get_style_obj) {
  char str[256];
  uint32_t len = 0;
  char* name = NULL;
  char* state = NULL;
  style_t** astyle;
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);
  return_value_if_fail(subpart != NULL && state_and_name != NULL && value != NULL, RET_BAD_PARAMS);
  return_value_if_fail(get_style_obj != NULL, RET_BAD_PARAMS);

  astyle = get_style_obj(widget, subpart);
  return_value_if_fail(astyle != NULL, RET_BAD_PARAMS);
  return_value_if_fail(widget_subpart_ensure_style_mutable(widget, astyle) == RET_OK,
                       RET_BAD_PARAMS);

  len = strlen(state_and_name);
  return_value_if_fail(len < sizeof(str), RET_BAD_PARAMS);

  memcpy(str, state_and_name, len);
  str[len] = '\0';

  name = strchr(str, ':');
  if (name != NULL) {
    *name++ = '\0';
    state = str;
  } else {
    name = str;
    state = WIDGET_STATE_NORMAL;
  }

  widget_invalidate(widget, NULL);

  if (strstr(name, "_color") != NULL && value->type == VALUE_TYPE_STRING) {
    value_t v;
    color_t c = color_parse(value_str(value));
    value_set_uint32(&v, c.color);

    return style_set(*astyle, state, name, &v);
  } else {
    return style_set(*astyle, state, name, value);
  }
}

ret_t widget_subpart_set_style_int(widget_t* widget, const char* subpart,
                                   const char* state_and_name, int32_t value,
                                   widget_subpart_get_style_obj_t get_style_obj) {
  value_t v;
  value_set_int(&v, value);
  return widget_subpart_set_style(widget, subpart, state_and_name, &v, get_style_obj);
}

ret_t widget_subpart_set_style_color(widget_t* widget, const char* subpart,
                                     const char* state_and_name, uint32_t value,
                                     widget_subpart_get_style_obj_t get_style_obj) {
  value_t v;
  value_set_int(&v, value);
  return widget_subpart_set_style(widget, subpart, state_and_name, &v, get_style_obj);
}

ret_t widget_subpart_set_style_str(widget_t* widget, const char* subpart,
                                   const char* state_and_name, const char* value,
                                   widget_subpart_get_style_obj_t get_style_obj) {
  value_t v;
  value_set_str(&v, value);
  return widget_subpart_set_style(widget, subpart, state_and_name, &v, get_style_obj);
}
