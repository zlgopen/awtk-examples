#include "awtk.h"
#include "base/assets_manager.h"
#ifndef WITH_FS_RES
#include "assets/default/inc/strings/en_US.data"
#include "assets/default/inc/strings/zh_CN.data"
#include "assets/default/inc/styles/default.data"
#include "assets/default/inc/styles/keyboard.data"
#include "assets/default/inc/ui/calibration_win.data"
#include "assets/default/inc/ui/kb_ascii.data"
#include "assets/default/inc/ui/kb_default.data"
#include "assets/default/inc/ui/kb_float.data"
#include "assets/default/inc/ui/kb_hex.data"
#include "assets/default/inc/ui/kb_int.data"
#include "assets/default/inc/ui/kb_phone.data"
#include "assets/default/inc/ui/kb_ufloat.data"
#include "assets/default/inc/ui/kb_uint.data"
#include "assets/default/inc/ui/keyboard.data"
#include "assets/default/inc/ui/main.data"
#include "assets/default/inc/ui/record.data"
#include "assets/default/inc/ui/setting.data"
#include "assets/default/inc/ui/system_bar.data"
#include "assets/default/inc/ui/timing.data"
#ifdef WITH_STB_IMAGE
#include "assets/default/inc/images/arr_left.res"
#include "assets/default/inc/images/arr_left_hover.res"
#include "assets/default/inc/images/arr_left_push.res"
#include "assets/default/inc/images/arr_right.res"
#include "assets/default/inc/images/arr_right_hover.res"
#include "assets/default/inc/images/arr_right_push.res"
#include "assets/default/inc/images/backspace.res"
#include "assets/default/inc/images/bell.res"
#include "assets/default/inc/images/bg0_mix.res"
#include "assets/default/inc/images/bg1_mix.res"
#include "assets/default/inc/images/btn_1_en.res"
#include "assets/default/inc/images/btn_1_hover_en.res"
#include "assets/default/inc/images/btn_1_hover_zh.res"
#include "assets/default/inc/images/btn_1_push_en.res"
#include "assets/default/inc/images/btn_1_push_zh.res"
#include "assets/default/inc/images/btn_1_zh.res"
#include "assets/default/inc/images/btn_2a_en.res"
#include "assets/default/inc/images/btn_2a_hover_en.res"
#include "assets/default/inc/images/btn_2a_hover_zh.res"
#include "assets/default/inc/images/btn_2a_push_en.res"
#include "assets/default/inc/images/btn_2a_push_zh.res"
#include "assets/default/inc/images/btn_2a_zh.res"
#include "assets/default/inc/images/btn_2_en.res"
#include "assets/default/inc/images/btn_2_hover_en.res"
#include "assets/default/inc/images/btn_2_hover_zh.res"
#include "assets/default/inc/images/btn_2_push_en.res"
#include "assets/default/inc/images/btn_2_push_zh.res"
#include "assets/default/inc/images/btn_2_zh.res"
#include "assets/default/inc/images/btn_3_en.res"
#include "assets/default/inc/images/btn_3_hover_en.res"
#include "assets/default/inc/images/btn_3_hover_zh.res"
#include "assets/default/inc/images/btn_3_push_en.res"
#include "assets/default/inc/images/btn_3_push_zh.res"
#include "assets/default/inc/images/btn_3_zh.res"
#include "assets/default/inc/images/btn_4_en.res"
#include "assets/default/inc/images/btn_4_hover_en.res"
#include "assets/default/inc/images/btn_4_hover_zh.res"
#include "assets/default/inc/images/btn_4_push_en.res"
#include "assets/default/inc/images/btn_4_push_zh.res"
#include "assets/default/inc/images/btn_4_zh.res"
#include "assets/default/inc/images/btn_5_en.res"
#include "assets/default/inc/images/btn_5_hover_en.res"
#include "assets/default/inc/images/btn_5_hover_zh.res"
#include "assets/default/inc/images/btn_5_push_en.res"
#include "assets/default/inc/images/btn_5_push_zh.res"
#include "assets/default/inc/images/btn_5_zh.res"
#include "assets/default/inc/images/clock.res"
#include "assets/default/inc/images/fan_1a.res"
#include "assets/default/inc/images/fan_1b.res"
#include "assets/default/inc/images/fan_2.res"
#include "assets/default/inc/images/language_en.res"
#include "assets/default/inc/images/language_zh.res"
#include "assets/default/inc/images/wind_in.res"
#include "assets/default/inc/images/wind_out.res"
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
  assets_manager_add(rm, image_arr_left);
  assets_manager_add(rm, image_arr_left_hover);
  assets_manager_add(rm, image_arr_left_push);
  assets_manager_add(rm, image_arr_right);
  assets_manager_add(rm, image_arr_right_hover);
  assets_manager_add(rm, image_arr_right_push);
  assets_manager_add(rm, image_backspace);
  assets_manager_add(rm, image_bell);
  assets_manager_add(rm, image_bg0_mix);
  assets_manager_add(rm, image_bg1_mix);
  assets_manager_add(rm, image_btn_1_en);
  assets_manager_add(rm, image_btn_1_hover_en);
  assets_manager_add(rm, image_btn_1_hover_zh);
  assets_manager_add(rm, image_btn_1_push_en);
  assets_manager_add(rm, image_btn_1_push_zh);
  assets_manager_add(rm, image_btn_1_zh);
  assets_manager_add(rm, image_btn_2a_en);
  assets_manager_add(rm, image_btn_2a_hover_en);
  assets_manager_add(rm, image_btn_2a_hover_zh);
  assets_manager_add(rm, image_btn_2a_push_en);
  assets_manager_add(rm, image_btn_2a_push_zh);
  assets_manager_add(rm, image_btn_2a_zh);
  assets_manager_add(rm, image_btn_2_en);
  assets_manager_add(rm, image_btn_2_hover_en);
  assets_manager_add(rm, image_btn_2_hover_zh);
  assets_manager_add(rm, image_btn_2_push_en);
  assets_manager_add(rm, image_btn_2_push_zh);
  assets_manager_add(rm, image_btn_2_zh);
  assets_manager_add(rm, image_btn_3_en);
  assets_manager_add(rm, image_btn_3_hover_en);
  assets_manager_add(rm, image_btn_3_hover_zh);
  assets_manager_add(rm, image_btn_3_push_en);
  assets_manager_add(rm, image_btn_3_push_zh);
  assets_manager_add(rm, image_btn_3_zh);
  assets_manager_add(rm, image_btn_4_en);
  assets_manager_add(rm, image_btn_4_hover_en);
  assets_manager_add(rm, image_btn_4_hover_zh);
  assets_manager_add(rm, image_btn_4_push_en);
  assets_manager_add(rm, image_btn_4_push_zh);
  assets_manager_add(rm, image_btn_4_zh);
  assets_manager_add(rm, image_btn_5_en);
  assets_manager_add(rm, image_btn_5_hover_en);
  assets_manager_add(rm, image_btn_5_hover_zh);
  assets_manager_add(rm, image_btn_5_push_en);
  assets_manager_add(rm, image_btn_5_push_zh);
  assets_manager_add(rm, image_btn_5_zh);
  assets_manager_add(rm, image_clock);
  assets_manager_add(rm, image_fan_1a);
  assets_manager_add(rm, image_fan_1b);
  assets_manager_add(rm, image_fan_2);
  assets_manager_add(rm, image_language_en);
  assets_manager_add(rm, image_language_zh);
  assets_manager_add(rm, image_wind_in);
  assets_manager_add(rm, image_wind_out);
  assets_manager_add(rm, style_default);
  assets_manager_add(rm, style_keyboard);
  assets_manager_add(rm, ui_calibration_win);
  assets_manager_add(rm, ui_kb_ascii);
  assets_manager_add(rm, ui_kb_default);
  assets_manager_add(rm, ui_kb_float);
  assets_manager_add(rm, ui_kb_hex);
  assets_manager_add(rm, ui_kb_int);
  assets_manager_add(rm, ui_kb_phone);
  assets_manager_add(rm, ui_kb_ufloat);
  assets_manager_add(rm, ui_kb_uint);
  assets_manager_add(rm, ui_keyboard);
  assets_manager_add(rm, ui_main);
  assets_manager_add(rm, ui_record);
  assets_manager_add(rm, ui_setting);
  assets_manager_add(rm, ui_system_bar);
  assets_manager_add(rm, ui_timing);
  assets_manager_add(rm, strings_en_US);
  assets_manager_add(rm, strings_zh_CN);
#endif

  tk_init_assets();
  return RET_OK;
}
