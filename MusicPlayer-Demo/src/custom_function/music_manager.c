#include "music_manager.h"
#include "player_timer_manager.h"
#include "../custom_widgets/photo_frame/frame_view.h"
#include "ext_widgets/image_animation/image_animation.h"
#include "widget_animators/widget_animator_opacity.h"
#include "widget_animators/widget_animator_rotation.h"
#include <locale.h>
#include <math.h>

#define STRLEN 128

/**
 * 歌词时间转换为秒
 */
int32_t timetosec(char* time) {
  int32_t min = 0;
  int32_t sec = 0;
  char* str = NULL;
  str = strtok(time, ":");
  min = atoi(str);
  str = strtok(NULL, ":");
  sec = atoi(str);

  return (min * 60 + sec);
}

/**
 * 根据设定的播放模式，改变歌曲播放顺序
 */
ret_t play_mode_choose(widget_t* win) {
  widget_t* btn_play_mode = widget_lookup(win, "btn_play_mode", TRUE);
  widget_t* btn_next = widget_lookup(win, "btn_next", TRUE);
  widget_t* frame_menu = widget_lookup(win, "frame_menu", TRUE);
  frame_view_t* frame_view = FRAME_VIEW(frame_menu);
  int32_t value = rand() % 3;
  event_t evt = event_init(EVT_CLICK, btn_next);
  if (tk_str_eq(btn_play_mode->style, "s_play_mode_one")) {
    init_player(win);
    swtich_frame_rotation_animator(win, TRUE);
    load_song(win, frame_view->value, TRUE);
  } else if (tk_str_eq(btn_play_mode->style, "s_play_mode_rand")) {
    while(frame_view->value == value){
      value = rand() % 3;
    }
    frame_view_set_value(frame_menu, value);
  } else if (tk_str_eq(btn_play_mode->style, "s_play_mode_clicle")) {
    widget_dispatch(btn_next, &evt);
    widget_invalidate(btn_next, NULL);
  }

  return RET_OK;
}

/**
 * 初始化歌曲显示信息
 */
void init_player(widget_t* win) {
  char time_zero[10] = {0};
  widget_t* btn_play = widget_lookup(win, "btn_play", TRUE);
  widget_t* slider = widget_lookup(win, "play_slider", TRUE);
  widget_t* label_min = widget_lookup(win, "slider_min", TRUE);
  value_t val;

  value_set_bool(&val, TRUE);
  widget_use_style(btn_play, "s_play");
  widget_invalidate(btn_play, NULL);
  slider_set_value(slider, 0);
  sprintf(time_zero, "%0.2d:%0.2d", 0, 0);
  widget_set_text_utf8(label_min, time_zero);
  player_timer_clear(win);
}

/**
 * 歌词动态刷新
 */
void update_lrc(int32_t value, widget_t* win) {
  value_t val;
  value_t v_lrc_time;
  widget_t* lrc_scroll = widget_lookup(win, "lrc_scroll", TRUE);

  for (int32_t i = 0; i < lrc_scroll->children->size; i++) {
    if (widget_get_prop(win, "lrc_move", &val) != RET_OK || value_bool(&val) == FALSE) break;
    widget_get_prop(WIDGET(lrc_scroll->children->elms[i]), "lrc_time", &v_lrc_time);
    if (value_int32(&v_lrc_time) >= value) {
      for (int32_t k = 0; k < lrc_scroll->children->size; k++) {
        widget_t* child = WIDGET(lrc_scroll->children->elms[k]);
        if (k == i) {
          widget_use_style(child, "empty_hl");
          scroll_view_scroll_to(lrc_scroll, 0, 25 * k, 10);
        } else {
          widget_use_style(child, "empty");
        }
        widget_invalidate(child, NULL);
      }
      break;
    }
  }
}

/**
 * 歌词解析
 */
