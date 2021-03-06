#include "awtk.h"
#include "base/assets_manager.h"
#ifndef WITH_FS_RES
#include "default_320_240/inc/ui/calendar.data"
#include "default_320_240/inc/ui/main.data"
#include "default_320_240/inc/ui/message.data"
#include "default_320_240/inc/ui/music.data"
#include "default_320_240/inc/ui/round_menu.data"
#include "default_320_240/inc/ui/training.data"
#include "default_320_240/inc/ui/watch_main_01.data"
#include "default_320_240/inc/ui/watch_main_02.data"
#include "default_320_240/inc/ui/weather.data"
#include "default_320_240/inc/ui/weather_amin.data"
#include "default_320_240/inc/strings/en_US.data"
#include "default_320_240/inc/strings/zh_CN.data"
#include "default_320_240/inc/styles/default.data"
#ifdef WITH_STB_IMAGE
#else
#include "default_320_240/inc/images/background.data"
#include "default_320_240/inc/images/background_head.data"
#include "default_320_240/inc/images/calendar_button_normal.data"
#include "default_320_240/inc/images/calendar_button_pressed.data"
#include "default_320_240/inc/images/calendar_current.data"
#include "default_320_240/inc/images/calender_down_normal.data"
#include "default_320_240/inc/images/calender_down_pressed.data"
#include "default_320_240/inc/images/calender_up_normal.data"
#include "default_320_240/inc/images/calender_up_pressed.data"
#include "default_320_240/inc/images/music_button_normal.data"
#include "default_320_240/inc/images/music_button_pressed.data"
#include "default_320_240/inc/images/music_immeditately_normal.data"
#include "default_320_240/inc/images/music_immeditately_pressed.data"
#include "default_320_240/inc/images/music_loop_normal.data"
#include "default_320_240/inc/images/music_loop_pressed.data"
#include "default_320_240/inc/images/music_loop_single_normal.data"
#include "default_320_240/inc/images/music_loop_single_pressed.data"
#include "default_320_240/inc/images/music_next_normal.data"
#include "default_320_240/inc/images/music_next_pressed.data"
#include "default_320_240/inc/images/music_perv_normal.data"
#include "default_320_240/inc/images/music_perv_pressed.data"
#include "default_320_240/inc/images/music_play_normal.data"
#include "default_320_240/inc/images/music_play_pressed.data"
#include "default_320_240/inc/images/music_sound.data"
#include "default_320_240/inc/images/music_stop_normal.data"
#include "default_320_240/inc/images/music_stop_pressed.data"
#include "default_320_240/inc/images/training_button_normal.data"
#include "default_320_240/inc/images/training_button_pressed.data"
#include "default_320_240/inc/images/watch_main_background_1.data"
#include "default_320_240/inc/images/watch_main_background_2.data"
#include "default_320_240/inc/images/watch_main_hour.data"
#include "default_320_240/inc/images/watch_main_minute.data"
#include "default_320_240/inc/images/watch_main_second.data"
#include "default_320_240/inc/images/weather_button_normal.data"
#include "default_320_240/inc/images/weather_button_pressed.data"
#include "default_320_240/inc/images/weather_cloudy.data"
#include "default_320_240/inc/images/weather_degree.data"
#include "default_320_240/inc/images/weather_rain.data"
#include "default_320_240/inc/images/weather_sun.data"
#include "default_320_240/inc/images/weather_thunder.data"
#endif /*WITH_STB_IMAGE*/
#ifdef WITH_VGCANVAS
#endif /*WITH_VGCANVAS*/
#if defined(WITH_TRUETYPE_FONT)
#else /*WITH_TRUETYPE_FONT*/
#include "default_320_240/inc/fonts/default_11.data"
#include "default_320_240/inc/fonts/default_18.data"
#include "default_320_240/inc/fonts/default_25.data"
#include "default_320_240/inc/fonts/default_43.data"
extern TK_CONST_DATA_ALIGN(const unsigned char font_default_26[]);
extern TK_CONST_DATA_ALIGN(const unsigned char font_default_38[]);
extern TK_CONST_DATA_ALIGN(const unsigned char font_default_76[]);
#endif /*WITH_TRUETYPE_FONT*/
#endif /*WITH_FS_RES*/

