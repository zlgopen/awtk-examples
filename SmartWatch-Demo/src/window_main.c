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
 * 2019-03-01 LiangJunquan created
 *
 */

#include "window_main.h"
#include "tkc/date_time.h"
#include "custom_widgets/gesture.h"
#include "custom_widgets/bar_graph.h"
#include "custom_widgets/health_circle.h"
#include "custom_widgets/guage_hour_pointer.h"
#include "custom_widgets/guage_minute_pointer.h"
#include "custom_widgets/guage_second_pointer.h"
#include "common/global.h"
#include "window_main.h"
#include "common/log.h"
#include "custom_widgets/custom_widgets.h"
#include "slide_appliction/app.h"
#include "watch/watch_01.h"
#include "watch/watch_02.h"
#include "watch/watch_04.h"
#include "watch/watch_06.h"
#include "slide_appliction/quick.h"

extern ret_t open_message(void* ctx, event_t* e);
extern ret_t on_open_quick(void* ctx, event_t* e);

static ret_t on_dial_long_pressed(void* ctx, event_t* e);
static ret_t on_window_close(void* ctx, event_t* e);

// 部件指针数组
widget_t** widget_pointer = NULL;

// 窗口名
static const char* s_window_select_dial = "select_dial";

#define WATCH_INIT(NUM)                                                    \
  win = window_open_and_close(name, window);                               \
  if (win != NULL) {                                                       \
    if (str_eq(&str, g_window_enum->quick.str)) {                          \
      widget_set_prop_str(win, WIDGET_PROP_OPEN_ANIM_HINT, "slide_down");  \
    } else {                                                               \
      widget_set_prop_str(win, WIDGET_PROP_OPEN_ANIM_HINT, "slide_right"); \
    }                                                                      \
    watch_0##NUM##_init(win);                                              \
  }

widget_t* open_watch_and_close(const char* name, widget_t* window) {
  widget_t* win = NULL;
  str_t str;
  str_init(&str, 0);
  str_set(&str, window->name);
  if (tk_str_eq(name, g_window_enum->watch_01.str)) {
    WATCH_INIT(1)
  } else if (tk_str_eq(name, g_window_enum->watch_04.str)) {
    WATCH_INIT(4)
  } else if (tk_str_eq(name, g_window_enum->watch_02.str)) {
    WATCH_INIT(2)
  } else if (tk_str_eq(name, g_window_enum->watch_06.str)) {
    WATCH_INIT(6)
  } else {
    LOG("name:%s", name);
    assert(FALSE);
  }
  str_reset(&str);
  return win;
}

#undef WATCH_INIT

// 选择不同表盘
static ret_t on_watch_click(void* ctx, event_t* e) {
  (void)e;
  widget_t* widget = WIDGET(ctx);
  widget_t* window = widget_get_window(widget);
  const char* name = widget->name;
  LOG("%s", name);
  widget_t* win = open_watch_and_close(name, window);
  widget_set_prop_str(win, WIDGET_PROP_OPEN_ANIM_HINT, "slide_left");
  return RET_OK;
}

static ret_t on_slideview_visit(void* ctx, const void* data) {
  widget_t* widget = WIDGET(data);
  widget_t* window = WIDGET(ctx);
  (void)window;
  const char* name = widget->name;
  LOG("name:%s", name);
  LOG("name:%s", window);
  if (tk_str_eq(name, g_window_enum->watch_01.str)) {
    widget_on(widget, EVT_CLICK, on_watch_click, widget);
  } else if (tk_str_eq(name, g_window_enum->watch_02.str)) {
    widget_on(widget, EVT_CLICK, on_watch_click, widget);
  } else if (tk_str_eq(name, g_window_enum->watch_04.str)) {
    widget_on(widget, EVT_CLICK, on_watch_click, widget);
  } else if (tk_str_eq(name, g_window_enum->watch_06.str)) {
    widget_on(widget, EVT_CLICK, on_watch_click, widget);
  }
  return RET_OK;
}

/**
 * ctx:主界面窗口指针
 * 长按更换表盘
 */
static ret_t on_dial_long_pressed(void* ctx, event_t* e) {
  log_debug("on_dial_long_pressed!!\n");
  (void)e;
  widget_t* window = widget_get_window(WIDGET(ctx));
  widget_t* win = window_open_and_close("select_dial", window);
  if (win != NULL) {
    /* 初始化更换表盘界面控件 */
    /* select 界面的选择按钮的点击事件在此处注册 */
    widget_t* digit_btn = widget_lookup(win, "select:digit_btn", TRUE);
    widget_t* time_btn = widget_lookup(win, "select:time_btn", TRUE);
    widget_t* view0 = widget_lookup(win, "select:digit_btns", TRUE);
    widget_t* view1 = widget_lookup(win, "select:time_btns", TRUE);
    (void)view1;
    (void)view0;
    (void)time_btn;
    (void)digit_btn;

    widget_on(win, EVT_WINDOW_CLOSE, on_window_close, win);

    widget_t* slideview = widget_lookup(win, "select:slideview", TRUE);
    if (slideview != NULL) {
      widget_foreach(slideview, on_slideview_visit, win);
      slide_menu_set_value(slideview, global_data.watch_type);
    } else {
      assert(FALSE);
    }
  }
  return RET_OK;
}

