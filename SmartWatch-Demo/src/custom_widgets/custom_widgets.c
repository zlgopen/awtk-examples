/**
 * File:   custom_widgets.c
 * Author: AWTK Develop Team
 * Brief:  custom widgets
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2018-12-11 Xu ChaoZe <xuchaoze@zlg.cn> created
 *
 */

#include "custom_widgets.h"
#include "health_circle.h"
#include "hour_weather.h"
#include "gesture.h"
#include "guage_hour_pointer.h"
#include "guage_minute_pointer.h"
#include "guage_second_pointer.h"
#include "bar_graph.h"

#include "slide_menu_v.h"
#include "../common/log.h"

ret_t custom_widgets_init() {
  widget_factory_register(widget_factory(), WIDGET_TYPE_HEALTH_CIRCLE, health_circle_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_HOUR_WEATHER, hour_weather_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_GESTURE, gesture_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_GUAGE_HOUR_POINTER,
                          guage_hour_pointer_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_GUAGE_MINUTE_POINTER,
                          guage_minute_pointer_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_GUAGE_SECOND_POINTER,
                          guage_second_pointer_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_BAR_GRAPH, bar_graph_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_SLIDE_MENU_V, slide_menu_v_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_SLIDE_VIEW, slide_view_create);

  return RET_OK;
}
