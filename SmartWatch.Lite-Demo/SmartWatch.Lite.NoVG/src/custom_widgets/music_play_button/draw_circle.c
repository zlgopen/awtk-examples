#include "draw_circle.h"
#include "awtk.h"
#include "tkc/mem.h"

#define FRR_POINT_PIX_CACHE_SIZE 64

#include "base/ffr_draw_rounded_rect.inc"

static frr_point_pix_t frr_point_pix_cache2_list[FRR_POINT_PIX_CACHE_SIZE];

/*
   | 6 | 0 |
 7     |     1
---------------
 4     |     2
   | 5 | 3 |
*/

typedef struct _frr_line_point_pix_t {
  frr_point_pix_t base;
  uint32_t angle;
  rgba_t color;
} frr_line_point_pix_t;

typedef struct _quadrant_angle_info_t {
  uint8_t start_angle;
  uint8_t end_angle;
  bool_t is_link;
} quadrant_angle_info_t;

static frr_line_point_pix_t
    frr_line_point_pix_cache1_list[FRR_POINT_PIX_CACHE_SIZE];

static frr_line_point_pix_t
    frr_line_point_pix_cache2_list[FRR_POINT_PIX_CACHE_SIZE];

static bool_t Wu_D_Line(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                        uint32_t angle, darray_t *point_list,
                        frr_line_point_pix_t *point_pix_cache_list) {
  int32_t x = x1;
  int32_t y = y1;
  int32_t dx = x2 - x1;
  int32_t dy = y2 - y1;
  int32_t ux = 0;
  int32_t uy = 0;
  float_t uk = 0.0f;
  float_t k = 0.0f;
  float_t e = 0.0f;
  uint32_t i = 0;
  frr_line_point_pix_t *tmp = NULL;

  k = (float_t)dy / (float_t)dx;
  ux = ((dx > 0) << 1) - 1;
  uy = ((dy > 0) << 1) - 1;
  uk = ((ux > 0 && uy > 0) || (ux < 0 && uy < 0)) ? -k : k;

  if (dx == 0 && dy == 0) {
    return FALSE;
  }

  if (dx == 0 && dy != 0) {
    for (y = y1; y != y2; y++) {
      tmp = &(point_pix_cache_list[i++]);
      tmp->base.x = x;
      tmp->base.y = y;
      tmp->base.e = 1.0f;
      tmp->angle = angle;
      darray_push(point_list, tmp);
    }
    e = 0.0f;
  } else if (dx != 0 && dy == 0) {
    for (x = x1; x != x2; x++) {
      tmp = &(point_pix_cache_list[i++]);
      tmp->base.x = x;
      tmp->base.y = y;
      tmp->base.e = 1.0f;
      tmp->angle = angle;
      darray_push(point_list, tmp);
    }
    e = 0.0f;
  } else if (abs(dx) > abs(dy)) {
    for (x = x1; x != x2; x += ux) {
      tmp = &(point_pix_cache_list[i++]);
      tmp->base.x = x;
      tmp->base.y = y;
      tmp->base.e = 1.0f - tk_abs(e);
      tmp->angle = angle;
      darray_push(point_list, tmp);
      e += uk;
      if ((e >= 1.0f && uk > 0.0f) || (e <= -1.0f && uk < 0.0f)) {
        e = e + (uk > 0 ? -1 : 1);
        y += uy;
      }
    }
  } else {
    uk = 1 / uk;
    for (y = y1; y != y2; y += uy) {
      tmp = &(point_pix_cache_list[i++]);
      tmp->base.x = x;
      tmp->base.y = y;
      tmp->base.e = 1.0f - tk_abs(e);
      tmp->angle = angle;
      darray_push(point_list, tmp);
      e += uk;
      e += uk;
      if ((e >= 1.0f && uk > 0.0f) || (e <= -1.0f && uk < 0.0f)) {
        e = e + (uk > 0 ? -1 : 1);
        x += ux;
      }
    }
  }
  tmp = &(point_pix_cache_list[i++]);
  tmp->base.x = x;
  tmp->base.y = y;
  tmp->base.e = 1.0f - tk_abs(e);
  tmp->angle = angle;
  darray_push(point_list, tmp);
  return TRUE;
}

static inline int32_t draw_circle_angle_to_0_360(int32_t angle) {
  int32_t tmp = 0;
  int32_t n = angle - (int32_t)angle;

  tmp = (int32_t)angle % 360;

  if (tmp < 0) {
    tmp = 360 + tmp;
  }

  if (n > 0) {
    tmp += n;
  }

  return tmp;
}

