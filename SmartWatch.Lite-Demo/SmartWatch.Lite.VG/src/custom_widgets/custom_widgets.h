#ifndef CUSTOM_WIDGETS_H
#define CUSTOM_WIDGETS_H

#include "awtk.h"
#include "gesture/gesture.h"
#include "gesture/gesture_receiver.h"
#include "round_menu/round_menu.h"
#include "color_button/color_button.h"
#include "calendar_month/calendar_month.h"
#include "window_anim_menu/window_anim_menu.h"
#include "music_play_button/music_play_button.h"
#include "watch_clock/watch_clock.h"

BEGIN_C_DECLS

/**
 * @method custom_widgets_init
 * 初始化自定义控件。
 * @annotation ["global", "scriptable"]
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t custom_widgets_init(void);

END_C_DECLS

#endif /*CUSTOM_WIDGETS_H*/
