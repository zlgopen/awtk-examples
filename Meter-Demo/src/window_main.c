/**
 * File:   window_main.c
 * Author: AWTK Develop Team
 * Brief:  main window
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
 * 2018-11-13 TangJunJie created
 *
 */
#include "awtk.h"
#include <time.h>
#include <math.h>
#include "widget_animators/widget_animator_rotation.h"

#define SMALL_RADIUS 97
#define BIG_RADIUS 132
#define SMALL_RADIUS_480 59
#define BIG_RADIUS_480 80

static bool_t g_is_big_lcd = TRUE;

/**
 * 初始化仪表指针
 */
static void init_guage(widget_t* win, int x, int y, int w, int h, char* img_name, float from,
                       float to, int duration, int radius) {
  widget_t* view = NULL;
  widget_t* guage_pointer = NULL;
  int32_t an_x, an_y;
  char anchor_x[10] = {0};
  char anchor_y[10] = {0};
  char layout_params[100] = {0};

  view = view_create(win, x, y, w, h);
  guage_pointer = guage_pointer_create(view, 0, 0, 0, 0);
  if (g_is_big_lcd) {
    if (BIG_RADIUS == radius) {
      tk_snprintf(layout_params, sizeof(layout_params) - 1, "default(x=c, y=32, w=20, h=%d)",
                  radius);
    } else {
      tk_snprintf(layout_params, sizeof(layout_params) - 1, "default(x=c, y=20, w=20, h=%d)",
                  radius);
    }
  } else {
    if (BIG_RADIUS_480 == radius) {
      tk_snprintf(layout_params, sizeof(layout_params) - 1, "default(x=c, y=13, w=12, h=%d)",
                  radius);
    } else {
      tk_snprintf(layout_params, sizeof(layout_params) - 1, "default(x=c, y=10, w=12, h=%d)",
                  radius);
    }
  }
  widget_set_self_layout(guage_pointer, layout_params);
  widget_layout(guage_pointer);
  guage_pointer_set_image(guage_pointer, img_name);

  /*指针图片锚点的横坐标在中心，纵坐标在高度的69%处*/
  an_x = (int32_t)(guage_pointer->w * 0.5);
  an_y = (int32_t)(guage_pointer->h * 0.69);
  tk_snprintf(anchor_x, sizeof(anchor_x), "%dpx", an_x);
  tk_snprintf(anchor_y, sizeof(anchor_y), "%dpx", an_y);
  guage_pointer_set_anchor(guage_pointer, anchor_x, anchor_y);

  char animation_str[100];
  memset(animation_str, 0, sizeof(animation_str));
  tk_snprintf(animation_str, sizeof(animation_str),
              "value(from=%f, to=%f, yoyo_times=0, duration=%d, easing=linear)", from, to,
              duration);
  widget_create_animator(guage_pointer, animation_str);
}

/**
 * 创建仪表
 */
static ret_t create_guage(widget_t* win) {
  char small_image[] = "pointer_small";
  char big_image[] = "pointer_big";
  char small_image_480[] = "pointer_small_480";
  char big_image_480[] = "pointer_big_480";

  if (g_is_big_lcd) {
    /* 这里的宽和高是指仪表盘宽度和高度，x, y指是各个仪表盘在背景图片上的左上标位置 */
    int width = 172;
    int height = 172;

    /* 左边：从上到下 */
    init_guage(win, 113, 3, width, height, small_image, -126, 126, 4500, SMALL_RADIUS);
    init_guage(win, 10, 140, width, height, small_image, -98, 98, 5000, SMALL_RADIUS);
    init_guage(win, 45, 307, width, height, small_image, -86, 86, 4000, SMALL_RADIUS);

    /* 中间 */
    init_guage(win, 282, 3, 242, 242, big_image, -128, 128, 4500, BIG_RADIUS);

    /* 右边：从上到下 */
    init_guage(win, 524, 3, width, height, small_image, -126, 126, 4500, SMALL_RADIUS);
    init_guage(win, 626, 140, width, height, small_image, -98, 98, 5000, SMALL_RADIUS);
    init_guage(win, 592, 307, width, height, small_image, -86, 86, 4000, SMALL_RADIUS);
  } else {
    /* 这里的宽和高是指仪表盘宽度和高度，x, y指是各个仪表盘在背景图片上的左上标位置 */
    int width = 100;
    int height = 100;

    /* 左边：从上到下 */
    init_guage(win, 70, 0, width, height, small_image_480, -126, 126, 4500, SMALL_RADIUS_480);
    init_guage(win, 8, 76, width, height, small_image_480, -80, 80, 5000, SMALL_RADIUS_480);
    init_guage(win, 29, 172, width, height, small_image_480, -86, 86, 4000, SMALL_RADIUS_480);

    /* 中间 */
    init_guage(win, 170, 3, 144, 138, big_image_480, -130, 130, 4500, BIG_RADIUS_480);

    /* 右边：从上到下 */
    init_guage(win, 315, 0, width, height, small_image_480, -126, 126, 4500, SMALL_RADIUS_480);
    init_guage(win, 377, 77, width, height, small_image_480, -80, 80, 5000, SMALL_RADIUS_480);
    init_guage(win, 357, 171, width, height, small_image_480, -86, 86, 4000, SMALL_RADIUS_480);
  }

  return RET_OK;
}