static inline frr_quadrant_t
draw_circle_get_quadrant_for_angle(int32_t angle, int32_t *offset_angle) {
  if (0 <= angle && angle < 45) {
    if (offset_angle != NULL) {
      *offset_angle = angle;
    }
    return FRR_QUADRANT_FOURTH_F;
  } else if (45 <= angle && angle < 90) {
    if (offset_angle != NULL) {
      *offset_angle = angle - 45;
    }
    return FRR_QUADRANT_FOURTH_S;
  } else if (90 <= angle && angle < 135) {
    if (offset_angle != NULL) {
      *offset_angle = angle - 90;
    }
    return FRR_QUADRANT_THIRD_S;
  } else if (135 <= angle && angle < 180) {
    if (offset_angle != NULL) {
      *offset_angle = angle - 135;
    }
    return FRR_QUADRANT_THIRD_F;
  } else if (180 <= angle && angle < 225) {
    if (offset_angle != NULL) {
      *offset_angle = angle - 180;
    }
    return FRR_QUADRANT_SECOND_S;
  } else if (225 <= angle && angle < 270) {
    if (offset_angle != NULL) {
      *offset_angle = angle - 225;
    }
    return FRR_QUADRANT_SECOND_F;
  } else if (270 <= angle && angle < 315) {
    if (offset_angle != NULL) {
      *offset_angle = angle - 270;
    }
    return FRR_QUADRANT_FIRST_F;
  } else {
    if (offset_angle != NULL) {
      *offset_angle = angle - 315;
    }
    return FRR_QUADRANT_FIRST_S;
  }
}

static inline void
draw_circle_set_quadrant_angle_info(quadrant_angle_info_t *quadrant_info_list,
                                    frr_quadrant_t type, int32_t offset_angle,
                                    bool_t is_ccw, bool_t is_end) {
  if (is_ccw) {
    switch (type) {
    case FRR_QUADRANT_FIRST_F:
    case FRR_QUADRANT_SECOND_S:
    case FRR_QUADRANT_THIRD_S:
    case FRR_QUADRANT_FOURTH_F:
      if (is_end) {
        quadrant_info_list->end_angle = 45;
        quadrant_info_list->start_angle = offset_angle;
      } else {
        quadrant_info_list->end_angle = offset_angle;
        quadrant_info_list->start_angle = 0;
      }
      break;
    case FRR_QUADRANT_FIRST_S:
    case FRR_QUADRANT_SECOND_F:
    case FRR_QUADRANT_THIRD_F:
    case FRR_QUADRANT_FOURTH_S:
      if (is_end) {
        quadrant_info_list->end_angle = 45 - offset_angle;
        quadrant_info_list->start_angle = 0;
      } else {
        quadrant_info_list->end_angle = 45;
        quadrant_info_list->start_angle = 45 - offset_angle;
      }
      break;
    default:
      break;
    }
  } else {
    switch (type) {
    case FRR_QUADRANT_FIRST_F:
    case FRR_QUADRANT_FOURTH_F:
    case FRR_QUADRANT_THIRD_S:
    case FRR_QUADRANT_SECOND_S:
      if (is_end) {
        quadrant_info_list->end_angle = offset_angle;
        quadrant_info_list->start_angle = 0;
      } else {
        quadrant_info_list->end_angle = 45;
        quadrant_info_list->start_angle = offset_angle;
      }
      break;
    case FRR_QUADRANT_FIRST_S:
    case FRR_QUADRANT_FOURTH_S:
    case FRR_QUADRANT_THIRD_F:
    case FRR_QUADRANT_SECOND_F:
      if (is_end) {
        quadrant_info_list->end_angle = 45;
        quadrant_info_list->start_angle = 45 - offset_angle;
      } else {
        quadrant_info_list->end_angle = 45 - offset_angle;
        quadrant_info_list->start_angle = 0;
      }
      break;
    default:
      break;
    }
  }
}

static inline void draw_circle_get_quadrant_link_model(frr_quadrant_t type,
                                                       frr_quadrant_t *n) {
  switch (type) {
  case FRR_QUADRANT_FIRST_F:
    *n = FRR_QUADRANT_FIRST_S;
    break;
  case FRR_QUADRANT_FIRST_S:
    *n = FRR_QUADRANT_FIRST_F;
    break;
  case FRR_QUADRANT_SECOND_F:
    *n = FRR_QUADRANT_SECOND_S;
    break;
  case FRR_QUADRANT_SECOND_S:
    *n = FRR_QUADRANT_SECOND_F;
    break;
  case FRR_QUADRANT_THIRD_F:
    *n = FRR_QUADRANT_THIRD_S;
    break;
  case FRR_QUADRANT_THIRD_S:
    *n = FRR_QUADRANT_THIRD_F;
    break;
  case FRR_QUADRANT_FOURTH_F:
    *n = FRR_QUADRANT_FOURTH_S;
    break;
  case FRR_QUADRANT_FOURTH_S:
    *n = FRR_QUADRANT_FOURTH_F;
    break;
  default:
    break;
  }
}

static inline void
draw_circle_check_quadrant_link(quadrant_angle_info_t quadrant_info_list[8]) {
  uint32_t i = 0;
  frr_quadrant_t n = FRR_QUADRANT_FIRST_F;
  frr_quadrant_t type = FRR_QUADRANT_FIRST_F;
  for (; i < 8; i++) {
    if (quadrant_info_list[i].start_angle != 0 ||
        quadrant_info_list[i].end_angle != 0) {
      type = (frr_quadrant_t)i;
      draw_circle_get_quadrant_link_model(type, &n);

      if ((quadrant_info_list[(uint32_t)n].start_angle != 0 ||
           quadrant_info_list[(uint32_t)n].end_angle != 0)) {
        quadrant_info_list[i].is_link = TRUE;
      }
    }
  }
}

