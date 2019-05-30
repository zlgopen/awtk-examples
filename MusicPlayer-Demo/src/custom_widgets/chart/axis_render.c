/**
 * File:   axis_render.h
 * Author: AWTK Develop Team
 * Brief:  axis render
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

#include "axis_render.h"
#include "chart_view.h"

#define AXIS_TICK_LEN 4
#define AXIS_LABEL_MARGIN 4

static void axis_render_init(widget_t* widget, canvas_t* c, wh_t w, wh_t h, wh_t margin) {
  uint32_t i, nr;
  axis_t* axis;
  wh_t label_margin = AXIS_LABEL_MARGIN;
  wh_t tick_len = AXIS_TICK_LEN;

  nr = chart_view_count_axis(widget, AXIS_ORIENTATION_X);
  for (i = 0; i < nr; i++) {
    axis = AXIS(chart_view_get_axis(widget, AXIS_ORIENTATION_X, i));
    assert(axis);
    if (axis->render == NULL) {
      axis->render = axis_render_create(axis);
    }

    axis->render->ruler.w = w;
    axis->render->ruler.h = axis->label.show ? axis->label.font.size : 1;
    axis->render->ruler.h += axis->label.show && axis->tick.show ? label_margin : 0;
    axis->render->ruler.h += axis->tick.show ? tick_len : 0;
    if (axis->type == AXIS_TYPE_VALUE) {
      axis->render->ruler.w -= axis->label.show ? axis_measure_label(WIDGET(axis), c) : 0;
    }
  }

  nr = chart_view_count_axis(widget, AXIS_ORIENTATION_Y);
  for (i = 0; i < nr; i++) {
    axis = AXIS(chart_view_get_axis(widget, AXIS_ORIENTATION_Y, i));
    assert(axis);
    if (axis->render == NULL) {
      axis->render = axis_render_create(axis);
    }

    axis->render->ruler.w = axis->label.show ? axis_measure_label(WIDGET(axis), c) : 1;
    axis->render->ruler.w += axis->label.show && axis->tick.show ? label_margin : 0;
    axis->render->ruler.w += axis->tick.show ? tick_len : 0;
    axis->render->ruler.h = h;
    if (axis->type == AXIS_TYPE_VALUE) {
      axis->render->ruler.h -= axis->label.show ? axis->label.font.size : 0;
    }
  }
}

static void axis_render_tune(widget_t* widget, canvas_t* c, wh_t w, wh_t h, wh_t margin) {
  axis_t* axis = NULL;
  bool_t has_top = FALSE;
  bool_t has_bottom = FALSE;
  bool_t has_left = FALSE;
  bool_t has_right = FALSE;
  wh_t l_x_axis_w = 0;
  wh_t r_x_axis_w = 0;
  wh_t t_y_axis_h = 0;
  wh_t b_y_axis_h = 0;
  wh_t label_w = 0;
  uint16_t axis_nr, tick_nr;
  uint16_t i, j;

  axis_nr = chart_view_count_axis(widget, AXIS_ORIENTATION_X);
  for (i = 0; i < axis_nr; i++) {
    axis = AXIS(chart_view_get_axis(widget, AXIS_ORIENTATION_X, i));
    assert(axis);
    if (axis->at == AXIS_AT_TOP) {
      has_top = TRUE;
      t_y_axis_h = tk_max(t_y_axis_h, axis->render->ruler.h);
    } else {
      has_bottom = TRUE;
      b_y_axis_h = tk_max(b_y_axis_h, axis->render->ruler.h);
    }

    if (axis->type == AXIS_TYPE_VALUE) {
      label_w = axis_measure_label(WIDGET(axis), c) / 2.0;
      l_x_axis_w = tk_max(l_x_axis_w, label_w);
      r_x_axis_w = tk_max(r_x_axis_w, label_w);
    }
  }

  axis_nr = chart_view_count_axis(widget, AXIS_ORIENTATION_Y);
  for (i = 0; i < axis_nr; i++) {
    axis = AXIS(chart_view_get_axis(widget, AXIS_ORIENTATION_Y, i));
    assert(axis);
    if (axis->at == AXIS_AT_RIGHT) {
      has_right = TRUE;
      r_x_axis_w = tk_max(r_x_axis_w, axis->render->ruler.w);
    } else {
      has_left = TRUE;
      l_x_axis_w = tk_max(l_x_axis_w, axis->render->ruler.w);
    }

    if (axis->type == AXIS_TYPE_VALUE) {
      t_y_axis_h = tk_max(t_y_axis_h, axis->label.font.size / 2);
      b_y_axis_h = tk_max(b_y_axis_h, axis->label.font.size / 2);
    }
  }

  axis_nr = chart_view_count_axis(widget, AXIS_ORIENTATION_X);
  for (i = 0; i < axis_nr; i++) {
    axis = AXIS(chart_view_get_axis(widget, AXIS_ORIENTATION_X, i));
    assert(axis);

    tick_nr = axis->label.data ? axis->label.data->size : 0;
    if (axis->type == AXIS_TYPE_CATEGORY) {
      tick_nr++;
    }
    tick_nr = tk_max(2, tick_nr);

    if (axis->render->tick == NULL) {
      axis->render->tick = TKMEM_ZALLOCN(float_t, tick_nr);
    } else {
      axis->render->tick = TKMEM_REALLOCT(float_t, axis->render->tick, tick_nr);
    }
    assert(axis->render->tick);

    if (axis->at == AXIS_AT_TOP) {
      axis->render->ruler.x = (l_x_axis_w ? (l_x_axis_w - 1) : 0) + margin;
      axis->render->ruler.y = margin;
      axis->render->ruler.w = w - l_x_axis_w - r_x_axis_w + (l_x_axis_w > 0) + (r_x_axis_w > 0);
      axis->render->ruler.h = t_y_axis_h;
    } else {
      axis->render->ruler.x = (l_x_axis_w ? (l_x_axis_w - 1) : 0) + margin;
      axis->render->ruler.y = h - b_y_axis_h + 1 - margin;
      axis->render->ruler.w = w - l_x_axis_w - r_x_axis_w + (l_x_axis_w > 0) + (r_x_axis_w > 0);
      axis->render->ruler.h = b_y_axis_h;
    }

    axis->render->split_line_len = h - t_y_axis_h - b_y_axis_h + 2;
    axis->render->show_1st_split_line = !has_left;
    axis->render->show_last__split_line = !has_right;

    for (j = 0; j < tick_nr; j++) {
      axis->render->tick[j] = (float_t)axis->render->ruler.w * (float_t)j / (float_t)(tick_nr - 1);
    }
  }

  axis_nr = chart_view_count_axis(widget, AXIS_ORIENTATION_Y);
  for (i = 0; i < axis_nr; i++) {
    axis = AXIS(chart_view_get_axis(widget, AXIS_ORIENTATION_Y, i));
    assert(axis);

    tick_nr = axis->label.data ? axis->label.data->size : 0;
    if (axis->type == AXIS_TYPE_CATEGORY) {
      tick_nr++;
    }
    tick_nr = tk_max(2, tick_nr);

    if (axis->render->tick == NULL) {
      axis->render->tick = TKMEM_ZALLOCN(float_t, tick_nr);
    } else {
      axis->render->tick = TKMEM_REALLOCT(float_t, axis->render->tick, tick_nr);
    }
    assert(axis->render->tick);

    if (axis->at == AXIS_AT_RIGHT) {
      axis->render->ruler.x = w - r_x_axis_w + 1 - margin;
      axis->render->ruler.y = (t_y_axis_h ? (t_y_axis_h - 1) : 0) + margin;
      axis->render->ruler.w = r_x_axis_w;
      axis->render->ruler.h = h - t_y_axis_h - b_y_axis_h + (t_y_axis_h > 0) + (b_y_axis_h > 0);
    } else {
      axis->render->ruler.x = margin;
      axis->render->ruler.y = (t_y_axis_h ? (t_y_axis_h - 1) : 0) + margin;
      axis->render->ruler.w = l_x_axis_w;
      axis->render->ruler.h = h - t_y_axis_h - b_y_axis_h + (t_y_axis_h > 0) + (b_y_axis_h > 0);
    }

    axis->render->split_line_len = w - l_x_axis_w - r_x_axis_w + 2;
    axis->render->show_1st_split_line = !has_top;
    axis->render->show_last__split_line = !has_bottom;

    for (j = 0; j < tick_nr; j++) {
      axis->render->tick[j] = (float_t)axis->render->ruler.h * (float_t)j / (float_t)(tick_nr - 1);
    }
  }
}

ret_t axis_render_layout(widget_t* widget, canvas_t* c, wh_t margin) {
  wh_t w = widget->w - 2 * margin;
  wh_t h = widget->h - 2 * margin;
  return_value_if_fail(widget != NULL && c != NULL && w > 0 && h > 0, RET_BAD_PARAMS);

  axis_render_init(widget, c, w, h, margin);
  axis_render_tune(widget, c, w, h, margin);

  return RET_OK;
}

void axis_render_draw_tick_x(canvas_t* c, float_t x, float_t y, float_t* tick, uint32_t tick_nr,
                             float_t tick_len, axis_tick_params_t* param) {
  if (param->show) {
    uint32_t i;
    float_t offset = 0.0;

    if (param->align_with_label) {
      offset = (tick[1] - tick[0]) / 2.0;
      tick_nr--;
    }

    canvas_set_stroke_color(c, param->style.color);
    for (i = 0; i < tick_nr; i++) {
      canvas_draw_vline(c, x + tick[i] + offset, y, tick_len);
    }
  }
}

void axis_render_draw_tick_y(canvas_t* c, float_t x, float_t y, float_t* tick, uint32_t tick_nr,
                             float_t tick_len, axis_tick_params_t* param) {
  if (param->show) {
    uint32_t i;
    float_t offset = 0.0;

    if (param->align_with_label) {
      offset = (tick[1] - tick[0]) / 2.0;
      tick_nr--;
    }

    canvas_set_stroke_color(c, param->style.color);
    for (i = 0; i < tick_nr; i++) {
      canvas_draw_hline(c, x, y + tick[i] + offset, tick_len);
    }
  }
}

void axis_render_draw_line_x(canvas_t* c, float_t x, float_t y, float_t w, float_t h,
                             axis_line_params_t* param) {
  if (param->show) {
    canvas_set_stroke_color(c, param->style.color);
    canvas_draw_hline(c, x, y, w);
  }
}

void axis_render_draw_line_y(canvas_t* c, float_t x, float_t y, float_t w, float_t h,
                             axis_line_params_t* param) {
  if (param->show) {
    canvas_set_stroke_color(c, param->style.color);
    canvas_draw_vline(c, x, y, h);
  }
}

void axis_render_draw_split_line_x(canvas_t* c, float_t x, float_t y, float_t* tick,
                                   uint32_t tick_nr, float_t split_line_len,
                                   axis_split_line_params_t* param, bool_t show_1st,
                                   bool_t show_last) {
  if (param->show) {
    uint32_t i;
    canvas_set_stroke_color(c, param->style.color);
    for (i = 0; i < tick_nr; i++) {
      if ((i == 0 && !show_1st) || (i == tick_nr - 1 && !show_last)) {
        continue;
      }
      canvas_draw_vline(c, x + tick[i], y, split_line_len);
    }
  }
}

void axis_render_draw_split_line_y(canvas_t* c, float_t x, float_t y, float_t* tick,
                                   uint32_t tick_nr, float_t split_line_len,
                                   axis_split_line_params_t* param, bool_t show_1st,
                                   bool_t show_last) {
  if (param->show) {
    uint32_t i;
    canvas_set_stroke_color(c, param->style.color);
    for (i = 0; i < tick_nr; i++) {
      if ((i == 0 && !show_1st) || (i == tick_nr - 1 && !show_last)) {
        continue;
      }
      canvas_draw_hline(c, x, y + tick[i], split_line_len);
    }
  }
}

void axis_render_draw_label_x(canvas_t* c, float_t x, float_t y, float_t w, float_t h,
                              float_t* tick, uint32_t tick_nr, float_t tick_len, float_t margin,
                              axis_label_params_t* param, axis_at_type_t at, axis_type_t type) {
  if (param->show && param->data) {
    const wstr_t** labels = (const wstr_t**)(param->data->elms);
    float_t offset = 0.0;
    uint32_t i;

    if (w < 0) {
      w = -w;
      x -= w - 1;
    }

    if (h < 0) {
      h = -h;
      y -= h - 1;
    }

    if (type == AXIS_TYPE_CATEGORY) {
      tick_nr--;
    } else {
      offset = (tick[1] - tick[0]) / 2.0;
    }

    canvas_set_text_color(c, param->font.color);
    canvas_set_font(c, param->font.name ? param->font.name : "default", param->font.size);
    if (at == AXIS_AT_TOP) {
      canvas_set_text_align(c, ALIGN_H_CENTER, ALIGN_V_BOTTOM);
      h -= -tick_len + margin;
    } else {
      canvas_set_text_align(c, ALIGN_H_CENTER, ALIGN_V_TOP);
      y += tick_len + margin;
    }
    rect_t rect = rect_init(x, y, tick[1] - tick[0], h);
    for (i = 0; i < tick_nr; i++) {
      rect.x = x + tick[i] - offset;
      canvas_draw_text_in_rect(c, labels[i]->str, labels[i]->size, &rect);
    }
  }
}

void axis_render_draw_label_y(canvas_t* c, float_t x, float_t y, float_t w, float_t h,
                              float_t* tick, uint32_t tick_nr, float_t tick_len, float_t margin,
                              axis_label_params_t* param, axis_at_type_t at, axis_type_t type) {
  if (param->show && param->data) {
    const wstr_t** labels = (const wstr_t**)(param->data->elms);
    float_t offset = 0.0;
    uint32_t i;

    if (w < 0) {
      w = -w;
      x -= w - 1;
    }

    if (h < 0) {
      h = -h;
      y -= h - 1;
    }

    if (type == AXIS_TYPE_CATEGORY) {
      tick_nr--;
    } else {
      offset = (tick[1] - tick[0]) / 2.0;
    }

    canvas_set_text_color(c, param->font.color);
    canvas_set_font(c, param->font.name ? param->font.name : "default", param->font.size);
    if (at == AXIS_AT_RIGHT) {
      canvas_set_text_align(c, ALIGN_H_LEFT, ALIGN_V_MIDDLE);
      x += tick_len + margin;
    } else {
      canvas_set_text_align(c, ALIGN_H_RIGHT, ALIGN_V_MIDDLE);
      w -= -tick_len + margin;
    }
    rect_t rect = rect_init(x, y, w, tick[1] - tick[0]);
    for (i = 0; i < tick_nr; i++) {
      rect.y = y + tick[i] - offset;
      canvas_draw_text_in_rect(c, labels[i]->str, labels[i]->size, &rect);
    }
  }
}

ret_t axis_render_on_paint(widget_t* widget, canvas_t* c) {
  float_t x, y, w, h, tick_len, split_line_len;
  uint32_t tick_nr;
  float_t* tick;
  axis_type_t type;
  axis_orientation_t orientation;
  axis_at_type_t at;
  axis_t* axis = AXIS(axis_cast(widget));
  return_value_if_fail(axis != NULL && c != NULL && axis->render != NULL, RET_BAD_PARAMS);

  tick_len = AXIS_TICK_LEN;
  x = axis->render->ruler.x;
  y = axis->render->ruler.y;
  w = axis->render->ruler.w;
  h = axis->render->ruler.h;
  split_line_len = axis->render->split_line_len;
  tick = axis->render->tick;
  tick_nr = axis->label.data ? axis->label.data->size : 0;
  type = axis->type;
  orientation = axis->orientation;
  at = axis->at;

  if (type == AXIS_TYPE_CATEGORY) {
    tick_nr++;
  }
  tick_nr = tk_max(2, tick_nr);

  if (orientation == AXIS_ORIENTATION_X) {
    if (at == AXIS_AT_TOP) {
      y = y + h - 1;
      h = -h;
      tick_len = -tick_len;
    } else {
      split_line_len = -split_line_len;
    }

    axis_render_draw_split_line_x(c, x, y, tick, tick_nr, split_line_len, &(axis->split_line),
                                  axis->render->show_1st_split_line,
                                  axis->render->show_last__split_line);
    axis_render_draw_tick_x(c, x, y, tick, tick_nr, tick_len, &(axis->tick));
    axis_render_draw_line_x(c, x, y, w, h, &(axis->line));
    axis_render_draw_label_x(c, x, y, w, h, tick, tick_nr, axis->tick.show ? tick_len : 0,
                             AXIS_LABEL_MARGIN, &(axis->label), at, type);
  } else if (orientation == AXIS_ORIENTATION_Y) {
    if (at == AXIS_AT_RIGHT) {
      split_line_len = -split_line_len;
    } else {
      x = x + w - 1;
      w = -w;
      tick_len = -tick_len;
    }

    axis_render_draw_split_line_y(c, x, y, tick, tick_nr, split_line_len, &(axis->split_line),
                                  axis->render->show_1st_split_line,
                                  axis->render->show_last__split_line);
    axis_render_draw_tick_y(c, x, y, tick, tick_nr, tick_len, &(axis->tick));
    axis_render_draw_line_y(c, x, y, w, h, &(axis->line));
    axis_render_draw_label_y(c, x, y, w, h, tick, tick_nr, axis->tick.show ? tick_len : 0,
                             AXIS_LABEL_MARGIN, &(axis->label), at, type);
  }

  return RET_OK;
}

axis_render_t* axis_render_create() {
  axis_render_t* render = TKMEM_ZALLOC(axis_render_t);
  return_value_if_fail(render != NULL, NULL);

  return render;
}

ret_t axis_render_destroy(axis_render_t* render) {
  return_value_if_fail(render != NULL, RET_BAD_PARAMS);

  if (render->tick) {
    TKMEM_FREE(render->tick);
  }
  memset(render, 0x00, sizeof(axis_render_t));
  TKMEM_FREE(render);

  return RET_OK;
}
