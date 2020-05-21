#include "awtk.h"
extern ret_t application_init(void);

#define CENTER_POINT_RADIUS "center_point_radius"

/**
 * 重绘
 */
static ret_t on_paint_needle(void* ctx, event_t* e) {
  value_t v;
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  widget_t* widget = WIDGET(evt->e.target);
  vgcanvas_t* vg = lcd_get_vgcanvas(c->lcd);

  float_t rotation = 0.0;
  float_t w = widget->w;
  float_t h = widget->h;
  float_t cx = w * 0.5f;
  float_t cy = h * 0.5f;
  float_t anchor_x = w * 0.5;
  float_t anchor_y = h * 0.5;
  float_t center_point_radius = 0.0;
  color_t bg = color_init(0xff, 0, 0, 0xff);

  if (widget_get_prop(widget, WIDGET_PROP_ROTATION, &v) == RET_OK) {
    rotation = value_float(&v);
  }

  if (widget_get_prop(widget, CENTER_POINT_RADIUS, &v) == RET_OK) {
    center_point_radius = value_float(&v);
  }

  if (vg != NULL) {
    vgcanvas_save(vg);
    vgcanvas_translate(vg, c->ox, c->oy);
    vgcanvas_translate(vg, anchor_x, anchor_y);
    vgcanvas_rotate(vg, rotation);
    vgcanvas_translate(vg, -anchor_x, -anchor_y);

    vgcanvas_begin_path(vg);
    vgcanvas_move_to(vg, cx, 0);
    vgcanvas_line_to(vg, cx, h * 0.6f);
    vgcanvas_set_line_width(vg, 4);
    vgcanvas_set_stroke_color(vg, bg);
    vgcanvas_stroke(vg);

    vgcanvas_begin_path(vg);
    vgcanvas_set_fill_color(vg, bg);
    vgcanvas_arc(vg, cx, cy, center_point_radius, 0, M_PI * 2, FALSE);
    vgcanvas_fill(vg);

    vgcanvas_restore(vg);
  }

  return RET_OK;
}

/**
 * 初始化图片和动画
 */
static void init_image(widget_t* win, int x, int y, int w, int h, float from, float to,
                       int duration, float_t radius) {
  widget_t* img = view_create(win, x, y, w, h);
  value_t val;
  value_set_float(&val, radius);
  widget_set_prop(img, CENTER_POINT_RADIUS, &val);
  widget_on(img, EVT_PAINT, on_paint_needle, NULL);

  char rotation_str[100];
  memset(rotation_str, 0, sizeof(rotation_str));
  tk_snprintf(rotation_str, sizeof(rotation_str),
              "rotation(from=%f ,to=%f, yoyo_times=0, duration=%d, easing=linear)", from, to,
              duration);
  widget_create_animator(img, rotation_str);
}

/**
 * 创建图片
 */
static ret_t create_images(widget_t* win) {
  bool_t is_big_lcd = win->w == 800 ? TRUE : FALSE;

  if (is_big_lcd) {
    /* 这里的宽和高是指仪表盘宽度和高度，x, y指是各个仪表盘在背景图片上的左上标位置 */
    int width = 186;
    int height = 186;
    float_t small_radius = 4;
    float_t big_radius = 8;

    /* 左边：从上到下 */
    init_image(win, 51, 47, width, height, -2.2, 2.2, 2000, small_radius);
    init_image(win, 24, 265, width, height, -1.7, 1.7, 4000, small_radius);

    /* 中间 */
    init_image(win, 256, 115, 288, 288, -2.5, 2.5, 2000, big_radius);

    /* 右边：从上到下 */
    init_image(win, 565, 47, width, height, -2.2, 2.2, 2000, small_radius);
    init_image(win, 590, 265, width, height, -1.7, 1.7, 4000, small_radius);
  } else {
    /* 这里的宽和高是指仪表盘宽度和高度，x, y指是各个仪表盘在背景图片上的左上标位置 */
    int width = 113;
    int height = 113;
    float_t small_radius = 2;
    float_t big_radius = 5;

    /* 左边：从上到下 */
    init_image(win, 30, 24, width, height, -2.2, 2.2, 2000, small_radius);
    init_image(win, 14, 146, width, height, -1.7, 1.7, 4000, small_radius);

    /* 中间 */
    init_image(win, 153, 60, 174, 174, -2.5, 2.5, 2000, big_radius);

    /* 右边：从上到下 */
    init_image(win, 339, 24, width, height, -2.2, 2.2, 2000, small_radius);
    init_image(win, 354, 146, width, height, -1.7, 1.7, 4000, small_radius);
  }

  return RET_OK;
}

/**
 * Label文本随机显示int
 */
static int32_t get_random_int(int32_t min_num, int32_t max_num) {
  return fmod(rand(), max_num - min_num) + min_num;
}

/**
 * 更新LED灯的状态
 */
static ret_t on_update_led_status(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);
  char label_name[32] = {0};
  static const char* led_styles[] = {"led_red", "led_green", "led_yellow", "led_gray"};

  int32_t label_index = get_random_int(0, 4);
  int32_t style_index = get_random_int(0, 4);
  tk_snprintf(label_name, sizeof(label_name) - 1, "label%d", label_index);
  widget_t* label = widget_lookup(win, label_name, TRUE);

  if (label) {
    const char* old_style = widget_get_prop_str(label, WIDGET_PROP_STYLE, "led_green");
    const char* new_style = led_styles[style_index];

    if (!tk_str_eq(old_style, new_style)) {
      widget_use_style(label, new_style);
    }
  }

  return RET_REPEAT;
}

/**
 * 初始化
 */
ret_t application_init() {
  system_info_set_default_font(system_info(), "default");
  window_manager_set_cursor(window_manager(), NULL);
  // window_manager_set_show_fps(window_manager(), TRUE);
  
  widget_t* win = window_open("home_page/home_page");
  if (win) {
    create_images(win);
    timer_add(on_update_led_status, win, 1000);
  }

  return RET_OK;
}

/**
* 退出
*/
ret_t application_exit(void) {
        log_debug("application_exit");
        return RET_OK;
}
