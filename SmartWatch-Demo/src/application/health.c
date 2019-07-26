#include <awtk.h>
#include "../custom_widgets/health_circle.h"
#include "../custom_widgets/gesture.h"
#include "../common/global.h"
#include "../custom_widgets/bar_graph.h"
#include <string.h>
#include "../common/log.h"

widget_t* health_win = NULL;
static uint32_t timer_act = 0;
static uint32_t timer_stand = 0;
static uint32_t timer_flash = 0;

static void init_children_widget(widget_t* widget);

static ret_t on_close_window(void* ctx, event_t* e) {
  (void)e;
  widget_t* win = WIDGET(ctx);

  if (win == health_win) {
    timer_remove(timer_act);
    timer_remove(timer_stand);
    timer_remove(timer_flash);
  }
  // window_close(win);

  open_and_close(win);

  return RET_OK;
}

static ret_t on_update_health_aims(void* ctx, event_t* e) {
  (void)ctx;
  value_t val;
  widget_t* main_hc = widget_lookup(health_win, "health:all", TRUE);
  widget_t* hc = e->target;
  widget_get_prop(hc, HEALTH_CIRCLE_PROP_VALUE_B, &val);

  // LOG("name:%s", hc->name);

  if (tk_str_start_with(hc->name, "activity")) {
    widget_set_prop(main_hc, HEALTH_CIRCLE_PROP_MAX_B, &val);
  } else if (tk_str_start_with(hc->name, "workout")) {
    widget_set_prop(main_hc, HEALTH_CIRCLE_PROP_MAX_M, &val);
  } else if (tk_str_start_with(hc->name, "stand")) {
    widget_set_prop(main_hc, HEALTH_CIRCLE_PROP_MAX_S, &val);
  } else {
    return RET_NOT_FOUND;
  }
  return RET_OK;
}

static void health_activity_bind(widget_t* win) {
  widget_t* main_hc = widget_lookup(win, "health:all", TRUE);
  widget_t* hc = widget_lookup(win, "activity:hc", TRUE);
  value_t val;
  widget_get_prop(main_hc, HEALTH_CIRCLE_PROP_MAX_B, &val);
  log_debug("aims is %d\n", (int)value_float(&val));
  widget_set_prop(hc, HEALTH_CIRCLE_PROP_VALUE_B, &val);

  /* 根据窗口中的value变化设置主窗口的圆环最大值 */
  widget_on(hc, EVT_VALUE_CHANGED, on_update_health_aims, win);
}

static void health_workout_bind(widget_t* win) {
  widget_t* main_hc = widget_lookup(win, "health:all", TRUE);
  widget_t* hc = widget_lookup(win, "workout:hc", TRUE);
  value_t val;
  widget_get_prop(main_hc, HEALTH_CIRCLE_PROP_MAX_M, &val);
  log_debug("aims is %d\n", (int)value_float(&val));
  widget_set_prop(hc, HEALTH_CIRCLE_PROP_VALUE_B, &val);

  /* 根据窗口中的value变化设置主窗口的圆环最大值 */
  widget_on(hc, EVT_VALUE_CHANGED, on_update_health_aims, win);
}

/**
 * @brief  打开设置锻炼目标窗口
 */
static ret_t on_open_set_workout(void* ctx, event_t* e) {
  (void)e;
  (void)ctx;
  widget_t* win = window_open("health_workout");
  if (win) {
    init_children_widget(win);
    /* 根据主窗口的圆环数据初始化本窗口圆环的数据 */
    widget_t* main_hc = widget_lookup(health_win, "health:all", TRUE);
    widget_t* hc = widget_lookup(win, "workout:hc", TRUE);
    value_t val;
    widget_get_prop(main_hc, HEALTH_CIRCLE_PROP_MAX_M, &val);
    log_debug("aims is %d\n", (int)value_float(&val));
    widget_set_prop(hc, HEALTH_CIRCLE_PROP_VALUE_B, &val);

    /* 根据窗口中的value变化设置主窗口的圆环最大值 */
    widget_on(hc, EVT_VALUE_CHANGED, on_update_health_aims, health_win);

    return RET_OK;
  }

  return RET_FAIL;
}

static void health_stand_bind(widget_t* win) {
  widget_t* main_hc = widget_lookup(win, "health:all", TRUE);
  widget_t* hc = widget_lookup(win, "stand:hc", TRUE);
  value_t val;
  widget_get_prop(main_hc, HEALTH_CIRCLE_PROP_MAX_S, &val);
  log_debug("aims is %d\n", (int)value_float(&val));
  widget_set_prop(hc, HEALTH_CIRCLE_PROP_VALUE_B, &val);

  /* 根据窗口中的value变化设置主窗口的圆环最大值 */
  widget_on(hc, EVT_VALUE_CHANGED, on_update_health_aims, win);
}

/**
 * @brief  打开设置站立目标窗口
 */
