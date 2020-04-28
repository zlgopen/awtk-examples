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


  /* 裁剪子控件的话，需要注意保存和还原 canvas 和 vg 这两个画布，*/
  /* 因为子控件可能会修改任意一个画布的裁剪区或者其他的配置，有概率会导致其他的控件的绘图不正常 */
  /* 如果创建新的裁剪区的话，需要和之前的裁剪区做交集，让新的裁剪区必须在旧的裁剪区中 */
  canvas_save(c);
  canvas_get_clip_rect(c, &r_save);
  if (vg != NULL) {
    vgcanvas_save(vg);
    r_vg_save = rect_init(vg->clip_rect.x, vg->clip_rect.y, vg->clip_rect.w, vg->clip_rect.h);
  }

  r = rect_init(c->ox, c->oy, widget->w, widget->h);
  r = rect_intersect(&r, &r_save);
  canvas_set_clip_rect(c, &r);
  if (vg != NULL) {
    vgcanvas_clip_rect(vg, r.x, r.y, r.w, r.h);
  }

  widget_on_paint_children_default(widget, c);
  if (vg != NULL) {
    vgcanvas_clip_rect(vg, r_vg_save.x, r_vg_save.y, r_vg_save.w, r_vg_save.h);
    vgcanvas_restore(vg);
  }
  canvas_set_clip_rect(c, &r_save);
  canvas_restore(c);

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
