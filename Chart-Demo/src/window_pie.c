/**
 * File:   window_pie.c
 * Author: AWTK Develop Team
 * Brief:  window pie
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
#include "custom_widgets/pie_slice/pie_slice.h"

#define SAVE_EXPLODED "save_exploded"
#define DELAY_TIME 100
#define DURATION_TIME 120

static bool_t save_pie_exploded[6];

typedef struct _pie_value_t {
  int32_t start_angle;
  int32_t value;
} pie_value_t;

static pie_value_t old_pie_data[] = {
    {270, 90}, {200, 70}, {130, 70}, {90, 40}, {30, 60}, {0, 30},
};

static pie_value_t new_pie_data[] = {
    {240, 120}, {150, 90}, {90, 60}, {40, 50}, {10, 30}, {0, 10},
};

/**
 * 设置按钮是否可以按下
 */
static ret_t set_btn_enable(widget_t* win, bool_t enable) {
  widget_t* new_pie = widget_lookup(win, "function_new_pie", TRUE);
  widget_t* arch = widget_lookup(win, "function_arch", TRUE);
  if (new_pie) {
    widget_set_enable(new_pie, enable);
  }
  if (arch) {
    widget_set_enable(arch, enable);
  }

  return RET_OK;
}

static widget_t* get_pie_slice(widget_t* win, const char* name) {
  widget_t* pie_slice = NULL;
  str_t str;
  str_init(&str, 20);
  str_append_with_len(&str, name, 4);
  str_append(&str, "_slice");
  pie_slice = widget_lookup(win, str.str, TRUE);
  str_reset(&str);

  return pie_slice;
}

/**
 * 点击扇形文本
 */
static ret_t on_btn_view(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* target = (widget_t*)e->target;
  widget_t* pie_slice = get_pie_slice(win, target->name);
  pie_slice_set_exploded_4_others(pie_slice);

  str_t str;
  str_init(&str, 20);

  WIDGET_FOR_EACH_CHILD_BEGIN(target->parent, iter, i)
  const char* name = iter->name;
  str_set(&str, name);
  if (str_start_with(&str, "pie") && str_end_with(&str, "label")) {
    widget_t* pie_slice = get_pie_slice(win, name);
    bool_t flag = widget_get_prop_bool(pie_slice, PIE_SLICE_PROP_IS_EXPLODED, FALSE);
    if (flag) {
      widget_use_style(iter, "pie_label_press");
    } else {
      widget_use_style(iter, "pie_label");
    }
    widget_invalidate(iter, NULL);
  }
  WIDGET_FOR_EACH_CHILD_END();
  str_reset(&str);

  return RET_OK;
}

/**
 * 点击扇形
 */
static ret_t on_pie_view(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* target = (widget_t*)e->target;

  str_t str;
  str_init(&str, 20);

  WIDGET_FOR_EACH_CHILD_BEGIN(target, iter, i)
  pie_slice_t* pie_slice = (pie_slice_t*)iter;
  str_set(&str, pie_slice->widget.name);
  str_replace(&str, "slice", "label");
  widget_t* result = widget_lookup(win, str.str, TRUE);
  if (pie_slice->is_exploded) {
    widget_use_style(result, "pie_label_press");
  } else {
    widget_use_style(result, "pie_label");
  }
  widget_invalidate(result, NULL);
  WIDGET_FOR_EACH_CHILD_END();
  str_reset(&str);

  return RET_OK;
}

/**
 * 恢复扇形原来是否展开的状态
 */
static ret_t on_save_exploded_timer(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);

  uint32_t count = widget_animator_manager_count(widget_animator_manager());
  if (count == 0) {
    widget_t* pie_view = widget_lookup(win, "pie_view", TRUE);
    if (pie_view) {
      WIDGET_FOR_EACH_CHILD_BEGIN_R(pie_view, iter, i)
      value_t v;
      value_set_bool(&v, !save_pie_exploded[i]);
      widget_set_prop(iter, PIE_SLICE_PROP_IS_EXPLODED, &v);
      pie_slice_set_exploded(iter);
      WIDGET_FOR_EACH_CHILD_END();
    }
    set_btn_enable(win, TRUE);

    return RET_REMOVE;
  }

  return RET_REPEAT;
}

