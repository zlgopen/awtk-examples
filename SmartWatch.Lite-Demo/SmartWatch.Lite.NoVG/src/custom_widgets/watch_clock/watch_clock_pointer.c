#include "tkc/mem.h"
#include "base/timer.h"
#include "tkc/utils.h"
#include "watch_clock_pointer.h"
#include "base/widget_vtable.h"
#include "../../clock_point_data/clock_point_read/clock_point_read.h"

#define ARRAY_LEN(a) sizeof(a) / sizeof(a[0])

typedef enum _quadrant_t {
  QUADRANT_FIRST_F = 0, //二维坐标第一象限的上区域
  QUADRANT_FIRST_S,     //二维坐标第一象限的下区域

  QUADRANT_SECOND_F, //二维坐标第二象限的上区域
  QUADRANT_SECOND_S, //二维坐标第二象限的下区域

  QUADRANT_THIRD_F, //二维坐标第三象限的上区域
  QUADRANT_THIRD_S, //二维坐标第三象限的下区域

  QUADRANT_FOURTH_F, //二维坐标第四象限的上区域
  QUADRANT_FOURTH_S, //二维坐标第四象限的下区域
} quadrant_t;

typedef struct _png_bitmap_t {
  wh_t w;
  wh_t h;
  uint8_t *data;
  uint32_t data_len;
} png_bitmap_t;

typedef void (*get_quadrant_point_t)(bitmap_t *new_bitmap, png_bitmap_t *bitmap,
                                     int32_t x, int32_t y, int32_t *new_x,
                                     int32_t *new_y);

static bool_t watch_clock_pointer_get_image_w_and_h(quadrant_t type, int32_t w,
                                                    int32_t h, int32_t *new_w,
                                                    int32_t *new_h) {

  switch (type) {
  case QUADRANT_FIRST_F:
  case QUADRANT_SECOND_F:
  case QUADRANT_THIRD_S:
  case QUADRANT_FOURTH_S:
    *new_w = w;
    *new_h = h;
    break;
  case QUADRANT_FIRST_S:
  case QUADRANT_SECOND_S:
  case QUADRANT_THIRD_F:
  case QUADRANT_FOURTH_F:
    *new_w = h;
    *new_h = w;
    break;
  default:
    return FALSE;
  }

  return TRUE;
}

static quadrant_t watch_clock_pointer_get_quadrant(uint8_t value,
                                                   uint8_t *image_number) {
  if (value < 8) {
    *image_number = value;
    return QUADRANT_FIRST_F;
  } else if (8 <= value && value < 15) {
    *image_number = 15 - value;
    return QUADRANT_FIRST_S;
  } else if (15 <= value && value < 23) {
    *image_number = value - 15;
    return QUADRANT_FOURTH_F;
  } else if (23 <= value && value < 30) {
    *image_number = 30 - value;
    return QUADRANT_FOURTH_S;
  } else if (30 <= value && value < 38) {
    *image_number = value - 30;
    return QUADRANT_THIRD_S;
  } else if (38 <= value && value < 45) {
    *image_number = 45 - value;
    return QUADRANT_THIRD_F;
  } else if (45 <= value && value < 53) {
    *image_number = value - 45;
    return QUADRANT_SECOND_S;
  } else {
    *image_number = 60 - value;
    return QUADRANT_SECOND_F;
  }
}

static ret_t watch_clock_set_pointer_image(widget_t *widget,
                                           const char *image_name) {
  char *name = NULL;
  watch_clock_pointer_t *watch_clock_pointer = WATCH_CLOCK_POINTER(widget);
  return_value_if_fail(watch_clock_pointer != NULL && image_name != NULL,
                       RET_BAD_PARAMS);

  watch_clock_pointer->pointer_image = tk_str_copy(name, image_name);

  return RET_OK;
}

