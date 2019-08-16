/**
 *
 *
 *
 */

/**
 * 弧度转坐标
 * x = r*cos(c)
 * y = r*sin(c)
 * c为弧度制，r为半径
 */
#include <math.h>
#include "../common/log.h"
#include <tkc/utils.h>
#include <stdio.h>

#include "hour_weather.h"
#ifdef _WIN32
#pragma warning(disable : 4244)
#endif  // _WIN32

#ifdef _WIN32
#pragma warning(disable : 4305)
#endif  // _WIN32

static float_t x_arr[] = {-0.866025404, -0.5, 0, 0.5,  0.866025404, 1,
                          0.866025404,  0.5,  0, -0.5, -0.866025404};
static float_t y_arr[] = {-0.5, -0.866025404, -1, -0.866025404, -0.5, 0,
                          0.5,  0.866025404,  1,  0.866025404,  0.5};

static ret_t canvas_draw_utf8_polar(canvas_t* c, const char* str, float_t x, float_t y, float_t d,
                                    float_t a) {
  xy_t p_x = x + cos(a) * d;
  xy_t p_y = y + sin(a) * d;
  return canvas_draw_utf8(c, str, p_x, p_y);
}

static void draw_clock_number(widget_t* widget, canvas_t* c, int32_t start, float_t r) {
  (void)start;
  vgcanvas_t* vg = canvas_get_vgcanvas(c);
  hour_weather_t* hour_weather = HOUR_WEATHER(widget);
  return_if_fail(hour_weather != NULL && vg != NULL && r > 0);

  // char str[3] = {0};      // 保存小时的字符串
  wchar_t str[3] = {0};    // 保存小时的字符串
  float_t text_width = 0;  // 字符串的宽度
  (void)text_width;
  (void)str;

  // 画数字
  float_t width = hour_weather->width;
  (void)width;
  canvas_save(c);

  float_t rr = widget->w / 2 > widget->h / 2 ? widget->h / 2 : widget->w / 2;
  float_t bar_r = rr / 2;

  uint8_t hour[25] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 1, 2};
  date_time_t dt;
  date_time_init(&dt);

  uint32_t s = 0;
  int32_t h = dt.hour;
  // h = 5;
  h = h <= 12 ? h : h % 12;
  if (h > 10) {
    s = h - 10;
  } else {
    s = 12 - (10 - h);
  }
  LOG("s:%d %d", s, hour[0 + s]);

  system_info_t* info = system_info();
  uint32_t lcd_w = info->lcd_w;

  size_t i = 0;
  for (i = 0; i < 12; i++) {
    if (i != 6) {
      float_t a = 2 * M_PI / 12.0 * i;
      char txt[20];
      memset(txt, '\0', sizeof(txt));
      tk_snprintf(txt, sizeof(txt), "%d", hour[i + s]);
      float_t tw = canvas_measure_utf8(c, txt);
      canvas_set_text_color_str(c, "#FFFFFF");
      if (lcd_w == 800) {
        canvas_set_font(c, "default", 30);
      } else if (lcd_w == 480) {
        canvas_set_font(c, "default", 18);
      } else {
        assert(FALSE);
      }
      float_t ex = 0;
      float_t ey = -12;

      if (lcd_w == 480) {
        ey = -7.5;
      }

      if (i == 0) {
        if (hour[i + s] < 10) {
          if (lcd_w == 480) {
            ex = 7;
          } else {
            ex = 10;
          }

          canvas_draw_utf8_polar(c, txt, widget->w / 2 - tw / 2.0 + ex, widget->h / 2 + ey, bar_r,
                                 a);
        } else {
          if (lcd_w == 480) {
            ex = 12;
          } else {
            ex = 20;
          }

          canvas_draw_utf8_polar(c, txt, widget->w / 2 - tw / 2.0 + ex, widget->h / 2 + ey, bar_r,
                                 a);
        }
      } else {
        canvas_draw_utf8_polar(c, txt, widget->w / 2 - tw / 2.0, widget->h / 2 + ey, bar_r, a);
      }
    }
  }

  canvas_restore(c);
}

