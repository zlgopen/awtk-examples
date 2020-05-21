#ifndef CLOCK_POINT_TYPES_H
#define CLOCK_POINT_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 4)

typedef struct _clock_point_info_base_t {
  unsigned int p;
  unsigned int len;
} clock_point_info_base_t;

typedef struct _clock_point_info_t {
  clock_point_info_base_t base;
  unsigned int *data;
} clock_point_info_t;

typedef struct _clock_point_head_t {
  unsigned int len;
  unsigned int point;
  unsigned short width;
  unsigned short height;
} clock_point_head_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif // CLOCK_POINT_TYPES_H
