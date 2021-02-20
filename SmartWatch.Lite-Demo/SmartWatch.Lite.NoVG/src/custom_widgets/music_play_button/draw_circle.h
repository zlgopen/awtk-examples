#ifndef __DRAW_CIRCL_H__
#define __DRAW_CIRCL_H__

#include "awtk.h"

#ifdef WITH_NANOVG_GL
#error "do not support OpenGL, please use AGGE!"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _draw_circle_t {
  int32_t radius;
  int32_t line_width;

  darray_t point1_list_45;
  darray_t point2_list_45;

  darray_t line1_point_list;
  darray_t line2_point_list;

  void *point_pix_cache1_list;
  void *point_pix_cache2_list;

} draw_circle_t;

draw_circle_t *draw_circle_fill_create(int32_t radius, int32_t line_width);

void draw_circle_fill_destroy(draw_circle_t *ctx);

void draw_circle_fill_arc_width(draw_circle_t *ctx, canvas_t *c, int32_t xc,
                                int32_t yc, int32_t start_angle,
                                int32_t end_angle, bool_t is_ccw,
                                rgba_t *color);

#ifdef __cplusplus
}
#endif

#endif /* NANOVG_AGGE_H */
