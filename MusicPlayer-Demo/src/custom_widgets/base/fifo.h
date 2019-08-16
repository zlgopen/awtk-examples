/**
 * File:   fifo.h
 * Author: AWTK Develop Team
 * Brief:  fifo.
 *
 * Copyright (c) 2018 - 2019  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2019-06-06 Xu ChaoZe <xuchaoze@zlg.cn> created
 *
 */

#ifndef TK_FIFO_H
#define TK_FIFO_H

#include "tkc/types_def.h"

BEGIN_C_DECLS

/**
 * @class fifo_t
 * FIFO，先进先出队列，环形缓存。
 *
 * 用fifo\_init初始化时，用fifo\_deinit释放。如：
 *
 * ```c
 * fifo_t fifo;
 * fifo_init(&fifo, 10, 10, compare);
 * ...
 * fifo_deinit(&fifo);
 * ```
 *
 * 用fifo\_create创建时，用fifo\_destroy销毁。如：
 *
 * ```c
 * fifo_t* fifo = fifo_create(10, 10, compare);
 * ...
 * fifo_destroy(fifo);
 * ```
 *
 */
typedef struct _fifo_t {
  /**
   * @property {uint32_t} capacity
   * @annotation ["readable"]
   * FIFO的容量大小。
   */
  uint32_t capacity;
  /**
   * @property {uint32_t} size
   * @annotation ["readable"]
   * FIFO中元素的个数。
   */
  uint32_t size;
  /**
   * @property {uint32_t} cursor
   * @annotation ["readable"]
   * FIFO中最后一个元素的索引。
   */
  uint32_t cursor;
  /**
   * @property {uint32_t} unit_size
   * @annotation ["readable"]
   * FIFO中单个元素的大小。
   */
  uint32_t unit_size;
  /**
   * @property {uint8_t*} buffer
   * @annotation ["readable"]
   * FIFO中的数据缓存。
   */
  uint8_t* buffer;
  /**
   * @property {tk_destroy_t} destroy
   * @annotation ["readable"]
   * 元素销毁函数。
   */
  tk_destroy_t destroy;
  /**
   * @property {tk_compare_t} compare
   * @annotation ["readable"]
   * 元素过滤函数。
   */
  tk_compare_t compare;
} fifo_t;

/**
 * @method fifo_create
 * @annotation ["constructor"]
 * 创建fifo对象。
 *
 * @param {uint32_t} capacity FIFO的初始容量。
 * @param {uint32_t} unit_size FIFO中单个元素的大小。
 * @param {tk_destroy_t} destroy 元素销毁函数。
 * @param {tk_compare_t} compare 元素比较函数。
 *
 * @return {fifo_t*} FIFO对象。
 */
fifo_t* fifo_create(uint32_t capacity, uint32_t unit_size, tk_destroy_t destroy,
                    tk_compare_t compare);

/**
 * @method fifo_init
 * 初始化fifo对象。
 *
 * @param {fifo_t*} fifo FIFO对象。
 * @param {uint32_t} capacity FIFO的初始容量。
 * @param {uint32_t} unit_size FIFO中单个元素的大小。
 * @param {tk_destroy_t} destroy 元素销毁函数。
 * @param {tk_compare_t} compare 元素比较函数。
 *
 * @return {fifo_t*} FIFO对象。
 */
fifo_t* fifo_init(fifo_t* fifo, uint32_t capacity, uint32_t unit_size, tk_destroy_t destroy,
                  tk_compare_t compare);

/**
 * @method fifo_clone
 * clone。
 *
 * @param {fifo_t*} fifo FIFO对象。
 *
 * @return {fifo_t*} 返回clone的对象。
 */
fifo_t* fifo_clone(fifo_t* fifo);

/**
 * @method fifo_clone
 * clone部分。
 *
 * @param {fifo_t*} fifo FIFO对象。
 * @param {fifo_t*} index 被clone元素在FIFO中的位置。
 * @param {fifo_t*} nr 被clone元素的数量。
 *
 * @return {fifo_t*} 返回clone的对象。
 */
fifo_t* fifo_part_clone(fifo_t* fifo, uint32_t index, uint32_t nr);

/**
 * @method fifo_at_abs
 * 返回特定（缓存）位置的元素。
 * @param {fifo_t*} fifo FIFO对象。
 * @param {uint32_t} index 元素在缓存区的位置。
 *
 * @return {void*} 如果找到，返回特定位置的元素，否则返回NULL。
 */
void* fifo_at_abs(fifo_t* fifo, uint32_t index);

/**
 * @method fifo_at
 * 返回特定位置的元素。
 * @param {fifo_t*} fifo FIFO对象。
 * @param {uint32_t} index 元素在FIFO中的位置。
 *
 * @return {void*} 如果找到，返回特定位置的元素，否则返回NULL。
 */
void* fifo_at(fifo_t* fifo, uint32_t index);

