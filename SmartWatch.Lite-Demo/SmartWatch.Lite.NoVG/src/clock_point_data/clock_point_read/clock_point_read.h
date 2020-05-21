#ifndef CLOCK_POINT_READ_H
#define CLOCK_POINT_READ_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned char *clock_point_next_image_data(const unsigned char *file_data);

unsigned char *clock_point_get_image_data(const unsigned char *file_data,
                                          int number, int *width, int *height,
                                          unsigned int *length);

unsigned char *clock_point_read_image_data(const unsigned char *file_data,
                                           unsigned int *point,
                                           unsigned int *length,
                                           unsigned int *image_data_length);

#ifdef __cplusplus
}
#endif

#endif // CLOCK_POINT_READ_H
