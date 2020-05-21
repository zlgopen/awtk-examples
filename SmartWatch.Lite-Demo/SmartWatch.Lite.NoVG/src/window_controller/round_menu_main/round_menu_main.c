#include "round_menu_main.h"
#include "../music_main/music_main.h"
#include "../weather_main/weather_main.h"
#include "../training_main/training_main.h"
#include "../calendar_main/calendar_main.h"

typedef struct _window_info_t {
  char *window_name;
  char *button_name;
  void *window_handle;
  controller_base_create_t create;
  controller_base_dispose_t dispose;
  controller_base_set_enable_t set_enable;
} window_info_t;

extern void controller_base_send_exit_fun(controller_base_t *handle);
extern ret_t controller_base_set_enable(controller_base_t *handle,
                                        bool_t enable);

static window_info_t window_info_list[] = {
    {UI_MUSIC, BUTTON_NAME_MUSIC, NULL,
     (controller_base_create_t)music_main_init, controller_base_dispose,
     (controller_base_set_enable_t)music_main_set_enable},
    {UI_WEATHER, BUTTON_NAME_WEATHER, NULL,
     (controller_base_create_t)weather_main_init, controller_base_dispose,
     (controller_base_set_enable_t)weather_main_set_enable},
    {UI_TRAINING, BUTTON_NAME_TRAINING, NULL,
     (controller_base_create_t)training_main_init, controller_base_dispose,
     (controller_base_set_enable_t)training_main_set_enable},
    {UI_CALENDAR, BUTTON_NAME_CALENDAR, NULL,
     (controller_base_create_t)calendar_main_init, controller_base_dispose,
     (controller_base_set_enable_t)calendar_main_set_enable},
};

static ret_t round_menu_main_on_click_gesture_fun(gesture_type_t gesture_type,
                                                  point_t *start_point,
                                                  point_t *curr_point,
                                                  void *ctx) {
  round_menu_main_t *handle = (round_menu_main_t *)(ctx);
  controller_base_t *base = (controller_base_t *)ctx;

  return_value_if_fail(base->enable, RET_BAD_PARAMS);

  if (gesture_type == GESTURE_LEFT_TO_RIGHT_TYPE) {
    round_menu_main_set_enable(handle, FALSE);
    controller_base_send_exit_fun(base);
  }

  return RET_OK;
}

static ret_t round_menu_main_on_open_window(void *ctx, event_t *e) {
  size_t j = 0;
  char *button_name = NULL;
  uint32_t len = ARRAY_LEN(window_info_list);
  controller_base_t *base = (controller_base_t *)ctx;
  round_menu_main_t *handle = (round_menu_main_t *)ctx;
  widget_t *widget = WIDGET(e->target);
  widget_t *parent = widget->parent;
  return_value_if_fail(widget->name != NULL && strlen(widget->name) > 5,
                       RET_FAIL) button_name = widget->name + 5;

  for (j = 0; j < len; j++) {
    if (button_name != NULL &&
        tk_str_cmp(button_name, window_info_list[j].button_name) == 0) {
      if (window_info_list[j].window_handle != NULL &&
          window_info_list[j].set_enable != NULL) {

        round_menu_main_set_enable(handle, FALSE);
        window_info_list[j].set_enable(window_info_list[j].window_handle, TRUE);
        window_anim_menu_play_anim_move_child_for_load(
            base->window_anim_menu, window_info_list[j].window_name,
            MENU_ANIMATING_MOVE_TYPE_RIGTH, FALSE);
        break;
      } else {
        assert(!"window not created");
      }
    }
  }

  if (parent != NULL) {
    WIDGET_FOR_EACH_CHILD_BEGIN(parent, iter, i)
    if (iter == widget) {
      handle->window_index = i;
      break;
    }
    WIDGET_FOR_EACH_CHILD_END()
  }

  return RET_FAIL;
}