static ret_t watch_clock_pointer_get_prop(widget_t *widget, const char *name,
                                          value_t *v) {
  watch_clock_pointer_t *watch_clock_pointer = WATCH_CLOCK_POINTER(widget);
  return_value_if_fail(watch_clock_pointer != NULL && widget != NULL,
                       RET_BAD_PARAMS);

  if (tk_str_eq(name, WATCH_CLOCK_POINTER_PROP_IMAGE)) {
    value_set_str(v, watch_clock_pointer->pointer_image);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_POINTER_PROP_ANCHOR_X)) {
    value_set_int32(v, watch_clock_pointer->anchor_x);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_POINTER_PROP_ANCHOR_L_Y)) {
    value_set_int32(v, watch_clock_pointer->anchor_y);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_POINTER_PROP_CLOCK_POINTER_TYPE)) {
    value_set_str(
        v, watch_clock_pointer_type_string_list[(
               uint32_t)watch_clock_pointer_get_clock_pointer_type(widget)]);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t watch_clock_pointer_set_prop(widget_t *widget, const char *name,
                                          const value_t *v) {
  watch_clock_pointer_t *watch_clock_pointer = WATCH_CLOCK_POINTER(widget);
  return_value_if_fail(watch_clock_pointer != NULL && widget != NULL,
                       RET_BAD_PARAMS);

  if (tk_str_eq(name, WATCH_CLOCK_POINTER_PROP_IMAGE)) {
    return watch_clock_set_pointer_image(widget, value_str(v));
  } else if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    watch_clock_pointer_set_value(widget, (uint8_t)value_uint32(v));
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_POINTER_PROP_ANCHOR_X)) {
    watch_clock_pointer->anchor_x = value_int32(v);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_POINTER_PROP_ANCHOR_L_Y)) {
    watch_clock_pointer->anchor_y = value_int32(v);
    return RET_OK;
  } else if (tk_str_eq(name, WATCH_CLOCK_POINTER_PROP_CLOCK_POINTER_TYPE)) {
    uint32_t i = 0;
    const char *tmp = value_str(v);
    return_value_if_fail(tmp != NULL, RET_BAD_PARAMS);
    for (; i < ARRAY_LEN(watch_clock_pointer_type_string_list); i++) {
      watch_clock_pointer_type_t type = (watch_clock_pointer_type_t)i;
      if (tk_str_cmp(watch_clock_pointer_type_string_list[i], tmp) == 0) {
        return watch_clock_pointer_set_clock_pointer_type(widget, type);
      }
    }
    return RET_BAD_PARAMS;
  }
  return RET_NOT_FOUND;
}

static inline void watch_clock_pointer_set_point_image_data_commond(
    bitmap_t *new_bitmap, png_bitmap_t *bitmap, get_quadrant_point_t fun,
    int32_t *px, int32_t *py) {
  int32_t x = 0;
  int32_t y = 0;
  int32_t new_x = 0;
  int32_t new_y = 0;
  int32_t temp_px = *px;
  int32_t temp_py = *py;
  uint8_t *dst_p = NULL;
  uint32_t i = 0;
  uint32_t bpp = 4;
  uint32_t image_point = 0;
  uint32_t image_info_len = 0;
  uint32_t image_data_len = 0;
  uint32_t tmp_image_data_len = 0;
  uint8_t *data = bitmap->data;
  uint8_t *dst_data = bitmap_lock_buffer_for_read(new_bitmap);
  uint8_t *image_data = clock_point_read_image_data(
      bitmap->data, &image_point, &image_info_len, &tmp_image_data_len);
  image_data_len += tmp_image_data_len;

  tk_memset32((uint32_t *)dst_data, 0x0, new_bitmap->w * new_bitmap->h);
  for (y = 0; y < bitmap->h; y++) {
    for (x = 0; x < bitmap->w; x++) {
      uint32_t p = x + y * bitmap->w;
      uint32_t l = image_point + image_info_len;

      if (image_point == p) {
        i = 0;
      }
      if (image_point <= p && p < l) {
        fun(new_bitmap, bitmap, x, y, &new_x, &new_y);

        dst_p = dst_data + (new_x * bpp + new_y * new_bitmap->line_length);

        *(uint32_t *)dst_p = *(uint32_t *)(((uint32_t *)image_data) + i);
        i++;
      } else if (p == l) {
        data = clock_point_next_image_data(data);
        image_data = clock_point_read_image_data(
            data, &image_point, &image_info_len, &tmp_image_data_len);
        image_data_len += tmp_image_data_len;
        if (image_data_len >= bitmap->data_len) {
          break;
        }
      }
    }
    if (image_data_len >= bitmap->data_len) {
      break;
    }
  }

  fun(new_bitmap, bitmap, temp_px, temp_py, px, py);

  bitmap_unlock_buffer(new_bitmap);
}

