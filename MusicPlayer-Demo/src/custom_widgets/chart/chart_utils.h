/**
 * File:   chart_utils.h
 * Author: AWTK Develop Team
 * Brief:  chart utils
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

#ifndef TK_CHART_UTILS_H
#define TK_CHART_UTILS_H

#include "awtk.h"

BEGIN_C_DECLS

color_t chart_utils_value_color(const value_t* v);

typedef void (*chart_utils_on_object_parse_t)(void* ctx, const char* name, const value_t* v);
ret_t chart_utils_object_parse(chart_utils_on_object_parse_t on_parse, void* ctx,
                               const char* object);

END_C_DECLS

#endif /*TK_CHART_UTILS_H*/
