/**
 * File:   fifo.c
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

#include "fifo.h"
#include "tkc/utils.h"
#include "tkc/mem.h"

#define FIFO_IS_VALID(fifo, index) ((index) >= 0 && (index) < fifo->size)
#define FIFO_ABS_HEAD_INDEX(fifo) \
  ((fifo->cursor - fifo->size + 1 + fifo->capacity) % fifo->capacity)
#define FIFO_TO_ABS_INDEX(fifo, index) \
  ((fifo->cursor - fifo->size + 1 + fifo->capacity + (index)) % fifo->capacity)
#define FIFO_ABS_ELM(fifo, index) ((void*)(fifo->buffer + fifo->unit_size * (index)))
#define FIFO_TO_INDEX(fifo, index) (((index)-fifo->cursor + fifo->size - 1) % fifo->capacity)

static int fifo_compare(fifo_t* fifo, const void* a, const void* b) {
  if (fifo->compare) {
    return fifo->compare(a, b);
  } else {
    return memcmp(a, b, fifo->unit_size);
  }
}

fifo_t* fifo_create(uint32_t capacity, uint32_t unit_size, tk_destroy_t destroy,
                    tk_compare_t compare) {
  fifo_t* fifo = TKMEM_ZALLOC(fifo_t);
  return_value_if_fail(fifo != NULL, NULL);

  if (fifo_init(fifo, capacity, unit_size, destroy, compare)) {
    return fifo;
  } else {
    TKMEM_FREE(fifo);
    return NULL;
  }
}

fifo_t* fifo_init(fifo_t* fifo, uint32_t capacity, uint32_t unit_size, tk_destroy_t destroy,
                  tk_compare_t compare) {
  return_value_if_fail(fifo != NULL && capacity > 0 && unit_size > 0, NULL);

  if (fifo->buffer != NULL) {
    TKMEM_FREE(fifo->buffer);
  }

  fifo->buffer = TKMEM_ZALLOCN(uint8_t, (unit_size * capacity));
  return_value_if_fail(fifo->buffer != NULL, NULL);

  fifo->capacity = capacity;
  fifo->unit_size = unit_size;
  fifo->cursor = 0;
  fifo->size = 0;
  fifo->compare = compare;
  fifo->destroy = destroy;

  return fifo;
}

fifo_t* fifo_clone(fifo_t* fifo) {
  fifo_t* clone = NULL;
  return_value_if_fail(fifo != NULL, NULL);

  clone = fifo_create(fifo->capacity, fifo->unit_size, fifo->destroy, fifo->compare);
  if (clone) {
    if (fifo->size > 0) {
      if (fifo->size >= fifo->capacity) {
        memcpy(clone->buffer, fifo->buffer, fifo->unit_size * fifo->capacity);
      } else {
        uint32_t head_index = FIFO_ABS_HEAD_INDEX(fifo);
        uint8_t* head = FIFO_ABS_ELM(fifo, head_index);
        if (head_index + fifo->size <= fifo->capacity) {
          memcpy(clone->buffer, head, fifo->unit_size * fifo->size);
        } else {
          uint32_t part = fifo->capacity - head_index;
          memcpy(FIFO_ABS_ELM(clone, head_index), head, fifo->unit_size * part);
          memcpy(clone->buffer, fifo->buffer, fifo->unit_size * (fifo->size - part));
        }
      }
      clone->size = fifo->size;
      clone->cursor = fifo->cursor;
    }
  }

  return clone;
}

fifo_t* fifo_part_clone(fifo_t* fifo, uint32_t index, uint32_t nr) {
  fifo_t* clone = NULL;
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && nr > 0, NULL);
  return_value_if_fail(FIFO_IS_VALID(fifo, index), NULL);
  return_value_if_fail(FIFO_IS_VALID(fifo, index + nr - 1), NULL);

  clone = fifo_create(nr, fifo->unit_size, fifo->destroy, fifo->compare);
  if (clone) {
    fifo_get(fifo, index, nr, clone->buffer);
    clone->size = nr;
    clone->cursor = clone->cursor + nr - 1;
  }

  return clone;
}

void* fifo_at_abs(fifo_t* fifo, uint32_t index) {
  uint32_t i;
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && fifo->size > 0, NULL);
  i = FIFO_TO_INDEX(fifo, index);
  return_value_if_fail(FIFO_IS_VALID(fifo, i), NULL);
  return FIFO_ABS_ELM(fifo, index);
}

void* fifo_at(fifo_t* fifo, uint32_t index) {
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && fifo->size > 0, NULL);
  return_value_if_fail(FIFO_IS_VALID(fifo, index), NULL);
  return FIFO_ABS_ELM(fifo, FIFO_TO_ABS_INDEX(fifo, index));
}

static ret_t fifo_set_internal(fifo_t* fifo, uint32_t index, const void* data, uint32_t nr) {
  uint8_t* start = (uint8_t*)(data);
  uint8_t* elm = (uint8_t*)FIFO_ABS_ELM(fifo, index);

  if (nr > fifo->capacity) {
    if (start != NULL) {
      start += (nr - fifo->capacity) * fifo->unit_size;
    }
    nr = fifo->capacity;
  }

  if (index + nr <= fifo->capacity) {
    if (start != NULL) {
      memcpy(elm, start, fifo->unit_size * nr);
    } else {
      memset(elm, 0x00, fifo->unit_size * nr);
    }
  } else {
    uint32_t part = fifo->capacity - index;
    if (start != NULL) {
      memcpy(elm, start, fifo->unit_size * part);
      memcpy(fifo->buffer, start + fifo->unit_size * part, fifo->unit_size * (nr - part));
    } else {
      memset(elm, 0x00, fifo->unit_size * part);
      memset(fifo->buffer, 0x00, fifo->unit_size * (nr - part));
    }
  }

  return RET_OK;
}

ret_t fifo_set_abs(fifo_t* fifo, uint32_t index, const void* data, uint32_t nr) {
  uint32_t i;
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && data != NULL && nr > 0,
                       RET_BAD_PARAMS);
  i = FIFO_TO_INDEX(fifo, index);
  return_value_if_fail(FIFO_IS_VALID(fifo, i), RET_BAD_PARAMS);
  return_value_if_fail(FIFO_IS_VALID(fifo, i + nr - 1), RET_BAD_PARAMS);
  return fifo_set_internal(fifo, index, data, nr);
}

ret_t fifo_set(fifo_t* fifo, uint32_t index, const void* data, uint32_t nr) {
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && nr > 0, RET_BAD_PARAMS);
  return_value_if_fail(FIFO_IS_VALID(fifo, index), RET_BAD_PARAMS);
  return_value_if_fail(FIFO_IS_VALID(fifo, index + nr - 1), RET_BAD_PARAMS);
  return fifo_set_internal(fifo, FIFO_TO_ABS_INDEX(fifo, index), data, nr);
}

static ret_t fifo_get_internal(fifo_t* fifo, uint32_t index, uint32_t nr, uint8_t* buffer) {
  uint8_t* start = (uint8_t*)FIFO_ABS_ELM(fifo, index);
  if (index + nr <= fifo->capacity) {
    memcpy(buffer, start, fifo->unit_size * nr);
  } else {
    uint32_t part = fifo->capacity - index;
    memcpy(buffer, start, fifo->unit_size * part);
    memcpy(buffer + fifo->unit_size * part, fifo->buffer, fifo->unit_size * (nr - part));
  }

  return RET_OK;
}

ret_t fifo_get_abs(fifo_t* fifo, uint32_t index, uint32_t nr, uint8_t* buffer) {
  uint32_t i;
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && nr > 0 && buffer != NULL,
                       RET_BAD_PARAMS);
  i = FIFO_TO_INDEX(fifo, index);
  return_value_if_fail(FIFO_IS_VALID(fifo, i), RET_BAD_PARAMS);
  return_value_if_fail(FIFO_IS_VALID(fifo, i + nr - 1), RET_BAD_PARAMS);
  return fifo_get_internal(fifo, index, nr, buffer);
}

ret_t fifo_get(fifo_t* fifo, uint32_t index, uint32_t nr, uint8_t* buffer) {
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && nr > 0 && buffer != NULL,
                       RET_BAD_PARAMS);
  return_value_if_fail(FIFO_IS_VALID(fifo, index), RET_BAD_PARAMS);
  return_value_if_fail(FIFO_IS_VALID(fifo, index + nr - 1), RET_BAD_PARAMS);
  return fifo_get_internal(fifo, FIFO_TO_ABS_INDEX(fifo, index), nr, buffer);
}

void* fifo_prev(fifo_t* fifo, void* data) {
  int index = fifo_index_of(fifo, data);
  return_value_if_fail(index >= 0, NULL);
  return fifo_at(fifo, index - 1);
}

void* fifo_next(fifo_t* fifo, void* data) {
  int index = fifo_index_of(fifo, data);
  return_value_if_fail(index >= 0, NULL);
  return fifo_at(fifo, index + 1);
}

void* fifo_tail(fifo_t* fifo) {
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && fifo->size > 0, NULL);
  return FIFO_ABS_ELM(fifo, fifo->cursor);
}

void* fifo_head(fifo_t* fifo) {
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && fifo->size > 0, NULL);
  return FIFO_ABS_ELM(fifo, FIFO_ABS_HEAD_INDEX(fifo));
}

int fifo_index_of(fifo_t* fifo, void* data) {
  bool_t valid;
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && fifo->size > 0, -1);

  valid = ((uint8_t*)data - fifo->buffer) % fifo->unit_size == 0;
  return_value_if_fail(valid, -1);

  return FIFO_TO_INDEX(fifo, ((uint8_t*)data - fifo->buffer) / fifo->unit_size);
}

int32_t fifo_count(fifo_t* fifo, void* ctx) {
  uint32_t i = 0;
  int32_t nr = 0;
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && fifo->size > 0, -1);

  for (i = 0; i < fifo->size; i++) {
    void* iter = fifo_at(fifo, i);
    if (fifo_compare(fifo, iter, ctx) == 0) {
      nr++;
    }
  }
  return nr;
}

void* fifo_find(fifo_t* fifo, void* ctx) {
  uint32_t i = 0;
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && fifo->size > 0, NULL);

  for (i = 0; i < fifo->size; i++) {
    void* iter = fifo_at(fifo, i);
    if (fifo_compare(fifo, iter, ctx) == 0) {
      return iter;
    }
  }
  return NULL;
}

int fifo_find_index(fifo_t* fifo, void* ctx) {
  uint32_t i = 0;
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && fifo->size > 0, -1);

  for (i = 0; i < fifo->size; i++) {
    void* iter = fifo_at(fifo, i);
    if (fifo_compare(fifo, iter, ctx) == 0) {
      return i;
    }
  }

  return -1;
}

ret_t fifo_push(fifo_t* fifo, const void* data) {
  uint8_t* elm;
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL, RET_BAD_PARAMS);

  if (fifo->size != 0) {
    fifo->cursor = (fifo->cursor + 1) % fifo->capacity;
  }
  fifo->size = tk_min(fifo->size + 1, fifo->capacity);
  elm = (uint8_t*)FIFO_ABS_ELM(fifo, fifo->cursor);
  if (data != NULL) {
    memcpy(elm, data, fifo->unit_size);
  } else {
    memset(elm, 0x00, fifo->unit_size);
  }

  return RET_OK;
}

void* fifo_pop(fifo_t* fifo) {
  void* elm = fifo_head(fifo);
  return_value_if_fail(elm != NULL, NULL);
  fifo->size--;
  return elm;
}

ret_t fifo_npush(fifo_t* fifo, const void* data, uint32_t nr) {
  uint32_t head_index;
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && nr > 0, RET_BAD_PARAMS);

  if (fifo->size != 0) {
    fifo->cursor = (fifo->cursor + nr) % fifo->capacity;
  } else {
    fifo->cursor = (fifo->cursor + nr - 1) % fifo->capacity;
  }

  fifo->size = tk_min(fifo->size + nr, fifo->capacity);
  head_index = (fifo->cursor - tk_min(nr, fifo->capacity) + 1 + fifo->capacity) % fifo->capacity;

  return fifo_set_internal(fifo, head_index, data, nr);
}

ret_t fifo_npop(fifo_t* fifo, uint32_t nr) {
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && nr > 0 && nr <= fifo->size,
                       RET_BAD_PARAMS);
  fifo->size -= nr;
  return RET_OK;
}

ret_t fifo_clear(fifo_t* fifo) {
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL, RET_BAD_PARAMS);

  if (fifo->buffer != NULL && fifo->destroy != NULL) {
    uint32_t i = 0;
    for (i = 0; i < fifo->size; i++) {
      void* iter = fifo_at(fifo, i);
      fifo->destroy(iter);
    }
  }

  fifo->size = 0;
  fifo->cursor = 0;

  return RET_OK;
}

ret_t fifo_foreach(fifo_t* fifo, tk_visit_t visit, void* ctx) {
  uint32_t i = 0;
  return_value_if_fail(fifo != NULL && fifo->buffer != NULL && visit != NULL, RET_BAD_PARAMS);

  for (i = 0; i < fifo->size; i++) {
    void* iter = fifo_at(fifo, i);
    ret_t ret = visit(ctx, iter);
    if (ret != RET_OK) {
      return ret;
    }
  }
  return RET_OK;
}

ret_t fifo_deinit(fifo_t* fifo) {
  return_value_if_fail(fifo != NULL, RET_BAD_PARAMS);

  if (fifo->buffer != NULL) {
    fifo_clear(fifo);
    TKMEM_FREE(fifo->buffer);
    memset(fifo, 0x00, sizeof(fifo_t));
  }

  return RET_OK;
}

ret_t fifo_destroy(fifo_t* fifo) {
  return_value_if_fail(fifo != NULL, RET_BAD_PARAMS);

  fifo_deinit(fifo);
  TKMEM_FREE(fifo);

  return RET_OK;
}