static ret_t parse_lrc_line(widget_t* win, const char* name) {
  char* p = NULL;
  char* right = NULL;
  char* left = NULL;
  char buff[STRLEN] = {0};
  char* p2 = NULL;
  value_t v_lrc_time;
  int32_t n = 0;
  widget_t* slider = widget_lookup(win, "play_slider", TRUE);
  widget_t* lrc_scroll = widget_lookup(win, "lrc_scroll", TRUE);
  widget_t* slider_max_label = widget_lookup(win, "slider_max", TRUE);
  tokenizer_t tokenizer;
  tokenizer_t* t = &tokenizer;

  if (lrc_scroll->children != NULL) {
    for (int index = 0; index < lrc_scroll->children->size; index++) {
      widget_set_visible(lrc_scroll->children->elms[index], FALSE, FALSE);
    }
  }
  asset_info_t* info = assets_manager_ref(assets_manager(), ASSET_TYPE_DATA, name);
  if (info == NULL) return RET_FAIL;
  tokenizer_init(t, info->data, strlen(info->data), "\n");
  while (tokenizer_has_more(t)) {
    const char* token = tokenizer_next(t);
    p = token;
    left = strchr(p, '[');
    if (left == NULL) {
      return RET_OK;
    }
    right = strchr(p, ']');
    while (right != NULL && left != NULL) {
      strncpy(buff, left + 1, right - left - 4);
      widget_set_text_utf8(slider_max_label, buff);
      p2 = strrchr(p, ']');
      if (p2 != NULL) {
        p2++;
        value_set_int32(&v_lrc_time, timetosec(buff));
        if (lrc_scroll->children == NULL || lrc_scroll->children->size < n + 1)
        list_item_create(lrc_scroll, 0, 0, 0, 0);
        widget_use_style(lrc_scroll->children->elms[n], "empty");
        widget_invalidate(lrc_scroll->children->elms[n], NULL);

        setlocale(LC_CTYPE, "");
        widget_set_prop(lrc_scroll->children->elms[n], "lrc_time", &v_lrc_time);
        widget_set_text_utf8(lrc_scroll->children->elms[n], p2);
        widget_set_visible(lrc_scroll->children->elms[n], TRUE, FALSE);
        slider_set_max(slider, value_int32(&v_lrc_time));
        n++;
      }
      p = right;
      *p = '\0';
      p = p + 1;
      left = strchr(p, '[');
      if (left == NULL) {
        continue;
      }
      right = strchr(p, ']');
    }
  }
  if (info != NULL) {
    assets_manager_unref(assets_manager(), info);
    info = NULL;
    assets_manager_clear_cache(assets_manager(), ASSET_TYPE_DATA);
  }
  tokenizer_deinit(t);
  return RET_OK;
}

/**
 * 歌曲信息加载(歌词以及界面控件状态)
 */
int32_t load_song(widget_t* win, int32_t song_iter, bool_t play_now) {
  const char* analy_name[] = {"yellow", "see_you_again", "let_it_go"};
  const char* analy_name1[] = {"yellow.bin", "see_you_again.bin", "let_it_go.bin"};
  const char* s_name[] = {"Yellow", "See you again", "Let it go"};
  const char* s_artist[] = {"by ColdPlay", "by Wiz Khalifa", "by Idina Menzel"};

  widget_t* btn_play = widget_lookup(win, "btn_play", TRUE);
  widget_t* song_name = widget_lookup(win, "song_name", TRUE);
  widget_t* song_artist = widget_lookup(win, "song_artist", TRUE);

  if (parse_lrc_line(win, analy_name[song_iter]) != RET_OK) {
    parse_lrc_line(win, analy_name1[song_iter]);
  }
  widget_set_text_utf8(song_name, s_name[song_iter]);
  widget_set_text_utf8(song_artist, s_artist[song_iter]);

  if (play_now) {
    widget_use_style(btn_play, "s_play_pause");
    player_timer_clear(win);
    player_timer_start(win);
  } else {
    widget_use_style(btn_play, "s_play");
    player_timer_clear(win);
  }
  widget_invalidate(btn_play, NULL);

  return song_iter;
}