static void draw_circle_get_quadrant_angle_info_list(
    quadrant_angle_info_t quadrant_info_list[8], int32_t start_angle,
    int32_t end_angle, bool_t is_ccw) {
  int32_t i = 0;
  frr_quadrant_t end_quadrant;
  frr_quadrant_t start_quadrant;
  int32_t value_quadrant = 0;
  int32_t offset_end_angle = 0;
  int32_t offset_start_angle = 0;
  end_angle = draw_circle_angle_to_0_360(end_angle);
  start_angle = draw_circle_angle_to_0_360(start_angle);

  end_quadrant =
      draw_circle_get_quadrant_for_angle(end_angle, &offset_end_angle);
  start_quadrant =
      draw_circle_get_quadrant_for_angle(start_angle, &offset_start_angle);

  if (start_angle > end_angle) {
    value_quadrant = end_angle + 360 - start_angle;
  } else {
    value_quadrant = end_angle - start_angle;
  }

  if (is_ccw) {
    value_quadrant = 360 - value_quadrant;
    for (i = 0; i < value_quadrant; i += 45) {
      uint32_t tmp = draw_circle_angle_to_0_360(end_angle + i);
      uint32_t number = (uint32_t)draw_circle_get_quadrant_for_angle(tmp, NULL);
      quadrant_info_list[number].end_angle = 45;
      quadrant_info_list[number].start_angle = 0;
    }
    draw_circle_set_quadrant_angle_info(
        &quadrant_info_list[(uint32_t)end_quadrant], end_quadrant,
        offset_end_angle, is_ccw, TRUE);
    if (end_quadrant != start_quadrant) {
      draw_circle_set_quadrant_angle_info(
          &quadrant_info_list[(uint32_t)start_quadrant], start_quadrant,
          offset_start_angle, is_ccw, FALSE);
    }
  } else {
    for (i = 0; i < value_quadrant; i += 45) {
      uint32_t tmp = draw_circle_angle_to_0_360(start_angle + i);
      uint32_t number = (uint32_t)draw_circle_get_quadrant_for_angle(tmp, NULL);
      quadrant_info_list[number].end_angle = 45;
      quadrant_info_list[number].start_angle = 0;
    }
    draw_circle_set_quadrant_angle_info(
        &quadrant_info_list[(uint32_t)end_quadrant], end_quadrant,
        offset_end_angle, is_ccw, TRUE);
    if (end_quadrant != start_quadrant) {
      draw_circle_set_quadrant_angle_info(
          &quadrant_info_list[(uint32_t)start_quadrant], start_quadrant,
          offset_start_angle, is_ccw, FALSE);
    }
  }

  draw_circle_check_quadrant_link(quadrant_info_list);
}

static inline void draw_circle_draw_circluar_point_to_quadrant(
    frr_image_info_t *image_info, canvas_t *c, int32_t xc, int32_t yc,
    int32_t px, int32_t py, float_t e, frr_quadrant_t type, rgba_t *color) {
  int32_t p = 0;
  int32_t x = 0;
  int32_t y = 0;
  rgba_t o_color;
  if (widget_get_circluar_point_to_quadrant(xc, yc, px, py, type, &x, &y)) {
    x += c->ox;
    y += c->oy;

    p = widget_frr_image_get_buff_point(image_info, x, y);
    if (p > 0) {
      widget_image_data2color(image_info->dst + p, image_info->format,
                              &o_color);

      widget_draw_antialiasing_for_point(image_info, c, x, y, e, *color,
                                         o_color);
    }
  }
}

static void draw_circle_draw_stroke_arc_point_list(
    frr_image_info_t *image_info, canvas_t *c, frr_quadrant_t type, int32_t xc,
    int32_t yc, darray_t *point_list, rgba_t *color) {
  uint32_t i = 0;
  void **elms = point_list->elms;
  for (i = 0; i < point_list->size; i++) {
    frr_point_pix_t *iter = (frr_point_pix_t *)elms[i];

    draw_circle_draw_circluar_point_to_quadrant(
        image_info, c, xc, yc, iter->x, iter->y, 1 - iter->e, type, color);

    draw_circle_draw_circluar_point_to_quadrant(
        image_info, c, xc, yc, iter->x, iter->y - 1, iter->e, type, color);
  }
}

