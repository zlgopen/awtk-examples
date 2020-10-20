#include "round_menu.h"
#include "tkc/mem.h"
#include "base/enums.h"
#include "tkc/utils.h"
#include "widgets/button.h"
#include "base/layout.h"
#include "base/widget_vtable.h"
#include "widget_animators/widget_animator_scroll.h"

const char *s_round_menu_properties[] = {
    WIDGET_PROP_VALUE, WIDGET_PROP_ROUND_MENU_ITEM_H,
    WIDGET_PROP_ROUND_MENU_INTERVAL_YOFFSET, NULL};

static ret_t round_menu_set_yoffset(round_menu_t *round_menu, int32_t yoffset) {
  if (round_menu->yoffset != yoffset) {
    round_menu->yoffset = yoffset;

    widget_layout_children(WIDGET(round_menu));
    widget_invalidate(WIDGET(round_menu), NULL);
  }

  return RET_OK;
}

int32_t round_menu_fix_index(widget_t *widget, int32_t index) {
  uint32_t nr = widget_count_children(widget);

  while (index < 0) {
    index += nr;
  }

  return (index % nr);
}

static widget_t *round_menu_get_child(widget_t *widget, int32_t index) {
  widget_t **children = NULL;
  return_value_if_fail(widget != NULL && widget->children, NULL);

  children = (widget_t **)(widget->children->elms);
  return_value_if_fail(children != NULL, NULL);

  return children[round_menu_fix_index(widget, index)];
}

static ret_t round_menu_on_paint_children(widget_t *widget, canvas_t *c) {
  uint32_t i = 0;
  round_menu_t *round_menu = ROUND_MENU(widget);
  if (widget_count_children(widget) >= 1) {
    rect_t save_r;
    vgcanvas_t *vg = canvas_get_vgcanvas(c);

    canvas_get_clip_rect(c, &save_r);

    canvas_save(c);

    for (; i < ROUND_MENU_CURR_WIDGET_LIST_NUMBER; i++) {
      widget_paint(round_menu->paint_widgets[i], c);
    }

    vgcanvas_clip_rect(vg, (float_t)save_r.x, (float_t)save_r.y,
                       (float_t)save_r.w, (float_t)save_r.h);
    canvas_set_clip_rect(c, &save_r);
    canvas_restore(c);
  }

  return RET_OK;
}

static uint32_t round_menu_get_curr_widget(round_menu_t *round_menu,
                                           int32_t *y_offset) {
  int32_t curr = round_menu->value;
  int32_t h = round_menu->interval_yoffset + round_menu->item_h;
  int32_t new_value = round_menu->yoffset / h;

  *y_offset = round_menu->yoffset % h;

  return curr - new_value;
}

static void round_menu_move_children(widget_t *widget, int32_t x, int32_t y,
                                     int32_t y_offset) {
  int32_t p_x = x;
  int32_t p_y = y + y_offset;
  if (y < p_y) {
    p_x += (y_offset * 3 / 5);
  } else if (y > p_y) {
    p_x -= (y_offset * 3 / 5);
  }

  widget_move(widget, p_x, p_y);
}

static ret_t round_menu_do_layout_children(widget_t *widget) {
  int32_t y_offset = 0;
  point_t center_p;
  round_menu_t *round_menu = ROUND_MENU(widget);
  uint32_t curr = round_menu_get_curr_widget(round_menu, &y_offset);

  int32_t h = round_menu->interval_yoffset + round_menu->item_h;

  widget_t *temp_widget = NULL;
  widget_t *next_widget = round_menu_get_child(widget, curr - 1);
  widget_t *curr_widget = round_menu_get_child(widget, curr);
  widget_t *prev_widget = round_menu_get_child(widget, curr + 1);

  center_p.x = widget->w / 2;
  center_p.y = widget->h / 2 - round_menu->item_h / 2;

  round_menu_move_children(next_widget, center_p.x - next_widget->w / 2,
                           center_p.y, y_offset - h);
  round_menu_move_children(curr_widget, center_p.x - curr_widget->w / 2,
                           center_p.y, y_offset);
  round_menu_move_children(prev_widget, center_p.x - prev_widget->w / 2,
                           center_p.y, y_offset + h);

  if (round_menu->yoffset > 0) {
    temp_widget = round_menu_get_child(widget, curr - 2);
    round_menu_move_children(temp_widget, center_p.x - next_widget->w / 2,
                             center_p.y, y_offset - h * 2);
  } else {
    temp_widget = round_menu_get_child(widget, curr + 2);
    round_menu_move_children(temp_widget, center_p.x - next_widget->w / 2,
                             center_p.y, y_offset + h * 2);
  }

  // printf("yoffset:%d, y_offset:%d, h:%d, center_p.y:%d \r\n",
  // round_menu->yoffset, y_offset, h, center_p.y);
  // printf("next_widget:%s:(%d, %d) \r\n", next_widget->style, next_widget->x,
  // next_widget->y);
  // printf("curr_widget:%s:(%d, %d) \r\n", curr_widget->style, curr_widget->x,
  // curr_widget->y);
  // printf("prev_widget:%s:(%d, %d) \r\n", prev_widget->style, prev_widget->x,
  // prev_widget->y);
  // printf("temp_widget:%s:(%d, %d) \r\n", temp_widget->style, temp_widget->x,
  // temp_widget->y);

  round_menu->paint_widgets[0] = curr_widget;
  round_menu->paint_widgets[1] = prev_widget;
  round_menu->paint_widgets[2] = next_widget;
  round_menu->paint_widgets[3] = temp_widget;

  return RET_OK;
}

