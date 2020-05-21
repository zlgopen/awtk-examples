#include "tkc/mem.h"
#include "tkc/utils.h"
#include "base/timer.h"
#include "base/ui_loader.h"
#include "window_anim_menu.h"
#include "widget_animators/widget_animator_scroll.h"

#define WIDGET_PROP_ANIMATING_TIME "animating_time"

static ret_t canvas_set_clip_rect_with_offset(canvas_t *c, rect_t *r,
                                              rect_t *save_r, int32_t ox,
                                              int32_t oy) {
#ifndef FRAGMENT_FRAME_BUFFER_SIZE
  rect_t rr = *r;
  vgcanvas_t *vg = canvas_get_vgcanvas(c);
  rr.x += ox;
  rr.y += oy;

  rr = rect_intersect(&rr, save_r);

  vgcanvas_clip_rect(vg, (float_t)rr.x, (float_t)rr.y, (float_t)rr.w, (float_t)rr.h);
  return canvas_set_clip_rect(c, &rr);

#else
  rect_t rr = *r;
  rect_t clip_r = {0, 0, 0, 0};
  rect_t dirty_r = {0, 0, 0, 0};
  rr.x += ox;
  rr.y += oy;
  dirty_r = rect_init(c->lcd->dirty_rect.x, c->lcd->dirty_rect.y,
                      c->lcd->dirty_rect.w, c->lcd->dirty_rect.h);

  clip_r = rect_intersect(&rr, save_r);
  clip_r = rect_intersect(&clip_r, &dirty_r);

  return canvas_set_clip_rect(c, &clip_r);
#endif
}

static ret_t window_anim_menu_on_paint_self(widget_t *widget, canvas_t *c) {
  (void)widget;
  (void)c;
  return RET_OK;
}

static ret_t window_anim_menu_on_scroll_done(void *ctx, event_t *e) {
  widget_t *widget = WIDGET(ctx);
  widget_t *prev_widget = NULL;
  window_anim_menu_t *window_anim_menu = WINDOW_ANIM_MENU(ctx);
  return_value_if_fail(widget != NULL && window_anim_menu != NULL,
                       RET_BAD_PARAMS);

  window_anim_menu->xoffset = 0;
  window_anim_menu->yoffset = 0;
  window_anim_menu->animating = FALSE;
  prev_widget = window_anim_menu->curr_widget;
  window_anim_menu->curr_widget = window_anim_menu->next_widget;
  window_anim_menu->next_widget = NULL;

  if (prev_widget != NULL) {
    if (window_anim_menu->on_dispose_widget_fun != NULL) {
      window_anim_menu->on_dispose_widget_fun(
          prev_widget, window_anim_menu->on_dispose_widget_fun_ctx);
    }
    widget_destroy(prev_widget);
  }
  widget_set_as_key_target(window_anim_menu->curr_widget);
  widget->dirty = FALSE;
  widget_invalidate(widget, NULL);

  return RET_OK;
}

static widget_t *window_anim_menu_find_target(widget_t *widget, xy_t x,
                                              xy_t y) {
  window_anim_menu_t *window_anim_menu = WINDOW_ANIM_MENU(widget);
  return_value_if_fail(widget != NULL && window_anim_menu != NULL, NULL);

  if (window_anim_menu->xoffset || window_anim_menu->yoffset) {
    return NULL;
  }

  return window_anim_menu->curr_widget;
}

static ret_t window_anim_menu_animating_move_horizontal(
    window_anim_menu_t *window_anim_menu, canvas_t *c, rect_t *save_r,
    widget_t *next_widget, bool_t is_left) {
  rect_t r;
  int32_t ox = c->ox;
  int32_t oy = c->oy;
  xy_t w = window_anim_menu->widget.w;
  xy_t h = window_anim_menu->widget.h;
  int32_t xoffset = tk_abs(window_anim_menu->xoffset);
  int32_t r_xoffset = window_anim_menu->is_move_back ? xoffset : w - xoffset;

  if (is_left) {
    r_xoffset *= -1;
  }

  r = rect_init(r_xoffset, 0, w, h);
  canvas_save(c);
  canvas_translate(c, r_xoffset, 0);
  canvas_set_clip_rect_with_offset(c, &r, save_r, ox, oy);
  widget_paint(next_widget, c);
  canvas_untranslate(c, r_xoffset, 0);
  canvas_restore(c);

  return RET_OK;
}

