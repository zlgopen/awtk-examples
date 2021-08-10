#include "awtk.h"
#include "base/assets_manager.h"
#ifndef WITH_FS_RES
#include "default/inc/ui/advanced/advanced.data"
#include "default/inc/ui/equalizer/equalizer.data"
#include "default/inc/ui/home_page/home_page.data"
#include "default/inc/ui/play_list/play_list.data"
#include "default/inc/styles/default.data"
#include "default/inc/styles/equalizer.data"
#include "default/inc/styles/home_page.data"
#include "default/inc/data/let_it_go_txt.data"
#include "default/inc/data/see_you_again_txt.data"
#include "default/inc/data/yellow_txt.data"
#ifdef WITH_STB_IMAGE
#include "default/inc/images/advanced_n.res"
#include "default/inc/images/bg_main.res"
#include "default/inc/images/btn_round.res"
#include "default/inc/images/b_anim_n.res"
#include "default/inc/images/b_anim_p.res"
#include "default/inc/images/close_n.res"
#include "default/inc/images/eq_n.res"
#include "default/inc/images/guage_pointer.res"
#include "default/inc/images/icon_list.res"
#include "default/inc/images/let_it_go.res"
#include "default/inc/images/lrc_rule.res"
#include "default/inc/images/mode_circleall_n.res"
#include "default/inc/images/mode_circleone_n.res"
#include "default/inc/images/mode_circlerand_n.res"
#include "default/inc/images/play_list_n.res"
#include "default/inc/images/play_n.res"
#include "default/inc/images/play_next_n.res"
#include "default/inc/images/play_pause_n.res"
#include "default/inc/images/play_prev_n.res"
#include "default/inc/images/play_prve_n.res"
#include "default/inc/images/see_you_again.res"
#include "default/inc/images/slider_drag.res"
#include "default/inc/images/Vinyl.res"
#include "default/inc/images/yellow.res"
#else
#endif /*WITH_STB_IMAGE*/
#ifdef WITH_VGCANVAS
#include "default/inc/images/pointer.bsvg"
#include "default/inc/images/round.bsvg"
#endif /*WITH_VGCANVAS*/
#ifdef WITH_TRUETYPE_FONT
#include "default/inc/fonts/default.res"
#else /*WITH_TRUETYPE_FONT*/
#endif /*WITH_TRUETYPE_FONT*/
#endif /*WITH_FS_RES*/

ret_t assets_init_default(void) {
  assets_manager_t* am = assets_manager();

#ifdef WITH_FS_RES
  assets_manager_preload(am, ASSET_TYPE_FONT, "default");
  assets_manager_preload(am, ASSET_TYPE_STYLE, "default");
#else
  assets_manager_add(am, ui_advanced_advanced);
  assets_manager_add(am, ui_equalizer_equalizer);
  assets_manager_add(am, ui_home_page_home_page);
  assets_manager_add(am, ui_play_list_play_list);
  assets_manager_add(am, style_default);
  assets_manager_add(am, style_equalizer);
  assets_manager_add(am, style_home_page);
  assets_manager_add(am, data_let_it_go_txt);
  assets_manager_add(am, data_see_you_again_txt);
  assets_manager_add(am, data_yellow_txt);
  assets_manager_add(am, image_advanced_n);
  assets_manager_add(am, image_bg_main);
  assets_manager_add(am, image_btn_round);
  assets_manager_add(am, image_b_anim_n);
  assets_manager_add(am, image_b_anim_p);
  assets_manager_add(am, image_close_n);
  assets_manager_add(am, image_eq_n);
  assets_manager_add(am, image_guage_pointer);
  assets_manager_add(am, image_icon_list);
  assets_manager_add(am, image_let_it_go);
  assets_manager_add(am, image_lrc_rule);
  assets_manager_add(am, image_mode_circleall_n);
  assets_manager_add(am, image_mode_circleone_n);
  assets_manager_add(am, image_mode_circlerand_n);
  assets_manager_add(am, image_play_list_n);
  assets_manager_add(am, image_play_n);
  assets_manager_add(am, image_play_next_n);
  assets_manager_add(am, image_play_pause_n);
  assets_manager_add(am, image_play_prev_n);
  assets_manager_add(am, image_play_prve_n);
  assets_manager_add(am, image_see_you_again);
  assets_manager_add(am, image_slider_drag);
  assets_manager_add(am, image_Vinyl);
  assets_manager_add(am, image_yellow);
#ifdef WITH_TRUETYPE_FONT
  assets_manager_add(am, font_default);
#else /*WITH_TRUETYPE_FONT*/
#endif /*WITH_TRUETYPE_FONT*/
#ifdef WITH_VGCANVAS
  assets_manager_add(am, image_pointer);
  assets_manager_add(am, image_round);
#endif /*WITH_VGCANVAS*/
#endif

  tk_init_assets();
  return RET_OK;
}
