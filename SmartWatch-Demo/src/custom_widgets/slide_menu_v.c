/**
 * File:   slide_menu_v.h
 * Author: AWTK Develop Team
 * Brief:  slide_menu_v
 *
 * Copyright (c) 2018 - 2019  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2019-05-20 Xu ChaoZe <xuchaoze@zlg.cn> created
 *
 */

#include "tkc/mem.h"
#include "base/enums.h"
#include "tkc/utils.h"
#include "widgets/button.h"
#include "base/layout.h"
#include "base/widget_vtable.h"
#include "slide_menu_v.h"
#include "widget_animators/widget_animator_scroll.h"

#ifdef _WIN32
#pragma warning(disable : 4244)
#endif  // _WIN32

#define MAX_VISIBLE_NR 10

const char* s_slide_menu_v_properties[] = {WIDGET_PROP_VALUE, WIDGET_PROP_ALIGN_H,
                                           SLIDE_MENU_V_PROP_MIN_SCALE, NULL};

static ret_t slide_menu_v_set_xoffset(slide_menu_v_t* slide_menu_v, int32_t xoffset) {
  if (slide_menu_v->xoffset != xoffset) {
    slide_menu_v->xoffset = xoffset;
    widget_invalidate(WIDGET(slide_menu_v), NULL);
  }

  return RET_OK;
}

static ret_t slide_menu_v_set_yoffset(slide_menu_v_t* slide_menu_v, int32_t yoffset) {
  if (slide_menu_v->yoffset != yoffset) {
    slide_menu_v->yoffset = yoffset;
    widget_layout_children(WIDGET(slide_menu_v));
    widget_invalidate(WIDGET(slide_menu_v), NULL);
  }

  return RET_OK;
}

static int32_t slide_menu_v_get_item_height(slide_menu_v_t* slide_menu_v) {
  return (slide_menu_v->item_height > 0 ? slide_menu_v->item_height : WIDGET(slide_menu_v)->w);
}

static int32_t slide_menu_v_get_item_width(slide_menu_v_t* slide_menu_v) {
  return (slide_menu_v->item_width > 0 ? slide_menu_v->item_width : WIDGET(slide_menu_v)->w);
}

int32_t slide_menu_v_fix_index(widget_t* widget, int32_t index) {
  uint32_t nr = widget_count_children(widget);

  while (index < 0) {
    index += nr;
  }

  return (index % nr);
}

static widget_t* slide_menu_v_get_child(widget_t* widget, int32_t index) {
  widget_t** children = NULL;
  return_value_if_fail(widget != NULL, NULL);

  children = (widget_t**)(widget->children->elms);
  return_value_if_fail(children != NULL, NULL);

  return children[slide_menu_v_fix_index(widget, index)];
}

static widget_t* slide_menu_v_find_target(widget_t* widget, xy_t x, xy_t y) {
  widget_t* current = NULL;
  point_t p = {x, y};
  int32_t r = 0;
  int32_t b = 0;
  int32_t xx = 0;
  int32_t yy = 0;
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);
  return_value_if_fail(widget != NULL && slide_menu_v != NULL, NULL);

  if (widget->grab_widget != NULL) {
    return widget->grab_widget;
  }

  widget_to_local(widget, &p);
  current = slide_menu_v_get_child(widget, slide_menu_v->value);
  r = current->x + current->w;
  b = current->y + current->h;
  xx = p.x;
  yy = p.y;

  if (current->enable && xx >= current->x && yy >= current->y && xx <= r && yy <= b) {
    return current;
  }

  return NULL;
}

static int32_t slide_menu_v_get_visible_nr(widget_t* widget) {
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);
  int32_t item_height = slide_menu_v_get_item_height(slide_menu_v);
  int32_t small_size = slide_menu_v->min_scale * item_height + slide_menu_v->spacing;
  int32_t n = tk_roundi((float_t)(widget->h - item_height) / (float_t)(small_size)) + 1;

  n = tk_min(n, MAX_VISIBLE_NR);
  n = tk_min(n, widget_count_children(widget));

  return n;
}

static rect_t slide_menu_v_get_clip_r(widget_t* widget) {
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);
  int32_t item_height = slide_menu_v_get_item_height(slide_menu_v);
  int32_t spacing = slide_menu_v->spacing;
  int32_t y = 0;
  int32_t h = 0;
  int32_t w = widget->w;
  int32_t nr = slide_menu_v_get_visible_nr(widget);

  nr = tk_max(1, nr);
  if (nr > 3 && (nr % 2) == 0) {
    nr--;
    /*keep nr is odd*/
  }

  h = item_height + (item_height * slide_menu_v->min_scale + spacing) * (nr - 1);
  h = tk_min(widget->h, h);
  y = tk_roundi((widget->h - h) / 2.0f);

  return rect_init(0, y, w, h);
}