static ret_t
window_anim_menu_animating_move_vertical(window_anim_menu_t *window_anim_menu,
                                         canvas_t *c, rect_t *save_r,
                                         widget_t *next_widget, bool_t is_top) {
  rect_t r;
  int32_t ox = c->ox;
  int32_t oy = c->oy;
  xy_t w = window_anim_menu->widget.w;
  xy_t h = window_anim_menu->widget.h;
  int32_t yoffset = tk_abs(window_anim_menu->yoffset);
  int32_t r_yoffset = window_anim_menu->is_move_back ? yoffset : h - yoffset;

  if (is_top) {
    r_yoffset *= -1;
  }

  r = rect_init(0, r_yoffset, w, h);
  canvas_save(c);
  canvas_translate(c, 0, r_yoffset);
  canvas_set_clip_rect_with_offset(c, &r, save_r, ox, oy);
  widget_paint(next_widget, c);
  canvas_untranslate(c, 0, r_yoffset);
  canvas_restore(c);

  return RET_OK;
}

static ret_t window_anim_menu_on_paint_children(widget_t *widget, canvas_t *c) {
  rect_t save_r;
  widget_t *curr_widget = NULL;
  widget_t *next_widget = NULL;
  vgcanvas_t *vg = canvas_get_vgcanvas(c);
  window_anim_menu_t *window_anim_menu = WINDOW_ANIM_MENU(widget);
  return_value_if_fail(widget != NULL && window_anim_menu != NULL,
                       RET_BAD_PARAMS);

  if (window_anim_menu->is_move_back && window_anim_menu->next_widget != NULL) {
    curr_widget = window_anim_menu->next_widget;
    next_widget = window_anim_menu->curr_widget;
  } else {
    curr_widget = window_anim_menu->curr_widget;
    next_widget = window_anim_menu->next_widget;
  }

  if (curr_widget != NULL) {
    widget_paint(curr_widget, c);
  }

  if (next_widget != NULL) {
    canvas_get_clip_rect(c, &save_r);

    switch (window_anim_menu->animating_move_type) {
    case MENU_ANIMATING_MOVE_TYPE_LETF:
      window_anim_menu_animating_move_horizontal(window_anim_menu, c, &save_r,
                                                 next_widget, TRUE);
      break;
    case MENU_ANIMATING_MOVE_TYPE_RIGTH:
      window_anim_menu_animating_move_horizontal(window_anim_menu, c, &save_r,
                                                 next_widget, FALSE);
      break;
    case MENU_ANIMATING_MOVE_TYPE_TOP:
      window_anim_menu_animating_move_vertical(window_anim_menu, c, &save_r,
                                               next_widget, TRUE);
      break;
    case MENU_ANIMATING_MOVE_TYPE_BOTTOM:
      window_anim_menu_animating_move_vertical(window_anim_menu, c, &save_r,
                                               next_widget, FALSE);
      break;
    default:
      break;
    }

    vgcanvas_clip_rect(vg, (float_t)save_r.x, (float_t)save_r.y,
                       (float_t)save_r.w, (float_t)save_r.h);
    canvas_set_clip_rect(c, &save_r);
  }

  return RET_OK;
}

