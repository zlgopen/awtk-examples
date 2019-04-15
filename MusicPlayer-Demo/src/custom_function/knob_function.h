/**
 * File:   knob_opera.c
 * Author: AWTK Develop Team
 * Brief:  knob function
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is dirich_text_nodeibuted in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */
/**
 * History:
 * ================================================================
 * 2019-02-20 ZhongWei1  created
 *
 */

#ifndef TK_KNOB_FUNCTION_H
#define TK_KNOB_FUNCTION_H

#include "awtk.h"

BEGIN_C_DECLS

ret_t knob_angle_change(widget_t* circle, widget_t* knob_pointer, point_t p);

ret_t knob_down_init(widget_t* knob_pointer, point_t p, char* wid_type);

END_C_DECLS

#endif