ret_t assets_init_default_320_240(void) {
  assets_manager_t* am = assets_manager();

#ifdef WITH_FS_RES
  assets_manager_preload(am, ASSET_TYPE_FONT, "default");
  assets_manager_preload(am, ASSET_TYPE_STYLE, "default");
#else
  assets_manager_add(am, ui_calendar_default_320_240);
  assets_manager_add(am, ui_main_default_320_240);
  assets_manager_add(am, ui_message_default_320_240);
  assets_manager_add(am, ui_music_default_320_240);
  assets_manager_add(am, ui_round_menu_default_320_240);
  assets_manager_add(am, ui_training_default_320_240);
  assets_manager_add(am, ui_watch_main_01_default_320_240);
  assets_manager_add(am, ui_watch_main_02_default_320_240);
  assets_manager_add(am, ui_weather_default_320_240);
  assets_manager_add(am, ui_weather_amin_default_320_240);
  assets_manager_add(am, strings_en_US_default_320_240);
  assets_manager_add(am, strings_zh_CN_default_320_240);
  assets_manager_add(am, style_default_default_320_240);
  assets_manager_add(am, image_background_default_320_240);
  assets_manager_add(am, image_background_head_default_320_240);
  assets_manager_add(am, image_calendar_button_normal_default_320_240);
  assets_manager_add(am, image_calendar_button_pressed_default_320_240);
  assets_manager_add(am, image_calendar_current_default_320_240);
  assets_manager_add(am, image_calender_down_normal_default_320_240);
  assets_manager_add(am, image_calender_down_pressed_default_320_240);
  assets_manager_add(am, image_calender_up_normal_default_320_240);
  assets_manager_add(am, image_calender_up_pressed_default_320_240);
  assets_manager_add(am, image_music_button_normal_default_320_240);
  assets_manager_add(am, image_music_button_pressed_default_320_240);
  assets_manager_add(am, image_music_immeditately_normal_default_320_240);
  assets_manager_add(am, image_music_immeditately_pressed_default_320_240);
  assets_manager_add(am, image_music_loop_normal_default_320_240);
  assets_manager_add(am, image_music_loop_pressed_default_320_240);
  assets_manager_add(am, image_music_loop_single_normal_default_320_240);
  assets_manager_add(am, image_music_loop_single_pressed_default_320_240);
  assets_manager_add(am, image_music_next_normal_default_320_240);
  assets_manager_add(am, image_music_next_pressed_default_320_240);
  assets_manager_add(am, image_music_perv_normal_default_320_240);
  assets_manager_add(am, image_music_perv_pressed_default_320_240);
  assets_manager_add(am, image_music_play_normal_default_320_240);
  assets_manager_add(am, image_music_play_pressed_default_320_240);
  assets_manager_add(am, image_music_sound_default_320_240);
  assets_manager_add(am, image_music_stop_normal_default_320_240);
  assets_manager_add(am, image_music_stop_pressed_default_320_240);
  assets_manager_add(am, image_training_button_normal_default_320_240);
  assets_manager_add(am, image_training_button_pressed_default_320_240);
  assets_manager_add(am, image_watch_main_background_1_default_320_240);
  assets_manager_add(am, image_watch_main_background_2_default_320_240);
  assets_manager_add(am, image_watch_main_hour_default_320_240);
  assets_manager_add(am, image_watch_main_minute_default_320_240);
  assets_manager_add(am, image_watch_main_second_default_320_240);
  assets_manager_add(am, image_weather_button_normal_default_320_240);
  assets_manager_add(am, image_weather_button_pressed_default_320_240);
  assets_manager_add(am, image_weather_cloudy_default_320_240);
  assets_manager_add(am, image_weather_degree_default_320_240);
  assets_manager_add(am, image_weather_rain_default_320_240);
  assets_manager_add(am, image_weather_sun_default_320_240);
  assets_manager_add(am, image_weather_thunder_default_320_240);
  assets_manager_add(am, font_default_11_default_320_240);
  assets_manager_add(am, font_default_18_default_320_240);
  assets_manager_add(am, font_default_25_default_320_240);
  assets_manager_add(am, font_default_43_default_320_240);
  assets_manager_add(am, font_default_26);
  assets_manager_add(am, font_default_38);
  assets_manager_add(am, font_default_76);
#ifdef WITH_VGCANVAS
#endif /*WITH_VGCANVAS*/
#endif

  tk_init_assets();
  return RET_OK;
}
