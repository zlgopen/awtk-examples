#include "awtk.h"
#include "base/assets_manager.h"
#ifndef WITH_FS_RES
#include "assets/default/inc/styles/default.data"
#include "assets/default/inc/ui/animation/animation.data"
#include "assets/default/inc/ui/basic/basic.data"
#include "assets/default/inc/ui/home_page/home_page.data"
#include "assets/default/inc/ui/kb_default.data"
#ifdef WITH_STB_IMAGE
#include "assets/default/inc/images/arrow_down_d.res"
#include "assets/default/inc/images/arrow_down_f.res"
#include "assets/default/inc/images/arrow_down_n.res"
#include "assets/default/inc/images/arrow_down_o.res"
#include "assets/default/inc/images/arrow_down_p.res"
#include "assets/default/inc/images/arrow_left_d.res"
#include "assets/default/inc/images/arrow_left_f.res"
#include "assets/default/inc/images/arrow_left_n.res"
#include "assets/default/inc/images/arrow_left_o.res"
#include "assets/default/inc/images/arrow_left_p.res"
#include "assets/default/inc/images/arrow_right_d.res"
#include "assets/default/inc/images/arrow_right_f.res"
#include "assets/default/inc/images/arrow_right_n.res"
#include "assets/default/inc/images/arrow_right_o.res"
#include "assets/default/inc/images/arrow_right_p.res"
#include "assets/default/inc/images/arrow_up_d.res"
#include "assets/default/inc/images/arrow_up_f.res"
#include "assets/default/inc/images/arrow_up_n.res"
#include "assets/default/inc/images/arrow_up_o.res"
#include "assets/default/inc/images/arrow_up_p.res"
#include "assets/default/inc/images/awtk_logo.res"
#include "assets/default/inc/images/big_earth.res"
#include "assets/default/inc/images/check_checked_d.res"
#include "assets/default/inc/images/check_checked_f.res"
#include "assets/default/inc/images/check_checked_n.res"
#include "assets/default/inc/images/check_checked_o.res"
#include "assets/default/inc/images/check_checked_p.res"
#include "assets/default/inc/images/check_d.res"
#include "assets/default/inc/images/check_f.res"
#include "assets/default/inc/images/check_n.res"
#include "assets/default/inc/images/check_o.res"
#include "assets/default/inc/images/check_p.res"
#include "assets/default/inc/images/clock_bg.res"
#include "assets/default/inc/images/clock_hour.res"
#include "assets/default/inc/images/clock_minute.res"
#include "assets/default/inc/images/clock_second.res"
#include "assets/default/inc/images/cross.res"
#include "assets/default/inc/images/guage_bg.res"
#include "assets/default/inc/images/guage_pointer.res"
#include "assets/default/inc/images/logo.res"
#include "assets/default/inc/images/num0.res"
#include "assets/default/inc/images/num1.res"
#include "assets/default/inc/images/num2.res"
#include "assets/default/inc/images/num3.res"
#include "assets/default/inc/images/num4.res"
#include "assets/default/inc/images/num5.res"
#include "assets/default/inc/images/num6.res"
#include "assets/default/inc/images/num7.res"
#include "assets/default/inc/images/num8.res"
#include "assets/default/inc/images/num9.res"
#include "assets/default/inc/images/num_dot.res"
#include "assets/default/inc/images/radio_checked_d.res"
#include "assets/default/inc/images/radio_checked_f.res"
#include "assets/default/inc/images/radio_checked_n.res"
#include "assets/default/inc/images/radio_checked_o.res"
#include "assets/default/inc/images/radio_checked_p.res"
#include "assets/default/inc/images/radio_d.res"
#include "assets/default/inc/images/radio_f.res"
#include "assets/default/inc/images/radio_n.res"
#include "assets/default/inc/images/radio_o.res"
#include "assets/default/inc/images/radio_p.res"
#include "assets/default/inc/images/rounded_n.res"
#include "assets/default/inc/images/rounded_o.res"
#include "assets/default/inc/images/rounded_p.res"
#else
#endif/*WITH_STB_IMAGE*/
#ifdef WITH_VGCANVAS
#endif/*WITH_VGCANVAS*/
#if defined(WITH_TRUETYPE_FONT)
#include "assets/default/inc/fonts/default.res"
#else/*WITH_TRUETYPE_FONT*/
#endif/*WITH_TRUETYPE_FONT*/
#endif/*WITH_FS_RES*/