static ret_t round_menu_layout_children(widget_t *widget) {
  if (widget_count_children(widget) > 0) {
    round_menu_do_layout_children(widget);
  }

  return RET_OK;
}

static ret_t round_menu_get_prop(widget_t *widget, const char *name,
                                 value_t *v) {
  round_menu_t *round_menu = ROUND_MENU(widget);
  return_value_if_fail(round_menu != NULL && widget != NULL, RET_BAD_PARAMS);
  if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    value_set_int(v, round_menu->value);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_ROUND_MENU_INTERVAL_YOFFSET)) {
    value_set_int(v, round_menu->interval_yoffset);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_ROUND_MENU_ITEM_H)) {
    value_set_int(v, round_menu->item_h);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_YOFFSET)) {
    value_set_int(v, round_menu->yoffset);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_XOFFSET)) {
    value_set_int(v, 0);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t round_menu_set_prop(widget_t *widget, const char *name,
                                 const value_t *v) {
  round_menu_t *round_menu = ROUND_MENU(widget);
  return_value_if_fail(round_menu != NULL && widget != NULL, RET_BAD_PARAMS);
  if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    round_menu_set_value(widget, value_int(v));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_ROUND_MENU_INTERVAL_YOFFSET)) {
    round_menu_set_interval_yoffset(widget, value_int(v));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_ROUND_MENU_ITEM_H)) {
    round_menu_set_item_h(widget, value_int(v));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_XOFFSET)) {
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_YOFFSET)) {
    round_menu_set_yoffset(round_menu, value_int(v));
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t round_menu_on_pointer_down(round_menu_t *round_menu,
                                        pointer_event_t *e) {
  velocity_t *v = &(round_menu->velocity);

  round_menu->ydown = e->y;

  velocity_reset(v);
  velocity_update(v, e->e.time, e->x, e->y);

  return RET_OK;
}

static ret_t round_menu_on_pointer_move(round_menu_t *round_menu,
                                        pointer_event_t *e) {
#ifndef WITHOUT_ANIMATOR
  velocity_t *v = &(round_menu->velocity);

  velocity_update(v, e->e.time, e->x, e->y);
  round_menu_set_yoffset(round_menu, e->y - round_menu->ydown);
#endif
  return RET_OK;
}

static ret_t round_menu_set_value_only(round_menu_t *round_menu,
                                       int32_t index) {
  widget_t *widget = WIDGET(round_menu);

  if (index != round_menu->value) {
    value_change_event_t evt;
    value_change_event_init(&evt, EVT_VALUE_WILL_CHANGE, widget);
    value_set_int32(&(evt.old_value), round_menu->value);
    value_set_int32(&(evt.new_value), index);
    if (widget_dispatch(widget, (event_t*)&evt) != RET_STOP) {
      round_menu->value = index;
      evt.e.type = EVT_VALUE_CHANGED;
      widget_dispatch(widget, (event_t*)&evt);
    }
  }

  return RET_OK;
}

static ret_t round_menu_on_scroll_done(void *ctx, event_t *e) {
  int32_t index = 0;
  int32_t delta_index = 0;
  widget_t *widget = WIDGET(ctx);
  round_menu_t *round_menu = ROUND_MENU(ctx);
  return_value_if_fail(widget != NULL && round_menu != NULL &&
                           widget->children != NULL,
                       RET_BAD_PARAMS);

  delta_index =
      tk_roundi((float_t)(round_menu->yoffset) /
                (float_t)(round_menu->interval_yoffset + round_menu->item_h));
  index = round_menu_fix_index(widget, round_menu->value - delta_index);

  round_menu->wa = NULL;
  round_menu->yoffset = 0;
  round_menu_set_value_only(round_menu, index);

  return RET_REMOVE;
}

#ifndef WITHOUT_ANIMATOR
static ret_t round_menu_scroll_to(widget_t *widget, int32_t yoffset_end) {
  int32_t yoffset = 0;
  round_menu_t *round_menu = ROUND_MENU(widget);
  return_value_if_fail(round_menu != NULL, RET_FAIL);

  yoffset = round_menu->yoffset;
  if (yoffset == yoffset_end && round_menu->wa != NULL) {
    return RET_OK;
  }

  round_menu->wa = widget_animator_scroll_create(widget, TK_ANIMATING_TIME, 0,
                                                 EASING_SIN_INOUT);
  return_value_if_fail(round_menu->wa != NULL, RET_OOM);

  widget_animator_scroll_set_params(round_menu->wa, 0, yoffset, 0, yoffset_end);
  widget_animator_on(round_menu->wa, EVT_ANIM_END, round_menu_on_scroll_done,
                     round_menu);
  widget_animator_start(round_menu->wa);

  return RET_OK;
}
#else
static ret_t round_menu_scroll_to(widget_t *widget, int32_t yoffset_end) {
  int32_t yoffset = 0;
  round_menu_t *round_menu = ROUND_MENU(widget);
  return_value_if_fail(round_menu != NULL, RET_FAIL);

  yoffset = round_menu->yoffset;
  if (yoffset == yoffset_end) {
    return RET_OK;
  }
  round_menu_set_yoffset(round_menu, yoffset_end);

  round_menu_on_scroll_done(round_menu, NULL);

  return RET_OK;
}
#endif

static ret_t round_menu_on_pointer_up(round_menu_t *round_menu,
                                      pointer_event_t *e) {
  int32_t i = 0;
  int32_t yoffset_end = 0;
  point_t p = {e->x, e->y};
  widget_t *widget = WIDGET(round_menu);
  velocity_t *v = &(round_menu->velocity);
  int32_t nr = widget_count_children(widget);
  int32_t center_p_y = widget->h / 2 - round_menu->item_h / 2;

  velocity_update(v, e->e.time, e->x, e->y);

  if (!round_menu->dragged) {
    widget_to_local(widget, &p);
    if (nr > 1) {
      for (; i < ROUND_MENU_CURR_WIDGET_LIST_NUMBER; i++) {
        int32_t top = round_menu->paint_widgets[i]->y;
        int32_t left = round_menu->paint_widgets[i]->x;
        int32_t rigth =
            round_menu->paint_widgets[i]->x + round_menu->paint_widgets[i]->w;
        int32_t bottom =
            round_menu->paint_widgets[i]->y + round_menu->paint_widgets[i]->h;

        if (left <= p.x && p.x <= rigth && top <= p.y && p.y <= bottom) {
          yoffset_end = center_p_y - top;
          break;
        }
      }
    }

    if (yoffset_end == 0) {
      widget_invalidate(widget, NULL);
      widget_layout_children(widget);

      return RET_OK;
    }
  } else {
#ifndef WITHOUT_ANIMATOR
    int32_t h = round_menu->interval_yoffset + round_menu->item_h;
    yoffset_end = (int32_t)(round_menu->yoffset + v->yv * 0.3f);
    yoffset_end = tk_roundi((float_t)yoffset_end / (float_t)(h)) * h;
#else
    int32_t y_offset = 0;
    widget_t *next_widget = NULL;
    uint32_t curr = round_menu_get_curr_widget(round_menu, &y_offset);

    if (v->yv > 0) {
      next_widget = round_menu_get_child(widget, curr - 1);
    } else {
      next_widget = round_menu_get_child(widget, curr + 1);
    }

    yoffset_end = center_p_y - next_widget->y;
#endif
  }
  round_menu_scroll_to(WIDGET(round_menu), yoffset_end);

  return RET_OK;
}

static ret_t round_menu_on_event(widget_t *widget, event_t *e) {
  ret_t ret = RET_OK;
  uint16_t type = e->type;
  round_menu_t *round_menu = ROUND_MENU(widget);
  return_value_if_fail(widget != NULL && round_menu != NULL, RET_BAD_PARAMS);

  if (round_menu->wa != NULL) {
    return RET_STOP;
  }

  switch (type) {
  case EVT_POINTER_DOWN:
    round_menu->dragged = FALSE;
    round_menu->pressed = TRUE;
    widget_grab(widget->parent, widget);
    round_menu_on_pointer_down(round_menu, (pointer_event_t *)e);
    break;
  case EVT_POINTER_UP: {
    if (round_menu->pressed) {
      round_menu_on_pointer_up(round_menu, (pointer_event_t *)e);
      ret = round_menu->dragged ? RET_STOP : RET_OK;
      widget_ungrab(widget->parent, widget);
      round_menu->pressed = FALSE;
      round_menu->dragged = FALSE;
    }
    break;
  }
  case EVT_POINTER_DOWN_ABORT: {
    if (round_menu->pressed) {
      round_menu->yoffset = 0;
      round_menu->pressed = FALSE;
      round_menu->dragged = FALSE;
      widget_ungrab(widget->parent, widget);
    }
    break;
  }
  case EVT_POINTER_MOVE: {
    pointer_event_t *evt = (pointer_event_t *)e;
    if (round_menu->dragged) {
      round_menu_on_pointer_move(round_menu, evt);
    } else if (evt->pressed && round_menu->pressed) {
      int32_t delta = evt->y - round_menu->ydown;

      if (tk_abs(delta) >= TK_DRAG_THRESHOLD) {
        pointer_event_t abort;
        pointer_event_init(&abort, EVT_POINTER_DOWN_ABORT, widget, evt->x,
                           evt->y);
        widget_dispatch_event_to_target_recursive(widget, (event_t *)(&abort));
        round_menu->dragged = TRUE;
      }
    }
    ret = round_menu->dragged ? RET_STOP : RET_OK;
    break;
  }
  default:
    break;
  }

  return ret;
}

static widget_t *round_menu_find_target(widget_t *widget, xy_t x, xy_t y) {
  point_t p = {x, y};
  widget_t *current = NULL;
  round_menu_t *round_menu = ROUND_MENU(widget);
  return_value_if_fail(widget != NULL && round_menu != NULL, NULL);

  if (widget->grab_widget != NULL) {
    return widget->grab_widget;
  }

  widget_to_local(widget, &p);
  current = round_menu_get_child(widget, round_menu->value);
  if (current != NULL) {
    int32_t top = current->y;
    int32_t left = current->x;
    int32_t rigth = current->x + current->w;
    int32_t bottom = current->y + current->h;

    if (left <= p.x && p.x <= rigth && top <= p.y && p.y <= bottom) {
      return current;
    }
  }

  return NULL;
}

TK_DECL_VTABLE(round_menu) = {.size = sizeof(round_menu_t),
                              .inputable = TRUE,
                              .type = WIDGET_TYPE_ROUND_MENU,
                              .clone_properties = s_round_menu_properties,
                              .persistent_properties = s_round_menu_properties,
                              .parent = TK_PARENT_VTABLE(widget),
                              .create = round_menu_create,
                              .set_prop = round_menu_set_prop,
                              .get_prop = round_menu_get_prop,
                              .find_target = round_menu_find_target,
                              .on_paint_children = round_menu_on_paint_children,
                              .on_layout_children = round_menu_layout_children,
                              .on_event = round_menu_on_event};

widget_t *round_menu_create(widget_t *parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t *widget =
      widget_create(parent, TK_REF_VTABLE(round_menu), x, y, w, h);
  round_menu_t *round_menu = ROUND_MENU(widget);
  return_value_if_fail(round_menu != NULL, NULL);

  round_menu->value = 1;
  round_menu->item_h = 0;
  round_menu->interval_yoffset = 0;
  return widget;
}

ret_t round_menu_set_value(widget_t *widget, uint32_t index) {
  round_menu_t *round_menu = ROUND_MENU(widget);
  return_value_if_fail(round_menu != NULL, RET_BAD_PARAMS);

  round_menu_set_value_only(round_menu, index);
  widget_layout_children(widget);

  return widget_invalidate(widget, NULL);
}

ret_t round_menu_set_interval_yoffset(widget_t *widget,
                                      int32_t interval_yoffset) {
  round_menu_t *round_menu = ROUND_MENU(widget);
  return_value_if_fail(round_menu != NULL, RET_BAD_PARAMS);

  round_menu->interval_yoffset = interval_yoffset;

  widget_layout_children(widget);

  return widget_invalidate(widget, NULL);
}

ret_t round_menu_set_item_h(widget_t *widget, int32_t item_h) {
  round_menu_t *round_menu = ROUND_MENU(widget);
  return_value_if_fail(round_menu != NULL, RET_BAD_PARAMS);

  round_menu->item_h = item_h;

  widget_layout_children(widget);

  return widget_invalidate(widget, NULL);
}

widget_t *round_menu_cast(widget_t *widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, round_menu), NULL);

  return widget;
}
