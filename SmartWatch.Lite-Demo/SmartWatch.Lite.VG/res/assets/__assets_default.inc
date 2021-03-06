#include "awtk.h"
#include "base/assets_manager.h"
#ifndef WITH_FS_RES
#include "default/inc/ui/calendar.data"
#include "default/inc/ui/main.data"
#include "default/inc/ui/message.data"
#include "default/inc/ui/music.data"
#include "default/inc/ui/round_menu.data"
#include "default/inc/ui/training.data"
#include "default/inc/ui/watch_main_01.data"
#include "default/inc/ui/watch_main_02.data"
#include "default/inc/ui/weather.data"
#include "default/inc/ui/weather_amin.data"
#include "default/inc/styles/default.data"
#ifdef WITH_STB_IMAGE
#else
#include "default/inc/images/calendar_button_normal.data"
#include "default/inc/images/calendar_button_pressed.data"
#include "default/inc/images/calendar_current.data"
#include "default/inc/images/calender_down_normal.data"
#include "default/inc/images/calender_down_pressed.data"
#include "default/inc/images/calender_up_normal.data"
#include "default/inc/images/calender_up_pressed.data"
#include "default/inc/images/music_button_normal.data"
#include "default/inc/images/music_button_pressed.data"
#include "default/inc/images/music_immeditately_normal.data"
#include "default/inc/images/music_immeditately_pressed.data"
#include "default/inc/images/music_loop_normal.data"
#include "default/inc/images/music_loop_pressed.data"
#include "default/inc/images/music_loop_single_normal.data"
#include "default/inc/images/music_loop_single_pressed.data"
#include "default/inc/images/music_next_normal.data"
#include "default/inc/images/music_next_pressed.data"
#include "default/inc/images/music_perv_normal.data"
#include "default/inc/images/music_perv_pressed.data"
#include "default/inc/images/music_play_normal.data"
#include "default/inc/images/music_play_pressed.data"
#include "default/inc/images/music_sound.data"
#include "default/inc/images/music_stop_normal.data"
#include "default/inc/images/music_stop_pressed.data"
#include "default/inc/images/training_button_normal.data"
#include "default/inc/images/training_button_pressed.data"
#include "default/inc/images/watch_main_background_1.data"
#include "default/inc/images/watch_main_background_2.data"
#include "default/inc/images/watch_main_hour.data"
#include "default/inc/images/watch_main_minute.data"
#include "default/inc/images/watch_main_second.data"
#include "default/inc/images/weather_button_normal.data"
#include "default/inc/images/weather_button_pressed.data"
#include "default/inc/images/weather_cloudy.data"
#include "default/inc/images/weather_degree.data"
#include "default/inc/images/weather_rain.data"
#include "default/inc/images/weather_sun.data"
#include "default/inc/images/weather_thunder.data"
#endif /*WITH_STB_IMAGE*/
#ifdef WITH_VGCANVAS
#endif /*WITH_VGCANVAS*/
#if defined(WITH_TRUETYPE_FONT)
#else /*WITH_TRUETYPE_FONT*/
#include "default/inc/fonts/default_18.data"
#include "default/inc/fonts/default_26.data"
#include "default/inc/fonts/default_38.data"
#include "default/inc/fonts/default_43.data"
#include "default/inc/fonts/default_76.data"
#endif /*WITH_TRUETYPE_FONT*/
#endif /*WITH_FS_RES*/

ret_t assets_init_default(void) {
  assets_manager_t* am = assets_manager();

#ifdef WITH_FS_RES
  assets_manager_preload(am, ASSET_TYPE_FONT, "default");
  assets_manager_preload(am, ASSET_TYPE_STYLE, "default");
#else
  assets_manager_add(am, ui_calendar);
  assets_manager_add(am, ui_main);
  assets_manager_add(am, ui_message);
  assets_manager_add(am, ui_music);
  assets_manager_add(am, ui_round_menu);
  assets_manager_add(am, ui_training);
  assets_manager_add(am, ui_watch_main_01);
  assets_manager_add(am, ui_watch_main_02);
  assets_manager_add(am, ui_weather);
  assets_manager_add(am, ui_weather_amin);
  assets_manager_add(am, style_default);
  assets_manager_add(am, image_calendar_button_normal);
  assets_manager_add(am, image_calendar_button_pressed);
  assets_manager_add(am, image_calendar_current);
  assets_manager_add(am, image_calender_down_normal);
  assets_manager_add(am, image_calender_down_pressed);
  assets_manager_add(am, image_calender_up_normal);
  assets_manager_add(am, image_calender_up_pressed);
  assets_manager_add(am, image_music_button_normal);
  assets_manager_add(am, image_music_button_pressed);
  assets_manager_add(am, image_music_immeditately_normal);
  assets_manager_add(am, image_music_immeditately_pressed);
  assets_manager_add(am, image_music_loop_normal);
  assets_manager_add(am, image_music_loop_pressed);
  assets_manager_add(am, image_music_loop_single_normal);
  assets_manager_add(am, image_music_loop_single_pressed);
  assets_manager_add(am, image_music_next_normal);
  assets_manager_add(am, image_music_next_pressed);
  assets_manager_add(am, image_music_perv_normal);
  assets_manager_add(am, image_music_perv_pressed);
  assets_manager_add(am, image_music_play_normal);
  assets_manager_add(am, image_music_play_pressed);
  assets_manager_add(am, image_music_sound);
  assets_manager_add(am, image_music_stop_normal);
  assets_manager_add(am, image_music_stop_pressed);
  assets_manager_add(am, image_training_button_normal);
  assets_manager_add(am, image_training_button_pressed);
  assets_manager_add(am, image_watch_main_background_1);
  assets_manager_add(am, image_watch_main_background_2);
  assets_manager_add(am, image_watch_main_hour);
  assets_manager_add(am, image_watch_main_minute);
  assets_manager_add(am, image_watch_main_second);
  assets_manager_add(am, image_weather_button_normal);
  assets_manager_add(am, image_weather_button_pressed);
  assets_manager_add(am, image_weather_cloudy);
  assets_manager_add(am, image_weather_degree);
  assets_manager_add(am, image_weather_rain);
  assets_manager_add(am, image_weather_sun);
  assets_manager_add(am, image_weather_thunder);
  assets_manager_add(am, font_default_18);
  assets_manager_add(am, font_default_26);
  assets_manager_add(am, font_default_38);
  assets_manager_add(am, font_default_43);
  assets_manager_add(am, font_default_76);
#ifdef WITH_VGCANVAS
#endif /*WITH_VGCANVAS*/
#endif

  tk_init_assets();
  return RET_OK;
}
