#include "custom_widgets.h"

ret_t custom_widgets_init() {

  widget_factory_register(widget_factory(), WIDGET_TYPE_GESTURE,
                          gesture_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_GESTURE_RECEIVER,
                          gesture_receiver_create);

  widget_factory_register(widget_factory(), WIDGET_TYPE_WATCH_CLOCK,
                          watch_clock_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_WATCH_CLOCK_POINTER,
                          watch_clock_pointer_create);

  widget_factory_register(widget_factory(), WIDGET_TYPE_ROUND_MENU,
                          round_menu_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_COLOR_BUTTON,
                          color_button_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_CALENDAR_MONTH,
                          calendar_month_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_WINDOW_ANIM_MENU,
                          window_anim_menu_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_MUSIC_PLAY_BUTTON,
                          music_play_button_create);

  return RET_OK;
}
