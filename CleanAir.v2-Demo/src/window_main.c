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
 * 2018-10-29 TangJunJie created
 *
 */
#include "awtk.h"
#include "ext_widgets/image_animation/image_animation.h"
#include "widget_animators/widget_animator_opacity.h"
#include "widget_animators/widget_animator_rotation.h"
#include <time.h>
#include <stdio.h>

/**
 * 是否使能背景淡出淡入
 */
#define WITH_BKGND_FADE

/**
 * 是否使能风扇动画
 */
#define WITH_WIND_ANIM

/**
 * 是否使能报警图标动画
 */
//#define WITH_ALARM_ANIM

/**
 * 背景(主题)数量
 */
#define BKGND_IMG_CNT 2

/**
 * 背景(主题)淡出/淡入的持续时间
 */
#define BKGND_FADE_TIME 500

/**
 * 打开窗口(定时、记录、设置)
 */
ret_t open_timing_window(time_t* result);
ret_t open_record_window(void);
ret_t open_setting_window(void);

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
 * Label文本显示随机int
 */
static void label_set_random_int(widget_t* label, int32_t min_num, int32_t max_num) {
  if (label) {
    char text[32];
    int32_t val = fmod(rand(), max_num - min_num) + min_num;
    widget_set_text_utf8(label, tk_itoa(text, sizeof(text), val));
  }
}

/**
 * Label文本显示随机double, 并返回是否超出报警范围
 */
static bool_t label_set_random_double(widget_t* label, double min_num, double max_num,
                                      double alarm) {
  if (label) {
    char text[64];
    double val = fmod(rand(), max_num - min_num) + min_num;
    tk_snprintf(text, sizeof(text), "%.1f", val);
    widget_set_text_utf8(label, text);

    return (val >= alarm);
  }
  return FALSE;
}

/**
 * 获取widget的动画对象, 没有则新建
 */
widget_animator_t* widget_animator_get(widget_t* widget, const char* name, uint32_t duration,
                                       bool_t opacity) {
  widget_animator_t* animator = NULL;
  value_t val;
  value_set_pointer(&val, NULL);
  if (widget_get_prop(widget, name, &val) != RET_OK || value_pointer(&val) == NULL) {
    if (opacity) {
      animator = widget_animator_opacity_create(widget, duration, 0, EASING_SIN_OUT);
    } else {
      animator = widget_animator_rotation_create(widget, duration, 0, EASING_LINEAR);
    }
    value_set_pointer(&val, animator);
    widget_set_prop(widget, name, &val);
  } else {
    animator = (widget_animator_t*)value_pointer(&val);
  }
  return animator;
}

/**
 * 手动停止widget的动画
 */
void widget_animator_stop_with_name(widget_t* widget, const char* name) {
  widget_animator_t* animator = NULL;
  value_t val;
  value_set_pointer(&val, NULL);
  if (widget_get_prop(widget, name, &val) == RET_OK && value_pointer(&val) != NULL) {
    animator = (widget_animator_t*)value_pointer(&val);
    widget_animator_destroy(animator);

    value_set_pointer(&val, NULL);
    widget_set_prop(widget, name, &val);
  }
}

/**
 * 背景淡出
 */
static void bkgnd_image_fade_out(widget_t* widget, uint32_t duration) {
  widget_animator_t* animator = widget_animator_opacity_create(widget, duration, 0, EASING_SIN_OUT);
  widget_animator_opacity_set_params(animator, 255, 30);
  widget_animator_start(animator);
}

/**
 * 背景淡入
 */
static void bkgnd_image_fade_in(widget_t* widget, uint32_t duration) {
  widget_animator_t* animator = widget_animator_opacity_create(widget, duration, 0, EASING_SIN_OUT);
  widget_animator_opacity_set_params(animator, 30, 255);
  widget_animator_start(animator);
}

/**
 * 获取当前时间的字符串
 */
static void time_now_str(char* str, size_t size) {
  date_time_t dt;
  date_time_init(&dt);
  snprintf(str, size, "%04d/%02d/%02d %02d:%02d:%02d", dt.year, dt.month, dt.day, dt.hour,
           dt.minute, dt.second);
}

/**
 * 更新Label上显示的系统时间
 */
static ret_t on_systime_update(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);
  widget_t* label = widget_lookup(win, "sys_time", TRUE);
  if (label) {
    char text[64];
    time_now_str(text, sizeof(text));
    widget_set_text_utf8(label, text);
  }

  return RET_REPEAT;
}

/**
 * 主题背景切换
 */
