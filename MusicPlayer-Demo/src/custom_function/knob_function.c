/**
 * File:   knob_opera.c
 * Author: AWTK Develop Team
 * Brief:  knob function
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
 * 2019-02-20 ZhongWei1  created
 *
 */
#include "knob_function.h"
#include <stdio.h>

#define PI 3.1415926535897932
#define BUFF_LEN 32

ret_t knob_angle_change(widget_t* circle, widget_t* knob_pointer, point_t p) {
  value_t val;
  value_t val_angle;
  value_t val_center_x;
  value_t val_center_y;
  value_t val_start_P;

  widget_get_prop(knob_pointer, "val_start_P", &val_start_P);
  widget_get_prop(knob_pointer, "val_angle", &val_angle);
  if (widget_get_prop(knob_pointer, "val_center_x", &val_center_x) != RET_OK)
    value_set_int32(&val_center_x, 0);
  if (widget_get_prop(knob_pointer, "val_center_y", &val_center_y) != RET_OK)
    value_set_int32(&val_center_y, 0);

  value_t val_pointer_name;
  int32_t ax = p.x - value_int32(&val_center_x);
  int32_t ay = value_int32(&val_center_y) - p.y;
  int32_t mv = atan2(ax, ay) * 180 / PI;

  widget_get_prop(knob_pointer, "val_pointer_name", &val_pointer_name);

  if (tk_str_eq(value_str(&val_pointer_name), knob_pointer->name) &&
      value_str(&val_pointer_name) != NULL) {
    int32_t angle_temp = value_int32(&val_angle) + (mv - value_int32(&val_start_P));
    if ((widget_get_prop(knob_pointer, "s_pmove", &val) == RET_OK && value_bool(&val) == TRUE)) {
      if (mv > 150 || mv < -150) {
        value_set_bool(&val, FALSE);
        value_set_int32(&val_angle, angle_temp);
        widget_set_prop(knob_pointer, "s_pmove", &val);
        widget_set_prop(knob_pointer, "val_angle", &val_angle);
        return RET_OK;
      }
      if (angle_temp > 120) angle_temp = 120;
      if (angle_temp < -120) angle_temp = -120;
      guage_pointer_set_angle(knob_pointer, angle_temp);
      progress_circle_set_value(circle, angle_temp + 120);
    } else {
      if (mv <= 150 && mv >= -150) {
        value_set_bool(&val, TRUE);
        value_set_int32(&val_start_P, mv);
        widget_set_prop(knob_pointer, "s_pmove", &val);
        widget_set_prop(knob_pointer, "val_start_P", &val_start_P);
      }
    }
  }

  return RET_OK;
}

ret_t knob_down_init(widget_t* knob_pointer, point_t p, char* wid_type) {
  widget_t* win = widget_get_window(knob_pointer);
  char buf[BUFF_LEN] = {0};
  widget_t* knob_view = widget_lookup(win, "guage_pointer_view", TRUE);
  sprintf(buf, "guage_%s", wid_type);
  widget_t* knob = widget_lookup(win, buf, TRUE);

  value_t val;
  value_t val_angle;
  value_t val_start_P;
  value_t val_center_x;
  value_t val_center_y;
  value_t val_pointer_name;
  value_set_bool(&val, TRUE);
  value_set_str(&val_pointer_name, knob_pointer->name);
  value_set_int32(&val_angle, widget_get_value(knob_pointer));
  value_set_int32(&val_center_x, knob_view->x + knob->x + knob_pointer->x + knob_pointer->w / 2);
  value_set_int32(&val_center_y, knob_view->y + knob->y + knob_pointer->y + knob_pointer->h / 2);

  widget_set_prop(knob_pointer, "s_pmove", &val);
  widget_set_prop(knob_pointer, "val_angle", &val_angle);
  widget_set_prop(knob_pointer, "val_pointer_name", &val_pointer_name);
  widget_set_prop(knob_pointer, "val_center_x", &val_center_x);
  widget_set_prop(knob_pointer, "val_center_y", &val_center_y);

  int32_t px = p.x - value_int32(&val_center_x);
  int32_t py = value_int32(&val_center_y) - p.y;
  value_set_int32(&val_start_P, atan2(px, py) * 180 / PI);
  widget_set_prop(knob_pointer, "val_start_P", &val_start_P);

  return RET_OK;
}