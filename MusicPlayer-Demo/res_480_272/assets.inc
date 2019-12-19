#include "awtk.h"
#include "base/assets_manager.h"
#ifndef WITH_FS_RES
#include "assets/default/inc/strings/en_US.data"
#include "assets/default/inc/strings/zh_CN.data"
#include "assets/default/inc/styles/default.data"
#include "assets/default/inc/styles/keyboard.data"
#include "assets/default/inc/ui/advanced.data"
#include "assets/default/inc/ui/equalizer.data"
#include "assets/default/inc/ui/main.data"
#include "assets/default/inc/ui/play_list.data"
#include "assets/default/inc/data/let_it_go.res"
#include "assets/default/inc/data/see_you_again.res"
#include "assets/default/inc/data/yellow.res"
#ifdef WITH_STB_IMAGE
#include "assets/default/inc/images/advanced_n.res"
#include "assets/default/inc/images/bg_main.res"
#include "assets/default/inc/images/btn_round.res"
#include "assets/default/inc/images/b_anim_n.res"
#include "assets/default/inc/images/b_anim_p.res"
#include "assets/default/inc/images/close_n.res"
#include "assets/default/inc/images/eq_n.res"
#include "assets/default/inc/images/guage_pointer.res"
#include "assets/default/inc/images/icon_list.res"
#include "assets/default/inc/images/let_it_go.res"
#include "assets/default/inc/images/lrc_rule.res"
#include "assets/default/inc/images/mode_circleall_n.res"
#include "assets/default/inc/images/mode_circleone_n.res"
#include "assets/default/inc/images/mode_circlerand_n.res"
#include "assets/default/inc/images/play_list_n.res"
#include "assets/default/inc/images/play_n.res"
#include "assets/default/inc/images/play_next_n.res"
#include "assets/default/inc/images/play_pause_n.res"
#include "assets/default/inc/images/play_prve_n.res"
#include "assets/default/inc/images/see_you_again.res"
#include "assets/default/inc/images/slider_bg.res"
#include "assets/default/inc/images/slider_drag.res"
#include "assets/default/inc/images/slider_fg.res"
#include "assets/default/inc/images/slider_v_bg.res"
#include "assets/default/inc/images/slider_v_fg.res"
#include "assets/default/inc/images/Vinyl.res"
#include "assets/default/inc/images/yellow.res"
#else
#endif/*WITH_STB_IMAGE*/
#ifdef WITH_VGCANVAS
#include "assets/default/inc/images/pointer.bsvg"
#include "assets/default/inc/images/round.bsvg"
#endif/*WITH_VGCANVAS*/
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
#ifdef WITH_VGCANVAS
  assets_manager_add(rm, image_pointer);
  assets_manager_add(rm, image_round);
#endif/*WITH_VGCANVAS*/
#endif

  tk_init_assets();
  return RET_OK;
}