/**
 * 点击创建新饼图或者拱形定时器
 */
static ret_t on_new_pie_timer(const timer_info_t* timer) {
  pie_value_t* pie_data = NULL;
  widget_t* win = WIDGET(timer->ctx);

  uint32_t count = widget_animator_manager_count(widget_animator_manager());
  if (count == 0) {
    value_t v;
    value_t v1;
    ret_t result = widget_get_prop(win, "is_new", &v);
    bool_t flag = (result == RET_NOT_FOUND) ? FALSE : value_bool(&v);
    if (flag) {
      pie_data = old_pie_data;
      widget_set_prop(win, "is_new", value_set_bool(&v1, FALSE));
    } else {
      pie_data = new_pie_data;
      widget_set_prop(win, "is_new", value_set_bool(&v1, TRUE));
    }
    widget_t* pie_view = widget_lookup(win, "pie_view", TRUE);
    if (pie_view) {
      WIDGET_FOR_EACH_CHILD_BEGIN(pie_view, iter, i)
      int32_t delay = DELAY_TIME;
      int32_t duration = DURATION_TIME;
      delay = delay * i;
      const pie_value_t* new_pie = pie_data + nr - 1 - i;
      pie_slice_set_start_angle(iter, new_pie->start_angle);
      char param[100];
      tk_snprintf(param, sizeof(param),
                  "value(name=%s, to=%d, duration=%d, delay=%d, easing=sin_out)", SAVE_EXPLODED,
                  new_pie->value, duration, delay);
      widget_create_animator(iter, param);
      WIDGET_FOR_EACH_CHILD_END();
    }
    timer_add(on_save_exploded_timer, win, 1000 / 60);

    return RET_REMOVE;
  }

  return RET_REPEAT;
}

/**
 * 创建扇形还原到原点动画
 */
static ret_t create_animator_to_zero(widget_t* win) {
  set_btn_enable(win, FALSE);

  widget_t* pie_view = widget_lookup(win, "pie_view", TRUE);
  if (pie_view) {
    WIDGET_FOR_EACH_CHILD_BEGIN_R(pie_view, iter, i)
    value_t v;
    widget_get_prop(iter, PIE_SLICE_PROP_IS_EXPLODED, &v);
    save_pie_exploded[i] = value_bool(&v);

    pie_slice_t* pie_slice = PIE_SLICE(iter);
    if (pie_slice->is_exploded) {
      pie_slice_set_exploded(iter);
    }
    int32_t delay = 80;
    delay = delay * (nr - 1 - i);
    char param[100];
    tk_snprintf(param, sizeof(param), "value(to=0, duration=50, delay=%d, easing=sin_out)", delay);
    widget_create_animator(iter, param);
    WIDGET_FOR_EACH_CHILD_END();
  }

  return RET_OK;
}

/**
 * 点击创建新饼图或者拱形
 */
static ret_t on_new_pie(void* ctx, event_t* e) {
  widget_t* win = (widget_t*)ctx;
  (void)e;

  create_animator_to_zero(win);
  timer_add(on_new_pie_timer, win, 1000 / 60);

  return RET_OK;
}

/**
 * 拱形定时器
 */
