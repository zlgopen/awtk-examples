/**
 * File:   custom_widgets.h
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

#ifndef CUSTOM_WIDGETS_H
#define CUSTOM_WIDGETS_H

#include "awtk.h"

BEGIN_C_DECLS

/**
 * @method custom_widgets_init
 * 初始化自定义控件。
 * @annotation ["global", "scriptable"]
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t custom_widgets_init(void);

END_C_DECLS

#endif /*CUSTOM_WIDGETS_H*/
