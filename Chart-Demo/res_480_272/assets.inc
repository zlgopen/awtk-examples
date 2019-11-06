#include "awtk.h"
#include "base/assets_manager.h"
#ifndef WITH_FS_RES
#include "assets/default/inc/strings/en_US.data"
#include "assets/default/inc/strings/zh_CN.data"
#include "assets/default/inc/styles/default.data"
#include "assets/default/inc/styles/keyboard.data"
#include "assets/default/inc/styles/system_bar.data"
#include "assets/default/inc/ui/kb_ascii.data"
#include "assets/default/inc/ui/kb_default.data"
#include "assets/default/inc/ui/kb_float.data"
#include "assets/default/inc/ui/kb_hex.data"
#include "assets/default/inc/ui/kb_int.data"
#include "assets/default/inc/ui/kb_phone.data"
#include "assets/default/inc/ui/kb_ufloat.data"
#include "assets/default/inc/ui/kb_uint.data"
#include "assets/default/inc/ui/main.data"
#include "assets/default/inc/ui/system_bar.data"
#include "assets/default/inc/ui/window_bar_series.data"
#include "assets/default/inc/ui/window_line_series.data"
#include "assets/default/inc/ui/window_meter.data"
#include "assets/default/inc/ui/window_pie.data"
#ifdef WITH_STB_IMAGE
#include "assets/default/inc/images/backspace.res"
#include "assets/default/inc/images/back_normal.res"
#include "assets/default/inc/images/back_pressed.res"
#include "assets/default/inc/images/bg_main.res"
#include "assets/default/inc/images/close_d.res"
#include "assets/default/inc/images/close_n.res"
#include "assets/default/inc/images/close_o.res"
#include "assets/default/inc/images/close_p.res"
#include "assets/default/inc/images/earth.res"
#include "assets/default/inc/images/graph_normal.res"
#include "assets/default/inc/images/graph_normal_en.res"
#include "assets/default/inc/images/graph_pressed.res"
#include "assets/default/inc/images/graph_pressed_en.res"
#include "assets/default/inc/images/guage_bg_1.res"
#include "assets/default/inc/images/guage_bg_2.res"
#include "assets/default/inc/images/guage_pointer_1.res"
#include "assets/default/inc/images/guage_pointer_2.res"
#include "assets/default/inc/images/histogram_normal.res"
#include "assets/default/inc/images/histogram_normal_en.res"
#include "assets/default/inc/images/histogram_pressed.res"
#include "assets/default/inc/images/histogram_pressed_en.res"
#include "assets/default/inc/images/icon_1_normal.res"
#include "assets/default/inc/images/icon_1_pressed.res"
#include "assets/default/inc/images/icon_2_normal.res"
#include "assets/default/inc/images/icon_2_pressed.res"
#include "assets/default/inc/images/icon_3_normal.res"
#include "assets/default/inc/images/icon_3_pressed.res"
#include "assets/default/inc/images/icon_4_normal.res"
#include "assets/default/inc/images/icon_4_pressed.res"
#include "assets/default/inc/images/icon_5_normal.res"
#include "assets/default/inc/images/icon_5_pressed.res"
#include "assets/default/inc/images/icon_6_normal.res"
#include "assets/default/inc/images/icon_6_pressed.res"
#include "assets/default/inc/images/language_en.res"
#include "assets/default/inc/images/language_zh.res"
#include "assets/default/inc/images/meter_normal.res"
#include "assets/default/inc/images/meter_normal_en.res"
#include "assets/default/inc/images/meter_pressed.res"
#include "assets/default/inc/images/meter_pressed_en.res"
#include "assets/default/inc/images/new_normal.res"
#include "assets/default/inc/images/new_pressed.res"
#include "assets/default/inc/images/pause_mormal.res"
#include "assets/default/inc/images/pause_pressed.res"
#include "assets/default/inc/images/pie_label_normal.res"
#include "assets/default/inc/images/pie_label_select.res"
#include "assets/default/inc/images/pie_normal.res"
#include "assets/default/inc/images/pie_normal_en.res"
#include "assets/default/inc/images/pie_pressed.res"
#include "assets/default/inc/images/pie_pressed_en.res"
#include "assets/default/inc/images/play_normal.res"
#include "assets/default/inc/images/play_pressed.res"
#include "assets/default/inc/images/shift.res"
#include "assets/default/inc/images/shifton.res"
#else
#endif/*WITH_STB_IMAGE*/
#if defined(WITH_STB_FONT) || defined(WITH_FT_FONT)
#include "assets/default/inc/fonts/default.res"
#else/*WITH_STB_FONT or WITH_FT_FONT*/
#endif/*WITH_STB_FONT or WITH_FT_FONT*/
#endif/*WITH_FS_RES*/