static ret_t slide_menu_v_paint_children(widget_t* widget, canvas_t* c) {
  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  int32_t top = c->oy + iter->y;
  int32_t bottom = top + iter->h;
  if (top >= (c->clip_bottom - 1) || bottom <= (c->clip_top + 1)) {
    iter->dirty = FALSE;
    continue;
  }
  widget_paint(iter, c);
  WIDGET_FOR_EACH_CHILD_END();

  return RET_OK;
}

static ret_t slide_menu_v_paint_mask(widget_t* widget, canvas_t* c, rect_t* r) {
  int32_t i = 0;
  int32_t y = 0;
  int32_t n = r->h / 2 + 2;
  int32_t cy = widget->h / 2;
  style_t* style = widget->astyle;
  color_t trans = color_init(0, 0, 0, 0);
  easing_func_t easing = easing_get(EASING_LINEAR);
  color_t mask_color = style_get_color(style, STYLE_ID_MASK_COLOR, trans);

  if (mask_color.rgba.a) {
    for (i = 0; i < n; i++) {
      y = cy - i;

      if (y >= 0) {
        mask_color.rgba.a = 0xff * easing((float_t)i / (float_t)n);
        canvas_set_stroke_color(c, mask_color);
        canvas_draw_hline(c, 0, y, widget->w);
      }
    }

    for (i = 0; i < n; i++) {
      y = cy + i;

      if (y < widget->h) {
        mask_color.rgba.a = 0xff * easing((float_t)i / (float_t)n);
        canvas_set_stroke_color(c, mask_color);
        canvas_draw_hline(c, 0, y, widget->w);
      }
    }
  }

  return RET_OK;
}

static ret_t slide_menu_v_on_paint_children(widget_t* widget, canvas_t* c) {
  if (slide_menu_v_get_visible_nr(widget) >= 1) {
    rect_t r;
    rect_t save_r;
    rect_t clip_r = slide_menu_v_get_clip_r(widget);

    clip_r.x += c->ox;
    clip_r.y += c->oy;
    canvas_get_clip_rect(c, &save_r);
    r = rect_intersect(&save_r, &clip_r);

    canvas_save(c);
    canvas_set_clip_rect(c, &r);
    slide_menu_v_paint_children(widget, c);
    canvas_set_clip_rect(c, &save_r);
    canvas_restore(c);

    slide_menu_v_paint_mask(widget, c, &clip_r);
  }

  return RET_OK;
}

static int32_t slide_menu_v_get_delta_index(widget_t* widget) {
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);
  return_value_if_fail(widget != NULL && slide_menu_v != NULL, 0);
  int32_t item_height = slide_menu_v_get_item_height(slide_menu_v);
  int32_t item_size = item_height + slide_menu_v->spacing;

  return tk_roundi((float_t)(slide_menu_v->yoffset - item_height / 2) / (float_t)(item_size));
}

static uint32_t slide_menu_v_get_visible_children(widget_t* widget,
                                                  widget_t* children[MAX_VISIBLE_NR]) {
  uint32_t i = 0;
  uint32_t curr = 0;
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);
  int32_t item_height = slide_menu_v_get_item_height(slide_menu_v);
  int32_t spacing = slide_menu_v->spacing;
  int32_t max_size = item_height + spacing;
  uint32_t nr = widget_count_children(widget);
  int32_t delta_index = slide_menu_v_get_delta_index(widget);
  int32_t index = slide_menu_v_fix_index(widget, slide_menu_v->value - delta_index);
  int32_t rounded_yoffset = delta_index * max_size;
  int32_t yoffset = slide_menu_v->yoffset - rounded_yoffset;

  for (i = 0; i < MAX_VISIBLE_NR; i++) {
    children[i] = NULL;
  }

  for (i = 0; i < nr; i++) {
    widget_get_child(widget, i)->visible = FALSE;
  }

  nr = slide_menu_v_get_visible_nr(widget);

  if (yoffset >= 0) {
    curr = MAX_VISIBLE_NR / 2;
  } else {
    curr = MAX_VISIBLE_NR / 2 - 1;
  }

  children[curr] = slide_menu_v_get_child(widget, index);
  for (i = 1; i <= (nr - 1) / 2; i++) {
    children[curr - i] = slide_menu_v_get_child(widget, index - i);
    children[curr + i] = slide_menu_v_get_child(widget, index + i);
  }

  if (nr % 2 == 0) {
    if (yoffset >= 0) {
      children[curr - i] = slide_menu_v_get_child(widget, index - i);
    } else {
      children[curr + i] = slide_menu_v_get_child(widget, index + i);
    }
  }

  for (i = 0; i < MAX_VISIBLE_NR; i++) {
    widget_t* iter = children[i];
    if (iter != NULL) {
      iter->visible = TRUE;
      /*log_debug("nr=%d %d %s\n", nr, i, iter->name);*/
    }
  }

  return curr;
}

