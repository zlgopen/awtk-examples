/**
 * File:   window_record.c
 * Author: AWTK Develop Team
 * Brief:  PlayList window
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
 * 2019-2-20  ZhongWei1 created
 *
 */

#include "awtk.h"
#include "custom_function/music_manager.h"

static int32_t g_evt_pointer_down_num;
static widget_t* m_win = NULL;

extern ret_t swtich_vinyl_head_animator(widget_t* win, bool_t start_anim, bool_t is_yoyo);
extern ret_t swtich_rotation_animator(widget_t* win, bool_t start_anim);
extern int32_t g_img_index;

static ret_t on_close(void* ctx, event_t* e) {
  widget_t* dialog = (widget_t*)ctx;
  (void)e;

  return dialog_quit(dialog, RET_QUIT);
}

static ret_t on_quit_anim(void* ctx, event_t* e) {
  widget_t* dialog = (widget_t*)ctx;
  widget_animator_t* animator = NULL;
  (void)e;

#if LCD_W == 480
  widget_create_animator(dialog, "move(x_from=240, x_to=480, duration=500)");
#else
  widget_create_animator(dialog, "move(x_from=400, x_to=800, duration=500)");
#endif
  animator = widget_find_animator(dialog, "move");
  widget_animator_on(animator, EVT_ANIM_END, on_close, dialog);
  widget_off(dialog, g_evt_pointer_down_num);

  return RET_OK;
}

static ret_t on_dialog_state(void* ctx, event_t* e) {
  widget_t* dialog = (widget_t*)ctx;
  pointer_event_t* evt = (pointer_event_t*)e;
  widget_t* close = widget_lookup(dialog, "close", TRUE);
  if (evt->x > (dialog->x + dialog->w) || evt->x < dialog->x || evt->y < dialog->y ||
      evt->y > (dialog->y + dialog->h)) {
    event_t et = event_init(EVT_CLICK, close);
    widget_dispatch(close, &et);
    widget_invalidate(close, NULL);
  }

  return RET_OK;
}

static ret_t list_view_clean(widget_t* parent) {
  return_value_if_fail(parent != NULL, RET_BAD_PARAMS);
  int i;
  for (i = 0; i < parent->children->size; i++) {
    widget_t* iter = (widget_t*)(parent->children->elms[i]);
    widget_use_style(iter, "empty_list");
    widget_invalidate(iter, NULL);
    widget_use_style(iter->children->elms[1], "left_list");
    widget_invalidate(iter->children->elms[1], NULL);
    widget_set_visible(iter->children->elms[0], FALSE, FALSE);
  }
  return RET_OK;
}

static ret_t show_curr_cover(widget_t* win, int32_t index) {
  widget_t* cover_view = widget_lookup(win, "cover_view", TRUE);

  WIDGET_FOR_EACH_CHILD_BEGIN(cover_view, iter, i)
  if (i == index) {
    iter->x = cover_view->x;
  } else {
    iter->x = cover_view->x + cover_view->w;
  }
  WIDGET_FOR_EACH_CHILD_END()

  return RET_OK;
}

static ret_t on_song_item_down(void* ctx, event_t* e) {
  if (m_win == NULL) return RET_FAIL;
  widget_t* widget = WIDGET(ctx);
  widget_t* vinyl_head = widget_lookup(m_win, "vinyl_head", TRUE);
  guage_pointer_t* vh = GUAGE_POINTER(vinyl_head);
  int32_t iter_index = atoi(strtok(widget->name, "_"));

  list_view_clean(widget->parent);
  widget_use_style(widget, "choose");
  widget_invalidate(widget, NULL);
  widget_use_style(widget->children->elms[1], "left_list_click");
  widget_invalidate(widget->children->elms[1], NULL);

  widget_set_visible(widget->children->elms[0], TRUE, FALSE);

  if (g_img_index != iter_index) {
    if (vh->angle == -70) {
      swtich_vinyl_head_animator(m_win, TRUE, FALSE);
    }
    g_img_index = iter_index;
    show_curr_cover(m_win, g_img_index);
    init_player(m_win);
    load_song(m_win, g_img_index, TRUE);
    swtich_rotation_animator(m_win, TRUE);
  }

  return RET_OK;
}

static ret_t on_song_item_enter(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  widget_use_style(widget, "choose");
  widget_invalidate(widget, NULL);

  return RET_OK;
}

static ret_t on_song_item_leave(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  widget_use_style(widget, "empty_list");
  widget_invalidate(widget, NULL);

  return RET_OK;
}

static ret_t init_choose_item(widget_t* widget) {
  return_value_if_fail(widget != NULL && m_win != NULL, RET_BAD_PARAMS);
  if (g_img_index == widget_index_of(widget)) {
    event_t evt = event_init(EVT_POINTER_DOWN, widget);
    widget_dispatch(widget, &evt);
    widget_invalidate(widget, NULL);
  }

  return RET_OK;
}
/**
 * 子控件初始化(主要是设置click回调、初始显示信息)
 */
static ret_t init_widget(void* ctx, const void* iter) {
  widget_t* widget = WIDGET(iter);

  (void)ctx;
  if (widget->name != NULL) {
    const char* name = widget->name;
    if (tk_str_eq(name, "close")) {
      widget_t* dialog = widget_get_window(widget);
      if (dialog) {
        widget_on(widget, EVT_CLICK, on_quit_anim, dialog);
      }
    } else if (tk_str_eq(name, "0_song")) {
      widget_t* dialog = widget_get_window(widget);
      if (dialog) {
        widget_on(widget, EVT_POINTER_DOWN, on_song_item_down, widget);
        widget_on(widget, EVT_POINTER_ENTER, on_song_item_enter, widget);
        widget_on(widget, EVT_POINTER_LEAVE, on_song_item_leave, widget);
        init_choose_item(widget);
      }
    } else if (tk_str_eq(name, "1_song")) {
      widget_t* dialog = widget_get_window(widget);
      if (dialog) {
        widget_on(widget, EVT_POINTER_DOWN, on_song_item_down, widget);
        widget_on(widget, EVT_POINTER_ENTER, on_song_item_enter, widget);
        widget_on(widget, EVT_POINTER_LEAVE, on_song_item_leave, widget);
        init_choose_item(widget);
      }
    } else if (tk_str_eq(name, "2_song")) {
      widget_t* dialog = widget_get_window(widget);
      if (dialog) {
        widget_on(widget, EVT_POINTER_DOWN, on_song_item_down, widget);
        widget_on(widget, EVT_POINTER_ENTER, on_song_item_enter, widget);
        widget_on(widget, EVT_POINTER_LEAVE, on_song_item_leave, widget);
        init_choose_item(widget);
      }
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
 * 打开播放列表窗口
 */
ret_t open_play_list_window(widget_t* s_parent) {
  widget_t* dialog = dialog_open("play_list");

  if (dialog) {
    g_evt_pointer_down_num = widget_on(dialog, EVT_POINTER_UP, on_dialog_state, dialog);
    m_win = s_parent;
    init_children_widget(dialog);
    return dialog_modal(dialog);
  }

  return RET_FAIL;
}
