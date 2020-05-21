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
#include "../base/fifo.h"

BEGIN_C_DECLS

typedef ret_t (*chart_animator_update_t)(void* ctx, const void* from, const void* to,
                                         float_t percent, void* cur);

/**
 * @class chart_animator_t
 * chart动画。
 */
typedef struct _chart_animator_t {
  widget_animator_t base;

  void* from;
  void* to;
  void* cur;
  void* ctx;

  chart_animator_update_t update;
  tk_destroy_t destroy;
  uint8_t should_free_from : 1;
  uint8_t should_free_to : 1;
  uint8_t should_free_cur : 1;
} chart_animator_t;

/**
 * @method chart_animator_create
 * 创建动画对象。
 * @param {widget_t*} widget 控件对象。
 * @param {uint32_t} duration 动画持续时间。
 * @param {uint32_t} delay 动画执行时间。
 * @param {easing_type_t} easing 插值函数类型。
 * @param {chart_animator_update_t} update 值更新回调。
 * @param {tk_destroy_t} 值缓存销毁函数。
 * @param {bool_t} should_free_from 是否释放from数据。
 * @param {bool_t} should_free_to 是否释放to数据。
 * @param {bool_t} should_free_cur 是否释放cur数据。
 *
 * @return {widget_animator_t*} 成功返回动画对象，失败返回NULL。
 */
widget_animator_t* chart_animator_create(widget_t* widget, uint32_t duration, uint32_t delay,
                                         easing_type_t easing, chart_animator_update_t update,
                                         tk_destroy_t destroy, bool_t should_free_from,
                                         bool_t should_free_to, bool_t should_free_cur);

/**
 * @method chart_animator_fifo_float_value_create
 * 创建浮点型fifo的值动画对象。
 * @param {widget_t*} widget 控件对象。
 * @param {uint32_t} duration 动画持续时间。
 * @param {uint32_t} delay 动画执行时间。
 * @param {easing_type_t} easing 插值函数类型。
 *
 * @return {widget_animator_t*} 成功返回动画对象，失败返回NULL。
 */
widget_animator_t* chart_animator_fifo_float_value_create(widget_t* widget, uint32_t duration,
                                                          uint32_t delay, easing_type_t easing);

/**
 * @method chart_animator_fifo_float_value_create
 * 创建彩色型fifo的值动画对象。
 * @param {widget_t*} widget 控件对象。
 * @param {uint32_t} duration 动画持续时间。
 * @param {uint32_t} delay 动画执行时间。
 * @param {easing_type_t} easing 插值函数类型。
 *
 * @return {widget_animator_t*} 成功返回动画对象，失败返回NULL。
 */
widget_animator_t* chart_animator_fifo_colorful_value_create(widget_t* widget, uint32_t duration,
                                                             uint32_t delay, easing_type_t easing);

/**
 * @method chart_animator_fifo_float_value_create
 * 创建最大最小型fifo的值动画对象。
 * @param {widget_t*} widget 控件对象。
 * @param {uint32_t} duration 动画持续时间。
 * @param {uint32_t} delay 动画执行时间。
 * @param {easing_type_t} easing 插值函数类型。
 *
 * @return {widget_animator_t*} 成功返回动画对象，失败返回NULL。
 */
widget_animator_t* chart_animator_fifo_minmax_value_create(widget_t* widget, uint32_t duration,
                                                           uint32_t delay, easing_type_t easing);

/**
 * @method chart_animator_fifo_value_set_params
 * 设置fifo数据动画对象的参数。
 * @param {widget_animator_t*} animator 动画对象本身。
 * @param {fifo_t*} fifo fifo对象。
 * @param {uint32_t} index 需要更改的起始位置。
 * @param {const void*} data 新数据。
 * @param {uint32_t} nr 新数据的数量。
 * @param {uint32_t} update_max 同时更新的最大数量。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t chart_animator_fifo_value_set_params(widget_animator_t* animator, fifo_t* fifo,
                                           uint32_t index, const void* data, uint32_t nr,
                                           uint32_t update_max);

END_C_DECLS

#endif /*TK_CHART_ANIMATOR_H*/