static void watch_clock_pointer_get_quadrant_point_first_f(bitmap_t *new_bitmap,
                                                           png_bitmap_t *bitmap,
                                                           int32_t x, int32_t y,
                                                           int32_t *new_x,
                                                           int32_t *new_y) {
  *new_x = x;
  *new_y = y;
}

static void watch_clock_pointer_get_quadrant_point_first_s(bitmap_t *new_bitmap,
                                                           png_bitmap_t *bitmap,
                                                           int32_t x, int32_t y,
                                                           int32_t *new_x,
                                                           int32_t *new_y) {
  *new_x = bitmap->h - 1 - y;
  *new_y = x;
  *new_y = tk_abs(new_bitmap->h - 1 - *new_y);
}

static void watch_clock_pointer_get_quadrant_point_fourth_f(
    bitmap_t *new_bitmap, png_bitmap_t *bitmap, int32_t x, int32_t y,
    int32_t *new_x, int32_t *new_y) {
  *new_x = bitmap->h - 1 - y;
  *new_y = x;
  *new_y = tk_abs(new_bitmap->h - 1 - *new_y);
  *new_y = new_bitmap->h - *new_y - 1;
}

static void watch_clock_pointer_get_quadrant_point_fourth_s(
    bitmap_t *new_bitmap, png_bitmap_t *bitmap, int32_t x, int32_t y,
    int32_t *new_x, int32_t *new_y) {
  *new_x = x;
  *new_y = bitmap->h - 1 - y;
}

static void watch_clock_pointer_get_quadrant_point_second_f(
    bitmap_t *new_bitmap, png_bitmap_t *bitmap, int32_t x, int32_t y,
    int32_t *new_x, int32_t *new_y) {
  *new_x = bitmap->w - 1 - x;
  *new_y = y;
}

static void watch_clock_pointer_get_quadrant_point_second_s(
    bitmap_t *new_bitmap, png_bitmap_t *bitmap, int32_t x, int32_t y,
    int32_t *new_x, int32_t *new_y) {
  *new_x = bitmap->h - 1 - y;
  *new_y = x;
  *new_x = tk_abs(new_bitmap->w - 1 - *new_x);
  *new_y = tk_abs(new_bitmap->h - 1 - *new_y);
}

static void watch_clock_pointer_get_quadrant_point_third_f(bitmap_t *new_bitmap,
                                                           png_bitmap_t *bitmap,
                                                           int32_t x, int32_t y,
                                                           int32_t *new_x,
                                                           int32_t *new_y) {
  *new_x = y;
  *new_y = x;
}

static void watch_clock_pointer_get_quadrant_point_third_s(bitmap_t *new_bitmap,
                                                           png_bitmap_t *bitmap,
                                                           int32_t x, int32_t y,
                                                           int32_t *new_x,
                                                           int32_t *new_y) {
  *new_x = bitmap->w - 1 - x;
  *new_y = bitmap->h - 1 - y;
}

static void watch_clock_pointer_set_point_image_data(
    watch_clock_pointer_t *watch_clock_pointer, quadrant_t quadrant_type,
    bitmap_t *new_bitmap, png_bitmap_t *bitmap, int32_t *px, int32_t *py) {
  *px = watch_clock_pointer->anchor_x;
  *py = bitmap->h - watch_clock_pointer->anchor_y;
  switch (quadrant_type) {
  case QUADRANT_FIRST_F:
    watch_clock_pointer_set_point_image_data_commond(
        new_bitmap, bitmap, watch_clock_pointer_get_quadrant_point_first_f, px,
        py);
    break;
  case QUADRANT_FIRST_S:
    watch_clock_pointer_set_point_image_data_commond(
        new_bitmap, bitmap, watch_clock_pointer_get_quadrant_point_first_s, px,
        py);
    *px = *px + 1;
    *py = *py + 1;
    break;
  case QUADRANT_SECOND_F:
    watch_clock_pointer_set_point_image_data_commond(
        new_bitmap, bitmap, watch_clock_pointer_get_quadrant_point_second_f, px,
        py);
    *px = *px + 2;
    break;
  case QUADRANT_SECOND_S:
    watch_clock_pointer_set_point_image_data_commond(
        new_bitmap, bitmap, watch_clock_pointer_get_quadrant_point_second_s, px,
        py);
    *px = *px + 1;
    *py = *py + 1;
    break;
  case QUADRANT_THIRD_F:
    watch_clock_pointer_set_point_image_data_commond(
        new_bitmap, bitmap, watch_clock_pointer_get_quadrant_point_third_f, px,
        py);
    *px = *px + 1;
    *py = *py - 1;
    break;
  case QUADRANT_THIRD_S:
    watch_clock_pointer_set_point_image_data_commond(
        new_bitmap, bitmap, watch_clock_pointer_get_quadrant_point_third_s, px,
        py);
    *px = *px + 2;
    break;
  case QUADRANT_FOURTH_F:
    watch_clock_pointer_set_point_image_data_commond(
        new_bitmap, bitmap, watch_clock_pointer_get_quadrant_point_fourth_f, px,
        py);
    *px = *px + 1;
    *py = *py - 1;
    break;
  case QUADRANT_FOURTH_S:
    watch_clock_pointer_set_point_image_data_commond(
        new_bitmap, bitmap, watch_clock_pointer_get_quadrant_point_fourth_s, px,
        py);
    *py = *py - 1;
    break;
  }
}