static void bkgnd_change(widget_t* widget) {
  if (widget) {
    char style[5];
    value_t val;
    value_set_uint32(&val, 0);
    if (widget_get_prop(widget, "style_id", &val) != RET_OK || value_uint32(&val) == 0) {
      value_set_uint32(&val, 1);
    } else {
      value_set_uint32(&val, (value_uint32(&val) + 1) % BKGND_IMG_CNT);
    }
    widget_set_prop(widget, "style_id", &val);

    tk_snprintf(style, sizeof(style), "bg%u", value_uint32(&val));
    widget_use_style(widget, style);
  }
}

/**
 * 主题背景延迟淡入
 */
static ret_t on_bkgnd_delay_in(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);
  widget_t* bkgnd = widget_lookup(win, "bkgnd", TRUE);

  if (bkgnd) {
    bkgnd_image_fade_in(bkgnd, BKGND_FADE_TIME);
    bkgnd_change(bkgnd);
  }

  return RET_REMOVE;
}

/**
 * 更新主题背景(淡出淡入)
 */
static ret_t on_bkgnd_update(const timer_info_t* timer) {
  widget_t* win = WIDGET(timer->ctx);
  widget_t* bkgnd = widget_lookup(win, "bkgnd", TRUE);

  if (bkgnd) {
#ifdef WITH_BKGND_FADE
    bkgnd_image_fade_out(bkgnd, BKGND_FADE_TIME);
    timer_add(on_bkgnd_delay_in, win, BKGND_FADE_TIME);
#else
    bkgnd_change(bkgnd);
#endif
  }

  return RET_REPEAT;
}

/**
 * 启动风扇(启动风扇、风向箭头的动画)
 */
static void on_wind_on(widget_t* win) {
  widget_t* wind_in = widget_lookup(win, "wind_in", TRUE);
  widget_t* wind_out = widget_lookup(win, "wind_out", TRUE);
  widget_t* fan_1 = widget_lookup(win, "fan_1", TRUE);
  widget_t* fan_2 = widget_lookup(win, "fan_2", TRUE);
  widget_animator_t* animator;

#ifdef WITH_WIND_ANIM
  animator = widget_animator_get(wind_in, "animator", 1000, TRUE);
  widget_animator_opacity_set_params(animator, 50, 255);
  widget_animator_set_yoyo(animator, 0);
  widget_animator_start(animator);

  animator = widget_animator_get(wind_out, "animator", 1000, TRUE);
  widget_animator_opacity_set_params(animator, 50, 255);
  widget_animator_set_yoyo(animator, 0);
  widget_animator_start(animator);

  animator = widget_animator_get(fan_2, "animator", 4000, FALSE);
  widget_animator_rotation_set_params(animator, 0, 2 * 3.14159265);
  widget_animator_set_repeat(animator, 0);
  widget_animator_start(animator);
#else
  (void)wind_in;
  (void)wind_out;
  (void)fan_2;
  (void)animator;
#endif

  image_animation_play(fan_1);
}

/**
 * 停止风扇(启动风扇、风向箭头的动画)
 */
static void on_wind_off(widget_t* win) {
  widget_t* wind_in = widget_lookup(win, "wind_in", TRUE);
  widget_t* wind_out = widget_lookup(win, "wind_out", TRUE);
  widget_t* fan_1 = widget_lookup(win, "fan_1", TRUE);
  widget_t* fan_2 = widget_lookup(win, "fan_2", TRUE);
#ifdef WITH_WIND_ANIM
  widget_animator_stop_with_name(wind_in, "animator");
  widget_animator_stop_with_name(wind_out, "animator");
  widget_animator_stop_with_name(fan_2, "animator");
  image_set_rotation(fan_2, 0);
#else
  (void)wind_in;
  (void)wind_out;
  (void)fan_2;
#endif
  image_animation_pause(fan_1);
}

/**
 * 显示报警指示器(报警图标闪烁)
 */
static void on_alarm_on(widget_t* win) {
  widget_t* alarm = widget_lookup(win, "alarm_status", TRUE);
#ifdef WITH_ALARM_ANIM
  widget_animator_t* animator = widget_animator_get(alarm, "animator", 1000, TRUE);
  widget_animator_opacity_set_params(animator, 50, 255);
  widget_animator_set_yoyo(animator, 0xFFFFFFFF);
  widget_animator_start(animator);
#endif
  widget_set_visible(alarm, TRUE, FALSE);
}

/**
 * 关闭报警指示器(报警图标隐藏)
 */
