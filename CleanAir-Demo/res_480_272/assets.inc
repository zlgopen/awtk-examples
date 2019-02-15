#include "awtk.h"
#include "base/assets_manager.h"
#ifndef WITH_FS_RES
#include "assets/inc/strings/en_US.data"
#include "assets/inc/strings/zh_CN.data"
#include "assets/inc/styles/default.data"
#include "assets/inc/styles/keyboard.data"
#include "assets/inc/ui/calibration_win.data"
#include "assets/inc/ui/kb_ascii.data"
#include "assets/inc/ui/kb_default.data"
#include "assets/inc/ui/kb_float.data"
#include "assets/inc/ui/kb_hex.data"
#include "assets/inc/ui/kb_int.data"
#include "assets/inc/ui/kb_phone.data"
#include "assets/inc/ui/kb_ufloat.data"
#include "assets/inc/ui/kb_uint.data"
#include "assets/inc/ui/keyboard.data"
#include "assets/inc/ui/main.data"
#include "assets/inc/ui/record.data"
#include "assets/inc/ui/setting.data"
#include "assets/inc/ui/timing.data"
#ifdef WITH_STB_IMAGE
#include "assets/inc/images/arr_left.res"
#include "assets/inc/images/arr_left_hover.res"
#include "assets/inc/images/arr_left_push.res"
#include "assets/inc/images/arr_right.res"
#include "assets/inc/images/arr_right_hover.res"
#include "assets/inc/images/arr_right_push.res"
#include "assets/inc/images/backspace.res"
#include "assets/inc/images/bell.res"
#include "assets/inc/images/bg0_mix.res"
#include "assets/inc/images/bg1_mix.res"
#include "assets/inc/images/btn_1_en.res"
#include "assets/inc/images/btn_1_hover_en.res"
#include "assets/inc/images/btn_1_hover_zh.res"
#include "assets/inc/images/btn_1_push_en.res"
#include "assets/inc/images/btn_1_push_zh.res"
#include "assets/inc/images/btn_1_zh.res"
#include "assets/inc/images/btn_2a_en.res"
#include "assets/inc/images/btn_2a_hover_en.res"
#include "assets/inc/images/btn_2a_hover_zh.res"
#include "assets/inc/images/btn_2a_push_en.res"
#include "assets/inc/images/btn_2a_push_zh.res"
#include "assets/inc/images/btn_2a_zh.res"
#include "assets/inc/images/btn_2_en.res"
#include "assets/inc/images/btn_2_hover_en.res"
#include "assets/inc/images/btn_2_hover_zh.res"
#include "assets/inc/images/btn_2_push_en.res"
#include "assets/inc/images/btn_2_push_zh.res"
#include "assets/inc/images/btn_2_zh.res"
#include "assets/inc/images/btn_3_en.res"
#include "assets/inc/images/btn_3_hover_en.res"
#include "assets/inc/images/btn_3_hover_zh.res"
#include "assets/inc/images/btn_3_push_en.res"
#include "assets/inc/images/btn_3_push_zh.res"
#include "assets/inc/images/btn_3_zh.res"
#include "assets/inc/images/btn_4_en.res"
#include "assets/inc/images/btn_4_hover_en.res"
#include "assets/inc/images/btn_4_hover_zh.res"
#include "assets/inc/images/btn_4_push_en.res"
#include "assets/inc/images/btn_4_push_zh.res"
#include "assets/inc/images/btn_4_zh.res"
#include "assets/inc/images/btn_5_en.res"
#include "assets/inc/images/btn_5_hover_en.res"
#include "assets/inc/images/btn_5_hover_zh.res"
#include "assets/inc/images/btn_5_push_en.res"
#include "assets/inc/images/btn_5_push_zh.res"
#include "assets/inc/images/btn_5_zh.res"
#include "assets/inc/images/clock.res"
#include "assets/inc/images/fan_1a.res"
#include "assets/inc/images/fan_1b.res"
#include "assets/inc/images/fan_2.res"
#include "assets/inc/images/language_en.res"
#include "assets/inc/images/language_zh.res"
#include "assets/inc/images/wind_in.res"
#include "assets/inc/images/wind_out.res"
#else
#endif/*WITH_STB_IMAGE*/
#ifdef WITH_STB_FONT
#ifdef WITH_MINI_FONT
#include "assets/inc/fonts/default.mini.res"
#else/*WITH_MINI_FONT*/
#endif/*WITH_MINI_FONT*/
#else/*WITH_STB_FONT*/
#endif/*WITH_STB_FONT*/
#endif/*WITH_FS_RES*/

ret_t assets_init(void) {
  assets_manager_t* rm = assets_manager();

#ifdef WITH_FS_RES
#ifdef WITH_MINI_FONT
  asset_info_t* info = assets_manager_load(rm, ASSET_TYPE_FONT, "default.mini");
  if (info) {
    strcpy(info->name, "default");
  }
#else 
  assets_manager_load(rm, ASSET_TYPE_FONT, "default");
#endif
  assets_manager_load(rm, ASSET_TYPE_STYLE, "default");
#else
#ifdef WITH_STB_FONT
  assets_manager_add(rm, font_default);
#endif
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
  assets_manager_add(rm, ui_timing);
  assets_manager_add(rm, strings_en_US);
  assets_manager_add(rm, strings_zh_CN);
#endif

  tk_init_assets();
  return RET_OK;
}
