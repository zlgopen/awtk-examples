#ifndef WEATHER_MAIN_H
#define WEATHER_MAIN_H

#include "awtk.h"
#include "../controller_base.h"

BEGIN_C_DECLS

typedef struct _weather_main_t {
  controller_base_t base;

  int32_t timer_id;
  widget_t *aqi_label;
  widget_t *now_min_temp_label;
  widget_t *now_max_temp_label;
  widget_t *tom_min_temp_label;
  widget_t *tom_max_temp_label;
  widget_t *weather_degree_label;
  widget_t *after_tom_min_temp_label;
  widget_t *after_tom_max_temp_label;

} weather_main_t;

weather_main_t *weather_main_init(widget_t *gesture_manager,
                                  widget_t *window_anim_menu);

ret_t weather_main_set_enable(weather_main_t *handle, bool_t enable);

END_C_DECLS

#endif /*WEATHER_MAIN_H*/
