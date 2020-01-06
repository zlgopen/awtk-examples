#include "global.h"
#include "log.h"
#include "../window_main.h"
#include "../slide_appliction/app.h"

#ifdef _WIN32
#pragma warning(disable : 4244)
#endif  // _WIN32

global_t global_data;
uint32_t heart_rate_timer;
window_name_t* g_window_enum;

global_t* global_active_data;
global_t* global_workout_data;
global_t* global_stand_data;

static ret_t on_destroy_bar(void* data) {
  bar_t* bar = (bar_t*)data;
  TKMEM_FREE(bar);
  return RET_OK;
}

ret_t on_timer_update_bar(const timer_info_t* timer) {
  darray_t* bar_array = (darray_t*)timer->ctx;

  if (bar_array->size == global_data.bar_max_size) {
    darray_remove_index(bar_array, 0);
  }

  float_t b_h = 0.0;
  float_t b_l = 0.0;
  while (b_h - b_l < 0.1) {
    b_h = rand() % 100 / 100.0;
    b_l = rand() % 100 / 100.0;
    b_h = tk_max(b_h, b_l);
    b_l = tk_min(b_h, b_l);

    if (b_h - b_l > 0.3) {
      break;
    }
  }

  bar_t* bar = TKMEM_ZALLOC(bar_t);
  bar->high = b_h;
  bar->low = b_l;
  darray_push(bar_array, bar);

  return RET_REPEAT;
}

// 添加活动、锻炼数据
ret_t on_timer_add_activity_bar(const timer_info_t* timer) {
  (void)timer;
  darray_t* activity_data = global_active_data->bar_data;
  darray_t* workout_data = global_workout_data->bar_data;

  float_t b_h = 0.0;
  float_t b_l = 0.0;

  if (activity_data->size == global_active_data->bar_max_size) {
    darray_clear(activity_data);
  }
  if (workout_data->size == global_workout_data->bar_max_size) {
    darray_clear(workout_data);
  }

  // 假设前6个小时不运动
  bool_t stop_6 = TRUE;
  if (stop_6 || activity_data->size >= 6) {
    while (b_h < 0.2) {
      b_h = rand() % 100 / 100.0;
    }
  }
  bar_t* bar = TKMEM_ZALLOC(bar_t);
  bar->high = b_h;
  bar->low = b_l;
  darray_push(activity_data, bar);

  // 锻炼添加数据
  b_h = 0.0;
  if (stop_6 || workout_data->size >= 6) {
    while (b_h < 0.2) {
      b_h = rand() % 100 / 100.0;
    }
  }
  bar = TKMEM_ZALLOC(bar_t);
  bar->high = b_h;
  bar->low = b_l;
  darray_push(workout_data, bar);

  return RET_REPEAT;
}

ret_t on_timer_add_stand_bar(const timer_info_t* timer) {
  (void)timer;
  darray_t* stand_data = global_stand_data->bar_data;

  float_t b_h = 0.0;
  float_t b_l = 0.0;

  if (stand_data->size == global_stand_data->bar_max_size) {
    darray_clear(stand_data);
  }

  // 假设前6小时没有站立
  bool_t stop_6 = TRUE;
  if (stop_6 || stand_data->size >= 6) {
    b_h = rand() % 100 / 100.0;
    // b_h = b_h > 0.5 ? 1 : 0;
    while (b_h < 0.2) {
      b_h = rand() % 100 / 100.0;
    }
  }
  bar_t* bar = TKMEM_ZALLOC(bar_t);
  bar->high = b_h;
  bar->low = b_l;
  darray_push(stand_data, bar);

  return RET_REPEAT;
}

static int on_compare_bar(const void* a, const void* b) {
  (void)a;
  (void)b;
  return -1;
}

// 遍历darray
#define DARRAY_FOR_EACH_BEGIN(_array, _iter)    \
  if (_array != NULL && _array->elms != NULL) { \
    void** elms = _array->elms;                 \
    uint32_t size = _array->size;               \
    uint32_t i = 0;                             \
    for (i = 0; i < size; i++) {                \
      void* _iter = elms[i];

#define DARRAY_FOR_EACH_END() \
  }                           \
  }

static ret_t on_timer(const timer_info_t* timer) {
  (void)timer;
  return RET_REPEAT;
}