static ret_t window_anim_menu_set_prop(widget_t *widget, const char *name,
                                       const value_t *v) {
  window_anim_menu_t *window_anim_menu = WINDOW_ANIM_MENU(widget);
  return_value_if_fail(widget != NULL && name != NULL && v != NULL,
                       RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_ANIMATING_TIME)) {
    return window_anim_menu_set_animating_time(widget, value_uint32(v));
  } else if (tk_str_eq(name, WIDGET_PROP_XOFFSET)) {
    window_anim_menu->xoffset = value_int(v);
    widget_invalidate(widget, NULL);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_YOFFSET)) {
    window_anim_menu->yoffset = value_int(v);
    widget_invalidate(widget, NULL);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t window_anim_menu_get_prop(widget_t *widget, const char *name,
                                       value_t *v) {
  window_anim_menu_t *window_anim_menu = WINDOW_ANIM_MENU(widget);
  return_value_if_fail(widget != NULL && name != NULL && v != NULL,
                       RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_ANIMATING_TIME)) {
    value_set_uint32(v, window_anim_menu->animating_time);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_XOFFSET)) {
    value_set_int(v, window_anim_menu->xoffset);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_YOFFSET)) {
    value_set_int(v, window_anim_menu->yoffset);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static const char *s_window_anim_menu_properties[] = {
    WIDGET_PROP_ANIMATING_TIME, NULL};
TK_DECL_VTABLE(window_anim_menu) = {
    .size = sizeof(window_anim_menu_t),
    .inputable = TRUE,
    .type = WIDGET_TYPE_WINDOW_ANIM_MENU,
    .only_active_child_visible = TRUE,
    .clone_properties = s_window_anim_menu_properties,
    .persistent_properties = s_window_anim_menu_properties,
    .parent = TK_PARENT_VTABLE(widget),
    .create = window_anim_menu_create,
    .get_prop = window_anim_menu_get_prop,
    .set_prop = window_anim_menu_set_prop,
    .find_target = window_anim_menu_find_target,
    .on_paint_children = window_anim_menu_on_paint_children,
    .on_paint_self = window_anim_menu_on_paint_self};

widget_t *window_anim_menu_create(widget_t *parent, xy_t x, xy_t y, wh_t w,
                                  wh_t h) {
  widget_t *widget =
      widget_create(parent, TK_REF_VTABLE(window_anim_menu), x, y, w, h);
  window_anim_menu_t *window_anim_menu = WINDOW_ANIM_MENU(widget);
  return_value_if_fail(window_anim_menu != NULL, NULL);

  window_anim_menu->is_move_back = FALSE;
  window_anim_menu->animating_time = TK_ANIMATING_TIME;
  window_anim_menu->animating_move_type = MENU_ANIMATING_MOVE_TYPE_TOP;

  return widget;
}

ret_t window_anim_menu_set_animating_time(widget_t *widget,
                                          uint32_t animating_time) {
  window_anim_menu_t *window_anim_menu = WINDOW_ANIM_MENU(widget);
  return_value_if_fail(window_anim_menu != NULL && animating_time > 0,
                       RET_BAD_PARAMS);

  window_anim_menu->animating_time = animating_time;
  return RET_OK;
}

#ifdef WATCH_ANIMATOR
static ret_t window_anim_menu_move_child(widget_t *widget) {
  widget_animator_t *animator = NULL;
  window_anim_menu_t *window_anim_menu = WINDOW_ANIM_MENU(widget);

  return_value_if_fail(window_anim_menu != NULL, RET_BAD_PARAMS);
  return_value_if_fail(!window_anim_menu->animating, RET_FAIL);

  animator = widget_animator_scroll_create(
      widget, window_anim_menu->animating_time, 0, EASING_LINEAR);

  if (window_anim_menu->animating_move_type == MENU_ANIMATING_MOVE_TYPE_TOP ||
      window_anim_menu->animating_move_type ==
          MENU_ANIMATING_MOVE_TYPE_BOTTOM) {
    widget_animator_scroll_set_params(animator, 0, 0, 0, widget->h);
  } else {
    widget_animator_scroll_set_params(animator, 0, 0, -widget->w, 0);
  }

  window_anim_menu->animating = TRUE;
  widget_animator_on(animator, EVT_ANIM_END, window_anim_menu_on_scroll_done,
                     window_anim_menu);
  widget_animator_start(animator);

  return RET_OK;
}

#else
static ret_t window_anim_menu_move_child(widget_t *widget) {
  window_anim_menu_t *window_anim_menu = WINDOW_ANIM_MENU(widget);
  if (window_anim_menu->animating_move_type == MENU_ANIMATING_MOVE_TYPE_TOP ||
      window_anim_menu->animating_move_type ==
          MENU_ANIMATING_MOVE_TYPE_BOTTOM) {
    window_anim_menu->yoffset = widget->h;
  } else {
    window_anim_menu->xoffset = widget->w;
  }

  window_anim_menu_on_scroll_done(window_anim_menu, NULL);
  return RET_OK;
}
#endif // WATCH_ANIMATOR

ret_t window_anim_menu_set_curr_widget(widget_t *widget,
                                       const char *widget_name) {
  widget_t *child_widget = NULL;
  window_anim_menu_t *window_anim_menu = WINDOW_ANIM_MENU(widget);
  return_value_if_fail(window_anim_menu != NULL, RET_BAD_PARAMS);

  child_widget = ui_loader_load_widget(widget_name);

  return_value_if_fail(child_widget != NULL, RET_OOM);

  if (window_anim_menu->on_load_widget_fun != NULL) {
    window_anim_menu->on_load_widget_fun(
        child_widget, window_anim_menu->on_load_widget_fun_ctx);
  }

  widget_add_child(widget, child_widget);
  window_anim_menu->curr_widget = child_widget;
  widget_invalidate(widget, NULL);

  return RET_OK;
}

ret_t window_anim_menu_play_anim_move_child_for_widget(
    widget_t *widget, widget_t *next_widget,
    animating_move_type_t animating_move_type, bool_t is_move_back) {
  window_anim_menu_t *window_anim_menu = WINDOW_ANIM_MENU(widget);
  return_value_if_fail(window_anim_menu != NULL, RET_BAD_PARAMS);

  window_anim_menu->is_move_back = is_move_back;
  window_anim_menu->next_widget = next_widget;
  window_anim_menu->animating_move_type = animating_move_type;

  return window_anim_menu_move_child(widget);
}

ret_t window_anim_menu_play_anim_move_child_for_load(
    widget_t *widget, const char *widget_name,
    animating_move_type_t animating_move_type, bool_t is_move_back) {
  widget_t *child_widget = NULL;
  window_anim_menu_t *window_anim_menu = WINDOW_ANIM_MENU(widget);
  return_value_if_fail(window_anim_menu != NULL, RET_BAD_PARAMS);
  return_value_if_fail(!window_anim_menu->animating, RET_BUSY);

  child_widget = ui_loader_load_widget(widget_name);

  if (child_widget != NULL) {
    if (window_anim_menu->on_load_widget_fun != NULL) {
      window_anim_menu->on_load_widget_fun(
          child_widget, window_anim_menu->on_load_widget_fun_ctx);
    }
    widget_add_child(widget, child_widget);
  } else {
    return RET_OOM;
  }

  return window_anim_menu_play_anim_move_child_for_widget(
      widget, child_widget, animating_move_type, is_move_back);
}

ret_t window_anim_menu_on_load_widget(widget_t *widget,
                                      event_load_widget_func_t on_event,
                                      void *ctx) {
  window_anim_menu_t *window_anim_menu = WINDOW_ANIM_MENU(widget);
  return_value_if_fail(window_anim_menu != NULL, RET_BAD_PARAMS);

  window_anim_menu->on_load_widget_fun_ctx = ctx;
  window_anim_menu->on_load_widget_fun = on_event;

  return RET_OK;
}

ret_t window_anim_menu_on_dispose_widget(widget_t *widget,
                                         event_dispose_widget_func_t on_event,
                                         void *ctx) {
  window_anim_menu_t *window_anim_menu = WINDOW_ANIM_MENU(widget);
  return_value_if_fail(window_anim_menu != NULL, RET_BAD_PARAMS);

  window_anim_menu->on_dispose_widget_fun_ctx = ctx;
  window_anim_menu->on_dispose_widget_fun = on_event;

  return RET_OK;
}

widget_t *window_anim_menu_cast(widget_t *widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, window_anim_menu), NULL);

  return widget;
}