static void draw_circle_draw_stroke_arc(
    frr_image_info_t *image_info, canvas_t *c, int32_t xc, int32_t yc,
    int32_t radius, quadrant_angle_info_t quadrant_info_list[8],
    darray_t *point_list_45, darray_t *tmp_point_list_45, rgba_t *color) {
  uint32_t i = 0;
  for (; i < 8; i++) {
    if (quadrant_info_list[i].start_angle != 0 ||
        quadrant_info_list[i].end_angle != 0) {
      widget_push_arc_point_list(TK_D2R(quadrant_info_list[i].start_angle),
                                 TK_D2R(quadrant_info_list[i].end_angle),
                                 radius, tmp_point_list_45, point_list_45,
                                 TRUE);

      draw_circle_draw_stroke_arc_point_list(image_info, c, (frr_quadrant_t)i,
                                             xc, yc, tmp_point_list_45, color);

      tmp_point_list_45->size = 0;
    }
  }
}

void draw_circle_draw_circle_arc(frr_image_info_t *image_info, canvas_t *c,
                                 int32_t xc, int32_t yc, draw_circle_t *ctx,
                                 quadrant_angle_info_t quadrant_info_list[8],
                                 bool_t is_ccw, rgba_t *color) {
  uint32_t size = 0;
  frr_point_pix_t *point_pix_cache_list = NULL;

  ctx->point1_list_45.size = 0;
  ctx->tmp_point1_list_45.size = 0;

  size = (uint32_t)(ANGLE2SIZE(M_FRR_PI_4, ctx->radius) * 1.5f);
  if (size <= FRR_POINT_PIX_CACHE_SIZE) {
    point_pix_cache_list = frr_point_pix_cache_list;
  } else {
    point_pix_cache_list = TKMEM_ZALLOCN(frr_point_pix_t, size);
  }

  Wu_D_Circle(ctx->radius, &ctx->point1_list_45, point_pix_cache_list);

  draw_circle_draw_stroke_arc(image_info, c, xc, yc, ctx->radius,
                              quadrant_info_list, &ctx->point1_list_45,
                              &ctx->tmp_point1_list_45, color);

  if (size > FRR_POINT_PIX_CACHE_SIZE) {
    TKMEM_FREE(point_pix_cache_list);
  }
}

static void draw_circle_draw_stroke_line_point_list(
    frr_image_info_t *image_info, canvas_t *c, frr_quadrant_t type, int32_t xc,
    int32_t yc, darray_t *point_list, bool_t is_link) {
  uint32_t i = 0;
  void **elms = point_list->elms;
  for (i = 0; i < point_list->size; i++) {
    frr_line_point_pix_t *iter = (frr_line_point_pix_t *)elms[i];
    if (!(iter->angle == 0 || (iter->angle == 45 && is_link))) {
      draw_circle_draw_circluar_point_to_quadrant(
          image_info, c, xc, yc, iter->base.x, iter->base.y, 1 - iter->base.e,
          type, &iter->color);
      draw_circle_draw_circluar_point_to_quadrant(
          image_info, c, xc, yc, iter->base.x, iter->base.y - 1, iter->base.e,
          type, &iter->color);
    }
  }
}

static void draw_circle_get_stroke_line_point_color(
    frr_image_info_t *image_info, canvas_t *c, frr_quadrant_t type, int32_t xc,
    int32_t yc, darray_t *point_list, bool_t is_link) {
  int32_t p = 0;
  int32_t x = 0;
  int32_t y = 0;
  uint32_t i = 0;

  void **elms = point_list->elms;
  for (i = 0; i < point_list->size; i++) {
    frr_line_point_pix_t *iter = (frr_line_point_pix_t *)elms[i];

    if (!(iter->angle == 0 || (iter->angle == 45 && is_link))) {
      if (widget_get_circluar_point_to_quadrant(xc, yc, iter->base.x,
                                                iter->base.y, type, &x, &y)) {
        x += c->ox;
        y += c->oy;

        p = widget_frr_image_get_buff_point(image_info, x, y);
        if (p > 0) {
          widget_image_data2color(image_info->dst + p, image_info->format,
                                  &iter->color);
        }
      }
    }
  }
}

static inline void draw_circle_draw_circluar_point_to_quadrant_hline(
    frr_image_info_t *image_info, canvas_t *c, int32_t xc, int32_t yc,
    int32_t w, int32_t px, int32_t py, float_t e, bool_t is_left,
    frr_quadrant_t type, rgba_t color, bool_t is_linear) {
  int32_t p = 0;
  int32_t x = 0;
  int32_t y = 0;
  int32_t x1 = 0;
  rgba_t o_color_1;
  rgba_t o_color_2;
  if (w > 0) {
    if (widget_get_circluar_point_to_quadrant(xc, yc, px, py, type, &x, &y)) {
      memset(&o_color_1, 0, sizeof(rgba_t));
      memset(&o_color_2, 0, sizeof(rgba_t));
      p = widget_frr_image_get_buff_point(image_info, x + c->ox, y + c->oy);
      if (p > 0) {
        widget_image_data2color(image_info->dst + p, image_info->format,
                                &o_color_1);
        x1 = x;
        if (is_left) {
          int32_t tmp = x - w;
          if (tmp < 0) {
            w += tmp;
            x = 0;
          } else {
            x = x - w;
          }
        } else {
          w++;
        }
        canvas_fill_rect(c, x, y, w, 1);
        if (is_left) {
          widget_draw_antialiasing_for_point(image_info, c, x1 + c->ox,
                                             y + c->oy, e, color, o_color_1);
        } else {
          widget_draw_antialiasing_for_point(image_info, c, x + c->ox,
                                             y + c->oy, e, color, o_color_1);
        }
      }
    }
  }
}