static float_t slide_menu_v_calc_scale(slide_menu_v_t* slide_menu_v, int32_t i, int32_t curr,
                                       int32_t yoffset) {
  float_t scale = 0;
  int32_t item_size = slide_menu_v_get_item_height(slide_menu_v) + slide_menu_v->spacing;
  float_t min_scale = slide_menu_v->min_scale;

  yoffset = yoffset + item_size * (i - curr);
  if (tk_abs(yoffset) < item_size) {
    scale = 1 + tk_abs(yoffset) * (min_scale - 1) / item_size;
  } else {
    scale = min_scale;
  }

  return scale;
}

static int32_t slide_menu_v_calc_child_x(slide_menu_v_t* slide_menu_v, int32_t max_size,
                                         int32_t size) {
  int32_t x = 0;

  switch (slide_menu_v->align_h) {
    case ALIGN_H_LEFT: {
      x = 0;
      break;
    }
    case ALIGN_H_CENTER: {
      x = (max_size - size) / 2;
      break;
    }
    default: {
      x = max_size - size;
      break;
    }
  }

  return x;
}

static int32_t slide_menu_v_calc_child_x_offset(slide_menu_v_t* slide_menu_v, int32_t i,
                                                int32_t curr, int32_t xoffset, int32_t yoffset) {
  int32_t offset = 0;
  int32_t item_size = slide_menu_v_get_item_height(slide_menu_v) + slide_menu_v->spacing;
  float_t factor = (float_t)(tk_abs(yoffset)) / (float_t)(item_size);

  if (xoffset != 0) {
    if (yoffset >= 0) {
      if (i < curr) {
        offset = (curr - i - factor) * xoffset;
      } else {
        offset = (i - curr + factor) * xoffset;
      }
    } else {
      if (i <= curr) {
        offset = (curr - i + factor) * xoffset;
      } else {
        offset = (i - curr - factor) * xoffset;
      }
    }
  }

  return offset;
}

static ret_t slide_menu_v_do_layout_children(widget_t* widget) {
  int32_t i = 0;
  int32_t x = 0;
  int32_t y = 0;
  int32_t w = 0;
  int32_t h = 0;
  float_t scale = 0.0f;
  int32_t visible_nr = MAX_VISIBLE_NR;
  widget_t* children[MAX_VISIBLE_NR];
  widget_t* iter = NULL;
  uint32_t curr = slide_menu_v_get_visible_children(widget, children);
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);
  int32_t item_height = slide_menu_v_get_item_height(slide_menu_v);
  int32_t item_width = slide_menu_v_get_item_width(slide_menu_v);
  int32_t spacing = slide_menu_v->spacing;
  int32_t xoffset = slide_menu_v->xoffset;
  int32_t rounded_yoffset = slide_menu_v_get_delta_index(widget) * (item_height + spacing);
  int32_t yoffset = slide_menu_v->yoffset - rounded_yoffset;

  /*curr widget*/
  iter = children[curr];
  scale = slide_menu_v_calc_scale(slide_menu_v, curr, curr, yoffset);
  w = item_width * scale;
  h = item_height * scale;
  x = slide_menu_v_calc_child_x(slide_menu_v, widget->w, w);
  x += slide_menu_v_calc_child_x_offset(slide_menu_v, curr, curr, xoffset, yoffset);
  y = (widget->h - h) / 2 + yoffset;
  widget_move_resize(iter, x, y, w, h);

  /*top*/
  for (i = curr - 1; i >= 0; i--) {
    iter = children[i];
    if (iter == NULL) break;

    scale = slide_menu_v_calc_scale(slide_menu_v, i, curr, yoffset);
    w = item_width * scale;
    h = item_height * scale;
    x = slide_menu_v_calc_child_x(slide_menu_v, widget->w, w);
    x += slide_menu_v_calc_child_x_offset(slide_menu_v, i, curr, xoffset, yoffset);
    y = children[i + 1]->y - h - spacing;
    widget_move_resize(iter, x, y, w, h);
  }

  /*bottom*/
  for (i = curr + 1; i < visible_nr; i++) {
    iter = children[i];
    if (iter == NULL) break;

    scale = slide_menu_v_calc_scale(slide_menu_v, i, curr, yoffset);
    w = item_width * scale;
    h = item_height * scale;
    x = slide_menu_v_calc_child_x(slide_menu_v, widget->w, w);
    x += slide_menu_v_calc_child_x_offset(slide_menu_v, i, curr, xoffset, yoffset);
    y = children[i - 1]->y + children[i - 1]->h + spacing;
    widget_move_resize(iter, x, y, w, h);
  }

  return RET_OK;
}

