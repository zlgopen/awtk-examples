#pragma once
#include <awtk.h>
#include "../custom_widgets/bar_graph.h"

typedef struct _watch_01_t {
  uint32_t battery;
  uint32_t foot;
} watch_01_t;
void watch_01_data_init(watch_01_t* watch_01);

typedef struct _watch_06_t {
  darray_t data;
  uint32_t max_size;
} watch_06_t;

typedef struct _music_state_t {
  uint8_t play_state;      // 0:暂停播放，1:正在播放
  uint8_t play_mode;       // 0:playlist 1:loop 2:rand
  uint8_t play_vol;        // 播放音量
  int32_t song_index;      // 播放的音乐
  uint32_t play_progress;  // 播放到的秒数
  uint32_t play_max;       // 歌曲的长度
  uint32_t timer_id;       // 计时器
  bool_t backstage;        // 后台
  darray_t song_list;      // 歌曲列表
} music_state_t;

typedef struct _heart_rate_t {
  int32_t result;  // 结果
  date_time_t dt;  // 检测的时间
} heart_rate_t;

// 当前选择哪个表盘
typedef enum _watch_type_t { WATCH_01 = 1, WATCH_02 = 2, WATCH_04 = 0, WATCH_06 = 3 } watch_type_t;

typedef struct app_t_ {
  size_t index;
} app_t;

typedef struct watch_t_ {
  watch_type_t type;
  str_t name;
} watch_t;

typedef struct window_name_t_ {
  str_t watch_01;
  str_t watch_02;
  str_t watch_04;
  str_t watch_06;
  str_t app;
  str_t message;
  str_t quick;
  str_t calendar;
  str_t health;
  str_t music;
  str_t weather;
} window_name_t;

void window_name_init(window_name_t* window_name);
void window_name_deinit(window_name_t* window_name);

typedef struct _global_t {
  darray_t* bar_data;
  uint32_t bar_max_size;
  widget_t* widget_pointer[14];
  uint8_t dial_type;

  watch_type_t watch_type;    // 表盘类型
  music_state_t music_state;  // 音乐状态

  watch_01_t watch_01;
  watch_06_t watch_06;      // 表盘六数据
  darray_t window_history;  // 窗口历史
  heart_rate_t heart_rate;  // 心率
  app_t app;                // app窗口

  str_t previous_window;
  watch_t current;

  window_name_t window;
} global_t;

extern global_t global_data;
extern uint32_t heart_rate_timer;
extern window_name_t* g_window_enum;

extern global_t* global_active_data;
extern global_t* global_workout_data;
extern global_t* global_stand_data;

void init_global(void);
void deinit_global(void);

ret_t on_timer_update_bar(const timer_info_t* timer);
ret_t on_timer_add_activity_bar(const timer_info_t* timer);
ret_t on_timer_add_stand_bar(const timer_info_t* timer);

int32_t next_index(music_state_t* music_state);
int32_t pre_index(music_state_t* music_state);

// 字符串
str_t* str_create(void);
void str_destroy(str_t* str);

// 窗口历史
void window_history_push(widget_t* window);
void window_history_pop(void);

void widget_unload_image_by_name(widget_t* widget, const char* name);

void open_and_close(widget_t* win);