static inline bool_t draw_circle_get_point_list_x(darray_t *point_list,
                                                  int32_t *number, int32_t y,
                                                  int32_t *x1, int32_t *y1,
                                                  bool_t is_head) {
  int32_t i = *number;
  frr_point_pix_t *iter = NULL;
  if (is_head) {
    iter = darray_get(point_list, i);
  } else {
    iter = darray_get(point_list, point_list->size - i - 1);
  }
  if (iter->y <= y) {
    *x1 = iter->x;
    *y1 = iter->y;
    return TRUE;
  } else {
    i++;
    for (; i < (int32_t)point_list->size; i++) {
      if (is_head) {
        iter = darray_get(point_list, i);
      } else {
        iter = darray_get(point_list, point_list->size - i - 1);
      }
      if (iter->y <= y) {
        *number = i;
        *x1 = iter->x;
        *y1 = iter->y;
        return TRUE;
      }
    }
  }
  return FALSE;
}

static inline bool_t draw_circle_get_point_list_y(darray_t *point_list,
                                                  int32_t *number, int32_t x,
                                                  int32_t *x1, int32_t *y1,
                                                  bool_t is_head) {
  int32_t i = *number;
  frr_point_pix_t *iter = NULL;
  if (is_head) {
    iter = darray_get(point_list, i);
  } else {
    iter = darray_get(point_list, point_list->size - i - 1);
  }
  if (iter->x >= x) {
    *x1 = iter->x;
    *y1 = iter->y;
    return TRUE;
  } else {
    i++;
    for (; i < (int32_t)point_list->size; i++) {
      if (is_head) {
        iter = darray_get(point_list, i);
      } else {
        iter = darray_get(point_list, point_list->size - i - 1);
      }
      if (iter->x >= x) {
        *number = i;
        *x1 = iter->x;
        *y1 = iter->y;
        return TRUE;
      }
    }
  }
  return FALSE;
}

static void draw_circle_draw_fill_arc_point_list_h(
    frr_image_info_t *image_info, canvas_t *c, int32_t xc, int32_t yc,
    frr_quadrant_t type, rgba_t *color, darray_t *point1_list,
    darray_t *point2_list, darray_t *line1_point_list,
    darray_t *line2_point_list, bool_t is_link) {
  int32_t w = 0;
  int32_t x = 0;
  int32_t y = 0;
  uint32_t i = 0;
  bool_t is_linear = TRUE;
  int32_t line1_number = 0;
  int32_t arc_line_number = 0;
  int32_t arc_line_y1 = ((frr_point_pix_t *)darray_tail(point1_list))->y;
  int32_t arc_line_y2 = ((frr_point_pix_t *)darray_head(point1_list))->y;
  int32_t line1_y1 = ((frr_point_pix_t *)darray_head(line1_point_list))->y;
  int32_t line1_y2 = ((frr_point_pix_t *)darray_tail(line1_point_list))->y;
  bool_t is_left =
      type == FRR_QUADRANT_FIRST_F || type == FRR_QUADRANT_FIRST_S ||
      type == FRR_QUADRANT_FOURTH_F || type == FRR_QUADRANT_FOURTH_S;

  draw_circle_get_stroke_line_point_color(image_info, c, type, xc, yc,
                                          line1_point_list, is_link);
  draw_circle_get_stroke_line_point_color(image_info, c, type, xc, yc,
                                          line2_point_list, is_link);

  for (i = 0; i < point2_list->size; i++) {
    frr_point_pix_t *iter = (frr_point_pix_t *)darray_get(point2_list, i);

    if (line1_y1 <= iter->y && iter->y <= line1_y2) {
      return_if_fail(draw_circle_get_point_list_x(
          line1_point_list, &line1_number, iter->y, &x, &y, FALSE));
    } else if (arc_line_y1 <= iter->y && iter->y <= arc_line_y2) {
      return_if_fail(draw_circle_get_point_list_x(point1_list, &arc_line_number,
                                                  iter->y, &x, &y, TRUE));
    } else {
      break;
    }
    w = iter->x - x;
    is_linear = 0 == x;
    draw_circle_draw_circluar_point_to_quadrant_hline(
        image_info, c, xc, yc, w, iter->x, iter->y, 1 - iter->e, is_left, type,
        *color, is_linear);
  }
  line1_number = arc_line_number = 0;
  for (i = 0; i < line2_point_list->size; i++) {
    frr_point_pix_t *iter = (frr_point_pix_t *)darray_get(
        line2_point_list, line2_point_list->size - i - 1);

    if (line1_y1 <= iter->y && iter->y <= line1_y2) {
      return_if_fail(draw_circle_get_point_list_x(
          line1_point_list, &line1_number, iter->y, &x, &y, FALSE));
    } else if (arc_line_y1 <= iter->y && iter->y <= arc_line_y2) {
      return_if_fail(draw_circle_get_point_list_x(point1_list, &arc_line_number,
                                                  iter->y, &x, &y, TRUE));
    } else {
      break;
    }
    w = iter->x - x;
    is_linear = 0 == x;
    draw_circle_draw_circluar_point_to_quadrant_hline(
        image_info, c, xc, yc, w, iter->x, iter->y, 1 - iter->e, is_left, type,
        *color, is_linear);
  }

  draw_circle_draw_stroke_line_point_list(image_info, c, type, xc, yc,
                                          line1_point_list, is_link);
  draw_circle_draw_stroke_line_point_list(image_info, c, type, xc, yc,
                                          line2_point_list, is_link);
}