static ret_t on_paint_fg(void* ctx, event_t* e) {
  canvas_t* c = ((paint_event_t*)e)->c;
  bitmap_t img;
  widget_t* wm = WIDGET(ctx);
  widget_t* win = window_manager_get_top_window(wm);

  widget_load_image(wm, "ico_background2", &img);

  rect_t s = {0, 0, img.w, img.h};

  system_info_t* sys_info = system_info();
  uint32_t lcd_w = sys_info->lcd_w;
  uint32_t lcd_h = sys_info->lcd_h;

  if (win->w == 390) {
    rect_t d = {189, 0, img.w, img.h};
    canvas_draw_image(c, &img, &s, &d);
    canvas_save(c);
    canvas_set_fill_color_str(c, "black");
    canvas_fill_rect(c, 0, 0, 189, lcd_h);

    canvas_fill_rect(c, 629, 0, lcd_w - 629, lcd_h);
    canvas_fill_rect(c, 0, 480, lcd_w, lcd_h - 480);

    canvas_restore(c);
  } else if (win->w == 240) {
    rect_t d = {(480 - 272) / 2 + 6, 0, img.w, img.h};
    canvas_draw_image(c, &img, &s, &d);
    canvas_save(c);
    canvas_set_fill_color_str(c, "black");
    canvas_fill_rect(c, 0, 0, (480 - 272) / 2 + 6, 272);
    canvas_fill_rect(c, 480 - (480 - 272) / 2 + 6, 0, (480 - 272) / 2 - 6, 272);
    canvas_restore(c);
  } else {
    assert(FALSE);
  }

  return RET_OK;
}

ret_t application_deinit(void* ctx, event_t* e) {
  (void)ctx;
  (void)e;
  deinit_global();
  return RET_OK;
}

/**
 * 初始化
 */
ret_t application_init() {
  widget_on(window_manager(), EVT_AFTER_PAINT, on_paint_fg, window_manager());
  widget_on(window_manager(), EVT_DESTROY, application_deinit, window_manager());
  // window_manager_set_show_fps(window_manager(), TRUE);

  custom_widgets_init();
  init_global();

  widget_t* win = window_open(g_window_enum->watch_01.str);
  if (win != NULL) {
    watch_01_init(win);
    widget_layout(win);
  }

  return RET_OK;
}

static ret_t on_window_close(void* ctx, event_t* e) {
  (void)e;
  widget_t* window = WIDGET(ctx);
  LOG("name:%s", window->name);

  str_set(&(global_data.previous_window), window->name);

  const char* name = window->name;
  if (tk_str_eq(name, g_window_enum->watch_01.str)) {
    widget_unload_image_by_name(window, "ico_watch_background_01");
  } else if (tk_str_eq(name, g_window_enum->watch_02.str)) {
    widget_unload_image_by_name(window, "ico_watch_background_02");
  } else if (tk_str_eq(name, g_window_enum->watch_04.str)) {
    widget_unload_image_by_name(window, "ico_watch_background_04");
  } else if (tk_str_eq(name, g_window_enum->watch_06.str)) {
    widget_unload_image_by_name(window, "ico_watch_background_06");
  } else if (tk_str_eq(name, s_window_select_dial)) {
    char* img[] = {"ico_watch_01", "ico_watch_02", "ico_watch_04", "ico_watch_06"};
    size_t size = ARRAY_SIZE(img);
    size_t i = 0;
    for (i = 0; i < size; i++) {
      widget_unload_image_by_name(window, img[i]);
    }
  }

  image_manager_unload_unused(image_manager(), 0);

  return RET_OK;
}

static ret_t on_change_watch(void* ctx, event_t* e) {
  (void)e;
  widget_t* to_close = WIDGET(ctx);
  switch (global_data.watch_type) {
    case WATCH_04: {
      open_watch_and_close(g_window_enum->watch_01.str, to_close);
      break;
    }
    case WATCH_01: {
      open_watch_and_close(g_window_enum->watch_02.str, to_close);
      break;
    }
    case WATCH_02: {
      open_watch_and_close(g_window_enum->watch_06.str, to_close);
      break;
    }
    case WATCH_06: {
      open_watch_and_close(g_window_enum->watch_04.str, to_close);
      break;
    }
    default:
      break;
  }
  return RET_OK;
}

void window_init(widget_t* window) {
  widget_t* gesture = widget_lookup(window, "gesture", TRUE);
  if (gesture != NULL) {
    widget_on(gesture, EVT_LONG_PRESS, on_dial_long_pressed, window);
    widget_on(gesture, EVT_SLIDE_DOWN, open_message, window);
    widget_on(gesture, EVT_SLIDE_LEFT, open_app, window);
    widget_on(gesture, EVT_SLIDE_UP, on_open_quick, window);
    widget_on(gesture, EVT_SLIDE_RIGHT, on_change_watch, window);

    widget_on(window, EVT_WINDOW_CLOSE, on_window_close, window);
  }
}

ret_t on_set_window_anim_hint_slide_right(widget_t* window, void* ctx) {
  (void)ctx;
  widget_set_prop_str(window, WIDGET_PROP_OPEN_ANIM_HINT, "slide_right");
  return RET_OK;
}

ret_t on_set_window_anim_hint_slide_down(widget_t* window, void* ctx) {
  (void)ctx;
  widget_set_prop_str(window, WIDGET_PROP_OPEN_ANIM_HINT, "slide_down");
  return RET_OK;
}
