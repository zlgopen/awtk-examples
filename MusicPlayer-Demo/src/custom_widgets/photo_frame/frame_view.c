/**
 * File:   window_record.c
 * Author: AWTK Develop Team
 * Brief:  Frame View
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
 * 2019-2-20  ZhongWei1 created
 *
 */

#include "frame_view.h"
#include "widget_animators/widget_animator_scroll.h"

#define MAX_VISIBLE_NR 10

const char* s_frame_view_properties[] = {WIDGET_PROP_VALUE, WIDGET_PROP_ALIGN_V,
                                         FRAME_VIEW_PROP_MIN_SCALE, NULL};

static ret_t frame_view_set_xoffset(frame_view_t* frame_view, int32_t xoffset) {
  if (frame_view->xoffset != xoffset) {
    frame_view->xoffset = xoffset;
    widget_layout_children(WIDGET(frame_view));
    widget_invalidate(WIDGET(frame_view), NULL);
  }

  return RET_OK;
}

int32_t frame_view_fix_index(widget_t* widget, int32_t index) {
  uint32_t nr = widget_count_children(widget);
  while (index < 0) {
    index += nr;
  }

  return (index % nr);
}

static widget_t* frame_view_get_child(widget_t* widget, int32_t index) {
  widget_t** children = (widget_t**)(widget->children->elms);

  return children[frame_view_fix_index(widget, index)];
}

static int32_t frame_view_get_visible_nr(widget_t* widget) {
  frame_view_t* frame_view = FRAME_VIEW(widget);
  int32_t n = (widget->w - widget->h) / (frame_view->min_scale * widget->h) + 1;

  n = tk_min(n, MAX_VISIBLE_NR);
  n = tk_min(n, widget_count_children(widget));

  return n;
}

static int32_t frame_view_get_delta_index(widget_t* widget) {
  frame_view_t* frame_view = FRAME_VIEW(widget);

  return tk_roundi((float_t)(frame_view->xoffset) / (float_t)(widget->h));
}

static uint32_t frame_view_get_visible_children(widget_t* widget,
                                                widget_t* children[MAX_VISIBLE_NR]) {
  uint32_t i = 0;
  uint32_t curr = 0;
  int32_t max_size = widget->h;
  uint32_t nr = widget_count_children(widget);
  frame_view_t* frame_view = FRAME_VIEW(widget);
  int32_t delta_index = frame_view_get_delta_index(widget);
  int32_t index = frame_view_fix_index(widget, frame_view->value - delta_index);
  int32_t rounded_xoffset = delta_index * max_size;
  int32_t xoffset = frame_view->xoffset - rounded_xoffset;

  for (i = 0; i < MAX_VISIBLE_NR; i++) {
    children[i] = NULL;
  }

  for (i = 0; i < nr; i++) {
    widget_get_child(widget, i)->visible = FALSE;
  }

  nr = frame_view_get_visible_nr(widget);

  if (xoffset >= 0) {
    curr = MAX_VISIBLE_NR / 2;
  } else {
    curr = MAX_VISIBLE_NR / 2 - 1;
  }

  children[curr] = frame_view_get_child(widget, index);
  for (i = 1; i <= (nr - 1) / 2; i++) {
    children[curr - i] = frame_view_get_child(widget, index - i);
    children[curr + i] = frame_view_get_child(widget, index + i);
  }

  if (nr % 2 == 0) {
    if (xoffset >= 0) {
      children[curr - i] = frame_view_get_child(widget, index - i);
    } else {
      children[curr + i] = frame_view_get_child(widget, index + i);
    }
  }

  for (i = 0; i < MAX_VISIBLE_NR; i++) {
    widget_t* iter = children[i];
    if (iter != NULL) {
      iter->visible = TRUE;
    }
  }

  return curr;
}

static int32_t frame_view_calc_child_y(align_v_t align_v, int32_t max_size, int32_t size) {
  int32_t y = 0;

  switch (align_v) {
    case ALIGN_V_TOP: {
      y = 0;
      break;
    }
    case ALIGN_V_MIDDLE: {
      y = (max_size - size) / 2;
      break;
    }
    default: {
      y = max_size - size;
      break;
    }
  }

  return y;
}

static ret_t frame_view_do_layout_children(widget_t* widget) {
  int32_t x = 0;
  int32_t y = 0;
  int32_t child_x = 0;
  int32_t child_y = 0;
  int32_t size = 0;
  widget_t* iter = NULL;
  widget_t** iter_child = NULL;
  int32_t max_size = widget->h;
  widget_t* children[MAX_VISIBLE_NR];
  frame_view_t* frame_view = FRAME_VIEW(widget);
  uint32_t curr = frame_view_get_visible_children(widget, children);
  int32_t rounded_xoffset = frame_view_get_delta_index(widget) * max_size;
  int32_t xoffset = frame_view->xoffset - rounded_xoffset;

  /*curr widget*/
  iter = children[curr];
  iter_child = (widget_t**)iter->children->elms;

  size = tk_min(WIDGET(frame_view)->w, WIDGET(frame_view)->h);
  x = (widget->w - max_size) / 2 + xoffset;
  y = frame_view_calc_child_y(frame_view->align_v, max_size, size);
  widget_move_resize(iter, x, y, size, size);

  uint32_t n_child = widget_count_children(iter);
  int i;
  for (i = 0; i < n_child; i++) {
    if (tk_str_eq(iter_child[i]->vt->type, WIDGET_TYPE_IMAGE)) {
      child_x = (size - iter_child[i]->w) / 2;
      child_y = (size - iter_child[i]->h) / 2;
      if (child_x > 0 && child_y > 0) {
        widget_move_resize(iter_child[i], child_x, child_y, iter_child[i]->w, iter_child[i]->h);
      }
    }
  }

  return RET_OK;
}