static ret_t slide_menu_v_layout_children(widget_t* widget) {
  if (widget_count_children(widget) > 0) {
    slide_menu_v_do_layout_children(widget);
  }

  return RET_OK;
}

static ret_t slide_menu_v_get_prop(widget_t* widget, const char* name, value_t* v) {
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);

  if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    value_set_int(v, slide_menu_v->value);
    return RET_OK;
  } else if (tk_str_eq(name, SLIDE_MENU_V_PROP_MIN_SCALE)) {
    value_set_float(v, slide_menu_v->min_scale);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_ALIGN_H)) {
    value_set_int(v, slide_menu_v->align_h);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_YOFFSET)) {
    value_set_int(v, slide_menu_v->yoffset);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_XOFFSET)) {
    value_set_int(v, slide_menu_v->xoffset);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_ITEM_HEIGHT)) {
    value_set_int(v, slide_menu_v->item_height);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_ITEM_WIDTH)) {
    value_set_int(v, slide_menu_v->item_width);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_SPACING)) {
    value_set_int(v, slide_menu_v->spacing);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t slide_menu_v_set_prop(widget_t* widget, const char* name, const value_t* v) {
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);

  if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    slide_menu_v_set_value(widget, value_int(v));
    return RET_OK;
  } else if (tk_str_eq(name, SLIDE_MENU_V_PROP_MIN_SCALE)) {
    slide_menu_v_set_min_scale(widget, value_float(v));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_YOFFSET)) {
    slide_menu_v_set_yoffset(slide_menu_v, value_int(v));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_XOFFSET)) {
    slide_menu_v_set_xoffset(slide_menu_v, value_int(v));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_ITEM_HEIGHT)) {
    slide_menu_v_set_item_height(widget, value_int(v));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_ITEM_WIDTH)) {
    slide_menu_v_set_item_width(widget, value_int(v));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_SPACING)) {
    slide_menu_v_set_spacing(widget, value_int(v));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_ALIGN_H)) {
    if (v->type == VALUE_TYPE_STRING) {
      const key_type_value_t* kv = align_h_type_find(value_str(v));
      if (kv != NULL) {
        slide_menu_v_set_align_h(widget, (align_h_t)(kv->value));
      }
    } else {
      slide_menu_v_set_align_h(widget, (align_h_t)(value_int(v)));
    }
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t slide_menu_v_on_pointer_down(slide_menu_v_t* slide_menu_v, pointer_event_t* e) {
  velocity_t* v = &(slide_menu_v->velocity);

  slide_menu_v->ydown = e->y;

  velocity_reset(v);
  velocity_update(v, e->e.time, e->x, e->y);

  return RET_OK;
}

static ret_t slide_menu_v_on_pointer_move(slide_menu_v_t* slide_menu_v, pointer_event_t* e) {
  velocity_t* v = &(slide_menu_v->velocity);

  velocity_update(v, e->e.time, e->x, e->y);
  slide_menu_v_set_yoffset(slide_menu_v, e->y - slide_menu_v->ydown);

  return RET_OK;
}

static ret_t slide_menu_v_set_value_only(slide_menu_v_t* slide_menu_v, int32_t index) {
  widget_t* widget = WIDGET(slide_menu_v);

  if (index != slide_menu_v->value) {
    event_t e = event_init(EVT_VALUE_WILL_CHANGE, widget);
    widget_dispatch(widget, &e);

    slide_menu_v->value = index;

    e = event_init(EVT_VALUE_CHANGED, widget);
    widget_dispatch(widget, &e);
  }

  return RET_OK;
}

static ret_t slide_menu_v_on_scroll_done(void* ctx, event_t* e) {
  (void)e;
  int32_t index = 0;
  int32_t delta_index = 0;
  widget_t* widget = WIDGET(ctx);
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(ctx);
  return_value_if_fail(widget != NULL && slide_menu_v != NULL, RET_BAD_PARAMS);

  delta_index = slide_menu_v_get_delta_index(widget);
  index = slide_menu_v_fix_index(widget, slide_menu_v->value - delta_index);

  slide_menu_v->wa = NULL;
  slide_menu_v->yoffset = 0;
  slide_menu_v_set_value_only(slide_menu_v, index);

  return RET_REMOVE;
}

static ret_t slide_menu_v_scroll_to(widget_t* widget, int32_t yoffset_end) {
  int32_t yoffset = 0;
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);
  return_value_if_fail(slide_menu_v != NULL, RET_FAIL);

  yoffset = slide_menu_v->yoffset;
  if (yoffset == yoffset_end) {
    return RET_OK;
  }

  slide_menu_v->wa = widget_animator_scroll_create(widget, TK_ANIMATING_TIME, 0, EASING_SIN_INOUT);
  return_value_if_fail(slide_menu_v->wa != NULL, RET_OOM);

  widget_animator_scroll_set_params(slide_menu_v->wa, 0, yoffset, 0, yoffset_end);
  widget_animator_on(slide_menu_v->wa, EVT_ANIM_END, slide_menu_v_on_scroll_done, slide_menu_v);
  widget_animator_start(slide_menu_v->wa);

  return RET_OK;
}