static ret_t
round_menu_main_on_exit_button_window_func(struct _controller_base_t *handle,
                                           void *ctx) {
  controller_base_t *base = (controller_base_t *)ctx;
  round_menu_main_t *round_menu_main_handle = (round_menu_main_t *)ctx;
  round_menu_main_set_enable(round_menu_main_handle, TRUE);
  window_anim_menu_play_anim_move_child_for_load(
      base->window_anim_menu, UI_ROUND_MENU, MENU_ANIMATING_MOVE_TYPE_RIGTH,
      TRUE);

  return RET_OK;
}

static ret_t
round_menu_main_on_window_anim_load_widget_func(widget_t *load_widget,
                                                void *ctx) {
  size_t n = 0;
  char *button_name = NULL;
  round_menu_t *round_menu = NULL;
  uint32_t len = ARRAY_LEN(window_info_list);
  round_menu_main_t *handle = (round_menu_main_t *)ctx;
  controller_base_t *base = (controller_base_t *)handle;
  if (load_widget->children->size > 0) {
    round_menu = ROUND_MENU(load_widget->children->elms[0]);
    if (round_menu != NULL) {
      round_menu_set_value(WIDGET(round_menu), handle->window_index);
      WIDGET_FOR_EACH_CHILD_BEGIN(WIDGET(round_menu), iter, i)
      if (tk_str_eq(iter->vt->type, "button") && iter->name != NULL &&
          strstr(iter->name, "open:") != NULL) {
        button_name = iter->name + 5;
        for (n = 0; n < len; n++) {
          if (window_info_list[n].window_handle == NULL &&
              button_name != NULL &&
              tk_str_cmp(button_name, window_info_list[n].button_name) == 0) {
            window_info_list[n].window_handle = window_info_list[n].create(
                base->gesture_manager, base->window_anim_menu);
            controller_base_set_exit_func(
                window_info_list[n].window_handle,
                round_menu_main_on_exit_button_window_func, handle);
          }
        }
        widget_on(iter, EVT_CLICK, round_menu_main_on_open_window, handle);
      }
      WIDGET_FOR_EACH_CHILD_END()
    }
  }
  return RET_OK;
}

ret_t round_menu_main_set_enable(round_menu_main_t *handle, bool_t enable) {
  controller_base_t *base = (controller_base_t *)handle;
  return_value_if_fail(controller_base_set_enable(base, enable) == RET_OK,
                       RET_BAD_PARAMS);

  if (enable) {
    gesture_on_click_event(base->gesture_manager,
                           round_menu_main_on_click_gesture_fun,
                           GESTURE_LEFT_TO_RIGHT_TYPE, handle);

    window_anim_menu_on_load_widget(
        base->window_anim_menu, round_menu_main_on_window_anim_load_widget_func,
        handle);
    window_anim_menu_on_dispose_widget(base->window_anim_menu, NULL, NULL);
  } else {
    return gesture_remove_all(base->gesture_manager);
  }

  return RET_OK;
}

round_menu_main_t *round_menu_main_init(widget_t *gesture_manager,
                                        widget_t *window_anim_menu) {
  round_menu_main_t *handle = TKMEM_ZALLOC(round_menu_main_t);
  controller_base_t *base = (controller_base_t *)handle;
  return_value_if_fail(handle != NULL, NULL);

  controller_base_init(base, gesture_manager, window_anim_menu);

  base->enable = FALSE;
  handle->window_index = 0;

  return handle;
}

ret_t round_menu_main_dispose(round_menu_main_t *handle) {
  uint32_t i = 0;
  uint32_t len = ARRAY_LEN(window_info_list);

  return_value_if_fail(handle != NULL, RET_BAD_PARAMS);

  for (i = 0; i < len; i++) {
    if (window_info_list[i].window_handle != NULL &&
        window_info_list[i].dispose != NULL) {
      window_info_list[i].dispose(window_info_list[i].window_handle);
    }
  }

  TKMEM_FREE(handle);

  return RET_OK;
}
