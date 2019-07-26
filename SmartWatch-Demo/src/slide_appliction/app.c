#include "app.h"
#include "../common/animator.h"
#include "../common/log.h"
#include "../custom_widgets/gesture.h"
//#include "../common/view.h"
#include "../common/global.h"
#include "../window_main.h"
#include "../custom_widgets/slide_menu_v.h"

#define SHAKE "_child_shake"
#define UP "_up"

#define NAME_APP "app"
#define NAME_VIEW "name_view"
#define NAME_APP_VIEW "name_app_view"
#define NAME_APP_VIEW "name_app_view"
#define LONG_PRESS_TIMER "_long_press_timer"
#define IS_MOVE "_is_move"
#define HAS_MOVE "_first_move"
#define MOVE_MODE "_move_mode"

// 判断项对项的操作
typedef enum _item_drag_mode_t {
  ITEM_DRAG_NONE = 0,
  ITEM_DRAG_HORIZONTAL,
  ITEM_DRAG_VERTICAL
} item_drag_mode_t;

// static const char* s_item_drag_mode = "_item_drag_mode";

typedef enum _move_mode_t { MOVE_MODE_NONE = 0, MOVE_MODE_DRAG, MOVE_MODE_SLIDE } move_mode_t;

#include "../application/calendar.h"
#include "../application/weather.h"
#include "../application/health.h"
#include "../application/music.h"
#include "../application/heart_rate.h"

ret_t open_app(void* ctx, event_t* e) {
  (void)e;
  widget_t* to_close = WIDGET(ctx);

  window_history_push(to_close);

  app_init(to_close, "slide_left");
  LOG("");
  return RET_OK;
}

static ret_t on_visit(void* ctx, const void* data) {
  (void)ctx;
  widget_t* widget = WIDGET(data);
  widget_t* button = widget;
  const char* name = button->name;
  (void)name;
  LOG("%s", name);

  return RET_OK;
}

static ret_t on_window_close(void* ctx, event_t* e) {
  (void)e;
  widget_t* window = WIDGET(ctx);
  LOG("name:%s", window->name);

  widget_t* app_slide = widget_lookup(window, "app_slide", TRUE);
  if (app_slide != NULL) {
    global_data.app.index = ((slide_menu_v_t*)app_slide)->value;
    LOG("value:%d", ((slide_menu_v_t*)app_slide)->value);
  }

  str_set(&(global_data.previous_window), window->name);

  image_manager_unload_unused(image_manager(), 0);

  return RET_OK;
}

static ret_t on_return_window(void* ctx, event_t* e) {
  (void)e;
  widget_t* win = WIDGET(ctx);

  darray_t* window_history = &(global_data.window_history);
  assert(window_history->size != 0);
  str_t* name = (str_t*)darray_tail(window_history);
  if (name != NULL) {
    widget_t* widget = open_watch_and_close(name->str, win);
    widget_set_prop_str(widget, WIDGET_PROP_OPEN_ANIM_HINT, "slide_right");
  }

  window_history_pop();

  return RET_OK;
}

ret_t on_app_mail_return(void* ctx, event_t* e) {
  (void)e;
  widget_t* widget = WIDGET(ctx);
  window_close(widget);
  return RET_OK;
}

static ret_t on_close_window(void* ctx, event_t* e) {
  (void)e;
  widget_t* win = WIDGET(ctx);

  open_and_close(win);

  return RET_OK;
}

ret_t on_app_mail(void* ctx, event_t* e) {
  widget_t* to_close = WIDGET(ctx);
  (void)e;
  widget_t* window = window_open_and_close("app_default", to_close);
  if (window != NULL) {
    widget_t* gesture = widget_lookup(window, "gesture", TRUE);
    if (gesture != NULL) {
      widget_on(gesture, EVT_SLIDE_RIGHT, on_close_window, window);
    }
  }
  return RET_OK;
}

void app_init(widget_t* to_close, const char* open_anim) {
  LOG("to_close:%s", to_close->name);
  widget_t* window = window_open_and_close(g_window_enum->app.str, to_close);
  widget_on(window, EVT_WINDOW_CLOSE, on_window_close, window);

  widget_set_prop_str(window, WIDGET_PROP_OPEN_ANIM_HINT, open_anim);

  widget_t* app_slide = widget_lookup(window, "app_slide", TRUE);
  if (app_slide != NULL) {
    darray_foreach(app_slide->children, on_visit, NULL);
    slide_menu_v_set_value(app_slide, (uint32_t)global_data.app.index);
  }

  widget_t* app_calendar = widget_lookup(window, "app_calendar", TRUE);
  if (app_calendar != NULL) {
    widget_on(app_calendar, EVT_CLICK, on_open_calendar, window);
  }

  widget_t* app_cloudy = widget_lookup(window, "app_cloudy", TRUE);
  if (app_cloudy != NULL) {
    widget_on(app_cloudy, EVT_CLICK, on_open_weather, window);
  }

  // 心率
  widget_t* app_heart = widget_lookup(window, "app_heart", TRUE);
  if (app_heart != NULL) {
    widget_on(app_heart, EVT_CLICK, on_open_heart_rate, window);
  }

  widget_t* app_music = widget_lookup(window, "app_music", TRUE);
  if (app_music != NULL) {
    widget_on(app_music, EVT_CLICK, on_open_music, window);
  }

  widget_t* app_training = widget_lookup(window, "app_training", TRUE);
  if (app_training != NULL) {
    widget_on(app_training, EVT_CLICK, on_open_health, window);
  }

  // 右滑返回
  widget_t* app_gesture = widget_lookup(window, "app_gesture", TRUE);
  if (app_gesture != NULL) {
    widget_on(app_gesture, EVT_SLIDE_RIGHT, on_return_window, window);
  }

  widget_t* app_mail = widget_lookup(window, "app_mail", TRUE);
  if (app_mail != NULL) {
    widget_on(app_mail, EVT_CLICK, on_app_mail, window);
  }

  widget_layout(window);
}

void app_remember(widget_t* window) {
  widget_t* app_slide = widget_lookup(window, "app_slide", TRUE);
  if (app_slide != NULL) {
    global_data.app.index = ((slide_menu_v_t*)app_slide)->value;
    LOG("value:%d", ((slide_menu_v_t*)app_slide)->value);
  }
}