static ret_t slide_menu_v_on_pointer_up(slide_menu_v_t* slide_menu_v, pointer_event_t* e) {
  int32_t yoffset_end = 0;
  point_t p = {e->x, e->y};
  velocity_t* v = &(slide_menu_v->velocity);
  widget_t* widget = WIDGET(slide_menu_v);
  int32_t nr = widget_count_children(widget);
  int32_t item_height = slide_menu_v_get_item_height(slide_menu_v);
  int32_t spacing = slide_menu_v->spacing;
  int32_t item_size = item_height + spacing;

  velocity_update(v, e->e.time, e->x, e->y);
  if (!slide_menu_v->dragged) {
    widget_to_local(widget, &p);
    if (nr > 1) {
      int32_t top = (widget->h - item_height) / 2;
      int32_t bottom = (widget->h + item_height) / 2;
      int32_t small_size = item_height * slide_menu_v->min_scale;

      if (p.y > bottom) {
        int32_t delta = (p.y - bottom) / (small_size + spacing) + 1;
        yoffset_end = -item_size * delta;
      } else if (p.y < top) {
        int32_t delta = (top - p.y) / (small_size + spacing) + 1;
        yoffset_end = item_size * delta;
      }
    }

    if (yoffset_end == 0) {
      widget_invalidate(WIDGET(slide_menu_v), NULL);
      widget_layout_children(WIDGET(slide_menu_v));

      return RET_OK;
    }
  } else {
    yoffset_end = slide_menu_v->yoffset + v->yv;
    yoffset_end = tk_roundi((float_t)yoffset_end / (float_t)(item_size)) * (item_size);
  }

  slide_menu_v_scroll_to(WIDGET(slide_menu_v), yoffset_end);

  return RET_OK;
}

