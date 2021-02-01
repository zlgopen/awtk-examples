#include "awtk.h"
extern ret_t application_init(void);
static void init_children_widget(widget_t* widget);
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
  widget_t* language_btn = WIDGET(ctx);
  (void)e;

  const char* language = locale_info()->language;
  if (tk_str_eq(language, "en")) {
    change_locale("zh_CN");
    widget_use_style(language_btn, "zh");
  } else {
    change_locale("en_US");
    widget_use_style(language_btn, "en");
  }
  return RET_OK;
}

static ret_t on_open_basic_window(void* ctx, event_t* e) {
  (void)ctx;
  (void)e;

  open_basic_window();

  return RET_OK;
}
static ret_t on_open_background_window(void* ctx, event_t* e) {
  (void)ctx;
  (void)e;

  open_background_window();

  return RET_OK;
}
static ret_t on_open_listview_window(void* ctx, event_t* e) {
  (void)ctx;
  (void)e;

  open_listview_window();

  return RET_OK;
}

static ret_t on_open_animator_window(void* ctx, event_t* e) {
  (void)ctx;
  (void)e;

  open_animator_window();

  return RET_OK;
}
/**
 * 子控件初始化(主要是设置click回调、初始显示信息)
 */
static ret_t init_widget(void* ctx, const void* iter) {
  widget_t* widget = WIDGET(iter);
  widget_t* win = widget_get_window(widget);
  (void)ctx;

  if (widget->name != NULL) {
    const char* name = widget->name;

    if (tk_str_eq(name, "language_btn")) {
      widget_on(widget, EVT_CLICK, on_language, widget);
    } else if (tk_str_eq(name, "basic_btn")) {
      widget_on(widget, EVT_CLICK, on_open_basic_window, widget);
      widget_on(widget, EVT_LONG_PRESS, on_open_basic_window, widget);
    } else if (tk_str_eq(name, "background_btn")) {
      widget_on(widget, EVT_CLICK, on_open_background_window, widget);
      widget_on(widget, EVT_LONG_PRESS, on_open_background_window, widget);
    } else if (tk_str_eq(name, "listview_btn")) {
      widget_on(widget, EVT_CLICK, on_open_listview_window, widget);
      widget_on(widget, EVT_LONG_PRESS, on_open_listview_window, widget);
    } else if (tk_str_eq(name, "animator_btn")) {
      widget_on(widget, EVT_CLICK, on_open_animator_window, widget);
      widget_on(widget, EVT_LONG_PRESS, on_open_animator_window, widget);
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
 * 初始化
 */
ret_t application_init() {
  widget_t* win = window_open("home_page/home_page");

  if (win) {
    init_children_widget(win);
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