static ret_t on_arch_timer(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);

  uint32_t new_inner_radius = win->h / 5;
  uint32_t inner_radius = 0;

  uint32_t count = widget_animator_manager_count(widget_animator_manager());
  if (count == 0) {
    value_t v;
    value_t v1;
    ret_t result = widget_get_prop(win, "is_arch", &v);
    bool_t flag = (result == RET_NOT_FOUND) ? FALSE : value_bool(&v);
    if (flag) {
      widget_set_prop(win, "is_arch", value_set_bool(&v1, FALSE));
      inner_radius = 550;
    } else {
      widget_set_prop(win, "is_arch", value_set_bool(&v1, TRUE));
      inner_radius = new_inner_radius;
    }
    widget_t* pie_view = widget_lookup(win, "pie_view", TRUE);
    if (pie_view) {
      WIDGET_FOR_EACH_CHILD_BEGIN(pie_view, iter, i)
      pie_slice_set_inner_radius(iter, inner_radius);

      if (flag) {
        pie_slice_set_semicircle(iter, FALSE);
        pie_slice_set_counter_clock_wise(iter, FALSE);
      } else {
        pie_slice_set_semicircle(iter, TRUE);
        pie_slice_set_counter_clock_wise(iter, TRUE);
      }

      int32_t delay = DELAY_TIME;
      int32_t duration = DURATION_TIME;
      delay = delay * i;
      const pie_value_t* new_pie = old_pie_data + nr - 1 - i;
      pie_slice_set_start_angle(iter, new_pie->start_angle);
      char param[100];
      tk_snprintf(param, sizeof(param),
                  "value(name=%s, to=%d, duration=%d, delay=%d, easing=sin_out)", SAVE_EXPLODED,
                  new_pie->value, duration, delay);
      widget_create_animator(iter, param);
      WIDGET_FOR_EACH_CHILD_END();
    }
    timer_add(on_save_exploded_timer, win, 1000 / 60);

    return RET_REMOVE;
  }

  return RET_REPEAT;
}

/**
 * 点击拱形按钮
 */
static ret_t on_arch(void* ctx, event_t* e) {
  widget_t* win = (widget_t*)ctx;

  create_animator_to_zero(win);
  timer_add(on_arch_timer, win, 1000 / 60);

  widget_t* target = (widget_t*)e->target;

  value_t v;
  ret_t result = widget_get_prop(win, "is_arch", &v);
  bool_t flag = (result == RET_NOT_FOUND) ? FALSE : value_bool(&v);
  if (flag) {
    widget_use_style(target, "pie_annular");
  } else {
    widget_use_style(target, "pie_circle");
  }
  widget_invalidate(target, NULL);
  return RET_OK;
}

/**
 * 点击关闭按钮
 */
static ret_t on_close(void* ctx, event_t* e) {
  widget_t* win = (widget_t*)ctx;
  (void)e;
  int i;
  for (i = 0; i < ARRAY_SIZE(save_pie_exploded); i++) {
    save_pie_exploded[i] = FALSE;
  }
  return window_close(win);
}

/**
 * 子控件初始化(主要是设置click回调、初始显示信息)
 */
static ret_t init_widget(void* ctx, const void* iter) {
  widget_t* widget = WIDGET(iter);

  (void)ctx;

  if (widget->name != NULL) {
    const char* name = widget->name;
    if (strstr(name, "label")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_btn_view, win);
    } else if (strstr(name, "_slice")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_pie_view, win);
    } else if (tk_str_eq(name, "function_new_pie")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_new_pie, win);
    } else if (tk_str_eq(name, "function_arch")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_arch, win);
    } else if (tk_str_eq(name, "close")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_close, win);
    }
  }

  return RET_OK;
}

/**
 * 打开窗口时使用动画展示饼图
 */
static ret_t on_window_open(void* ctx, event_t* e) {
  widget_t* open = WIDGET(e->target);

  if (open != NULL && open->name != NULL) {
    timer_info_t info;
    info.ctx = ctx;
    on_new_pie_timer(&info);
  }

  return RET_REMOVE;
}

/**
 * 初始化窗口上的子控件
 */
static void init_children_widget(widget_t* widget) {
  widget_foreach(widget, init_widget, widget);
}

/**
 * 打卡饼图窗口
 */
ret_t open_pie_window(void) {
  widget_t* win = window_open("window_pie");
  if (win) {
    init_children_widget(win);
    widget_on(win, EVT_WINDOW_OPEN, on_window_open, win);

    return RET_OK;
  }

  return RET_FAIL;
}