static void draw_circle_draw_fill_arc_point_list_v(
    frr_image_info_t *image_info, canvas_t *c, int32_t xc, int32_t yc,
    frr_quadrant_t type, rgba_t *color, darray_t *point1_list,
    darray_t *point2_list, darray_t *line1_point_list,
    darray_t *line2_point_list, bool_t is_link) {
  int32_t w = 0;
  int32_t x = 0;
  int32_t y = 0;
  uint32_t i = 0;
  bool_t is_linear = TRUE;
  int32_t line2_number = 0;
  int32_t arc_line_number = 0;
  int32_t arc_line_x1 = ((frr_point_pix_t *)darray_head(point1_list))->x;
  int32_t arc_line_x2 = ((frr_point_pix_t *)darray_tail(point1_list))->x;
  int32_t line2_x1 = ((frr_point_pix_t *)darray_head(line2_point_list))->x;
  int32_t line2_x2 = ((frr_point_pix_t *)darray_tail(line2_point_list))->x;
  bool_t is_left =
      type == FRR_QUADRANT_FIRST_F || type == FRR_QUADRANT_FIRST_S ||
      type == FRR_QUADRANT_FOURTH_F || type == FRR_QUADRANT_FOURTH_S;

  draw_circle_get_stroke_line_point_color(image_info, c, type, xc, yc,
                                          line1_point_list, is_link);
  draw_circle_get_stroke_line_point_color(image_info, c, type, xc, yc,
                                          line2_point_list, is_link);

  for (i = 0; i < line1_point_list->size; i++) {
    frr_point_pix_t *iter = (frr_point_pix_t *)darray_get(line1_point_list, i);
    if (arc_line_x1 <= iter->x && iter->x <= arc_line_x2) {
      return_if_fail(draw_circle_get_point_list_y(point1_list, &arc_line_number,
                                                  iter->x, &x, &y, TRUE));
    } else if (line2_x1 <= iter->x && iter->x <= line2_x2) {
      return_if_fail(draw_circle_get_point_list_y(
          line2_point_list, &line2_number, iter->x, &x, &y, TRUE));
    } else {
      break;
    }

    w = iter->y - y;
    is_linear = 0 == x;
    draw_circle_draw_circluar_point_to_quadrant_hline(
        image_info, c, xc, yc, w, iter->x, iter->y, 1 - iter->e, is_left, type,
        *color, is_linear);
  }
  line2_number = arc_line_number = 0;
  for (i = 0; i < point2_list->size; i++) {
    frr_point_pix_t *iter = (frr_point_pix_t *)darray_get(point2_list, i);
    if (arc_line_x1 <= iter->x && iter->x <= arc_line_x2) {
      return_if_fail(draw_circle_get_point_list_y(point1_list, &arc_line_number,
                                                  iter->x, &x, &y, TRUE));
    } else if (line2_x1 <= iter->x && iter->x <= line2_x2) {
      return_if_fail(draw_circle_get_point_list_y(
          line2_point_list, &line2_number, iter->x, &x, &y, TRUE));
    } else {
      break;
    }

    w = iter->y - y;
    is_linear = 0 == x;
    draw_circle_draw_circluar_point_to_quadrant_hline(
        image_info, c, xc, yc, w, iter->x, iter->y, 1 - iter->e, is_left, type,
        *color, is_linear);
  }

  draw_circle_draw_stroke_line_point_list(image_info, c, type, xc, yc,
                                          line1_point_list, is_link);
  draw_circle_draw_stroke_line_point_list(image_info, c, type, xc, yc,
                                          line2_point_list, is_link);
}

static void draw_circle_push_arc_point_list_all(darray_t *point_list,
                                                darray_t *point_list_45) {
  uint32_t i = 0;
  void **elms = point_list_45->elms;

  for (i = 0; i < point_list_45->size; i++) {
    frr_point_pix_t *iter = (frr_point_pix_t *)elms[i];
    darray_push(point_list, iter);
  }
}

