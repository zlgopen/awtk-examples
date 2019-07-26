#include "awtk.h"
#include "custom_widgets/chart/chart_view.h"
#include "custom_widgets/chart/line_series.h"
#include "custom_widgets/chart/bar_series.h"

#include <math.h>
#include <stdlib.h>

#define PROP_NEWEST_INDEX "newest"
#define PROP_TIME_NOW "now"
#define PROP_UPDATED "updated"

static ret_t on_series_prop_changed(void* ctx, event_t* e, const char* prop, const char* btn_name,
                                    const char* style, const char* style_select) {
  widget_t* win = WIDGET(ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    WIDGET_FOR_EACH_CHILD_BEGIN(chart_view, iter, i)
    if (widget_is_series(iter)) {
      value_t v;
      if (widget_get_prop(iter, prop, &v) == RET_OK) {
        value_set_bool(&v, !value_bool(&v));
        widget_set_prop(iter, prop, &v);

        widget_t* btn = widget_lookup(win, btn_name, TRUE);
        if (btn) {
          widget_use_style(btn, value_bool(&v) ? style_select : style);
        }
      }
    }
    WIDGET_FOR_EACH_CHILD_END()
  }

  return RET_OK;
}

ret_t on_series_line_show_changed(void* ctx, event_t* e) {
  return on_series_prop_changed(ctx, e, SERIES_PROP_LINE_SHOW, "line", "line", "line_select");
}

ret_t on_series_area_show_changed(void* ctx, event_t* e) {
  return on_series_prop_changed(ctx, e, SERIES_PROP_LINE_AREA_SHOW, "area", "area", "area_select");
}

ret_t on_series_symbol_show_changed(void* ctx, event_t* e) {
  return on_series_prop_changed(ctx, e, SERIES_PROP_SYMBOL_SHOW, "symbol", "symbol",
                                "symbol_select");
}

ret_t on_series_smooth_changed(void* ctx, event_t* e) {
  return on_series_prop_changed(ctx, e, SERIES_PROP_LINE_SMOOTH, "smooth", "smooth",
                                "smooth_select");
}

