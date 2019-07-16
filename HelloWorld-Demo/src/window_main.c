/**
 * File:   edit.c
 * Author: AWTK Develop Team
 * Brief:  window main
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
 * 2018-11-20 TangJunJie created
 *
 */
#include "awtk.h"
extern ret_t application_init(void);
/**
 * Label文本的数值 + offset
 */
static ret_t label_add(widget_t* label, int32_t offset) {
  if (label) {
    int32_t val = 0;
    if (wstr_to_int(&(label->text), &val) == RET_OK) {
      char text[32];
      val += offset;
      val = tk_max(-200, tk_min(val, 200));
      tk_snprintf(text, sizeof(text), "%d", val);
      widget_set_text_utf8(label, text);

      return RET_OK;
    }
  }

  return RET_FAIL;
}

/**
 * 递增按钮事件
 */
static ret_t on_inc_click(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* label = widget_lookup(win, "label_4_btn", TRUE);
  label_add(label, 1);

  return RET_OK;
}

/**
 * 递减按钮事件
 */
static ret_t on_dec_click(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* label = widget_lookup(win, "label_4_btn", TRUE);
  label_add(label, -1);

  return RET_OK;
}

/**
 * 正在编辑事件
 */
static ret_t on_changing(void* ctx, event_t* evt) {
  widget_t* target = WIDGET(evt->target);
  widget_t* win = WIDGET(ctx);
  widget_t* label = widget_lookup(win, "label_4_edit", TRUE);
  widget_set_text(label, target->text.str);

  return RET_OK;
}

/**
 * 初始化
 */
ret_t application_init() {
  widget_t* win = window_create(NULL, 0, 0, 0, 0);

  char height[] = "40";
  int screen_width = win->w;
  int screen_height = win->h;
  char label_width[] = "70", label_height[] = "20";
  char label_text[32], right_bottom_x[32], right_bottom_y[32];
  tk_snprintf(label_text, sizeof(label_text) - 1, "%d * %d", screen_width, screen_height);

  /* 创建文本框*/
  widget_t* label_4_edit = label_create(win, 0, 0, 0, 0);
  widget_set_text(label_4_edit, L"hello world");
  widget_set_name(label_4_edit, "label_4_edit");
  widget_set_self_layout_params(label_4_edit, "center", "20%", "60%", height);

  /* 创建编辑框 */
  widget_t* edit = edit_create(win, 0, 0, 0, 0);
  edit_set_input_type(edit, INPUT_EMAIL);
  widget_set_text(edit, L"hello world");
  widget_set_self_layout_params(edit, "center", "40%", "60%", height);
  widget_on(edit, EVT_VALUE_CHANGING, on_changing, win);

  /* 创建递减按钮 */
  widget_t* dec_btn = button_create(win, 0, 0, 0, 0);
  widget_set_text(dec_btn, L"dec");
  widget_set_self_layout_params(dec_btn, "20%", "60%", "20%", height);
  widget_on(dec_btn, EVT_CLICK, on_dec_click, win);

  /* 创建label显示递增数值 */
  widget_t* label_4_btn = label_create(win, 0, 0, 0, 0);
  widget_set_text(label_4_btn, L"88");
  widget_set_name(label_4_btn, "label_4_btn");
  widget_set_self_layout_params(label_4_btn, "center", "60%", "20%", height);

  /* 创建递增按钮 */
  widget_t* inc_btn = button_create(win, 0, 0, 0, 0);
  widget_set_text(inc_btn, L"inc");
  widget_set_self_layout_params(inc_btn, "60%", "60%", "20%", height);
  widget_on(inc_btn, EVT_CLICK, on_inc_click, win);

  /* 创建左上角label显示屏幕分辨率 */
  widget_t* left_top = label_create(win, 0, 0, 0, 0);
  widget_set_text_utf8(left_top, label_text);
  widget_set_self_layout_params(left_top, "0", "0", label_width, label_height);
  widget_use_style(left_top, "left_top");

  /* 创建右下角label显示屏幕分辨率 */
  widget_t* right_bottom = label_create(win, 0, 0, 0, 0);
  widget_set_text_utf8(right_bottom, label_text);
  tk_itoa(right_bottom_x, sizeof(right_bottom_x), screen_width - tk_atoi(label_width));
  tk_itoa(right_bottom_y, sizeof(right_bottom_y), screen_height - tk_atoi(label_height));
  widget_set_self_layout_params(right_bottom, right_bottom_x, right_bottom_y, label_width,
                                label_height);
  widget_use_style(right_bottom, "right_bottom");

  widget_layout(win);

  return RET_OK;
}