/**
 * @method fifo_set_abs
 * 设置特定（缓存）位置开始的多个元素。
 * @param {fifo_t*} fifo FIFO对象。
 * @param {uint32_t} index 元素在缓存区的位置。
 * @param {const void*} data 元素数据。
 * @param {uint32_t} nr 元素数量。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t fifo_set_abs(fifo_t* fifo, uint32_t index, const void* data, uint32_t nr);

/**
 * @method fifo_set
 * 设置特定位置开始的多个元素。
 * @param {fifo_t*} fifo FIFO对象。
 * @param {uint32_t} index 元素在FIFO中的位置。
 * @param {const void*} data 元素数据。
 * @param {uint32_t} nr 元素数量。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t fifo_set(fifo_t* fifo, uint32_t index, const void* data, uint32_t nr);

/**
 * @method fifo_at
 * 返回特定（缓存）位置开始的多个元素。
 * @param {fifo_t*} fifo FIFO对象。
 * @param {uint32_t} index 元素在缓存区的位置。
 * @param {uint32_t} nr 元素数量。
 * @param {uint8_t*} buffer 返回元素的缓存。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t fifo_get_abs(fifo_t* fifo, uint32_t index, uint32_t nr, uint8_t* buffer);

/**
 * @method fifo_get
 * 返回特定位置开始的多个元素。
 * @param {fifo_t*} fifo FIFO对象。
 * @param {uint32_t} index 元素在FIFO中的位置。
 * @param {uint32_t} nr 元素数量。
 * @param {uint8_t*} buffer 返回元素的缓存。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t fifo_get(fifo_t* fifo, uint32_t index, uint32_t nr, uint8_t* buffer);

/**
 * @method fifo_prev
 * 返回上一个元素。
 * @param {fifo_t*} fifo FIFO对象。
 * @param {void*} data 当前元素。
 *
 * @return {void*} 如果找到，返回上一个元素，否则返回NULL。
 */
void* fifo_prev(fifo_t* fifo, void* data);

/**
 * @method fifo_next
 * 返回下一个元素。
 * @param {fifo_t*} fifo FIFO对象。
 * @param {void*} data 当前元素。
 *
 * @return {void*} 如果找到，返回下一个元素，否则返回NULL。
 */
void* fifo_next(fifo_t* fifo, void* data);

/**
 * @method fifo_tail
 * 返回最后一个元素。
 * @param {fifo_t*} fifo FIFO对象。
 *
 * @return {void*} 成功返回最后一个元素，失败返回NULL。
 */
void* fifo_tail(fifo_t* fifo);

/**
 * @method fifo_head
 * 返回第一个元素。
 * @param {fifo_t*} fifo FIFO对象。
 *
 * @return {void*} 成功返回最后一个元素，失败返回NULL。
 */
void* fifo_head(fifo_t* fifo);

/**
 * @method fifo_index_of
 * 返回元素的位置。
 * @param {fifo_t*} fifo FIFO对象。
 * @param {void*} data 元素。
 *
 * @return {int} 如果找到，返回元素的位置，否则返回-1。
 */
int fifo_index_of(fifo_t* fifo, void* data);

/**
 * @method fifo_count
 * 返回满足条件元素的个数。
 * @param {fifo_t*} fifo 单向链表对象。
 * @param {void*} ctx 比较函数的上下文。
 *
 * @return {int32_t} 返回元素个数。
 */
int32_t fifo_count(fifo_t* fifo, void* ctx);

/**
 * @method fifo_find
 * 查找第一个满足条件的元素。
 * @param {fifo_t*} fifo FIFO对象。
 * @param {void*} ctx 比较函数的上下文。
 *
 * @return {void*} 如果找到，返回满足条件的对象，否则返回NULL。
 */
void* fifo_find(fifo_t* fifo, void* ctx);

/**
 * @method fifo_find_index
 * 查找第一个满足条件的元素，并返回位置。
 * @param {fifo_t*} fifo FIFO对象。
 * @param {void*} ctx 比较函数的上下文。
 *
 * @return {int} 如果找到，返回满足条件的对象的位置，否则返回-1。
 */
int fifo_find_index(fifo_t* fifo, void* ctx);

/**
 * @method fifo_push
 * 在尾巴追加一个元素。
 * @param {fifo_t*} fifo FIFO对象。
 * @param {const void*} data 待追加的元素。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t fifo_push(fifo_t* fifo, const void* data);

/**
 * @method fifo_pop
 * 弹出第一个元素。
 * @param {fifo_t*} fifo FIFO对象。
 *
 * @return {void*} 成功返回第一个元素，失败返回NULL。
 */
void* fifo_pop(fifo_t* fifo);

/**
 * @method fifo_npush
 * 在尾巴追加多个元素。
 * @param {fifo_t*} fifo FIFO对象。
 * @param {const void*} data 待追加的元素。
 * @param {uint32_t} nr 待追加的元素个数。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t fifo_npush(fifo_t* fifo, const void* data, uint32_t nr);

/**
 * @method fifo_npop
 * 弹出开头多个元素。
 * @param {fifo_t*} fifo FIFO对象。
 * @param {uint32_t} nr 待弹出的元素个数。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t fifo_npop(fifo_t* fifo, uint32_t nr);

/**
 * @method fifo_clear
 * 清除全部元素。
 * @param {fifo_t*} fifo FIFO对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t fifo_clear(fifo_t* fifo);

/**
 * @method fifo_foreach
 * 遍历元素。
 * @param {fifo_t*} fifo FIFO对象。
 * @param {tk_visit_t} visit 遍历函数。
 * @param {void*} ctx 遍历函数的上下文。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t fifo_foreach(fifo_t* fifo, tk_visit_t visit, void* ctx);

/**
 * @method fifo_deinit
 * 清除全部元素，并释放elms。
 * @param {fifo_t*} fifo FIFO对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t fifo_deinit(fifo_t* fifo);

/**
 * @method fifo_destroy
 * 销毁fifo对象。
 * @param {fifo_t*} fifo FIFO对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t fifo_destroy(fifo_t* fifo);

END_C_DECLS

#endif /*TK_FIFO_H*/