static void draw_circle_draw_circle_arc_width(
    frr_image_info_t *image_info, canvas_t *c, int32_t xc, int32_t yc,
    draw_circle_t *ctx, frr_quadrant_t type, uint32_t start_angle,
    uint32_t end_angle, darray_t *point1_list_45, darray_t *tmp_point1_list_45,
    darray_t *point2_list_45, darray_t *tmp_point2_list_45, rgba_t *color,
    bool_t is_link) {
  uint32_t size1 = 0;
  uint32_t size2 = 0;
  frr_point_pix_t *point_head_1 = NULL;
  frr_point_pix_t *point_head_2 = NULL;
  frr_point_pix_t *point_tail_1 = NULL;
  frr_point_pix_t *point_tail_2 = NULL;
  frr_line_point_pix_t *point_pix_cache1_list = NULL;
  frr_line_point_pix_t *point_pix_cache2_list = NULL;

  tmp_point1_list_45->size = 0;
  tmp_point2_list_45->size = 0;
  ctx->line1_point_list.size = 0;
  ctx->line2_point_list.size = 0;

  if (start_angle == end_angle) {
    return;
  }
  if (start_angle == 0 && end_angle == 45) {
    draw_circle_push_arc_point_list_all(tmp_point1_list_45, point1_list_45);
    draw_circle_push_arc_point_list_all(tmp_point2_list_45, point2_list_45);
  } else {
    widget_push_arc_point_list(TK_D2R(start_angle), TK_D2R(end_angle),
                               ctx->radius, tmp_point1_list_45, point1_list_45,
                               TRUE);
    widget_push_arc_point_list(TK_D2R(start_angle), TK_D2R(end_angle),
                               ctx->radius + ctx->line_width,
                               tmp_point2_list_45, point2_list_45, TRUE);
  }

  if (tmp_point1_list_45->size <= 0 || tmp_point2_list_45->size <= 0) {
    return;
  }

  point_head_1 = (frr_point_pix_t *)darray_head(tmp_point1_list_45);
  point_head_2 = (frr_point_pix_t *)darray_head(tmp_point2_list_45);
  point_tail_1 = (frr_point_pix_t *)darray_tail(tmp_point1_list_45);
  point_tail_2 = (frr_point_pix_t *)darray_tail(tmp_point2_list_45);

  size1 = (uint32_t)(sqrt(pow(point_head_1->x - point_head_2->x, 2) +
                          pow(point_head_1->y - point_head_2->y, 2)) *
                     1.5);
  size2 = (uint32_t)(sqrt(pow(point_tail_1->x - point_tail_2->x, 2) +
                          pow(point_tail_1->y - point_tail_2->y, 2)) *
                     1.5);

  if (size1 <= FRR_POINT_PIX_CACHE_SIZE) {
    point_pix_cache1_list = frr_line_point_pix_cache1_list;
  } else {
    point_pix_cache1_list = TKMEM_ZALLOCN(frr_line_point_pix_t, size1);
  }

  if (size2 <= FRR_POINT_PIX_CACHE_SIZE) {
    point_pix_cache2_list = frr_line_point_pix_cache2_list;
  } else {
    point_pix_cache2_list = TKMEM_ZALLOCN(frr_line_point_pix_t, size2);
  }

  if (!Wu_D_Line(point_head_1->x, point_head_1->y, point_head_2->x,
                 point_head_2->y, start_angle, &ctx->line1_point_list,
                 point_pix_cache1_list)) {
    return;
  }
  if (!Wu_D_Line(point_tail_1->x, point_tail_1->y, point_tail_2->x,
                 point_tail_2->y, end_angle, &ctx->line2_point_list,
                 point_pix_cache2_list)) {
    return;
  }

  switch (type) {
  case FRR_QUADRANT_FIRST_F:
  case FRR_QUADRANT_SECOND_F:
  case FRR_QUADRANT_THIRD_S:
  case FRR_QUADRANT_FOURTH_S:
    draw_circle_draw_fill_arc_point_list_h(
        image_info, c, xc, yc, type, color, tmp_point1_list_45,
        tmp_point2_list_45, &ctx->line1_point_list, &ctx->line2_point_list,
        is_link);
    break;
  case FRR_QUADRANT_FIRST_S:
  case FRR_QUADRANT_SECOND_S:
  case FRR_QUADRANT_THIRD_F:
  case FRR_QUADRANT_FOURTH_F:
    draw_circle_draw_fill_arc_point_list_v(
        image_info, c, xc, yc, type, color, tmp_point1_list_45,
        tmp_point2_list_45, &ctx->line1_point_list, &ctx->line2_point_list,
        is_link);
    break;
  default:
    break;
  }

  if (size1 > FRR_POINT_PIX_CACHE_SIZE) {
    TKMEM_FREE(point_pix_cache1_list);
  }
  if (size2 > FRR_POINT_PIX_CACHE_SIZE) {
    TKMEM_FREE(point_pix_cache2_list);
  }
}

