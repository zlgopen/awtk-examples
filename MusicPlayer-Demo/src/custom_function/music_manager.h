#ifndef TK_MUSIC_MANAGER_H
#define TK_MUSIC_MANAGER_H

#include "awtk.h"

BEGIN_C_DECLS

void init_player(widget_t* win);

ret_t play_mode_choose(widget_t* win);

int32_t load_song(widget_t* win, int32_t song_iter, bool_t play_now);

void update_lrc(int32_t value, widget_t* win);

END_C_DECLS

#endif