static ret_t on_series_visible_changed(void* ctx, event_t* e, uint32_t index, const char* icon_name,
                                       const char* show, const char* hide) {
  uint32_t cnt = 0;
  widget_t* icon;
  widget_t* win = WIDGET(ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  return_value_if_fail(chart_view != NULL, RET_BAD_PARAMS);

  WIDGET_FOR_EACH_CHILD_BEGIN(chart_view, iter, i)
  if (widget_is_series(iter)) {
    if (index == cnt) {
      widget_set_visible(iter, !iter->visible, FALSE);

      icon = widget_lookup(win, icon_name, TRUE);
      if (icon) {
        widget_use_style(icon, iter->visible ? show : hide);
      }
    }

    cnt++;
  }
  WIDGET_FOR_EACH_CHILD_END()

  return RET_OK;
}

ret_t on_series_typeie_visible_changed(void* ctx, event_t* e) {
  return on_series_visible_changed(ctx, e, 0, "typeie_icon", "typeie", "series_hide");
}

ret_t on_series_dayas_visible_changed(void* ctx, event_t* e) {
  return on_series_visible_changed(ctx, e, 1, "dayas_icon", "dayas", "series_hide");
}

ret_t on_series_drean_visible_changed(void* ctx, event_t* e) {
  return on_series_visible_changed(ctx, e, 2, "drean_icon", "drean", "series_hide");
}

ret_t on_series_bring_to_top(widget_t* win, uint32_t index, const char* icon_name,
                             const char* unfocus, const char* focus) {
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  return_value_if_fail(chart_view != NULL, RET_BAD_PARAMS);

  if (chart_view_set_top_series(chart_view, index) == RET_OK) {
    const char* icons[] = {"typeie_icon", "dayas_icon", "drean_icon"};
    const char* unfocus_styles[] = {"typeie", "dayas", "drean"};
    const char* focus_styles[] = {"typeie_focus", "dayas_focus", "drean_focus"};
    widget_t* icon;
    uint32_t i;
    for (i = 0; i < 3; i++) {
      icon = widget_lookup(win, icons[i], TRUE);
      if (icon) {
        widget_set_focused(icon, tk_str_eq(icons[i], icon_name));
        widget_use_style(icon, icon->focused ? unfocus_styles[i] : focus_styles[i]);
      }
    }
  }

  return RET_OK;
}

ret_t on_series_typeie_bring_to_top(void* ctx, event_t* e) {
  (void)e;
  return on_series_bring_to_top(WIDGET(ctx), 0, "typeie_icon", "typeie", "typeie_focus");
}

ret_t on_series_dayas_bring_to_top(void* ctx, event_t* e) {
  (void)e;
  return on_series_bring_to_top(WIDGET(ctx), 1, "dayas_icon", "dayas", "dayas_focus");
}

ret_t on_series_drean_bring_to_top(void* ctx, event_t* e) {
  (void)e;
  return on_series_bring_to_top(WIDGET(ctx), 2, "drean_icon", "drean", "drean_fucus");
}

static void generate_ufloat_data(void* buffer, uint32_t size) {
  uint32_t i;
  float_t* b = (float_t*)buffer;
  for (i = 0; i < size; i++) {
    b[i] = (float_t)(rand() % 120 + 20);
  }
}

static void generate_float_data(void* buffer, uint32_t size) {
  uint32_t i;
  float_t* b = (float_t*)buffer;
  for (i = 0; i < size; i++) {
    b[i] = (float_t)(rand() % 240 - 120);
  }
}

static void generate_colorful_data(void* buffer, uint32_t size) {
  uint32_t i;
  series_colorful_data_t* b = (series_colorful_data_t*)buffer;
  for (i = 0; i < size; i++) {
    b[i].v = (float_t)(rand() % 120 + 20);
    b[i].c.color = rand();
    b[i].c.rgba.a = 0xff;
  }
}

static void generate_minmax_data(void* buffer, uint32_t size) {
  uint32_t i;
  series_minmax_data_t* b = (series_minmax_data_t*)buffer;
  for (i = 0; i < size; i++) {
    b[i].min = (float_t)(rand() % 280 - 140);
    b[i].max = b[i].min + rand() % 200;
  }
}

typedef void (*_generate_data_t)(void* buffer, uint32_t size);
static void on_series_push_data(widget_t* widget, uint32_t count, uint32_t unit_size,
                                _generate_data_t gen) {
  uint32_t tdiv = 2000;
  uint32_t sdiv = 10;
  uint32_t now;
  bool_t updated = FALSE;
  widget_t* axis;
  void* buffer = TKMEM_CALLOC(count, unit_size);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  if (widget_is_series(iter)) {
    gen(buffer, count);
    series_push(iter, buffer, count);

    axis = widget_get_prop_pointer(iter, SERIES_PROP_SERIES_AXIS);
    if (axis) {
      updated = widget_get_prop_bool(axis, PROP_UPDATED, FALSE);
      if (!updated) {
        now = widget_get_prop_int(axis, PROP_TIME_NOW, 0);
        widget_set_prop_int(axis, PROP_TIME_NOW, now + count * tdiv / sdiv);
        widget_set_prop_int(axis, PROP_NEWEST_INDEX, series_count(iter) - 1);
        widget_set_prop_bool(axis, PROP_UPDATED, TRUE);
      }
    }
  }
  WIDGET_FOR_EACH_CHILD_END()

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  if (widget_is_axis(iter)) {
    widget_set_prop_bool(iter, PROP_UPDATED, FALSE);
  }
  WIDGET_FOR_EACH_CHILD_END()

  TKMEM_FREE(buffer);
}

void on_series_push_ufloat_data(widget_t* widget, uint32_t count) {
  on_series_push_data(widget, count, sizeof(float_t), generate_ufloat_data);
}

void on_series_push_float_data(widget_t* widget, uint32_t count) {
  on_series_push_data(widget, count, sizeof(float_t), generate_float_data);
}

void on_series_push_colorful_data(widget_t* widget, uint32_t count) {
  on_series_push_data(widget, count, sizeof(series_colorful_data_t), generate_colorful_data);
}

void on_series_push_minmax_data(widget_t* widget, uint32_t count) {
  on_series_push_data(widget, count, sizeof(series_minmax_data_t), generate_minmax_data);
}

static void on_series_rset_data(widget_t* widget, uint32_t count, uint32_t unit_size,
                                _generate_data_t gen) {
  void* buffer = TKMEM_CALLOC(count, unit_size);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  if (widget_is_series(iter)) {
    gen(buffer, count);
    series_rset(iter, count - 1, buffer, count);
  }
  WIDGET_FOR_EACH_CHILD_END()

  TKMEM_FREE(buffer);
}

void on_series_rset_ufloat_data(widget_t* widget, uint32_t count) {
  on_series_rset_data(widget, count, sizeof(float_t), generate_ufloat_data);
}

void on_series_rset_float_data(widget_t* widget, uint32_t count) {
  on_series_rset_data(widget, count, sizeof(float_t), generate_float_data);
}

void on_series_rset_colorful_data(widget_t* widget, uint32_t count) {
  on_series_rset_data(widget, count, sizeof(series_colorful_data_t), generate_colorful_data);
}

void on_series_rset_minmax_data(widget_t* widget, uint32_t count) {
  on_series_rset_data(widget, count, sizeof(series_minmax_data_t), generate_minmax_data);
}

static uint32_t get_series_capacity_min(widget_t* widget) {
  uint32_t ret = -1;

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  if (widget_is_series(iter)) {
    ret = tk_min(ret, SERIES(iter)->capacity);
  }
  WIDGET_FOR_EACH_CHILD_END()

  ret = tk_min(ret, 100);

  return ret;
}

ret_t on_series_rset_rand_ufloat_data(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    on_series_rset_ufloat_data(chart_view, get_series_capacity_min(chart_view));
  }
  return RET_OK;
}