static void draw_weather_icon(widget_t* widget, vgcanvas_t* vg, int32_t start, float_t r) {
  (void)start;
  hour_weather_t* hour_weather = HOUR_WEATHER(widget);
  return_if_fail(hour_weather != NULL && vg != NULL && r > 0);

  char image_name[32] = {0};

  vgcanvas_save(vg);
  vgcanvas_translate(vg, widget->w / 2, widget->h / 2);
  int i = 0;
  for (i = 0; i < 11; i++) {
    bitmap_t image;
    memset(image_name, 0, 32);
    sprintf(image_name, "%d", hour_weather->weather[i]);
    widget_load_image(widget, image_name, &image);

    vgcanvas_save(vg);
    vgcanvas_translate(vg, r * x_arr[i], r * y_arr[i]);
    vgcanvas_translate(vg, -image.w / 2, -image.h / 2);
    vgcanvas_draw_image(vg, &image, 0, 0, image.w, image.h, 0, 0, image.w, image.h);

    vgcanvas_restore(vg);
  }
  vgcanvas_restore(vg);
}

static void draw_end_point(widget_t* widget, canvas_t* c) {
  vgcanvas_t* vg = canvas_get_vgcanvas(c);
  hour_weather_t* hour_weather = HOUR_WEATHER(widget);
  float_t rr = widget->w / 2 > widget->h / 2 ? widget->h / 2 : widget->w / 2;
  float_t bar_r = rr / 2;

  float_t width = hour_weather->width;
  system_info_t* info = system_info();
  if (info->lcd_w == 480) {
    width = width * 240 / 390;
  }

  {
    vgcanvas_save(vg);
    vgcanvas_translate(vg, c->ox, c->oy);

    {
      vgcanvas_save(vg);
      vgcanvas_translate(vg, widget->w / 2, widget->h / 2);

      vgcanvas_translate(vg, bar_r * cos(-5.0 / 6.0 * M_PI), bar_r * sin(-5.0 / 6.0 * M_PI));

      vgcanvas_begin_path(vg);
      vgcanvas_ellipse(vg, 0, 0, width / 2, width / 2);
      vgcanvas_set_fill_color(vg, hour_weather->bar_color);
      vgcanvas_fill(vg);
      vgcanvas_restore(vg);
    }

    {
      vgcanvas_save(vg);
      vgcanvas_translate(vg, widget->w / 2, widget->h / 2);

      vgcanvas_translate(vg, bar_r * cos(5.0 / 6.0 * M_PI), bar_r * sin(5.0 / 6.0 * M_PI));

      vgcanvas_begin_path(vg);
      vgcanvas_ellipse(vg, 0, 0, width / 2, width / 2);
      vgcanvas_set_fill_color(vg, hour_weather->bar_color);
      vgcanvas_fill(vg);
      vgcanvas_restore(vg);
    }

    vgcanvas_restore(vg);
  }
}