static ret_t on_open_set_stand(void* ctx, event_t* e) {
  (void)e;
  (void)ctx;
  widget_t* win = window_open("health_stand");
  if (win) {
    init_children_widget(win);
    /* 根据主窗口的圆环数据初始化本窗口圆环的数据 */
    widget_t* main_hc = widget_lookup(health_win, "health:all", TRUE);
    widget_t* hc = widget_lookup(win, "stand:hc", TRUE);
    value_t val;
    widget_get_prop(main_hc, HEALTH_CIRCLE_PROP_MAX_S, &val);
    log_debug("aims is %d\n", (int)value_float(&val));
    widget_set_prop(hc, HEALTH_CIRCLE_PROP_VALUE_B, &val);

    /* 根据窗口中的value变化设置主窗口的圆环最大值 */
    widget_on(hc, EVT_VALUE_CHANGED, on_update_health_aims, health_win);

    return RET_OK;
  }

  return RET_FAIL;
}

static ret_t on_hc_value_changed(void* ctx, event_t* e) {
  char text[48] = {0};
  value_t val;
  widget_t* win = WIDGET(ctx);
  widget_t* hc = e->target;
  widget_t* label = NULL;
  widget_get_prop(hc, HEALTH_CIRCLE_PROP_VALUE_B, &val);

  LOG("name:%s val:%d", hc->name, (int)value_float(&val));

  if (tk_str_start_with(hc->name, "activity")) {
    tk_snprintf(text, sizeof(text), "%d", (int)value_float(&val));
    label = widget_lookup(win, "activity:label", TRUE);
  } else if (tk_str_start_with(hc->name, "workout")) {
    tk_snprintf(text, sizeof(text), "%d", (int)value_float(&val));
    label = widget_lookup(win, "workout:label", TRUE);
  } else if (tk_str_start_with(hc->name, "stand")) {
    tk_snprintf(text, sizeof(text), "%d", (int)value_float(&val));
    label = widget_lookup(win, "stand:label", TRUE);
  } else {
    return RET_NOT_FOUND;
  }
  widget_set_text_utf8(label, text);

  return RET_OK;
}

static float_t widget_get_prop_float(widget_t* widget, const char* name, float_t defval) {
  value_t v;
  return_value_if_fail(widget_get_prop(widget, name, &v) == RET_OK, defval);

  return value_float(&v);
}

static void update_label_text(widget_t* widget, widget_t* label) {
  str_t str;

  str_init(&str, 0);
  str_from_int(&str, (int)widget_get_prop_float(widget, HEALTH_CIRCLE_PROP_VALUE_B, -1));
  widget_set_text_utf8(label, str.str);
  str_reset(&str);
}

// 当数据更新更新标签
static ret_t on_update_labels(void* ctx, event_t* e) {
  widget_t* hc = WIDGET(e->target);
  widget_t* win = WIDGET(ctx);
  return_value_if_fail(hc != NULL && win != NULL, RET_BAD_PARAMS);

  widget_t* mc = widget_lookup(win, "health:all_m", TRUE);
  widget_t* sc = widget_lookup(win, "health:all_s", TRUE);
  return_value_if_fail(mc != NULL && sc != NULL, RET_BAD_PARAMS);

  widget_t* label_h = widget_lookup(win, "health:label1", TRUE);
  widget_t* label_m = widget_lookup(win, "health:label2", TRUE);
  widget_t* label_s = widget_lookup(win, "health:label3", TRUE);
  assert(label_h != NULL && label_m != NULL && label_s != NULL);

  update_label_text(hc, label_h);
  update_label_text(mc, label_m);
  update_label_text(sc, label_s);

  return RET_OK;
}

/**
 * 初始化窗口控件
 */
static ret_t init_widget(void* ctx, const void* iter) {
  (void)ctx;
  widget_t* widget = WIDGET(iter);

  if (widget != NULL && widget->name != NULL) {
    widget_t* win = widget_get_window(widget);
    if (tk_str_start_with(widget->name, "health:")) {
      char* name = widget->name + 7;
      if (tk_str_eq(name, "gesture")) {
        // widget_on(widget, EVT_SLIDE_UP, on_open_set_activity, win);
        widget_on(widget, EVT_SLIDE_RIGHT, on_close_window, win);
      } else if (tk_str_eq(name, "all")) {
        widget_on(widget, EVT_VALUE_CHANGED, on_update_labels, win);
      }
    } else if (tk_str_start_with(widget->name, "activity:")) {
      char* name = widget->name + 9;
      if (tk_str_eq(name, "gesture")) {
        widget_on(widget, EVT_SLIDE_DOWN, on_close_window, win);
        widget_on(widget, EVT_SLIDE_UP, on_open_set_workout, win);
      } else if (tk_str_eq(name, "hc")) {
        widget_on(widget, EVT_VALUE_CHANGED, on_hc_value_changed, win);
      }
    } else if (tk_str_start_with(widget->name, "workout:")) {
      char* name = widget->name + 8;
      if (tk_str_eq(name, "gesture")) {
        widget_on(widget, EVT_SLIDE_DOWN, on_close_window, win);
        widget_on(widget, EVT_SLIDE_UP, on_open_set_stand, win);
      } else if (tk_str_eq(name, "hc")) {
        widget_on(widget, EVT_VALUE_CHANGED, on_hc_value_changed, win);
      }
    } else if (tk_str_start_with(widget->name, "stand:")) {
      char* name = widget->name + 6;
      if (tk_str_eq(name, "gesture")) {
        widget_on(widget, EVT_SLIDE_DOWN, on_close_window, win);
      } else if (tk_str_eq(name, "hc")) {
        widget_on(widget, EVT_VALUE_CHANGED, on_hc_value_changed, win);
      }
    }
  }

  return RET_OK;
}

