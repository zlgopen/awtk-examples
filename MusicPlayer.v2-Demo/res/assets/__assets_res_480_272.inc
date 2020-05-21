#include "awtk.h"
#include "base/assets_manager.h"
#ifndef WITH_FS_RES
#include "res_480_272/inc/ui/advanced/advanced.data"
#include "res_480_272/inc/ui/equalizer/equalizer.data"
#include "res_480_272/inc/ui/home_page/home_page.data"
#include "res_480_272/inc/ui/play_list/play_list.data"
#include "res_480_272/inc/styles/default.data"
#include "res_480_272/inc/styles/equalizer.data"
#include "res_480_272/inc/styles/home_page.data"
#include "res_480_272/inc/styles/play_list.data"
#include "res_480_272/inc/data/let_it_go_txt.data"
#include "res_480_272/inc/data/see_you_again_txt.data"
#include "res_480_272/inc/data/yellow_txt.data"
#ifdef WITH_STB_IMAGE
#include "res_480_272/inc/images/advanced_n.res"
#include "res_480_272/inc/images/bg_main.res"
#include "res_480_272/inc/images/btn_round.res"
#include "res_480_272/inc/images/b_anim_n.res"
#include "res_480_272/inc/images/b_anim_p.res"
#include "res_480_272/inc/images/close_n.res"
#include "res_480_272/inc/images/eq_n.res"
#include "res_480_272/inc/images/guage_pointer.res"
#include "res_480_272/inc/images/icon_list.res"
#include "res_480_272/inc/images/let_it_go.res"
#include "res_480_272/inc/images/lrc_rule.res"
#include "res_480_272/inc/images/mode_circleall_n.res"
#include "res_480_272/inc/images/mode_circleone_n.res"
#include "res_480_272/inc/images/mode_circlerand_n.res"
#include "res_480_272/inc/images/play_list_n.res"
#include "res_480_272/inc/images/play_n.res"
#include "res_480_272/inc/images/play_next_n.res"
#include "res_480_272/inc/images/play_pause_n.res"
#include "res_480_272/inc/images/play_prve_n.res"
#include "res_480_272/inc/images/see_you_again.res"
#include "res_480_272/inc/images/slider_bg.res"
#include "res_480_272/inc/images/slider_drag.res"
#include "res_480_272/inc/images/slider_fg.res"
#include "res_480_272/inc/images/slider_v_bg.res"
#include "res_480_272/inc/images/slider_v_fg.res"
#include "res_480_272/inc/images/Vinyl.res"
#include "res_480_272/inc/images/yellow.res"
extern TK_CONST_DATA_ALIGN(const unsigned char image_play_prev_n[]);
#else
#endif /*WITH_STB_IMAGE*/
#ifdef WITH_VGCANVAS
#include "res_480_272/inc/images/pointer.bsvg"
#include "res_480_272/inc/images/round.bsvg"
#endif /*WITH_VGCANVAS*/
#if defined(WITH_TRUETYPE_FONT)
#include "res_480_272/inc/fonts/default.res"
#else /*WITH_TRUETYPE_FONT*/
#endif /*WITH_TRUETYPE_FONT*/
#endif /*WITH_FS_RES*/

ret_t assets_init_res_480_272(void) {
  assets_manager_t* am = assets_manager();

#ifdef WITH_FS_RES
  assets_manager_preload(am, ASSET_TYPE_FONT, "default");
  assets_manager_preload(am, ASSET_TYPE_STYLE, "default");
#else
  assets_manager_add(am, ui_advanced_advanced_res_480_272);
  assets_manager_add(am, ui_equalizer_equalizer_res_480_272);
  assets_manager_add(am, ui_home_page_home_page_res_480_272);
  assets_manager_add(am, ui_play_list_play_list_res_480_272);
  assets_manager_add(am, style_default_res_480_272);
  assets_manager_add(am, style_equalizer_res_480_272);
  assets_manager_add(am, style_home_page_res_480_272);
  assets_manager_add(am, style_play_list_res_480_272);
  assets_manager_add(am, data_let_it_go_txt_res_480_272);
  assets_manager_add(am, data_see_you_again_txt_res_480_272);
  assets_manager_add(am, data_yellow_txt_res_480_272);
  assets_manager_add(am, image_advanced_n_res_480_272);
  assets_manager_add(am, image_bg_main_res_480_272);
  assets_manager_add(am, image_btn_round_res_480_272);
  assets_manager_add(am, image_b_anim_n_res_480_272);
  assets_manager_add(am, image_b_anim_p_res_480_272);
  assets_manager_add(am, image_close_n_res_480_272);
  assets_manager_add(am, image_eq_n_res_480_272);
  assets_manager_add(am, image_guage_pointer_res_480_272);
  assets_manager_add(am, image_icon_list_res_480_272);
  assets_manager_add(am, image_let_it_go_res_480_272);
  assets_manager_add(am, image_lrc_rule_res_480_272);
  assets_manager_add(am, image_mode_circleall_n_res_480_272);
  assets_manager_add(am, image_mode_circleone_n_res_480_272);
  assets_manager_add(am, image_mode_circlerand_n_res_480_272);
  assets_manager_add(am, image_play_list_n_res_480_272);
  assets_manager_add(am, image_play_n_res_480_272);
  assets_manager_add(am, image_play_next_n_res_480_272);
  assets_manager_add(am, image_play_pause_n_res_480_272);
  assets_manager_add(am, image_play_prve_n_res_480_272);
  assets_manager_add(am, image_see_you_again_res_480_272);
  assets_manager_add(am, image_slider_bg_res_480_272);
  assets_manager_add(am, image_slider_drag_res_480_272);
  assets_manager_add(am, image_slider_fg_res_480_272);
  assets_manager_add(am, image_slider_v_bg_res_480_272);
  assets_manager_add(am, image_slider_v_fg_res_480_272);
  assets_manager_add(am, image_Vinyl_res_480_272);
  assets_manager_add(am, image_yellow_res_480_272);
  assets_manager_add(am, image_play_prev_n);
  assets_manager_add(am, font_default_res_480_272);
#ifdef WITH_VGCANVAS
  assets_manager_add(am, image_pointer_res_480_272);
  assets_manager_add(am, image_round_res_480_272);
#endif /*WITH_VGCANVAS*/
#endif

  tk_init_assets();
  return RET_OK;
}
