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

#ifndef TK_CHART_ANIMATOR_H
#define TK_CHART_ANIMATOR_H

#include "awtk.h"

BEGIN_C_DECLS

struct _chart_animator_t;
typedef struct _chart_animator_t chart_animator_t;
typedef void (*chart_animator_on_update_t)(chart_animator_t* animator, float_t percent);
typedef void (*chart_animator_on_destroy_t)(chart_animator_t* animator);

/**
 * @class chart_animator_t
 * 图表动画。
 */
struct _chart_animator_t {
  widget_animator_t base;

  value_t to;
  value_t from;

  void* ctx;
  chart_animator_on_update_t on_update;
  chart_animator_on_destroy_t on_destroy;
};

/**
 * @method chart_animator_create
 * 创建动画对象。
 * @param {widget_t*} widget 控件对象。
 * @param {uint32_t} duration 动画持续时间。
 * @param {uint32_t} delay 动画执行时间。
 * @param {easing_type_t} easing 插值函数类型。
 *
 * @return {widget_animator_t*} 成功返回动画对象，失败返回NULL。
 */
widget_animator_t* chart_animator_create(widget_t* widget, uint32_t duration, uint32_t delay,
                                         easing_type_t easing);

END_C_DECLS

#endif /*TK_CHART_ANIMATOR_H*/