static void init_children_widget(widget_t* widget) {
  widget_foreach(widget, init_widget, widget);
}

static ret_t visit_count(void* ctx, const void* data) {
  bar_t* elem = (bar_t*)data;
  *(float*)ctx += elem->high - elem->low;
  return RET_OK;
}

static ret_t on_updata_interface(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);
  widget_t* hc = widget_lookup(win, "health:all", TRUE);
  return_value_if_fail(hc != NULL, RET_NOT_FOUND);

  float sum = 0;
  darray_t* data = global_active_data->bar_data;
  darray_foreach(data, visit_count, &sum);
  value_t v;
  value_set_float(&v, sum * 5);
  widget_set_prop(hc, HEALTH_CIRCLE_PROP_VALUE_B, &v);

  sum = 0;
  data = global_workout_data->bar_data;
  darray_foreach(data, visit_count, &sum);
  value_reset(&v);
  value_set_float(&v, sum * 5);
  // widget_set_prop(hc, HEALTH_CIRCLE_PROP_VALUE_M, &v);
  widget_t* mc = widget_lookup(win, "health:all_m", TRUE);
  widget_set_prop(mc, HEALTH_CIRCLE_PROP_VALUE_B, &v);

  sum = 0;
  data = global_stand_data->bar_data;
  darray_foreach(data, visit_count, &sum);
  value_reset(&v);
  value_set_float(&v, sum * 10);
  // widget_set_prop(hc, HEALTH_CIRCLE_PROP_VALUE_S, &v);
  widget_t* sc = widget_lookup(win, "health:all_s", TRUE);
  widget_set_prop(sc, HEALTH_CIRCLE_PROP_VALUE_B, &v);

  widget_invalidate(win, NULL);

  return RET_REPEAT;
}

// 添加计时器来更新数据
static ret_t init_data(widget_t* win) {
  // 绑定activity bar数据
  widget_t* activity = widget_lookup(win, "activity_bar", TRUE);
  assert(activity != NULL);
  if (activity) {
    bar_graph_t* bar = BAR_GRAPH(activity);
    uint32_t size = widget_get_prop_int(activity, "size", -1);
    global_active_data->bar_max_size = size;
    bar->data = global_active_data->bar_data;
  }

  // 绑定workout bar数据
  widget_t* workout = widget_lookup(win, "workout_bar", TRUE);
  assert(workout != NULL);
  if (workout) {
    bar_graph_t* bar = BAR_GRAPH(workout);
    uint32_t size = widget_get_prop_int(workout, "size", -1);
    global_workout_data->bar_max_size = size;
    bar->data = global_workout_data->bar_data;
  }

  // 绑定stand bar数据
  widget_t* stand = widget_lookup(win, "stand_bar", TRUE);
  assert(stand != NULL);
  if (stand) {
    bar_graph_t* bar = BAR_GRAPH(stand);
    uint32_t size = widget_get_prop_int(stand, "size", -1);
    global_stand_data->bar_max_size = size;
    bar->data = global_stand_data->bar_data;
  }

  if (timer_act != TK_INVALID_ID) {
    timer_remove(timer_act);
    darray_clear(global_active_data->bar_data);
    darray_clear(global_workout_data->bar_data);
  }
  if (timer_stand != TK_INVALID_ID) {
    timer_remove(timer_stand);
    darray_clear(global_stand_data->bar_data);
  }
  if (timer_flash != TK_INVALID_ID) {
    timer_remove(timer_flash);
  }

  timer_act = widget_add_timer(win, on_timer_add_activity_bar, 2000);
  timer_stand = widget_add_timer(win, on_timer_add_stand_bar, 2000);
  timer_flash = widget_add_timer(win, on_updata_interface, 2000);  // 每2秒刷新界面

  return RET_OK;
}

ret_t on_open_health(void* ctx, event_t* e) {
  widget_t* to_close = WIDGET(ctx);
  (void)e;
  health_win = window_open_and_close(g_window_enum->health.str, to_close);
  if (health_win != NULL) {
    init_children_widget(health_win);
    init_data(health_win);

    health_activity_bind(health_win);

    health_workout_bind(health_win);

    health_stand_bind(health_win);

    return RET_OK;
  }
  return RET_FAIL;
}