static ret_t hour_weather_on_paint_self_vgcanvas(widget_t* widget, canvas_t* c) {
  vgcanvas_t* vg = canvas_get_vgcanvas(c);
  hour_weather_t* hour_weather = HOUR_WEATHER(widget);
  float_t rr = widget->w / 2 > widget->h / 2 ? widget->h / 2 : widget->w / 2;
  float_t bar_r = rr / 2;
  vgcanvas_save(vg);
  vgcanvas_translate(vg, c->ox, c->oy);

  float_t width = hour_weather->width;
  system_info_t* info = system_info();
  if (info->lcd_w == 480) {
    width = width * 240 / 390;
  }

  /* 绘制弧形背景 */
  vgcanvas_save(vg);
  vgcanvas_set_line_width(vg, width);
  vgcanvas_translate(vg, widget->w / 2, widget->h / 2);
  vgcanvas_set_line_cap(vg, "round");
  vgcanvas_set_stroke_color(vg, hour_weather->bar_color);
  vgcanvas_begin_path(vg);
  vgcanvas_arc(vg, 0, 0, bar_r, -5.0 / 6.0 * M_PI, 5.0 / 6.0 * M_PI, FALSE);
  vgcanvas_stroke(vg);
  vgcanvas_restore(vg);

  vgcanvas_restore(vg);

  // agge不支持线段孤度，所以要自己画
  draw_end_point(widget, c);

  // 填充数字（时间）
  draw_clock_number(widget, c, hour_weather->time, bar_r);

  vgcanvas_save(vg);
  vgcanvas_translate(vg, c->ox, c->oy);

  // 填充圆点
  {
    vgcanvas_save(vg);
    vgcanvas_translate(vg, widget->w / 2, widget->h / 2);

    date_time_t dt;
    date_time_init(&dt);

    vgcanvas_translate(vg, bar_r * cos(hour_weather->point_rad),
                       bar_r * sin(hour_weather->point_rad));
    vgcanvas_begin_path(vg);
    vgcanvas_ellipse(vg, 0, 0, width / 2 - 1, width / 2 - 1);
    vgcanvas_set_fill_color(vg, hour_weather->point_color);
    vgcanvas_fill(vg);
    vgcanvas_restore(vg);
  }

  // 绘制周围天气图标
  draw_weather_icon(widget, vg, hour_weather->time, 0.4 * widget->w);

  // 绘制中心温度文字
  // draw_temp(widget, c, hour_weather->time, 0.35 * widget->w);
  vgcanvas_restore(vg);
  return RET_OK;
}

static ret_t hour_weather_on_paint_self_canvas(widget_t* widget, canvas_t* c) {
  // vgcanvas_t* vg = canvas_get_vgcanvas(c);
  hour_weather_t* hour_weather = HOUR_WEATHER(widget);
  (void)hour_weather;
  float_t rr = widget->w / 2 > widget->h / 2 ? widget->h / 2 : widget->w / 2;
  float_t bar_r = rr / 2;
  (void)bar_r;
  canvas_save(c);

  /* 绘制弧形背景 */
  canvas_save(c);
  canvas_restore(c);

  canvas_restore(c);
  return RET_OK;
}

static ret_t hour_weather_on_paint_self(widget_t* widget, canvas_t* c) {
  if (TRUE) {
    return hour_weather_on_paint_self_vgcanvas(widget, c);  // 兼容appe
  } else {
    return hour_weather_on_paint_self_canvas(widget, c);
  }
}

static uint8_t hour_health_get_click_select(widget_t* widget, xy_t x_f, xy_t y_f) {
  (void)widget;
  float_t result = 0;  // 记录弧度
  float_t rad = atan(fabs((float)y_f) / fabs((float)x_f));
  if (x_f > 0 && y_f > 0) {
    result = rad;
  }
  if (x_f < 0 && y_f > 0) {
    result = M_PI - rad;
  }
  if (x_f < 0 && y_f < 0) {
    result = M_PI + rad;
  }
  if (x_f > 0 && y_f < 0) {
    result = 2 * M_PI - rad;
  }

  result *= 12;
  uint8_t select = 255;
  if (result > M_PI && result < 3 * M_PI) {
    select = 6;
  } else if (result > 3 * M_PI && result < 5 * M_PI) {
    select = 7;
  } else if (result > 5 * M_PI && result < 7 * M_PI) {
    select = 8;
  } else if (result > 7 * M_PI && result < 9 * M_PI) {
    select = 9;
  } else if (result > 9 * M_PI && result < 11 * M_PI) {
    select = 10;
  } else if (result > 13 * M_PI && result < 15 * M_PI) {
    select = 0;
  } else if (result > 15 * M_PI && result < 17 * M_PI) {
    select = 1;
  } else if (result > 17 * M_PI && result < 19 * M_PI) {
    select = 2;
  } else if (result > 19 * M_PI && result < 21 * M_PI) {
    select = 3;
  } else if (result > 21 * M_PI && result < 23 * M_PI) {
    select = 4;
  } else if (result > 23 * M_PI || result < M_PI) {
    select = 5;
  } else {
    select = 0;
  }

  log_debug("select is %d\n", select);
  return select;
}