ret_t assets_init(void) {
  assets_manager_t* rm = assets_manager();

#ifdef WITH_FS_RES
  assets_manager_preload(rm, ASSET_TYPE_FONT, "default");
  assets_manager_preload(rm, ASSET_TYPE_STYLE, "default");
#else
  assets_manager_add(rm, style_default);
  assets_manager_add(rm, ui_kb_default);
  assets_manager_add(rm, font_default);
  assets_manager_add(rm, image_arrow_down_d);
  assets_manager_add(rm, image_arrow_down_f);
  assets_manager_add(rm, image_arrow_down_n);
  assets_manager_add(rm, image_arrow_down_o);
  assets_manager_add(rm, image_arrow_down_p);
  assets_manager_add(rm, image_arrow_left_d);
  assets_manager_add(rm, image_arrow_left_f);
  assets_manager_add(rm, image_arrow_left_n);
  assets_manager_add(rm, image_arrow_left_o);
  assets_manager_add(rm, image_arrow_left_p);
  assets_manager_add(rm, image_arrow_right_d);
  assets_manager_add(rm, image_arrow_right_f);
  assets_manager_add(rm, image_arrow_right_n);
  assets_manager_add(rm, image_arrow_right_o);
  assets_manager_add(rm, image_arrow_right_p);
  assets_manager_add(rm, image_arrow_up_d);
  assets_manager_add(rm, image_arrow_up_f);
  assets_manager_add(rm, image_arrow_up_n);
  assets_manager_add(rm, image_arrow_up_o);
  assets_manager_add(rm, image_arrow_up_p);
  assets_manager_add(rm, image_awtk_logo);
  assets_manager_add(rm, image_big_earth);
  assets_manager_add(rm, image_check_checked_d);
  assets_manager_add(rm, image_check_checked_f);
  assets_manager_add(rm, image_check_checked_n);
  assets_manager_add(rm, image_check_checked_o);
  assets_manager_add(rm, image_check_checked_p);
  assets_manager_add(rm, image_check_d);
  assets_manager_add(rm, image_check_f);
  assets_manager_add(rm, image_check_n);
  assets_manager_add(rm, image_check_o);
  assets_manager_add(rm, image_check_p);
  assets_manager_add(rm, image_clock_bg);
  assets_manager_add(rm, image_clock_hour);
  assets_manager_add(rm, image_clock_minute);
  assets_manager_add(rm, image_clock_second);
  assets_manager_add(rm, image_cross);
  assets_manager_add(rm, image_guage_bg);
  assets_manager_add(rm, image_guage_pointer);
  assets_manager_add(rm, image_logo);
  assets_manager_add(rm, image_num0);
  assets_manager_add(rm, image_num1);
  assets_manager_add(rm, image_num2);
  assets_manager_add(rm, image_num3);
  assets_manager_add(rm, image_num4);
  assets_manager_add(rm, image_num5);
  assets_manager_add(rm, image_num6);
  assets_manager_add(rm, image_num7);
  assets_manager_add(rm, image_num8);
  assets_manager_add(rm, image_num9);
  assets_manager_add(rm, image_num_dot);
  assets_manager_add(rm, image_radio_checked_d);
  assets_manager_add(rm, image_radio_checked_f);
  assets_manager_add(rm, image_radio_checked_n);
  assets_manager_add(rm, image_radio_checked_o);
  assets_manager_add(rm, image_radio_checked_p);
  assets_manager_add(rm, image_radio_d);
  assets_manager_add(rm, image_radio_f);
  assets_manager_add(rm, image_radio_n);
  assets_manager_add(rm, image_radio_o);
  assets_manager_add(rm, image_radio_p);
  assets_manager_add(rm, image_rounded_n);
  assets_manager_add(rm, image_rounded_o);
  assets_manager_add(rm, image_rounded_p);
  assets_manager_add(rm, ui_animation_animation);
  assets_manager_add(rm, ui_basic_basic);
  assets_manager_add(rm, ui_home_page_home_page);
#ifdef WITH_VGCANVAS
#endif/*WITH_VGCANVAS*/
#endif

  tk_init_assets();
  return RET_OK;
}
