/**
 * File:   cover_view.h
 * Author: AWTK Develop Team
 * Brief:  cover_view
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
 * 2018-04-05 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "cover_view.h"

static ret_t cover_view_on_paint_children(widget_t* widget, canvas_t* c) {
  cover_view_t* cover_view = COVER_VIEW(widget);
  vgcanvas_t* vg = canvas_get_vgcanvas(c);
  rect_t r;
  rect_t r_save;
  rect_t r_vg_save;
  return_value_if_fail(cover_view != NULL, RET_BAD_PARAMS);

  canvas_get_clip_rect(c, &r_save);
  r_vg_save = vg->clip_rect;

  r = rect_init(c->ox, c->oy, widget->w, widget->h);
  canvas_set_clip_rect(c, &r);
  vgcanvas_clip_rect(vg, c->ox, c->oy, widget->w, widget->h);

  widget_on_paint_children_default(widget, c);

  vgcanvas_clip_rect(vg, r_vg_save.x, r_vg_save.y, r_vg_save.w, r_vg_save.h);
  canvas_set_clip_rect(c, &r_save);

  return RET_OK;
}

TK_DECL_VTABLE(cover_view) = {.size = sizeof(cover_view_t),
                              .type = WIDGET_TYPE_VIEW,
                              .parent = TK_PARENT_VTABLE(widget),
                              .create = cover_view_create,
                              .on_paint_children = cover_view_on_paint_children};

widget_t* cover_view_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  return widget_create(parent, TK_REF_VTABLE(cover_view), x, y, w, h);
}

widget_t* cover_view_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, cover_view), NULL);

  return widget;
}