static void on_change_select(widget_t* widget) {
  hour_weather_t* hour_weather = HOUR_WEATHER(widget);
  return_if_fail(widget != NULL);

  widget_destroy_animator(widget, NULL);
  float_t change_rad = (hour_weather->selected - 5) * M_PI / 6;
  char param[128] = {0};
  tk_snprintf(param, 128, "point_rad(from=%f, to=%f, duration=300, easing=linear)",
              hour_weather->point_rad, change_rad);
  log_debug("from %f, to %f\n", hour_weather->point_rad, change_rad);

  // hour_weather->point_rad = change_rad;
  widget_create_animator(widget, param);
}

static ret_t hour_weather_on_event(widget_t* widget, event_t* e) {
  uint16_t type = e->type;
  hour_weather_t* hour_weather = HOUR_WEATHER(widget);
  switch (type) {
    case EVT_POINTER_DOWN: {
      hour_weather->pressed = TRUE;
      break;
    }
    case EVT_POINTER_UP: {
      pointer_event_t* pointer_event = (pointer_event_t*)e;
      point_t point = {pointer_event->x, pointer_event->y};
      widget_to_local(widget, &point);
      log_debug("(%d, %d) clicked!\n", (int32_t)point.x, (int32_t)point.y);

      xy_t x_c = point.x - widget->w / 2;
      xy_t y_c = point.y - widget->h / 2;
      hour_weather->selected = hour_health_get_click_select(widget, x_c, y_c);

      on_change_select(widget);
      hour_weather->pressed = FALSE;

      event_t event = event_init(EVT_CHANGE_SELECT, widget);
      widget_dispatch(widget, &event);

      break;
    }
    default:
      break;
  }

  return RET_OK;
}

ret_t hour_weather_set_point_rad(widget_t* widget, float_t rad) {
  hour_weather_t* hour_weather = HOUR_WEATHER(widget);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  hour_weather->point_rad = rad;

  return widget_invalidate(widget, NULL);
}

static ret_t hour_weather_on_set_prop(widget_t* widget, const char* name, const value_t* v) {
  if (tk_str_eq(name, "point_rad")) {
    return hour_weather_set_point_rad(widget, value_float(v));
  }
  return RET_NOT_FOUND;
}

static const widget_vtable_t s_hour_weather_vtable = {.size = sizeof(hour_weather_t),
                                                      .type = WIDGET_TYPE_HOUR_WEATHER,
                                                      .create = hour_weather_create,
                                                      .on_paint_self = hour_weather_on_paint_self,
                                                      .on_event = hour_weather_on_event,
                                                      .set_prop = hour_weather_on_set_prop};

widget_t* hour_weather_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, &s_hour_weather_vtable, x, y, w, h);
  hour_weather_t* hour_weather = HOUR_WEATHER(widget);
  return_value_if_fail(hour_weather != NULL, NULL);

  hour_weather->weather_info = NULL;
  hour_weather->width = 30;
  hour_weather->bar_color = color_init(0x01, 0x44, 0x95, 0xff);    // #242021
  hour_weather->point_color = color_init(0x00, 0xFF, 0xEF, 0xff);  // #33CCCC 00FFEF
  hour_weather->selected = 0;

  // 按当前时间初始化
  date_time_t dt;
  date_time_init(&dt);
  int32_t hour = dt.hour;
  if (hour == 9) {
    hour = 10;
  }
  hour = 10;
  hour_weather->point_rad = hour % 12 / 12.0 * 2 * M_PI - 0.5 * M_PI;

  hour_weather->time = dt.hour % 12;
  if (dt.hour == 12) {
    hour_weather->time = 12;
  }

  int i = 0;
  for (i = 0; i < 11; i++) {
    hour_weather->temp[i] = i - 4;
    hour_weather->weather[i] = i;
  }

  return widget;
}
