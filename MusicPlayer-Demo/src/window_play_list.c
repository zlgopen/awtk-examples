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
#include "custom_widgets/photo_frame/frame_view.h"

static widget_t* m_win = NULL;
extern ret_t swtich_frame_rotation_animator(widget_t* win, bool_t start_anim);
extern ret_t swtich_frame_vinyl_head_animator(widget_t* win, bool_t start_anim);

static ret_t on_close(void* ctx, event_t* e) {
  widget_t* dialog = (widget_t*)ctx;
  (void)e;

  return dialog_quit(dialog, RET_QUIT);
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

static ret_t on_song_item_down(void* ctx, event_t* e) {
  if (m_win == NULL) return RET_FAIL;
  widget_t* widget = WIDGET(ctx);
  widget_t* frame_menu = widget_lookup(m_win, "frame_menu", TRUE);
  frame_view_t* frame_view = FRAME_VIEW(frame_menu);
  widget_t* vinyl_head = widget_lookup(m_win, "vinyl_head", TRUE);
  guage_pointer_t* vh = GUAGE_POINTER(vinyl_head);
  int32_t iter_index = atoi(strtok(widget->name, "_"));

  list_view_clean(widget->parent);
  widget_use_style(widget, "choose");
  widget_invalidate(widget, NULL);
  widget_use_style(widget->children->elms[1], "left_list_click");
  widget_invalidate(widget->children->elms[1], NULL);

  widget_set_visible(widget->children->elms[0], TRUE, FALSE);

  if (frame_view->value != iter_index) {
    if (vh->angle == -70) {
      swtich_frame_vinyl_head_animator(m_win, TRUE);
    }
    swtich_frame_rotation_animator(m_win, FALSE);
    frame_view_set_value(frame_menu, iter_index);
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
  widget_t* frame_menu = widget_lookup(m_win, "frame_menu", TRUE);
  frame_view_t* frame_view = FRAME_VIEW(frame_menu);
  if (frame_view->value == widget_index_of(widget)) {
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
        widget_on(widget, EVT_CLICK, on_close, dialog);
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
    widget_on(dialog, EVT_POINTER_DOWN, on_dialog_state, dialog);
    m_win = s_parent;
    init_children_widget(dialog);
    return dialog_modal(dialog);
  }

  return RET_FAIL;
}