ret_t on_series_push_rand_ufloat_data(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    on_series_push_ufloat_data(chart_view, 1);
  }
  return RET_REPEAT;
}

ret_t on_series_rset_rand_ufloat_data_4_timer(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    on_series_rset_ufloat_data(chart_view, get_series_capacity_min(chart_view));
  }
  return RET_REPEAT;
}

ret_t on_series_rset_rand_float_data(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    on_series_rset_float_data(chart_view, get_series_capacity_min(chart_view));
  }
  return RET_OK;
}

ret_t on_series_push_rand_float_data(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    on_series_push_float_data(chart_view, 1);
  }
  return RET_REPEAT;
}

ret_t on_series_rset_rand_colorful_data(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    on_series_rset_colorful_data(chart_view, get_series_capacity_min(chart_view));
  }
  return RET_OK;
}

ret_t on_series_push_rand_colorful_data(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    on_series_push_colorful_data(chart_view, 1);
  }
  return RET_REPEAT;
}

ret_t on_series_rset_rand_minmax_data(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    on_series_rset_minmax_data(chart_view, get_series_capacity_min(chart_view));
  }
  return RET_OK;
}

ret_t on_series_push_rand_minmax_data(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);
  widget_t* chart_view = widget_lookup(win, "chartview", TRUE);
  if (chart_view) {
    on_series_push_minmax_data(chart_view, 1);
  }
  return RET_REPEAT;
}

static const char* time_format(char* str, uint32_t size, uint32_t t) {
  uint32_t h = t / 1000 / 60 / 60 % 24;
  uint32_t m = t / 1000 / 60 % 60;
  uint32_t s = t / 1000 % 60;
  tk_snprintf(str, size, "%02d:%02d:%02d", h, m, s);
  return str;
}

ret_t axis_time_generate(void* ctx, uint32_t begin, uint32_t end, uint32_t middle, float_t interval,
                         darray_t* v) {
  widget_t* axis = WIDGET(ctx);
  axis_data_t* d;
  uint32_t tdiv = 2000;
  uint32_t sdiv = 10;
  uint32_t offset = 0;
  uint32_t index = 0;
  uint32_t nr;
  uint32_t t;
  uint32_t i = 0;
  uint32_t cnt = 0;
  uint32_t newest = widget_get_prop_int(axis, PROP_NEWEST_INDEX, 0);
  uint32_t now = widget_get_prop_int(axis, PROP_TIME_NOW, 0);
  char str[64] = {0};

  t = now - (newest - middle) * tdiv / sdiv;
  offset = (tdiv - t % tdiv) * sdiv / tdiv;
  nr = end - middle + 1;

  for (i = 0; i < 2; i++) {
    if (i == 0) {
      t = now - (newest - middle) * tdiv / sdiv;
      offset = (tdiv - t % tdiv) * sdiv / tdiv;
      nr = end - middle + 1;
    } else {
      t = now - (newest - begin) * tdiv / sdiv;
      offset = (tdiv - t % tdiv) * sdiv / tdiv + nr;
      nr = end - begin + 1;
      index = cnt = 0;
    }

    while ((index = cnt * sdiv + offset) < nr) {
      d = axis_data_create(index * interval, NULL);
      time_format(str, sizeof(str), (t / tdiv + cnt + 1) * tdiv);
      wstr_set_utf8(&(d->text), str);
      darray_push(v, d);
      cnt++;
    }
  }

  return RET_OK;
}

ret_t axis_time_init(widget_t* widget) {
  uint32_t t;
  date_time_t dt;
  widget_t* chart_view = widget_lookup(widget, "chartview", TRUE);
  return_value_if_fail(chart_view != NULL, RET_BAD_PARAMS);

  date_time_init(&dt);
  t = dt.hour * 60 * 60 * 1000 + dt.minute * 60 * 1000 + dt.second * 1000;

  WIDGET_FOR_EACH_CHILD_BEGIN(chart_view, iter, i)
  if (widget_is_axis(iter) && strstr(iter->name, "from_series") != NULL) {
    widget_set_prop_int(iter, PROP_NEWEST_INDEX, 0);
    widget_set_prop_int(iter, PROP_TIME_NOW, t);
    widget_set_prop_bool(iter, PROP_UPDATED, FALSE);
    axis_set_data_from_series(iter, axis_time_generate, iter);
  }
  WIDGET_FOR_EACH_CHILD_END()

  return RET_OK;
}