static ret_t frame_view_layout_children(widget_t* widget) {
  return_value_if_fail(widget->w >= 1.5 * widget->h, RET_BAD_PARAMS);

  if (widget_count_children(widget) > 0) {
    frame_view_do_layout_children(widget);
  }

  return RET_OK;
}

static ret_t frame_view_get_prop(widget_t* widget, const char* name, value_t* v) {
  frame_view_t* frame_view = FRAME_VIEW(widget);

  if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    value_set_int(v, frame_view->value);
    return RET_OK;
  } else if (tk_str_eq(name, FRAME_VIEW_PROP_MIN_SCALE)) {
    value_set_float(v, frame_view->min_scale);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_ALIGN_V)) {
    value_set_int(v, frame_view->align_v);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_YOFFSET)) {
    value_set_int(v, 0);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_XOFFSET)) {
    value_set_int(v, frame_view->xoffset);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t frame_view_set_prop(widget_t* widget, const char* name, const value_t* v) {
  frame_view_t* frame_view = FRAME_VIEW(widget);

  if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    frame_view_set_value(widget, value_int(v));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_XOFFSET)) {
    frame_view_set_xoffset(frame_view, value_int(v));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_YOFFSET)) {
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_ALIGN_V)) {
    if (v->type == VALUE_TYPE_STRING) {
      const key_type_value_t* kv = align_v_type_find(value_str(v));
      if (kv != NULL) {
        frame_view_set_align_v(widget, (align_v_t)(kv->value));
      }
    } else {
      frame_view_set_align_v(widget, (align_v_t)(value_int(v)));
    }
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t frame_view_set_value_only(frame_view_t* frame_view, int32_t index) {
  widget_t* widget = WIDGET(frame_view);

  if (index != frame_view->value) {
    event_t e = event_init(EVT_VALUE_WILL_CHANGE, widget);
    widget_dispatch(widget, &e);

    frame_view->value = index;
    e = event_init(EVT_VALUE_CHANGED, widget);
    widget_dispatch(widget, &e);
  }

  return RET_OK;
}

static ret_t frame_view_on_scroll_done(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  frame_view_t* frame_view = FRAME_VIEW(ctx);
  int32_t delta_index = frame_view_get_delta_index(widget);
  int32_t index = frame_view_fix_index(widget, frame_view->value - delta_index);
  frame_view->aim = NULL;
  frame_view->xoffset = 0;
  frame_view_set_value_only(frame_view, index);

  return RET_REMOVE;
}

static ret_t frame_view_scroll_to(widget_t* widget, int32_t xoffset_end) {
  int32_t xoffset = 0;
  return_value_if_fail(widget != NULL, RET_FAIL);
  frame_view_t* frame_view = FRAME_VIEW(widget);

  xoffset = frame_view->xoffset;
  if (xoffset == xoffset_end) {
    return RET_OK;
  }

  frame_view->aim = widget_animator_scroll_create(widget, TK_ANIMATING_TIME, 0, EASING_SIN_INOUT);
  return_value_if_fail(frame_view->aim != NULL, RET_OOM);

  widget_animator_scroll_set_params(frame_view->aim, xoffset, 0, xoffset_end, 0);
  widget_animator_on(frame_view->aim, EVT_ANIM_END,frame_view_on_scroll_done, frame_view);
  widget_animator_start(frame_view->aim);

  return RET_OK;
}

static const widget_vtable_t s_frame_view_vtable = {
    .size = sizeof(frame_view_t),
    .type = WIDGET_TYPE_FRAME_VIEW,
    .create = frame_view_create,
    .set_prop = frame_view_set_prop,
    .get_prop = frame_view_get_prop,
    .on_layout_children = frame_view_layout_children};

widget_t* frame_view_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, &s_frame_view_vtable, x, y, w, h);
  frame_view_t* frame_view = FRAME_VIEW(widget);
  return_value_if_fail(frame_view != NULL, NULL);

  frame_view->value = 0;
  frame_view->min_scale = 1.0f;
  frame_view->align_v = ALIGN_V_BOTTOM;
  return widget;
}

ret_t frame_view_set_value(widget_t* widget, uint32_t index) {
  frame_view_t* frame_view = FRAME_VIEW(widget);
  return_value_if_fail(frame_view != NULL, RET_BAD_PARAMS);

  frame_view_set_value_only(frame_view, index);
  widget_layout_children(widget);

  return widget_invalidate(widget, NULL);
}

ret_t frame_view_set_then(widget_t* widget, bool_t operate) {
  frame_view_t* frame_view = FRAME_VIEW(widget);
  return_value_if_fail(frame_view != NULL, RET_BAD_PARAMS);

  int32_t xoffset_end = 0;
  if (operate)
    xoffset_end = widget->h * -1;
  else
    xoffset_end = widget->h;
  frame_view_scroll_to(WIDGET(frame_view), xoffset_end);
  return RET_OK;
}

ret_t frame_view_set_align_v(widget_t* widget, align_v_t align_v) {
  frame_view_t* frame_view = FRAME_VIEW(widget);
  return_value_if_fail(frame_view != NULL, RET_BAD_PARAMS);

  frame_view->align_v = align_v;

  return widget_invalidate(widget, NULL);
}

widget_t* frame_view_cast(widget_t* widget) {
  return_value_if_fail(widget != NULL && widget->vt == &s_frame_view_vtable, NULL);

  return widget;
}