static void on_alarm_off(widget_t* win) {
  widget_t* alarm = widget_lookup(win, "alarm_status", TRUE);
#ifdef WITH_ALARM_ANIM
  widget_animator_stop(alarm, "animator");
#endif
  widget_set_visible(alarm, FALSE, FALSE);
}

/**
 * 模拟温度、湿度等读数变化(随机数值), 并判断温度是否超过报警值，超过则报警
 */
static ret_t on_reading_update(const timer_info_t* timer) {
  bool_t alarm = FALSE;
  widget_t* win = WIDGET(timer->ctx);
  widget_t* fan1 = widget_lookup(win, "fan1", TRUE);
  widget_t* fan2 = widget_lookup(win, "fan2", TRUE);
  widget_t* co_2 = widget_lookup(win, "CO_2", TRUE);
  widget_t* flow = widget_lookup(win, "flow", TRUE);
  widget_t* pm2_5 = widget_lookup(win, "PM2_5", TRUE);
  widget_t* temperature_out1 = widget_lookup(win, "temperature_out1", TRUE);
  widget_t* temperature_out2 = widget_lookup(win, "temperature_out2", TRUE);

  label_set_random_int(fan1, 80, 85);
  label_set_random_int(fan2, 80, 82);
  label_set_random_int(pm2_5, 18, 20);
  label_set_random_int(co_2, 350, 352);
  label_set_random_int(flow, 500, 505);

  if (label_set_random_double(temperature_out1, 20.5, 22.8, 22.0)) {
    widget_use_style(temperature_out1, "reading_temp_hum_alarm");
    alarm = TRUE;
  } else {
    widget_use_style(temperature_out1, "reading_temp_hum_black");
  }

  if (label_set_random_double(temperature_out2, 20.5, 22.8, 22.0)) {
    widget_use_style(temperature_out2, "reading_temp_hum_alarm");
    alarm = TRUE;
  } else {
    widget_use_style(temperature_out2, "reading_temp_hum_black");
  }

  if (alarm) {
    on_alarm_on(win);
  } else {
    on_alarm_off(win);
  }

  return RET_REPEAT;
}

/**
 * 启动/关闭设备(开始/停止模拟温度、湿度等读数变化, 风扇动画)
 */
static ret_t on_switch(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* btn = widget_lookup(win, "switch", TRUE);
  value_t val;
  value_set_uint32(&val, 0);
  if (widget_get_prop(btn, "timer_id", &val) != RET_OK || value_uint32(&val) == 0) {
    value_set_uint32(&val, timer_add(on_reading_update, win, 1000));
    on_wind_on(win);
  } else {
    timer_remove(value_uint32(&val));
    on_wind_off(win);
    value_set_uint32(&val, 0);
  }
  widget_set_prop(btn, "timer_id", &val);

  return RET_OK;
}

/**
 * 开始/停止背景切换(10s一次)
 */
static ret_t on_auto(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* btn = widget_lookup(win, "auto", TRUE);
  value_t val;
  value_set_uint32(&val, 0);
  if (widget_get_prop(btn, "timer_id", &val) != RET_OK || value_uint32(&val) == 0) {
    value_set_uint32(&val, timer_add(on_bkgnd_update, win, 10000));
    widget_use_style(btn, "auto_btn");
  } else {
    timer_remove(value_uint32(&val));
    value_set_uint32(&val, 0);
    widget_use_style(btn, "auto_btn_2a");
  }
  widget_set_prop(btn, "timer_id", &val);

  return RET_OK;
}

/**
 * 弹出定时窗口
 */
static ret_t on_timing(void* ctx, event_t* e) {
  (void)e;
  time_t t;
  widget_t* win = WIDGET(ctx);
  widget_t* timing = widget_lookup(win, "timing_status", TRUE);
  if (open_timing_window(&t) == RET_OK) {
    widget_set_visible(timing, TRUE, FALSE);
  } else {
    widget_set_visible(timing, FALSE, FALSE);
  }
  return RET_OK;
}

/**
 * 弹出记录窗口
 */
static ret_t on_record(void* ctx, event_t* e) {
  (void)ctx;
  (void)e;
  return open_record_window();
}

/**
 * 弹出设置窗口
 */
static ret_t on_setting(void* ctx, event_t* e) {
  (void)ctx;
  (void)e;
  return open_setting_window();
}

/**
 * 频率+1
 */
static ret_t on_frequency_inc(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* label = widget_lookup(win, "frequency", TRUE);
  (void)e;

  return label_add(label, 1);
}

/**
 * 频率-1
 */