ret_t assets_init(void) {
  assets_manager_t* rm = assets_manager();

#ifdef WITH_FS_RES
  assets_manager_preload(rm, ASSET_TYPE_FONT, "default");
  assets_manager_preload(rm, ASSET_TYPE_STYLE, "default");
#else
  assets_manager_add(rm, font_default);
  assets_manager_add(rm, image_backspace);
  assets_manager_add(rm, image_back_normal);
  assets_manager_add(rm, image_back_pressed);
  assets_manager_add(rm, image_bg_main);
  assets_manager_add(rm, image_close_d);
  assets_manager_add(rm, image_close_n);
  assets_manager_add(rm, image_close_o);
  assets_manager_add(rm, image_close_p);
  assets_manager_add(rm, image_earth);
  assets_manager_add(rm, image_graph_normal);
  assets_manager_add(rm, image_graph_normal_en);
  assets_manager_add(rm, image_graph_pressed);
  assets_manager_add(rm, image_graph_pressed_en);
  assets_manager_add(rm, image_guage_bg_1);
  assets_manager_add(rm, image_guage_bg_2);
  assets_manager_add(rm, image_guage_pointer_1);
  assets_manager_add(rm, image_guage_pointer_2);
  assets_manager_add(rm, image_histogram_normal);
  assets_manager_add(rm, image_histogram_normal_en);
  assets_manager_add(rm, image_histogram_pressed);
  assets_manager_add(rm, image_histogram_pressed_en);
  assets_manager_add(rm, image_icon_1_normal);
  assets_manager_add(rm, image_icon_1_pressed);
  assets_manager_add(rm, image_icon_2_normal);
  assets_manager_add(rm, image_icon_2_pressed);
  assets_manager_add(rm, image_icon_3_normal);
  assets_manager_add(rm, image_icon_3_pressed);
  assets_manager_add(rm, image_icon_4_normal);
  assets_manager_add(rm, image_icon_4_pressed);
  assets_manager_add(rm, image_icon_5_normal);
  assets_manager_add(rm, image_icon_5_pressed);
  assets_manager_add(rm, image_icon_6_normal);
  assets_manager_add(rm, image_icon_6_pressed);
  assets_manager_add(rm, image_language_en);
  assets_manager_add(rm, image_language_zh);
  assets_manager_add(rm, image_meter_normal);
  assets_manager_add(rm, image_meter_normal_en);
  assets_manager_add(rm, image_meter_pressed);
  assets_manager_add(rm, image_meter_pressed_en);
  assets_manager_add(rm, image_new_normal);
  assets_manager_add(rm, image_new_pressed);
  assets_manager_add(rm, image_pause_mormal);
  assets_manager_add(rm, image_pause_pressed);
  assets_manager_add(rm, image_pie_label_normal);
  assets_manager_add(rm, image_pie_label_select);
  assets_manager_add(rm, image_pie_normal);
  assets_manager_add(rm, image_pie_normal_en);
  assets_manager_add(rm, image_pie_pressed);
  assets_manager_add(rm, image_pie_pressed_en);
  assets_manager_add(rm, image_play_normal);
  assets_manager_add(rm, image_play_pressed);
  assets_manager_add(rm, image_shift);
  assets_manager_add(rm, image_shifton);
  assets_manager_add(rm, style_default);
  assets_manager_add(rm, style_keyboard);
  assets_manager_add(rm, style_system_bar);
  assets_manager_add(rm, ui_kb_ascii);
  assets_manager_add(rm, ui_kb_default);
  assets_manager_add(rm, ui_kb_float);
  assets_manager_add(rm, ui_kb_hex);
  assets_manager_add(rm, ui_kb_int);
  assets_manager_add(rm, ui_kb_phone);
  assets_manager_add(rm, ui_kb_ufloat);
  assets_manager_add(rm, ui_kb_uint);
  assets_manager_add(rm, ui_main);
  assets_manager_add(rm, ui_system_bar);
  assets_manager_add(rm, ui_window_bar_series);
  assets_manager_add(rm, ui_window_line_series);
  assets_manager_add(rm, ui_window_meter);
  assets_manager_add(rm, ui_window_pie);
  assets_manager_add(rm, strings_en_US);
  assets_manager_add(rm, strings_zh_CN);
#endif

  tk_init_assets();
  return RET_OK;
}