/**
 * 初始化label
 */
static void init_label(widget_t* win, int x, int y, int w, int h, const wchar_t* text,
                       const char* widget_name) {
  widget_t* label = label_create(win, x, y, w, h);
  widget_set_text(label, text);
  widget_set_name(label, widget_name);
  if (g_is_big_lcd == FALSE) {
    widget_use_style(label, "label_480_272");
  }
}

/**
 * 创建label
 */
static ret_t create_labels(widget_t* win) {
  int width = 30;
  int height = 30;

  if (g_is_big_lcd) {
    /* 左边：从上到下 */
    init_label(win, 330, 314, width, height, L"3005", "left_top_label");
    init_label(win, 325, 367, width, height, L"12", "left_center_label");
    init_label(win, 320, 419, width, height, L"13", "left_bottom_label");

    /* 右边：从上到下 */
    init_label(win, 515, 314, width, height, L"14", "right_top_label");
    init_label(win, 522, 367, width, height, L"15", "right_center_label");
    init_label(win, 522, 419, width, height, L"16", "right_bottom_label");
  } else {
    /* 左边：从上到下 */
    init_label(win, 192, 172, width, height, L"3005", "left_top_label");
    init_label(win, 189, 202, width, height, L"12", "left_center_label");
    init_label(win, 186, 231, width, height, L"13", "left_bottom_label");

    /* 右边：从上到下 */
    init_label(win, 304, 171, width, height, L"14", "right_top_label");
    init_label(win, 307, 201, width, height, L"15", "right_center_label");
    init_label(win, 307, 231, width, height, L"16", "right_bottom_label");
  }

  return RET_OK;
}

/**
 * Label文本随机显示int
 */
static void label_set_random_int(widget_t* label, int32_t min_num, int32_t max_num) {
  if (label) {
    char text[32];
    int32_t val = fmod(rand(), max_num - min_num) + min_num;
    widget_set_text_utf8(label, tk_itoa(text, sizeof(text), val));
  }
}

/**
 * 更新Label上的显示数值
 */
static ret_t on_update_label(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);

  /* 左边 */
  widget_t* left_top_label = widget_lookup(win, "left_top_label", TRUE);
  if (left_top_label) {
    label_set_random_int(left_top_label, 3000, 5000);
  }

  widget_t* left_center_label = widget_lookup(win, "left_center_label", TRUE);
  if (left_center_label) {
    label_set_random_int(left_center_label, 10, 30);
  }

  widget_t* left_bottom_label = widget_lookup(win, "left_bottom_label", TRUE);
  if (left_bottom_label) {
    label_set_random_int(left_bottom_label, 1, 15);
  }

  /* 右边 */
  widget_t* right_top_label = widget_lookup(win, "right_top_label", TRUE);
  if (right_top_label) {
    label_set_random_int(right_top_label, 1, 15);
  }

  widget_t* right_center_label = widget_lookup(win, "right_center_label", TRUE);
  if (right_center_label) {
    label_set_random_int(right_center_label, 1, 15);
  }

  widget_t* right_bottom_label = widget_lookup(win, "right_bottom_label", TRUE);
  if (right_bottom_label) {
    label_set_random_int(right_bottom_label, 1, 15);
  }

  return RET_REPEAT;
}

/**
 * 创建背景label
 */
static ret_t create_bg_labels(widget_t* win) {
  value_t w_value;
  value_t h_value;
  widget_get_prop(win, "w", &w_value);
  widget_get_prop(win, "h", &h_value);
  int w = value_int32(&w_value);
  int h = value_int32(&h_value);
  widget_t* bg_label = label_create(win, 0, 0, w, h);
  if (bg_label) {
    widget_use_style(bg_label, "bg");
    if (w == 480) {
      widget_use_style(bg_label, "bg_480_272");
      g_is_big_lcd = FALSE;
    }
  }
  return RET_OK;
}

/**
 * 打开主窗口
 */
ret_t application_init(void) {
  widget_t* win = window_create(NULL, 0, 0, 0, 0);

#if defined(LCD_W) && defined(LCD_H)
  int32_t lcd_w = LCD_W;
  int32_t lcd_h = LCD_H;
#else

  int32_t lcd_w = 800;
  int32_t lcd_h = 480;

#endif

  widget_t* main_win = view_create(win, 0, 0, lcd_w, lcd_h);
  if (main_win) {
    create_bg_labels(main_win);
    create_guage(main_win);
    create_labels(main_win);
    timer_add(on_update_label, main_win, 1000);
#if 0
    widget_t* widget = window_manager();
    window_manager_set_show_fps(widget, TRUE);
#endif
    return RET_OK;
  }

  return RET_FAIL;
}
