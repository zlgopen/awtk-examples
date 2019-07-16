/**
 * File:   window_calibration.c
 * Author: AWTK Develop Team
 * Brief:  calibration window
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
 * 2018-10-29 TangJunJie created
 *
 */

#include "awtk.h"

extern ret_t application_init(void);

static ret_t init_widget(void* ctx, const void* iter) {
  (void)iter;
  (void)ctx;

  return RET_OK;
}

static void init_children_widget(widget_t* widget) {
  widget_foreach(widget, init_widget, widget);
}

/**
 * 校准完成回调函数
 */
ret_t on_calibration_win__done(void* ctx, point_t points[4]) {
  return application_init();
}

/**
 * 校准点击事件回调函数
 */
ret_t on_calibration_win_click(void* ctx, uint32_t index, point_t p) {
  return RET_OK;
}

ret_t open_calibration_win(void) {
  widget_t* win = window_open("calibration_win");

  calibration_win_set_on_done(win, on_calibration_win__done, win);
  calibration_win_set_on_click(win, on_calibration_win_click, win);

  if (win) {
    init_children_widget(win);

    return RET_OK;
  }
  return RET_FAIL;
}