ret_t on_watch_06_update_timer(const timer_info_t* timer) {
  watch_06_t* watch_06 = timer->ctx;
  darray_t* data = &(watch_06->data);
  if (watch_06->max_size != 0) {
    if (data->size == watch_06->max_size) {
      darray_remove_index(data, 0);
    }

    float_t b_h = 0.0;
    float_t b_l = 0.0;
    while (b_h - b_l < 0.3) {
      b_h = rand() % 100 / 100.0;
      b_l = rand() % 100 / 100.0;
      b_h = tk_max(b_h, b_l);
      b_l = tk_min(b_h, b_l);
    }

    bar_t* bar = TKMEM_ZALLOC(bar_t);
    bar->high = b_h;
    bar->low = b_l;
    darray_push(data, bar);
  }
  return RET_REPEAT;
}

ret_t on_music_timer(const timer_info_t* timer) {
  (void)timer;
  music_state_t* music_state = &(global_data.music_state);
  LOG("play_state:%d", music_state->play_state);
  if (music_state->play_state == 1) {
    int32_t value = music_state->play_progress + 1;
    if (value == 100) {
      music_state->play_progress = 0;
      music_state->song_index = next_index(music_state);
    } else {
      music_state->play_progress = value;
    }
  }
  LOG("play_progress:%d", music_state->play_progress);
  return RET_REPEAT;
}

static ret_t on_str_destroy(void* data) {
  str_t* str = (str_t*)data;
  str_destroy(str);
  return RET_OK;
}

int on_str_compare(const void* a, const void* b) {
  str_t* ta = (str_t*)a;
  str_t* tb = (str_t*)b;
  return tk_str_cmp(ta->str, tb->str);
}

static char* song_list[] = {"Shape of you", "That Girl",           "Listen",
                            "Wolves",       "Trouble is a firend", "Fire"};

int32_t next_index(music_state_t* music_state) {
  switch (music_state->play_mode) {
    case 0: {
      int32_t next = music_state->song_index + 1;
      if ((uint32_t)next >= music_state->song_list.size) {
        return 0;
      } else {
        return next;
      }
    }
    case 1: {
      return music_state->song_index;
    }
    case 2: {
      uint16_t index = rand() % music_state->song_list.size;
      return index;
    }
    default: {
      assert(FALSE);
      return -1;
    }
  }
}

int32_t pre_index(music_state_t* music_state) {
  switch (music_state->play_mode) {
    case 0: {
      int32_t pre = music_state->song_index - 1;
      if (pre < 0) {
        return music_state->song_list.size - 1;
      } else {
        return pre;
      }
    }
    case 1: {
      return music_state->song_index;
    }
    case 2: {
      uint16_t index = rand() % music_state->song_list.size;
      return index;
    }
    default: {
      assert(FALSE);
      return -1;
    }
  }
}

static void set_str_with_name(str_t* name, const char* text) {
  str_t* str = name;
  str_init(str, 0);
  str_set(str, text);
}

void window_name_init(window_name_t* window_name) {
  window_name_t* window = window_name;

  set_str_with_name(&(window->watch_01), "watch_01");
  set_str_with_name(&(window->watch_02), "watch_02");
  set_str_with_name(&(window->watch_04), "watch_04");
  set_str_with_name(&(window->watch_06), "watch_06");
  set_str_with_name(&(window->app), "app");
  set_str_with_name(&(window->quick), "quick");
  set_str_with_name(&(window->message), "message");
  set_str_with_name(&(window->calendar), "calendar");
  set_str_with_name(&(window->health), "health");
  set_str_with_name(&(window->music), "music");
  set_str_with_name(&(window->weather), "weather");
}

static void reset_str_with_name(str_t* name) {
  str_t* str = name;
  str_reset(str);
}

void window_name_deinit(window_name_t* window_name) {
  window_name_t* window = window_name;

  reset_str_with_name(&(window->watch_01));
  reset_str_with_name(&(window->watch_02));
  reset_str_with_name(&(window->watch_04));
  reset_str_with_name(&(window->watch_06));
  reset_str_with_name(&(window->app));
  reset_str_with_name(&(window->quick));
  reset_str_with_name(&(window->message));
  reset_str_with_name(&(window->calendar));
  reset_str_with_name(&(window->health));
  reset_str_with_name(&(window->music));
  reset_str_with_name(&(window->weather));
}

void watch_01_data_init(watch_01_t* watch_01) {
  watch_01->battery = 100;
  watch_01->foot = 0;
}

static global_t* init_global_data() {
  global_t* data = TKMEM_ZALLOC(global_t);
  data->bar_data = darray_create(0, on_destroy_bar, on_compare_bar);
  data->bar_max_size = 24;

  return data;
}