static ret_t slide_menu_v_on_event(widget_t* widget, event_t* e) {
  ret_t ret = RET_OK;
  uint16_t type = e->type;
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);
  return_value_if_fail(widget != NULL && slide_menu_v != NULL, RET_BAD_PARAMS);

  if (slide_menu_v->wa != NULL) {
    return RET_STOP;
  }

  switch (type) {
    case EVT_POINTER_DOWN:
      slide_menu_v->dragged = FALSE;
      slide_menu_v->pressed = TRUE;
      widget_grab(widget->parent, widget);
      slide_menu_v_on_pointer_down(slide_menu_v, (pointer_event_t*)e);
      break;
    case EVT_POINTER_UP: {
      if (slide_menu_v->pressed) {
        slide_menu_v_on_pointer_up(slide_menu_v, (pointer_event_t*)e);
        ret = slide_menu_v->dragged ? RET_STOP : RET_OK;
        widget_ungrab(widget->parent, widget);
        slide_menu_v->pressed = FALSE;
        slide_menu_v->dragged = FALSE;
      }
      break;
    }
    case EVT_POINTER_DOWN_ABORT: {
      if (slide_menu_v->pressed) {
        slide_menu_v->yoffset = 0;
        slide_menu_v->pressed = FALSE;
        slide_menu_v->dragged = FALSE;
        widget_ungrab(widget->parent, widget);
        log_debug("slide menu: EVT_POINTER_DOWN_ABORT\n");
      }
      break;
    }
    case EVT_POINTER_MOVE: {
      pointer_event_t* evt = (pointer_event_t*)e;
      if (slide_menu_v->dragged) {
        slide_menu_v_on_pointer_move(slide_menu_v, evt);
      } else if (evt->pressed && slide_menu_v->pressed) {
        int32_t delta = evt->y - slide_menu_v->ydown;

        if (tk_abs(delta) >= TK_DRAG_THRESHOLD) {
          pointer_event_t abort;
          pointer_event_init(&abort, EVT_POINTER_DOWN_ABORT, widget, evt->x, evt->y);
          widget_dispatch_event_to_target_recursive(widget, (event_t*)(&abort));
          slide_menu_v->dragged = TRUE;
        }
      }
      ret = slide_menu_v->dragged ? RET_STOP : RET_OK;
      break;
    }
    default:
      break;
  }

  return ret;
}

TK_DECL_VTABLE(slide_menu_v) = {.size = sizeof(slide_menu_v_t),
                                .type = WIDGET_TYPE_SLIDE_MENU_V,
                                .clone_properties = s_slide_menu_v_properties,
                                .persistent_properties = s_slide_menu_v_properties,
                                .parent = TK_PARENT_VTABLE(widget),
                                .create = slide_menu_v_create,
                                .set_prop = slide_menu_v_set_prop,
                                .get_prop = slide_menu_v_get_prop,
                                .find_target = slide_menu_v_find_target,
                                .on_paint_children = slide_menu_v_on_paint_children,
                                .on_layout_children = slide_menu_v_layout_children,
                                .on_event = slide_menu_v_on_event};

widget_t* slide_menu_v_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(slide_menu_v), x, y, w, h);
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);
  return_value_if_fail(slide_menu_v != NULL, NULL);

  slide_menu_v->value = 1;
  slide_menu_v->min_scale = 0.8f;
  slide_menu_v->align_h = ALIGN_H_RIGHT;

  return widget;
}

ret_t slide_menu_v_set_value(widget_t* widget, uint32_t index) {
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);
  return_value_if_fail(slide_menu_v != NULL, RET_BAD_PARAMS);

  slide_menu_v_set_value_only(slide_menu_v, index);
  widget_layout_children(widget);

  return widget_invalidate(widget, NULL);
}

ret_t slide_menu_v_set_min_scale(widget_t* widget, float_t min_scale) {
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);
  return_value_if_fail(slide_menu_v != NULL, RET_BAD_PARAMS);

  slide_menu_v->min_scale = tk_max(0.5f, tk_min(min_scale, 1));

  return widget_invalidate(widget, NULL);
}

ret_t slide_menu_v_set_align_h(widget_t* widget, align_h_t align_h) {
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);
  return_value_if_fail(slide_menu_v != NULL, RET_BAD_PARAMS);

  slide_menu_v->align_h = align_h;

  return widget_invalidate(widget, NULL);
}

ret_t slide_menu_v_set_item_height(widget_t* widget, int32_t item_height) {
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);
  return_value_if_fail(slide_menu_v != NULL, RET_BAD_PARAMS);

  slide_menu_v->item_height = item_height;

  return widget_invalidate(widget, NULL);
}

ret_t slide_menu_v_set_item_width(widget_t* widget, int32_t item_width) {
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);
  return_value_if_fail(slide_menu_v != NULL, RET_BAD_PARAMS);

  slide_menu_v->item_width = item_width;

  return widget_invalidate(widget, NULL);
}

ret_t slide_menu_v_set_spacing(widget_t* widget, int32_t spacing) {
  slide_menu_v_t* slide_menu_v = SLIDE_MENU_V(widget);
  return_value_if_fail(slide_menu_v != NULL, RET_BAD_PARAMS);

  slide_menu_v->spacing = spacing;

  return widget_invalidate(widget, NULL);
}

widget_t* slide_menu_v_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, slide_menu_v), NULL);

  return widget;
}
