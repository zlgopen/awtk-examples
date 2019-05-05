#include "awtk.h"
#include "base/assets_manager.h"
#ifndef WITH_FS_RES
#include "assets/inc/strings/en_US.data"
#include "assets/inc/strings/zh_CN.data"
#include "assets/inc/styles/default.data"
#include "assets/inc/styles/keyboard.data"
#include "assets/inc/ui/advanced.data"
#include "assets/inc/ui/equalizer.data"
#include "assets/inc/ui/main.data"
#include "assets/inc/ui/play_list.data"
#include "assets/inc/data/let_it_go.res"
#include "assets/inc/data/see_you_again.res"
#include "assets/inc/data/yellow.res"
#ifdef WITH_STB_IMAGE
#include "assets/inc/images/advanced_n.res"
#include "assets/inc/images/bg_main.res"
#include "assets/inc/images/btn_round.res"
#include "assets/inc/images/b_anim_n.res"
#include "assets/inc/images/b_anim_p.res"
#include "assets/inc/images/close_n.res"
#include "assets/inc/images/eq_n.res"
#include "assets/inc/images/guage_pointer.res"
#include "assets/inc/images/icon_list.res"
#include "assets/inc/images/let_it_go.res"
#include "assets/inc/images/lrc_rule.res"
#include "assets/inc/images/mode_circleall_n.res"
#include "assets/inc/images/mode_circleone_n.res"
#include "assets/inc/images/mode_circlerand_n.res"
#include "assets/inc/images/play_list_n.res"
#include "assets/inc/images/play_n.res"
#include "assets/inc/images/play_next_n.res"
#include "assets/inc/images/play_pause_n.res"
#include "assets/inc/images/play_prve_n.res"
#include "assets/inc/images/see_you_again.res"
#include "assets/inc/images/slider_bg.res"
#include "assets/inc/images/slider_drag.res"
#include "assets/inc/images/slider_fg.res"
#include "assets/inc/images/slider_v_bg.res"
#include "assets/inc/images/slider_v_fg.res"
#include "assets/inc/images/Vinyl.res"
#include "assets/inc/images/yellow.res"
#else
#endif/*WITH_STB_IMAGE*/
#ifdef WITH_STB_FONT
#ifdef WITH_MINI_FONT
#include "assets/inc/fonts/default_mini.res"
#else/*WITH_MINI_FONT*/
#endif/*WITH_MINI_FONT*/
#else/*WITH_STB_FONT*/
#endif/*WITH_STB_FONT*/
#endif/*WITH_FS_RES*/

ret_t assets_init(void) {
  assets_manager_t* rm = assets_manager();

#ifdef WITH_FS_RES
#if defined(WITH_MINI_FONT)
  assets_manager_preload(rm, ASSET_TYPE_FONT, "default_mini");
#else/*WITH_MINI_FONT*/
  assets_manager_preload(rm, ASSET_TYPE_FONT, "default");
#endif/*WITH_MINI_FONT*/
  assets_manager_preload(rm, ASSET_TYPE_STYLE, "default");
#else
#if defined(WITH_MINI_FONT) && (defined(WITH_STB_FONT) || defined(WITH_FT_FONT))
  assets_manager_add(rm, font_default_mini);
#else/*WITH_MINI_FONT*/
   assets_manager_add(rm, font_default);
#endif
  assets_manager_add(rm, image_advanced_n);
  assets_manager_add(rm, image_bg_main);
  assets_manager_add(rm, image_btn_round);
  assets_manager_add(rm, image_b_anim_n);
  assets_manager_add(rm, image_b_anim_p);
  assets_manager_add(rm, image_close_n);
  assets_manager_add(rm, image_eq_n);
  assets_manager_add(rm, image_guage_pointer);
  assets_manager_add(rm, image_icon_list);
  assets_manager_add(rm, image_let_it_go);
  assets_manager_add(rm, image_lrc_rule);
  assets_manager_add(rm, image_mode_circleall_n);
  assets_manager_add(rm, image_mode_circleone_n);
  assets_manager_add(rm, image_mode_circlerand_n);
  assets_manager_add(rm, image_play_list_n);
  assets_manager_add(rm, image_play_n);
  assets_manager_add(rm, image_play_next_n);
  assets_manager_add(rm, image_play_pause_n);
  assets_manager_add(rm, image_play_prve_n);
  assets_manager_add(rm, image_see_you_again);
  assets_manager_add(rm, image_slider_bg);
  assets_manager_add(rm, image_slider_drag);
  assets_manager_add(rm, image_slider_fg);
  assets_manager_add(rm, image_slider_v_bg);
  assets_manager_add(rm, image_slider_v_fg);
  assets_manager_add(rm, image_Vinyl);
  assets_manager_add(rm, image_yellow);
  assets_manager_add(rm, style_default);
  assets_manager_add(rm, style_keyboard);
  assets_manager_add(rm, ui_advanced);
  assets_manager_add(rm, ui_equalizer);
  assets_manager_add(rm, ui_main);
  assets_manager_add(rm, ui_play_list);
  assets_manager_add(rm, strings_en_US);
  assets_manager_add(rm, strings_zh_CN);
  assets_manager_add(rm, data_let_it_go_bin);
  assets_manager_add(rm, data_see_you_again_bin);
  assets_manager_add(rm, data_yellow_bin);
#endif

  tk_init_assets();
  return RET_OK;
}