void init_global() {
  window_name_init(&(global_data.window));
  g_window_enum = &(global_data.window);

  watch_01_data_init(&(global_data.watch_01));

  str_init(&(global_data.previous_window), 0);

  global_data.bar_data = darray_create(0, on_destroy_bar, on_compare_bar);
  global_data.bar_max_size = 24;

  global_data.watch_type = WATCH_01;

  // 心率
  heart_rate_t* heart_rate = &(global_data.heart_rate);
  heart_rate->result = 100;
  date_time_init(&(heart_rate->dt));

  // app窗口
  global_data.app.index = 0;

  // 音乐
  music_state_t* music_state = &(global_data.music_state);
  music_state->play_state = 1;
  music_state->play_mode = 2;
  music_state->play_vol = 50;
  music_state->play_progress = 0;
  music_state->play_max = 100;
  darray_init(&(music_state->song_list), 0, on_str_destroy, on_str_compare);
  size_t i = 0;
  for (i = 0; i < sizeof(song_list) / sizeof(char*); i++) {
    str_t* str = TKMEM_ZALLOC(str_t);
    str_init(str, 0);
    str_append(str, song_list[i]);
    darray_push(&(music_state->song_list), str);
  }
  LOG("size:%d", music_state->song_list.size);
  timer_add(on_music_timer, NULL, 1000);

  // 表盘6的心率数据
  darray_init(&(global_data.watch_06.data), 0, on_destroy_bar, on_compare_bar);
  global_data.watch_06.max_size = 0;
  timer_add(on_watch_06_update_timer, &(global_data.watch_06), 1000);

  global_active_data = init_global_data();
  global_workout_data = init_global_data();
  global_stand_data = init_global_data();

  heart_rate_timer = TK_INVALID_ID;
  heart_rate_timer = timer_add(on_timer_update_bar, global_data.bar_data, 1000);

  i = 0;
  for (i = 0; i < 14; i++) {
    global_data.widget_pointer[i] = NULL;
  }

  timer_add(on_timer, NULL, 100);

  // 窗口历史
  darray_init(&(global_data.window_history), 0, on_str_destroy, on_str_compare);
}

str_t* str_create() {
  str_t* str = TKMEM_ZALLOC(str_t);
  str_init(str, 0);
  return str;
}

void str_destroy(str_t* str) {
  str_reset(str);
  TKMEM_FREE(str);
}

void deinit_global() {
  window_name_deinit(&(global_data.window));

  str_reset(&(global_data.previous_window));

  if (global_data.bar_data->size > 0) {
    darray_destroy(global_data.bar_data);
  } else {
    TKMEM_FREE(global_data.bar_data);
  }

  if (global_active_data->bar_data->size > 0) {
    darray_destroy(global_active_data->bar_data);
  } else {
    TKMEM_FREE(global_active_data->bar_data);
  }

  if (global_stand_data->bar_data->size > 0) {
    darray_destroy(global_stand_data->bar_data);
  } else {
    TKMEM_FREE(global_stand_data->bar_data);
  }

  if (global_workout_data->bar_data->size > 0) {
    darray_destroy(global_workout_data->bar_data);
  } else {
    TKMEM_FREE(global_workout_data->bar_data);
  }

  darray_deinit(&(global_data.watch_06.data));
  TKMEM_FREE(global_active_data);
  TKMEM_FREE(global_stand_data);
  TKMEM_FREE(global_workout_data);

  // 窗口历史
  darray_deinit(&(global_data.window_history));

  if (heart_rate_timer != TK_INVALID_ID) {
    timer_remove(heart_rate_timer);
  }

  size_t i = 0;
  for (i = 0; i < 14; i++) {
    if (global_data.widget_pointer[i] != NULL) {
    }
  }

  // 音乐
  darray_deinit(&(global_data.music_state.song_list));
}

// 添加窗口历史
void window_history_push(widget_t* window) {
  str_t* name = str_create();
  str_append(name, window->name);
  darray_push(&(global_data.window_history), name);
  LOG(name->str);
}

void window_history_pop() {
  darray_t* window_history = &(global_data.window_history);
  str_t* name = (str_t*)darray_pop(window_history);
  LOG(name->str);
  window_history->destroy(name);
}

void widget_unload_image_by_name(widget_t* widget, const char* name) {
  bitmap_t bitmap;
  ret_t ret = widget_load_image(widget, name, &bitmap);
  if (ret == RET_OK) {
    widget_unload_image(widget, &bitmap);
  }
}

void open_and_close(widget_t* win) {
  str_t* prev_win = &(global_data.previous_window);
  if (prev_win->size > 0) {
    if (str_start_with(prev_win, "watch")) {
      open_watch_and_close(prev_win->str, win);
    } else if (str_eq(prev_win, g_window_enum->app.str)) {
      app_init(win, "slide_right");
    } else {
      LOG("name:%s", prev_win->str);
      assert(FALSE);
    }
  } else {
    assert(FALSE);
  }

  image_manager_unload_unused(image_manager(), 0);
}
