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
#include "chart/chart_view.h"
#include "chart/x_axis.h"
#include "chart/y_axis.h"
#include "chart/line_series.h"
#include "chart/bar_series.h"
#include "chart/tooltip.h"
#include "pie_slice/pie_slice.h"
#include "photo_frame/cover_view.h"

ret_t custom_widgets_init() {
  widget_factory_register(widget_factory(), WIDGET_TYPE_CHART_VIEW, chart_view_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_X_AXIS, x_axis_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_Y_AXIS, y_axis_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_LINE_SERIES, line_series_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_LINE_SERIES_COLORFUL,
                          line_series_colorful_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_BAR_SERIES, bar_series_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_BAR_SERIES_MINMAX,
                          bar_series_minmax_create);
  widget_factory_register(widget_factory(), WIDGET_TYPE_TOOLTIP, tooltip_create_default);
  widget_factory_register(widget_factory(), WIDGET_TYPE_PIE_SLICE, pie_slice_create);

  return RET_OK;
}
