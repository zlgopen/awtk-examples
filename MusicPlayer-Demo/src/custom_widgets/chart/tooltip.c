/**
 * File:   tooltip.c
 * Author: AWTK Develop Team
 * Brief:  tooltip
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

#include "tooltip.h"
#include "tkc/utils.h"
#include "tkc/wstr.h"
#include "base/style_factory.h"
#include "widget_animators/widget_animator_prop2.h"
#include "series_p.h"

// 坐标取整加0.5，防止线宽为1时显示2个像素
#define _VG_XY(v) (float_t)((xy_t)(v) + 0.5)
#define _VGCANVAS_ARC(vg, x, y, r, start, end, ccw) \
  vgcanvas_arc(vg, _VG_XY(x), _VG_XY(y), r, start, end, ccw)
#define _VGCANVAS_RECT(vg, x, y, w, h) vgcanvas_rect(vg, _VG_XY(x), _VG_XY(y), w, h)
#define _VGCANVAS_ROUNDED_RECT(vg, x, y, w, h, r) \
  vgcanvas_rounded_rect(vg, _VG_XY(x), _VG_XY(y), w, h, r)

void tooltip_parse_line_params(void* ctx, const char* name, const value_t* v) {
  tooltip_t* tooltip = TOOLTIP(ctx);
  ENSURE(tooltip != NULL && name != NULL && v != NULL);

  if (tk_str_eq(name, "style")) {
    tooltip_subpart_use_style(WIDGET(tooltip), TOOLTIP_SUBPART_LINE, value_str(v));
  } else if (tk_str_eq(name, "show")) {
    tooltip->line.show = value_bool(v);
  }
}

void tooltip_parse_symbol_params(void* ctx, const char* name, const value_t* v) {
  tooltip_t* tooltip = TOOLTIP(ctx);
  ENSURE(tooltip != NULL && name != NULL && v != NULL);

  if (tk_str_eq(name, "style")) {
    tooltip_subpart_use_style(WIDGET(tooltip), TOOLTIP_SUBPART_SYMBOL, value_str(v));
  } else if (tk_str_eq(name, "show")) {
    tooltip->symbol.show = value_bool(v);
  }
}

void tooltip_parse_tip_params(void* ctx, const char* name, const value_t* v) {
  tooltip_t* tooltip = TOOLTIP(ctx);
  ENSURE(tooltip != NULL && name != NULL && v != NULL);

  if (tk_str_eq(name, "style")) {
    tooltip_subpart_use_style(WIDGET(tooltip), TOOLTIP_SUBPART_TIP, value_str(v));
  } else if (tk_str_eq(name, "show")) {
    tooltip->tip.show = value_bool(v);
  }
}

static ret_t tooltip_get_prop(widget_t* widget, const char* name, value_t* v) {
  tooltip_t* tooltip = TOOLTIP(widget);
  return_value_if_fail(tooltip != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, TOOLTIP_PROP_DOWN_X)) {
    value_set_int(v, tooltip->down.x);
    return RET_OK;
  } else if (tk_str_eq(name, TOOLTIP_PROP_DOWN_Y)) {
    value_set_int(v, tooltip->down.y);
    return RET_OK;
  } else if (tk_str_eq(name, TOOLTIP_PROP_LINE_STYLE)) {
    value_set_str(v, tooltip->line.style);
    return RET_OK;
  } else if (tk_str_eq(name, TOOLTIP_PROP_LINE_SHOW)) {
    value_set_bool(v, tooltip->line.show);
    return RET_OK;
  } else if (tk_str_eq(name, TOOLTIP_PROP_SYMBOL_STYLE)) {
    value_set_str(v, tooltip->symbol.style);
    return RET_OK;
  } else if (tk_str_eq(name, TOOLTIP_PROP_SYMBOL_SHOW)) {
    value_set_bool(v, tooltip->symbol.show);
    return RET_OK;
  } else if (tk_str_eq(name, TOOLTIP_PROP_TIP_STYLE)) {
    value_set_str(v, tooltip->tip.style);
    return RET_OK;
  } else if (tk_str_eq(name, TOOLTIP_PROP_TIP_SHOW)) {
    value_set_bool(v, tooltip->tip.show);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t tooltip_set_prop(widget_t* widget, const char* name, const value_t* v) {
  tooltip_t* tooltip = TOOLTIP(widget);
  return_value_if_fail(tooltip != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, TOOLTIP_PROP_DOWN_X)) {
    tooltip->down.x = value_int(v);
    return RET_OK;
  } else if (tk_str_eq(name, TOOLTIP_PROP_DOWN_Y)) {
    tooltip->down.y = value_int(v);
    return RET_OK;
  } else if (tk_str_eq(name, TOOLTIP_PROP_LINE)) {
    return chart_utils_object_parse(tooltip_parse_line_params, tooltip, value_str(v));
  } else if (tk_str_eq(name, TOOLTIP_PROP_LINE_STYLE)) {
    return tooltip_subpart_use_style(widget, TOOLTIP_SUBPART_LINE, value_str(v));
    return RET_OK;
  } else if (tk_str_eq(name, TOOLTIP_PROP_LINE_SHOW)) {
    tooltip->line.show = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, TOOLTIP_PROP_SYMBOL)) {
    return chart_utils_object_parse(tooltip_parse_symbol_params, tooltip, value_str(v));
  } else if (tk_str_eq(name, TOOLTIP_PROP_SYMBOL_STYLE)) {
    return tooltip_subpart_use_style(widget, TOOLTIP_SUBPART_SYMBOL, value_str(v));
    return RET_OK;
  } else if (tk_str_eq(name, TOOLTIP_PROP_SYMBOL_SHOW)) {
    tooltip->symbol.show = value_bool(v);
    return RET_OK;
  } else if (tk_str_eq(name, TOOLTIP_PROP_TIP)) {
    return chart_utils_object_parse(tooltip_parse_tip_params, tooltip, value_str(v));
  } else if (tk_str_eq(name, TOOLTIP_PROP_TIP_STYLE)) {
    return tooltip_subpart_use_style(widget, TOOLTIP_SUBPART_TIP, value_str(v));
    return RET_OK;
  } else if (tk_str_eq(name, TOOLTIP_PROP_TIP_SHOW)) {
    tooltip->tip.show = value_bool(v);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static bool_t tooltip_is_in_series(widget_t* widget) {
  point_t p;
  tooltip_t* tooltip = TOOLTIP(widget);
  return_value_if_fail(tooltip != NULL, FALSE);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget->parent, iter, i)
  if (widget_is_series(iter)) {
    p = tooltip->down;
    widget_to_global(widget, &p);
    if (series_is_point_in(iter, p.x, p.y, FALSE)) {
      return TRUE;
    }
  }
  WIDGET_FOR_EACH_CHILD_END()

  return FALSE;
}

static bool_t tooltip_is_series_vertical(widget_t* widget) {
  tooltip_t* tooltip = TOOLTIP(widget);
  return_value_if_fail(tooltip != NULL, FALSE);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget->parent, iter, i)
  if (widget_is_series(iter)) {
    if (series_p_is_vertical(iter)) {
      return TRUE;
    }
  }
  WIDGET_FOR_EACH_CHILD_END()

  return FALSE;
}

ret_t tooltip_draw_line(widget_t* widget, canvas_t* c, bool_t vertical) {
  color_t trans = color_init(0, 0, 0, 0);
  color_t color;
  style_t* style;
  bitmap_t img;
  const char* image_name;
  tooltip_t* tooltip = TOOLTIP(widget);
  return_value_if_fail(tooltip != NULL && c != NULL, RET_BAD_PARAMS);

  style = tooltip->line.astyle;
  return_value_if_fail(style != NULL, RET_BAD_PARAMS);

  color = style_get_color(style, STYLE_ID_FG_COLOR, trans);
  image_name = style_get_str(style, STYLE_ID_FG_IMAGE, NULL);

  if (color.rgba.a) {
    canvas_set_stroke_color(c, color);

    if (vertical) {
      canvas_draw_vline(c, tooltip->down.x, 0, widget->h);
    } else {
      canvas_draw_hline(c, 0, tooltip->down.y, widget->w);
    }
  }

  if (image_name != NULL) {
    if (widget_load_image(widget, image_name, &img) == RET_OK) {
      const char* draw_type_key = STYLE_ID_FG_IMAGE_DRAW_TYPE;
      image_draw_type_t draw_type =
          (image_draw_type_t)style_get_int(style, draw_type_key, IMAGE_DRAW_PATCH3_X);
      rect_t r;
      if (vertical) {
        r = rect_init(tooltip->down.x, 0, 1, widget->h);
      } else {
        r = rect_init(0, tooltip->down.y, widget->w, 1);
      }
      canvas_draw_image_ex(c, &img, draw_type, &r);
    }
  }

  return RET_OK;
}

ret_t tooltip_draw_symbol(widget_t* widget, canvas_t* c) {
  point_t p;
  int32_t index;
  color_t trans = color_init(0, 0, 0, 0);
  color_t fg_color, bd_color;
  float_t border_width;
  style_t* style;
  tooltip_t* tooltip = TOOLTIP(widget);
  vgcanvas_t* vg = canvas_get_vgcanvas(c);
  return_value_if_fail(tooltip != NULL && vg != NULL, RET_BAD_PARAMS);

  style = tooltip->symbol.astyle;
  return_value_if_fail(style != NULL, RET_BAD_PARAMS);

  fg_color = style_get_color(style, STYLE_ID_FG_COLOR, trans);
  bd_color = style_get_color(style, STYLE_ID_BORDER_COLOR, trans);
  border_width = style_get_int(style, STYLE_ID_BORDER_WIDTH, 1);

  if (fg_color.rgba.a || bd_color.rgba.a) {
    vgcanvas_save(vg);
    vgcanvas_translate(vg, c->ox, c->oy);
    vgcanvas_set_fill_color(vg, fg_color);
    vgcanvas_set_stroke_color(vg, bd_color);
    vgcanvas_set_line_width(vg, border_width);

    WIDGET_FOR_EACH_CHILD_BEGIN(widget->parent, iter, i)
    if (widget_is_series(iter)) {
      p = tooltip->down;
      widget_to_global(widget, &p);
      index = series_index_of_point_in(iter, p.x, p.y, FALSE);
      if (index >= 0 && index < series_count(iter) && series_to_local(iter, index, &p) == RET_OK) {
        widget_to_global(iter, &p);
        widget_to_local(widget, &p);

        if (p.x == tooltip->down.x || p.y == tooltip->down.y) {
          vgcanvas_begin_path(vg);
          _VGCANVAS_ARC(vg, p.x, p.y, 5, 0, 2 * M_PI, FALSE);

          if (fg_color.rgba.a) {
            vgcanvas_fill(vg);
          }

          if (bd_color.rgba.a) {
            vgcanvas_stroke(vg);
          }
        }
      }
    }
    WIDGET_FOR_EACH_CHILD_END()

    vgcanvas_restore(vg);
  }

  return RET_OK;
}

static uint32_t tooltip_calc_tip(widget_t* widget, canvas_t* c, int32_t spacer, int32_t margin_left,
                                 int32_t margin_right, int32_t margin_top, int32_t margin_bottom,
                                 uint16_t font_size, rect_t* r, wstr_t* str) {
  point_t p;
  point_t right_top = {0};
  wstr_t temp;
  int32_t index;
  uint32_t spacing = 10;
  uint32_t cnt = 0;
  tooltip_t* tooltip = TOOLTIP(widget);
  assert(tooltip != NULL && widget->parent != NULL && c != NULL && str != NULL);

  wstr_init(str, 0);
  wstr_init(&temp, 0);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget->parent, iter, i)
  if (widget_is_series(iter)) {
    p = tooltip->down;
    widget_to_global(widget, &p);
    index = series_index_of_point_in(iter, p.x, p.y, FALSE);
    if (index >= 0 && index < series_count(iter) && series_to_local(iter, index, &p) == RET_OK) {
      widget_to_global(iter, &p);
      widget_to_local(widget, &p);
      if (p.x == tooltip->down.x || p.y == tooltip->down.y) {
        if (cnt == 0) {
          right_top = p;
        } else {
          right_top.x = tk_max(p.x, right_top.x);
          right_top.y = tk_min(p.y, right_top.y);
        }

        if (series_get_tooltip(iter, index, &temp) == RET_OK) {
          wstr_append(str, temp.str);
          r->w = tk_max(r->w, canvas_measure_text(c, temp.str, temp.size));
        }
        wstr_append(str, L"\n");

        cnt++;
      }
    }
  }
  WIDGET_FOR_EACH_CHILD_END()

  wstr_reset(&temp);

  r->x = right_top.x + spacing;
  r->y = right_top.y + spacing;
  r->w = r->w + margin_left + margin_right;
  r->h = cnt * font_size + (cnt - 1) * spacer + margin_top + margin_bottom;

  if (r->x + r->w > widget->x + widget->w) {
    r->x = tk_min(right_top.x - r->w - spacing, widget->x + widget->w - r->w - 1);
  }

  if (r->x < 0) {
    r->x = 0;
  }

  if (r->y + r->h > widget->y + widget->h) {
    r->y = tk_min(right_top.y - r->h - spacing, widget->y + widget->h - r->h - 1);
  }

  if (r->y < 0) {
    r->y = 0;
  }

  return cnt;
}

ret_t tooltip_draw_tip(widget_t* widget, canvas_t* c) {
  rect_t r = rect_init(0, 0, 0, 0);
  wstr_t str;
  uint32_t str_nr = 0;
  color_t trans = color_init(0, 0, 0, 0);
  int32_t spacer = 0;
  int32_t margin = 0;
  int32_t margin_left = 0;
  int32_t margin_right = 0;
  int32_t margin_top = 0;
  int32_t margin_bottom = 0;
  color_t bg_color, bd_color, text_color;
  float_t border_width;
  uint32_t radius;
  const char* font_name;
  uint16_t font_size = 0;
  style_t* style;
  tooltip_t* tooltip = TOOLTIP(widget);
  vgcanvas_t* vg = canvas_get_vgcanvas(c);
  return_value_if_fail(tooltip != NULL && vg != NULL, RET_BAD_PARAMS);

  style = tooltip->tip.astyle;
  return_value_if_fail(style != NULL, RET_BAD_PARAMS);

  spacer = style_get_int(style, STYLE_ID_SPACER, 2);
  margin = style_get_int(style, STYLE_ID_MARGIN, 0);
  margin_top = style_get_int(style, STYLE_ID_MARGIN_TOP, margin);
  margin_left = style_get_int(style, STYLE_ID_MARGIN_LEFT, margin);
  margin_right = style_get_int(style, STYLE_ID_MARGIN_RIGHT, margin);
  margin_bottom = style_get_int(style, STYLE_ID_MARGIN_BOTTOM, margin);
  bg_color = style_get_color(style, STYLE_ID_BG_COLOR, trans);
  bd_color = style_get_color(style, STYLE_ID_BORDER_COLOR, trans);
  border_width = style_get_int(style, STYLE_ID_BORDER_WIDTH, 1);
  radius = style_get_int(style, STYLE_ID_ROUND_RADIUS, 0);
  text_color = style_get_color(style, STYLE_ID_TEXT_COLOR, trans);
  font_name = style_get_str(style, STYLE_ID_FONT_NAME, NULL);
  font_size = style_get_int(style, STYLE_ID_FONT_SIZE, TK_DEFAULT_FONT_SIZE);

  if (bg_color.rgba.a || bd_color.rgba.a || text_color.rgba.a) {
    vgcanvas_t* vg = canvas_get_vgcanvas(c);
    bool_t use_vg = vg != NULL && ((radius > 3 || border_width > 1));

    str_nr = tooltip_calc_tip(widget, c, spacer, margin_left, margin_right, margin_top,
                              margin_bottom, font_size, &r, &str);

    canvas_set_fill_color(c, bg_color);
    canvas_set_stroke_color(c, bd_color);

    if (use_vg) {
      vgcanvas_save(vg);
      vgcanvas_translate(vg, c->ox, c->oy);
      vgcanvas_set_fill_color(vg, bg_color);
      vgcanvas_set_stroke_color(vg, bd_color);
      vgcanvas_set_line_width(vg, border_width);
      vgcanvas_begin_path(vg);
      if (radius > 3) {
        _VGCANVAS_ROUNDED_RECT(vg, r.x, r.y, r.w, r.h, radius);
      } else {
        _VGCANVAS_RECT(vg, r.x, r.y, r.w, r.h);
      }

      if (bg_color.rgba.a) {
        vgcanvas_fill(vg);
      }

      if (bd_color.rgba.a) {
        vgcanvas_stroke(vg);
      }

      vgcanvas_restore(vg);
    } else {
      if (bg_color.rgba.a) {
        canvas_fill_rect(c, r.x, r.y, r.w, r.h);
      }

      if (bd_color.rgba.a) {
        canvas_stroke_rect(c, r.x, r.y, r.w, r.h);
      }
    }

    if (str_nr > 0) {
      wchar_t* e = (wchar_t*)wcs_chr(str.str, L'\n');
      wchar_t* s = str.str;

      r.x += margin_left;
      r.w -= margin_left;
      r.y += margin_top;
      r.h -= margin_top;
      r.h = font_size;
      canvas_set_text_color(c, text_color);
      canvas_set_font(c, font_name, font_size);
      canvas_set_text_align(c, ALIGN_H_LEFT, ALIGN_V_MIDDLE);

      while (e != NULL) {
        *e = L'\0';
        canvas_draw_text_in_rect(c, s, wcs_len(s), &r);
        r.y += font_size + spacer;
        s = e + 1;
        e = (wchar_t*)wcs_chr(s, L'\n');
      }
    }
  }

  wstr_reset(&str);

  return RET_OK;
}

static ret_t tooltip_on_paint_self(widget_t* widget, canvas_t* c) {
  bool_t vertical = tooltip_is_series_vertical(widget);
  tooltip_t* tooltip = TOOLTIP(widget);
  return_value_if_fail(tooltip != NULL && widget->parent != NULL, RET_BAD_PARAMS);

  return_value_if_true(!tooltip->moved, RET_OK);
  return_value_if_true(!tooltip_is_in_series(widget), RET_OK);

  tooltip_draw_line(widget, c, vertical);
  if (tooltip->wa_move == NULL) {
    tooltip_draw_symbol(widget, c);
    tooltip_draw_tip(widget, c);
  }

  return RET_OK;
}

static ret_t tooltip_start_init_if_not_inited(widget_t* widget) {
  tooltip_t* tooltip = TOOLTIP(widget);
  return_value_if_fail(tooltip != NULL, RET_BAD_PARAMS);

  if (!tooltip->inited) {
    assert(tooltip->line.astyle != NULL);
    widget_subpart_update_style(tooltip->line.astyle, widget, TOOLTIP_SUBPART_LINE,
                                tooltip->line.style);

    assert(tooltip->symbol.astyle != NULL);
    widget_subpart_update_style(tooltip->symbol.astyle, widget, TOOLTIP_SUBPART_SYMBOL,
                                tooltip->symbol.style);

    assert(tooltip->tip.astyle != NULL);
    widget_subpart_update_style(tooltip->tip.astyle, widget, TOOLTIP_SUBPART_TIP,
                                tooltip->tip.style);

    tooltip->inited = TRUE;
  }

  return RET_OK;
}

ret_t tooltip_on_paint_begin(widget_t* widget, canvas_t* c) {
  (void)c;
  return tooltip_start_init_if_not_inited(widget);
}

static ret_t tooltip_on_move_animate_end(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  tooltip_t* tooltip = TOOLTIP(widget);
  return_value_if_fail(tooltip != NULL, RET_REMOVE);

  tooltip->wa_move = NULL;
  widget_invalidate(widget, NULL);
  return RET_REMOVE;
}

ret_t tooltip_move(widget_t* widget, xy_t x, xy_t y) {
  point_t p = {x, y};
  tooltip_t* tooltip = TOOLTIP(widget);
  return_value_if_fail(tooltip != NULL, RET_REMOVE);

  widget_to_local(widget, &p);

  if (p.x != tooltip->down.x || p.y != tooltip->down.y) {
    if (tooltip->wa_move != NULL) {
      widget_animator_destroy(tooltip->wa_move);
      tooltip->wa_move = NULL;
    }

    tooltip->wa_move = widget_animator_prop2_create(widget, TK_ANIMATING_TIME, 0, EASING_SIN_INOUT,
                                                    TOOLTIP_PROP_DOWN_X, TOOLTIP_PROP_DOWN_Y);
    widget_animator_on(tooltip->wa_move, EVT_ANIM_END, tooltip_on_move_animate_end, tooltip);
    widget_animator_prop2_set_params(tooltip->wa_move, tooltip->down.x, tooltip->down.y, p.x, p.y);
    widget_animator_start(tooltip->wa_move);
  }

  tooltip->moved = TRUE;

  return RET_OK;
}

static ret_t tooltip_subpart_set_style_name(widget_t* widget, const char* subpart,
                                            const char* value) {
  tooltip_t* tooltip = TOOLTIP(widget);
  return_value_if_fail(tooltip != NULL && subpart != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(subpart, TOOLTIP_SUBPART_LINE)) {
    tooltip->line.style = tk_str_copy(tooltip->line.style, value);
  } else if (tk_str_eq(subpart, TOOLTIP_SUBPART_SYMBOL)) {
    tooltip->symbol.style = tk_str_copy(tooltip->symbol.style, value);
  } else if (tk_str_eq(subpart, TOOLTIP_SUBPART_TIP)) {
    tooltip->tip.style = tk_str_copy(tooltip->tip.style, value);
  } else {
    return RET_NOT_FOUND;
  }

  return RET_OK;
}

static style_t** tooltip_subpart_get_style_obj(widget_t* widget, const char* subpart) {
  tooltip_t* tooltip = TOOLTIP(widget);
  return_value_if_fail(tooltip != NULL && subpart != NULL, NULL);

  if (tk_str_eq(subpart, TOOLTIP_SUBPART_LINE)) {
    return &(tooltip->line.astyle);
  } else if (tk_str_eq(subpart, TOOLTIP_SUBPART_SYMBOL)) {
    return &(tooltip->symbol.astyle);
  } else if (tk_str_eq(subpart, TOOLTIP_SUBPART_TIP)) {
    return &(tooltip->tip.astyle);
  } else {
    return NULL;
  }
}

ret_t tooltip_subpart_use_style(widget_t* widget, const char* subpart, const char* style) {
  return widget_subpart_use_style(widget, subpart, style, tooltip_subpart_set_style_name,
                                  tooltip_subpart_get_style_obj);
}

ret_t tooltip_subpart_set_style(widget_t* widget, const char* subpart, const char* state_and_name,
                                const value_t* value) {
  return widget_subpart_set_style(widget, subpart, state_and_name, value,
                                  tooltip_subpart_get_style_obj);
}

ret_t tooltip_subpart_set_style_int(widget_t* widget, const char* subpart,
                                    const char* state_and_name, int32_t value) {
  return widget_subpart_set_style_int(widget, subpart, state_and_name, value,
                                      tooltip_subpart_get_style_obj);
}

ret_t tooltip_subpart_set_style_color(widget_t* widget, const char* subpart,
                                      const char* state_and_name, uint32_t value) {
  return widget_subpart_set_style_color(widget, subpart, state_and_name, value,
                                        tooltip_subpart_get_style_obj);
}

ret_t tooltip_subpart_set_style_str(widget_t* widget, const char* subpart,
                                    const char* state_and_name, const char* value) {
  return widget_subpart_set_style_str(widget, subpart, state_and_name, value,
                                      tooltip_subpart_get_style_obj);
}

static ret_t tooltip_on_destroy(widget_t* widget) {
  tooltip_t* tooltip = TOOLTIP(widget);
  return_value_if_fail(tooltip != NULL, RET_BAD_PARAMS);

  TKMEM_FREE(tooltip->line.style);
  style_destroy(tooltip->line.astyle);
  tooltip->line.astyle = NULL;

  TKMEM_FREE(tooltip->symbol.style);
  style_destroy(tooltip->symbol.astyle);
  tooltip->symbol.astyle = NULL;

  TKMEM_FREE(tooltip->tip.style);
  style_destroy(tooltip->tip.astyle);
  tooltip->tip.astyle = NULL;

  return RET_OK;
}

TK_DECL_VTABLE(tooltip) = {.size = sizeof(tooltip_t),
                           .type = WIDGET_TYPE_TOOLTIP,
                           .parent = TK_PARENT_VTABLE(widget),
                           .create = tooltip_create_default,
                           .set_prop = tooltip_set_prop,
                           .get_prop = tooltip_get_prop,
                           .on_paint_begin = tooltip_on_paint_begin,
                           .on_paint_self = tooltip_on_paint_self,
                           .on_destroy = tooltip_on_destroy};

widget_t* tooltip_create(widget_t* parent, const widget_vtable_t* vt, xy_t x, xy_t y, wh_t w,
                         wh_t h) {
  widget_t* widget = widget_create(parent, vt, x, y, w, h);
  tooltip_t* tooltip = TOOLTIP(widget);
  return_value_if_fail(tooltip != NULL, NULL);

  if (tooltip->line.astyle == NULL) {
    tooltip->line.astyle = style_factory_create_style(style_factory(), widget);
  }

  if (tooltip->symbol.astyle == NULL) {
    tooltip->symbol.astyle = style_factory_create_style(style_factory(), widget);
  }

  if (tooltip->tip.astyle == NULL) {
    tooltip->tip.astyle = style_factory_create_style(style_factory(), widget);
  }

  return widget;
}

widget_t* tooltip_create_default(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  return tooltip_create(parent, TK_REF_VTABLE(tooltip), x, y, w, h);
}

widget_t* tooltip_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, tooltip), NULL);

  return widget;
}

bool_t widget_is_tooltip(widget_t* widget) {
#ifdef WITH_WIDGET_TYPE_CHECK
  return WIDGET_IS_INSTANCE_OF(widget, tooltip);
#else
  return (widget != NULL && strstr(widget->vt->type, "_tooltip") != NULL);
#endif
}
