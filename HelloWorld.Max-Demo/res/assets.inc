#include "awtk.h"
#include "base/assets_manager.h"
#ifndef WITH_FS_RES
#include "assets/default/inc/strings/en_US.data"
#include "assets/default/inc/strings/zh_CN.data"
#include "assets/default/inc/styles/default.data"
#include "assets/default/inc/styles/keyboard.data"
#include "assets/default/inc/ui/animator.data"
#include "assets/default/inc/ui/background.data"
#include "assets/default/inc/ui/basic.data"
#include "assets/default/inc/ui/kb_default.data"
#include "assets/default/inc/ui/list_view.data"
#include "assets/default/inc/ui/main.data"
#ifdef WITH_STB_IMAGE
#include "assets/default/inc/images/awtk_logo.res"
#include "assets/default/inc/images/backspace.res"
#include "assets/default/inc/images/bg_landscape_1.res"
#include "assets/default/inc/images/bg_landscape_2.res"
#include "assets/default/inc/images/big_earth.res"
#include "assets/default/inc/images/earth.res"
#include "assets/default/inc/images/language_en.res"
#include "assets/default/inc/images/language_zh.res"
#include "assets/default/inc/images/logo.res"
#include "assets/default/inc/images/shift.res"
#include "assets/default/inc/images/shifton.res"
#include "assets/default/inc/images/slider_drag.res"
#include "assets/default/inc/images/slider_drag_o.res"
#include "assets/default/inc/images/slider_drag_p.res"
#include "assets/default/inc/images/slider_v_bg.res"
#include "assets/default/inc/images/slider_v_fg.res"
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
  assets_manager_add(rm, image_awtk_logo);
  assets_manager_add(rm, image_backspace);
  assets_manager_add(rm, image_bg_landscape_1);
  assets_manager_add(rm, image_bg_landscape_2);
  assets_manager_add(rm, image_big_earth);
  assets_manager_add(rm, image_earth);
  assets_manager_add(rm, image_language_en);
  assets_manager_add(rm, image_language_zh);
  assets_manager_add(rm, image_logo);
  assets_manager_add(rm, image_shift);
  assets_manager_add(rm, image_shifton);
  assets_manager_add(rm, image_slider_drag);
  assets_manager_add(rm, image_slider_drag_o);
  assets_manager_add(rm, image_slider_drag_p);
  assets_manager_add(rm, image_slider_v_bg);
  assets_manager_add(rm, image_slider_v_fg);
  assets_manager_add(rm, style_default);
  assets_manager_add(rm, style_keyboard);
  assets_manager_add(rm, ui_animator);
  assets_manager_add(rm, ui_background);
  assets_manager_add(rm, ui_basic);
  assets_manager_add(rm, ui_kb_default);
  assets_manager_add(rm, ui_list_view);
  assets_manager_add(rm, ui_main);
  assets_manager_add(rm, strings_en_US);
  assets_manager_add(rm, strings_zh_CN);
#endif

  tk_init_assets();
  return RET_OK;
}