static ret_t on_frequency_dec(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* label = widget_lookup(win, "frequency", TRUE);
  (void)e;

  return label_add(label, -1);
}

/**
 * 温度+1
 */
static ret_t on_temperature_inc(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* label = widget_lookup(win, "temperature", TRUE);
  (void)e;

  return label_add(label, 1);
}

/**
 * 温度-1
 */
static ret_t on_temperature_dec(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* label = widget_lookup(win, "temperature", TRUE);
  (void)e;

  return label_add(label, -1);
}

/**
 * 湿度+1
 */
static ret_t on_humidity_inc(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* label = widget_lookup(win, "humidity", TRUE);
  (void)e;

  return label_add(label, 1);
}

/**
 * 湿度-1
 */
static ret_t on_humidity_dec(void* ctx, event_t* e) {
  widget_t* win = WIDGET(ctx);
  widget_t* label = widget_lookup(win, "humidity", TRUE);
  (void)e;

  return label_add(label, -1);
}

/**
 * 子控件初始化(主要是设置click回调、初始显示信息)
 */
static ret_t init_widget(void* ctx, const void* iter) {
  widget_t* widget = WIDGET(iter);

  (void)ctx;

  if (widget->name != NULL) {
    const char* name = widget->name;
    if (tk_str_eq(name, "switch")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_switch, win);
    } else if (tk_str_eq(name, "auto")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_auto, win);
    } else if (tk_str_eq(name, "timing")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_timing, win);
    } else if (tk_str_eq(name, "record")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_record, win);
    } else if (tk_str_eq(name, "setting")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_setting, win);
    } else if (tk_str_eq(name, "frequency_inc")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_frequency_inc, win);
      button_set_repeat(widget, 200);
    } else if (tk_str_eq(name, "frequency_dec")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_frequency_dec, win);
      button_set_repeat(widget, 200);
    } else if (tk_str_eq(name, "temperature_inc")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_temperature_inc, win);
      button_set_repeat(widget, 200);
    } else if (tk_str_eq(name, "temperature_dec")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_temperature_dec, win);
      button_set_repeat(widget, 200);
    } else if (tk_str_eq(name, "humidity_inc")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_humidity_inc, win);
      button_set_repeat(widget, 200);
    } else if (tk_str_eq(name, "humidity_dec")) {
      widget_t* win = widget_get_window(widget);
      widget_on(widget, EVT_CLICK, on_humidity_dec, win);
      button_set_repeat(widget, 200);
    } else if (tk_str_eq(name, "sys_time")) {
      char text[64];
      time_now_str(text, sizeof(text));
      widget_set_text_utf8(widget, text);
    } else if (tk_str_eq(name, "alarm_status")) {
      widget_set_visible(widget, FALSE, FALSE);
    } else if (tk_str_eq(name, "timing_status")) {
      widget_set_visible(widget, FALSE, FALSE);
    }
  }

  return RET_OK;
}

/**
 * 初始化窗口上的子控件
 */
static void init_children_widget(widget_t* widget) {
  widget_foreach(widget, init_widget, widget);
}

/**
 * 中英文互译
 */
static ret_t change_locale(const char* str) {
  char country[3];
  char language[3];

  strncpy(language, str, 2);
  strncpy(country, str + 3, 2);
  locale_info_change(locale_info(), language, country);

  return RET_OK;
}

/**
 * 点击中英文互译按钮
 */
static ret_t on_language(void* ctx, event_t* e) {
  (void)ctx;

  widget_t* language_btn = (widget_t*)e->target;
  const char* language = locale_info()->language;
  if (tk_str_eq(language, "en")) {
    change_locale("zh_CN");
    widget_use_style(language_btn, "language_zh");  
  } else {
    change_locale("en_US");
    widget_use_style(language_btn, "language_en");
  }
  return RET_OK;
}

/**
 * 打开主窗口
 */
ret_t application_init(void) {
  system_info_set_default_font(system_info(), "default");
  window_manager_set_cursor(window_manager(), NULL);
  
  widget_t* win = window_open("home_page/home_page");
  if (win) {
    init_children_widget(win);

    change_locale("zh_CN");
    widget_t* lang_btn = widget_lookup(win, "language_btn", TRUE);
    widget_on(lang_btn, EVT_CLICK, on_language, win);

    timer_add(on_systime_update, win, 1000);
    on_switch(win, NULL);
    on_auto(win, NULL);
    return RET_OK;
  }
  return RET_FAIL;
}

/**
* 退出
*/
ret_t application_exit(void) {
        log_debug("application_exit");
        return RET_OK;
}
