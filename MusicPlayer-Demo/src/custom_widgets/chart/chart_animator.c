/**
 * File:   chart_animator.h
 * Author: AWTK Develop Team
 * Brief:  animate widget by change its value
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
 * 2018-12-05 Xu ChaoZe <xuchaoze@zlg.cn> created
 *
 */

#include "chart_animator.h"

static ret_t chart_animator_update(widget_animator_t* animator, float_t percent) {
  chart_animator_t* chart = (chart_animator_t*)animator;
  return_value_if_fail(chart != NULL, RET_BAD_PARAMS);

  if (chart->on_update) {
    chart->on_update(chart, percent);
  }
  widget_invalidate(animator->widget, NULL);

  return RET_OK;
}

static ret_t chart_animator_destroy(widget_animator_t* animator) {
  chart_animator_t* chart = (chart_animator_t*)animator;
  if (chart->on_destroy) {
    chart->on_destroy(chart);
  }
  TKMEM_FREE(animator);
  return RET_OK;
}

widget_animator_t* chart_animator_create(widget_t* widget, uint32_t duration, uint32_t delay,
                                         easing_type_t easing) {
  widget_animator_t* animator = NULL;
  return_value_if_fail(widget != NULL && duration > 0, NULL);

  animator = (widget_animator_t*)TKMEM_ZALLOC(chart_animator_t);
  return_value_if_fail(
      widget_animator_init(animator, widget, duration, delay, easing_get(easing)) == RET_OK, NULL);
  animator->update = chart_animator_update;
  animator->destroy = chart_animator_destroy;

  return animator;
}
