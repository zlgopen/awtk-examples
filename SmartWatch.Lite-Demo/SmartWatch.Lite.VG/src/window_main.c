#include "awtk.h"
#include "custom_widgets/custom_widgets.h"
#include "window_controller/watch_main/watch_main.h"
#include "window_controller/round_menu_main/round_menu_main.h"

#ifdef WITHOUT_EXT_WIDGETS
#include "awtk_ext_widgets.h"
#endif

static widget_t *gesture_manager = NULL;
static widget_t *window_anim_menu = NULL;

static watch_main_t *watch_main_handle = NULL;
static round_menu_main_t *round_menu_main_handle = NULL;

static ret_t window_main_install_one(void *ctx, const void *iter) {
  widget_t *widget = WIDGET(iter);
  widget_t *win = widget_get_window(widget);
  if (tk_str_eq(widget->vt->type, "window_anim_menu")) {
    window_anim_menu = widget;
  } else if (widget->name != NULL) {
    if (tk_str_eq(widget->name, "gesture_manager")) {
      gesture_manager = widget;
    }
  }

  return RET_OK;
}

static ret_t watch_main_exit_func(struct _controller_base_t *handle,
                                  void *ctx) {
  round_menu_main_set_enable(round_menu_main_handle, TRUE);

  window_anim_menu_play_anim_move_child_for_load(
      handle->window_anim_menu, UI_ROUND_MENU, MENU_ANIMATING_MOVE_TYPE_RIGTH,
      FALSE);

  return RET_OK;
}

static ret_t round_menu_main_exit_func(struct _controller_base_t *handle,
                                       void *ctx) {
  watch_main_set_enable(watch_main_handle, TRUE);

  return RET_OK;
}

/* 裁剪了不需要的AWTK的扩展控件，减低 RAM 的消耗 */
ret_t window_main_ext_widgets_init() {
  widget_factory_t *f = widget_factory();

  widget_factory_register(f, WIDGET_TYPE_SLIDE_VIEW, slide_view_create);
  widget_factory_register(f, WIDGET_TYPE_SLIDE_INDICATOR,
                          slide_indicator_create);
  widget_factory_register(f, WIDGET_TYPE_SLIDE_INDICATOR_ARC,
                          slide_indicator_create_arc);

  return RET_OK;
}

/**
 * 初始化
 */
ret_t application_init() {

  window_manager_set_show_fps(window_manager(), TRUE);

#ifndef WITHOUT_EXT_WIDGETS
  /* 初始化扩展控件 */
  // tk_ext_widgets_init();
  window_main_ext_widgets_init();
#endif

  custom_widgets_init();

  widget_t *main = window_open("main");

  widget_foreach(main, window_main_install_one, main);

  watch_main_handle = watch_main_init(gesture_manager, window_anim_menu);
  round_menu_main_handle =
      round_menu_main_init(gesture_manager, window_anim_menu);

  return_value_if_fail(watch_main_handle != NULL, RET_OOM);
  return_value_if_fail(round_menu_main_handle != NULL, RET_OOM);

  controller_base_set_exit_func((controller_base_t *)watch_main_handle,
                                watch_main_exit_func, NULL);
  controller_base_set_exit_func((controller_base_t *)round_menu_main_handle,
                                round_menu_main_exit_func, NULL);

  return RET_OK;
}

ret_t application_exit() {
  watch_main_dispose(watch_main_handle);
  round_menu_main_dispose(round_menu_main_handle);
  return RET_OK;
}
