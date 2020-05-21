#include "clock_point_read.h"
#include "../clock_point_types.h"

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

unsigned char *clock_point_get_image_data(const unsigned char *file_data,
                                          int number, int *width, int *height,
                                          unsigned int *length) {
  uint8_t size = (uint8_t)*file_data;
  uint8_t *data = (uint8_t *)file_data;
  clock_point_head_t *clock_point_head = NULL;

  if ((uint32_t)size <= (uint32_t)number) {
    return NULL;
  }

  clock_point_head =
      (clock_point_head_t *)(file_data + sizeof(uint8_t) +
                             sizeof(clock_point_head_t) * number);

  *width = clock_point_head->width;
  *height = clock_point_head->height;
  *length = clock_point_head->len;

  return data + clock_point_head->point;
}

unsigned char *clock_point_read_image_data(const unsigned char *file_data,
                                           unsigned int *point,
                                           unsigned int *length,
                                           unsigned int *image_data_length) {
  uint8_t *data = (uint8_t *)file_data;
  clock_point_info_base_t *base = (clock_point_info_base_t *)file_data;
  *point = base->p;
  *length = base->len;
  *image_data_length =
      base->len * sizeof(uint32_t) + sizeof(clock_point_info_base_t);
  return data + sizeof(clock_point_info_base_t);
}

unsigned char *clock_point_next_image_data(const unsigned char *file_data) {
  uint8_t *data = (uint8_t *)file_data;
  clock_point_info_base_t *base = (clock_point_info_base_t *)file_data;

  return data + base->len * sizeof(uint32_t) + sizeof(clock_point_info_base_t);
}
