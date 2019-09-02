#include "awtk.h"
#include "base/assets_manager.h"
#ifndef WITH_FS_RES
#include "assets/inc/strings/en_US.data"
#include "assets/inc/strings/zh_CN.data"
#include "assets/inc/styles/default.data"
#ifdef WITH_STB_IMAGE
#include "assets/inc/images/bg.res"
#include "assets/inc/images/bg_480_272.res"
#include "assets/inc/images/pointer_big.res"
#include "assets/inc/images/pointer_small.res"
#else
#endif/*WITH_STB_IMAGE*/
#if defined(WITH_STB_FONT) || defined(WITH_FT_FONT)
#include "assets/inc/fonts/default.res"
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
  assets_manager_add(rm, image_bg);
  assets_manager_add(rm, image_bg_480_272);
  assets_manager_add(rm, image_pointer_big);
  assets_manager_add(rm, image_pointer_small);
  assets_manager_add(rm, style_default);
  assets_manager_add(rm, strings_en_US);
  assets_manager_add(rm, strings_zh_CN);
#endif

  tk_init_assets();
  return RET_OK;
}
