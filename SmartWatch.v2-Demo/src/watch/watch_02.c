#include "watch_02.h"
#include "../common/global.h"
#include "../window_main.h"

#ifdef _WIN32
#pragma warning(disable : 4244)
#endif  // _WIN32

static ret_t on_paint(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  widget_t* window = widget_get_window(widget);

  paint_event_t* event = (paint_event_t*)e;
  assert(widget != NULL && event != NULL);
  canvas_t* c = event->c;
  vgcanvas_t* vg = canvas_get_vgcanvas(c);
  wh_t h = widget->h;
  (void)h;

  vgcanvas_save(vg);
  // vgcanvas_translate(vg, c->ox, c->oy);
  // vgcanvas_translate(vg, window->w / 2, window->h / 2);

  vgcanvas_set_line_width(vg, 0);
  vgcanvas_set_stroke_color(vg, color_init(0xFF, 0xFF, 0xFF, 0xFF));
  vgcanvas_set_fill_color(vg, color_init(0xFF, 0xFF, 0xFF, 0xFF));

  system_info_t* sys_info = system_info();
  uint32_t lcd_w = sys_info->lcd_w;
  uint32_t lcd_h = sys_info->lcd_h;

  if (window->w == 390) {
    float_t d = 242 / 2 - 7;
    date_time_t dt;
    date_time_init(&dt);
    vgcanvas_translate(vg, window->x + window->w / 2, window->y + window->h / 2);

    size_t i = 0;
    for (i = 0; i <= dt.second; i++) {
      float_t a = i / 60.0 * 2 * M_PI - 0.5 * M_PI;
      float_t x = d * cos(a);
      float_t y = d * sin(a);

      // vgcanvas_ellipse(vg, x, y, 3, 3);
      vgcanvas_stroke(vg);
      vgcanvas_fill(vg);

      vgcanvas_save(vg);
      vgcanvas_set_line_width(vg, 2);
      vgcanvas_begin_path(vg);
      vgcanvas_move_to(vg, x, y);

      float_t n_x = 7 * cos(a);
      float_t n_y = 7 * sin(a);

      vgcanvas_line_to(vg, x + n_x, y + n_y);
      vgcanvas_stroke(vg);
      vgcanvas_restore(vg);
    }
  } else if (window->w == 240) {
    float_t d = 149 / 2 - 4;
    date_time_t dt;
    date_time_init(&dt);
    vgcanvas_translate(vg, lcd_w / 2, lcd_h / 2);

    size_t i = 0;
    for (i = 0; i <= dt.second; i++) {
      float_t a = i / 60.0 * 2 * M_PI - 0.5 * M_PI;
      float_t x = d * cos(a);
      float_t y = d * sin(a);

      // vgcanvas_ellipse(vg, x, y, 3, 3);
      vgcanvas_stroke(vg);
      vgcanvas_fill(vg);

      vgcanvas_save(vg);
      vgcanvas_set_line_width(vg, 2);
      vgcanvas_begin_path(vg);
      vgcanvas_move_to(vg, x, y);

      float_t n_x = 4 * cos(a);
      float_t n_y = 4 * sin(a);

      vgcanvas_line_to(vg, x + n_x, y + n_y);
      vgcanvas_stroke(vg);
      vgcanvas_restore(vg);
    }
  } else {
    assert(FALSE);
  }

  vgcanvas_restore(vg);

  return RET_OK;
}

static void watch_02_update(widget_t* window) {
  widget_t* wday = widget_lookup(window, "wday", TRUE);
  if (wday != NULL) {
    date_time_t dt;
    date_time_init(&dt);

    wstr_t str_wday;
    wstr_init(&str_wday, 0);

    switch (dt.wday) {
      case 0: {
        wstr_append(&str_wday, L"星期日");
      } break;
      case 1: {
        wstr_append(&str_wday, L"星期一");
      } break;
      case 2: {
        wstr_append(&str_wday, L"星期二");
      } break;
      case 3: {
        wstr_append(&str_wday, L"星期三");
      } break;
      case 4: {
        wstr_append(&str_wday, L"星期四");
      } break;
      case 5: {
        wstr_append(&str_wday, L"星期五");
      } break;
      case 6: {
        wstr_append(&str_wday, L"星期六");
      } break;
      default:
        break;
    }

    widget_set_text(wday, str_wday.str);
    wstr_reset(&str_wday);
  }
}

static ret_t on_watch_02_timer(const timer_info_t* timer) {
  widget_t* widget = WIDGET(timer->ctx);
  widget_t* window = widget_get_window(widget);

  watch_02_update(widget);

  widget_invalidate(window, NULL);

  return RET_REPEAT;
}

void watch_02_init(widget_t* window) {
  widget_t* gesture = widget_lookup(window, "gesture", TRUE);
  if (gesture != NULL) {
    global_data.watch_type = WATCH_02;
    window_init(window);
  }

  timer_info_t timer;
  timer.ctx = window;
  on_watch_02_timer(&timer);

  widget_t* canvas = widget_lookup(window, "canvas", TRUE);
  if (canvas != NULL) {
    widget_on(canvas, EVT_PAINT, on_paint, window);
  }

  widget_add_timer(window, on_watch_02_timer, 1000);
}