static ret_t watch_clock_pointer_on_paint_self(widget_t *widget, canvas_t *c) {
  rect_t src_r;
  rect_t dst_r;
  int32_t w = 0;
  int32_t h = 0;
  int32_t px = 0;
  int32_t py = 0;
  uint32_t image_len = 0;
  quadrant_t quadrant_type;
  uint8_t image_number = 0;
  uint8_t *bitmap_data = NULL;
  png_bitmap_t png_bitmap;
  asset_info_t *asset_image_info = NULL;
  watch_clock_pointer_t *watch_clock_pointer = WATCH_CLOCK_POINTER(widget);

  return_value_if_fail(watch_clock_pointer != NULL && widget != NULL,
                       RET_BAD_PARAMS);
  if (watch_clock_pointer->asset_image_info == NULL) {
    asset_image_info = (asset_info_t *)assets_manager_ref(
        assets_manager(), ASSET_TYPE_DATA, watch_clock_pointer->pointer_image);
    watch_clock_pointer->asset_image_info = asset_image_info;
  } else {
    asset_image_info = watch_clock_pointer->asset_image_info;
  }

  return_value_if_fail(asset_image_info != NULL, RET_BAD_PARAMS);

  if (watch_clock_pointer->point_image_data == NULL &&
      !watch_clock_pointer->is_oom) {
    int32_t tmp_w = 0;
    int32_t tmp_h = 0;
    clock_point_get_image_data(asset_image_info->data, 7, &tmp_w, &tmp_h,
                               &image_len);
    watch_clock_pointer->point_image_data =
        (uint8_t *)TKMEM_CALLOC(1, (tmp_w * tmp_h * 4));

    if (watch_clock_pointer->point_image_data == NULL) {
      watch_clock_pointer->is_oom = TRUE;
      return RET_OOM;
    }
  }

  quadrant_type = watch_clock_pointer_get_quadrant(watch_clock_pointer->value,
                                                   &image_number);

  bitmap_data =
      clock_point_get_image_data(asset_image_info->data, image_number,
                                 &(png_bitmap.w), &(png_bitmap.h), &image_len);

  if (bitmap_data != NULL) {
    png_bitmap.data = bitmap_data;
    png_bitmap.data_len = image_len;
    if (watch_clock_pointer_get_image_w_and_h(quadrant_type, png_bitmap.w,
                                              png_bitmap.h, &w, &h)) {
      if (watch_clock_pointer->bitmap.buffer == NULL) {
        bitmap_init(&watch_clock_pointer->bitmap, w, h, BITMAP_FMT_RGBA8888,
                    watch_clock_pointer->point_image_data);
      }
      if (watch_clock_pointer->bitmap.buffer != NULL) {
        watch_clock_pointer->bitmap.w = w;
        watch_clock_pointer->bitmap.h = h;
        bitmap_set_line_length(&watch_clock_pointer->bitmap, 0);
        graphic_buffer_attach(watch_clock_pointer->bitmap.buffer, watch_clock_pointer->point_image_data, w, h);

        watch_clock_pointer_set_point_image_data(
            watch_clock_pointer, quadrant_type, &watch_clock_pointer->bitmap,
            &png_bitmap, &px, &py);

        watch_clock_pointer->bitmap.image_manager = image_manager();
        src_r = rect_init(0, 0, w, h);
        dst_r = rect_init(widget->w / 2 - px, widget->h / 2 - py, w, h);

        canvas_draw_image(c, &watch_clock_pointer->bitmap, &src_r, &dst_r);
      }
    }
  }

  return RET_OK;
}