void draw_circle_draw_circle_arc_width_all(
    frr_image_info_t *image_info, canvas_t *c, int32_t xc, int32_t yc,
    draw_circle_t *ctx, quadrant_angle_info_t quadrant_info_list[8],
    rgba_t *color) {
  uint32_t i = 0;
  uint32_t size1 = 0;
  uint32_t size2 = 0;
  frr_point_pix_t *point_pix_cache1_list = NULL;
  frr_point_pix_t *point_pix_cache2_list = NULL;

  ctx->point1_list_45.size = 0;
  ctx->point2_list_45.size = 0;
  ctx->tmp_point1_list_45.size = 0;
  ctx->tmp_point2_list_45.size = 0;

  size1 = (uint32_t)(ANGLE2SIZE(M_FRR_PI_4, ctx->radius) * 1.5f);
  size2 = (uint32_t)(ANGLE2SIZE(M_FRR_PI_4, (ctx->radius + ctx->line_width)) *
                     1.5f);

  if (size1 <= FRR_POINT_PIX_CACHE_SIZE) {
    point_pix_cache1_list = frr_point_pix_cache_list;
  } else {
    point_pix_cache1_list = TKMEM_ZALLOCN(frr_point_pix_t, size1);
  }
  if (size2 <= FRR_POINT_PIX_CACHE_SIZE) {
    point_pix_cache2_list = frr_point_pix_cache2_list;
  } else {
    point_pix_cache2_list = TKMEM_ZALLOCN(frr_point_pix_t, size2);
  }

  Wu_D_Circle(ctx->radius, &ctx->point1_list_45, point_pix_cache1_list);
  Wu_D_Circle(ctx->radius + ctx->line_width, &ctx->point2_list_45,
              point_pix_cache2_list);

  for (; i < 8; i++) {
    if (quadrant_info_list[i].start_angle != 0 ||
        quadrant_info_list[i].end_angle != 0) {
      draw_circle_draw_circle_arc_width(
          image_info, c, xc, yc, ctx, (frr_quadrant_t)i,
          quadrant_info_list[i].start_angle, quadrant_info_list[i].end_angle,
          &ctx->point1_list_45, &ctx->tmp_point1_list_45, &ctx->point2_list_45,
          &ctx->tmp_point2_list_45, color, quadrant_info_list[i].is_link);
    }
  }

  if (size1 > FRR_POINT_PIX_CACHE_SIZE) {
    TKMEM_FREE(point_pix_cache1_list);
  }
  if (size2 > FRR_POINT_PIX_CACHE_SIZE) {
    TKMEM_FREE(point_pix_cache2_list);
  }
}

draw_circle_t *draw_circle_fill_create(int32_t radius, int32_t line_width) {
  uint32_t size1 = 0;
  uint32_t size2 = 0;
  draw_circle_t *ctx = TKMEM_ZALLOC(draw_circle_t);
  return_value_if_fail(ctx != NULL, NULL);

  size1 = (uint32_t)(ANGLE2SIZE(M_FRR_PI_4, radius) * 1.5f);
  size2 = (uint32_t)(ANGLE2SIZE(M_FRR_PI_4, (radius + line_width)) * 1.5f);

  ctx->radius = radius;
  ctx->line_width = line_width;

  darray_init(&ctx->point1_list_45, size1, NULL, NULL);
  darray_init(&ctx->point2_list_45, size2, NULL, NULL);
  darray_init(&ctx->tmp_point1_list_45, size1, NULL, NULL);
  darray_init(&ctx->tmp_point2_list_45, size2, NULL, NULL);

  darray_init(&ctx->line1_point_list, radius, NULL, NULL);
  darray_init(&ctx->line2_point_list, (radius + line_width), NULL, NULL);

  return ctx;
}

void draw_circle_fill_destroy(draw_circle_t *ctx) {
  if (ctx != NULL) {
    darray_deinit(&ctx->point1_list_45);
    darray_deinit(&ctx->point2_list_45);
    darray_deinit(&ctx->line1_point_list);
    darray_deinit(&ctx->line2_point_list);
    darray_deinit(&ctx->tmp_point1_list_45);
    darray_deinit(&ctx->tmp_point2_list_45);

    TKMEM_FREE(ctx);
  }
}

void draw_circle_fill_arc_width(draw_circle_t *ctx, canvas_t *c, int32_t xc,
                                int32_t yc, int32_t start_angle,
                                int32_t end_angle, bool_t is_ccw,
                                rgba_t *color) {
  if (ctx != NULL) {
    frr_image_info_t image_info;
    quadrant_angle_info_t quadrant_info_list[8];
    color_t tmp_color = color_init(color->r, color->g, color->b, color->a);

    memset(quadrant_info_list, 0, sizeof(quadrant_info_list));

    canvas_set_fill_color(c, tmp_color);

    widget_image_info_create(&image_info, c);

    draw_circle_get_quadrant_angle_info_list(quadrant_info_list, start_angle,
                                             end_angle, is_ccw);

    draw_circle_draw_circle_arc(&image_info, c, xc, yc, ctx, quadrant_info_list,
                                is_ccw, color);

    draw_circle_draw_circle_arc_width_all(&image_info, c, xc, yc, ctx,
                                          quadrant_info_list, color);
  }
}