static ret_t watch_clock_pointer_on_destroy(widget_t *widget) {
  watch_clock_pointer_t *watch_clock_pointer = WATCH_CLOCK_POINTER(widget);
  return_value_if_fail(widget != NULL && watch_clock_pointer != NULL,
                       RET_BAD_PARAMS);

  if (watch_clock_pointer->asset_image_info != NULL) {
    asset_info_unref(watch_clock_pointer->asset_image_info);
    watch_clock_pointer->asset_image_info = NULL;
  }

  if (watch_clock_pointer->point_image_data != NULL) {
    TKMEM_FREE(watch_clock_pointer->point_image_data);
  }

  if (watch_clock_pointer->pointer_image != NULL) {
    TKMEM_FREE(watch_clock_pointer->pointer_image);
  }

  if (watch_clock_pointer->bitmap.buffer != NULL) {
    graphic_buffer_destroy(watch_clock_pointer->bitmap.buffer);
  }
  
  return RET_OK;
}

static const char *const s_watch_clock_pointer_properties[] = {
    WATCH_CLOCK_POINTER_PROP_IMAGE, WATCH_CLOCK_POINTER_PROP_ANCHOR_X,
    WATCH_CLOCK_POINTER_PROP_ANCHOR_L_Y, NULL};

TK_DECL_VTABLE(watch_clock_pointer) = {
    .size = sizeof(watch_clock_pointer_t),
    .type = WIDGET_TYPE_WATCH_CLOCK_POINTER,
    .space_key_to_activate = TRUE,
    .return_key_to_activate = TRUE,
    .parent = TK_PARENT_VTABLE(widget),
    .create = watch_clock_pointer_create,
    .clone_properties = s_watch_clock_pointer_properties,
    .persistent_properties = s_watch_clock_pointer_properties,
    .set_prop = watch_clock_pointer_set_prop,
    .get_prop = watch_clock_pointer_get_prop,
    .on_destroy = watch_clock_pointer_on_destroy,
    .on_paint_self = watch_clock_pointer_on_paint_self};

widget_t *watch_clock_pointer_create(widget_t *parent, xy_t x, xy_t y, wh_t w,
                                     wh_t h) {
  widget_t *widget =
      widget_create(parent, TK_REF_VTABLE(watch_clock_pointer), x, y, w, h);
  watch_clock_pointer_t *watch_clock_pointer = WATCH_CLOCK_POINTER(widget);
  return_value_if_fail(watch_clock_pointer != NULL, NULL);

  watch_clock_pointer->value = 0;
  watch_clock_pointer->anchor_x = 0;
  watch_clock_pointer->anchor_y = 0;
  watch_clock_pointer->is_oom = FALSE;
  watch_clock_pointer->pointer_image = NULL;
  watch_clock_pointer->point_image_data = NULL;

  return widget;
}

widget_t *watch_clock_pointer_cast(widget_t *widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, watch_clock_pointer),
                       NULL);

  return widget;
}

ret_t watch_clock_pointer_set_value(widget_t *widget, uint8_t value) {
  watch_clock_pointer_t *watch_clock_pointer = WATCH_CLOCK_POINTER(widget);
  return_value_if_fail(watch_clock_pointer != NULL && value < 60,
                       RET_BAD_PARAMS);

  watch_clock_pointer->value = value;
  return RET_OK;
}

watch_clock_pointer_type_t
watch_clock_pointer_get_clock_pointer_type(widget_t *widget) {
  watch_clock_pointer_t *watch_clock_pointer = WATCH_CLOCK_POINTER(widget);
  return_value_if_fail(watch_clock_pointer != NULL, WATCH_CLOCK_POINTER_NONE);

  return watch_clock_pointer->clock_pointer_type;
}

ret_t watch_clock_pointer_set_clock_pointer_type(
    widget_t *widget, watch_clock_pointer_type_t type) {
  watch_clock_pointer_t *watch_clock_pointer = WATCH_CLOCK_POINTER(widget);
  return_value_if_fail(watch_clock_pointer != NULL &&
                           type != WATCH_CLOCK_POINTER_NONE,
                       RET_BAD_PARAMS);

  watch_clock_pointer->clock_pointer_type = type;

  return RET_OK;
}
