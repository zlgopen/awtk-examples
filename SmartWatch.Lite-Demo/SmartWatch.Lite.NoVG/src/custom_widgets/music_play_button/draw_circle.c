#include "draw_circle.h"
#include "awtk.h"
#include "tkc/mem.h"

#define IS_TEST_DRAW_CIRCLE 1

#include "base/ffr_draw_rounded_rect.inc"

#ifndef FRR_POINT_PIX_CACHE_SIZE
#define FRR_POINT_PIX_CACHE_SIZE 200
#endif // FRR_POINT_PIX_CACHE_SIZE


typedef enum _circle_quadrant_t {
	QUADRANT_FOURTH = 0,  //二维坐标第四象限的上区域
	QUADRANT_THIRD,  //二维坐标第三象限的上区域
	QUADRANT_SECOND,  //二维坐标第二象限的上区域
	QUADRANT_FIRST,  //二维坐标第一象限的上区域

	QUADRANT_COUNT = QUADRANT_FIRST + 1,
} circle_quadrant_t;

typedef struct _frr_line_point_pix_t {
	frr_point_pix_t base;
	uint32_t angle;
	rgba_t color;
} frr_line_point_pix_t;

typedef struct _frr_draw_image_info_t {
	frr_image_info_t image_info;
	canvas_t *c;
	int32_t xc;
	int32_t yc;
	rgba_t* color;
	draw_circle_t *ctx;
} frr_draw_image_info_t;

typedef struct _quadrant_info_t {
	bool_t is_one;
	bool_t is_link_v;
	bool_t is_link_h;
	uint16_t end_angle;
	uint16_t start_angle;
	frr_quadrant_t top_type;
	frr_quadrant_t bottom_type;
} quadrant_info_t;

static frr_line_point_pix_t frr_line_point_pix_cache1_list[FRR_POINT_PIX_CACHE_SIZE];

static frr_line_point_pix_t frr_line_point_pix_cache2_list[FRR_POINT_PIX_CACHE_SIZE];

draw_circle_t *draw_circle_fill_create(int32_t radius, int32_t line_width) {
	uint32_t size1 = 0;
	uint32_t size2 = 0;
	frr_point_pix_t *point_pix_cache1_list = NULL;
	frr_point_pix_t *point_pix_cache2_list = NULL;
	draw_circle_t *ctx = TKMEM_ZALLOC(draw_circle_t);
	return_value_if_fail(ctx != NULL, NULL);

	size1 = (uint32_t)(ANGLE2SIZE(M_FRR_PI_4, radius) * 1.5f);
	size2 = (uint32_t)(ANGLE2SIZE(M_FRR_PI_4, (radius + line_width)) * 1.5f);

	ctx->radius = radius;
	ctx->line_width = line_width;

	darray_init(&ctx->point1_list_45, size1, NULL, NULL);
	darray_init(&ctx->point2_list_45, size2, NULL, NULL);

	darray_init(&ctx->line1_point_list, radius, NULL, NULL);
	darray_init(&ctx->line2_point_list, (radius + line_width), NULL, NULL);

	ctx->point_pix_cache1_list = point_pix_cache1_list = TKMEM_ZALLOCN(frr_point_pix_t, size1);

	ctx->point_pix_cache2_list = point_pix_cache2_list = TKMEM_ZALLOCN(frr_point_pix_t, size2);

	Wu_D_Circle(ctx->radius, &ctx->point1_list_45, point_pix_cache1_list);
	Wu_D_Circle(ctx->radius + ctx->line_width, &ctx->point2_list_45, point_pix_cache2_list);

	return ctx;
}

void draw_circle_fill_destroy(draw_circle_t *ctx) {
	if (ctx != NULL) {
		darray_deinit(&ctx->point1_list_45);
		darray_deinit(&ctx->point2_list_45);
		darray_deinit(&ctx->line1_point_list);
		darray_deinit(&ctx->line2_point_list);

		if (ctx->point_pix_cache1_list != NULL) {
			TKMEM_FREE(ctx->point_pix_cache1_list);
		}
		if (ctx->point_pix_cache2_list != NULL) {
			TKMEM_FREE(ctx->point_pix_cache2_list);
		}

		TKMEM_FREE(ctx);
	}
}

static bool_t Wu_D_Line(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
	uint32_t angle, darray_t *point_list,
	frr_line_point_pix_t *point_pix_cache_list) {
	int32_t x = x1;
	int32_t y = y1;
	int32_t dx = x2 - x1;
	int32_t dy = y2 - y1;
	int32_t ux = 0;
	int32_t uy = 0;
	float_t uk = 0.0f;
	float_t k = 0.0f;
	float_t e = 0.0f;
	uint32_t i = 0;
	frr_line_point_pix_t *tmp = NULL;

	k = (float_t)dy / (float_t)dx;
	ux = ((dx > 0) << 1) - 1;
	uy = ((dy > 0) << 1) - 1;
	uk = ((ux > 0 && uy > 0) || (ux < 0 && uy < 0)) ? -k : k;

	if (dx == 0 && dy == 0) {
		return FALSE;
	}

	if (dx == 0 && dy != 0) {
		int32_t offset_y = y1 > y2 ? -1 : 1;
		for (y = y1; y != y2; y += offset_y) {
			tmp = &(point_pix_cache_list[i++]);
			tmp->base.x = x;
			tmp->base.y = y;
			tmp->base.e = 1.0f;
			tmp->angle = angle;
			darray_push(point_list, tmp);
		}
		e = 0.0f;
	}
	else if (dx != 0 && dy == 0) {
		int32_t offset_x = x1 > x2 ? -1 : 1;
		for (x = x1; x != x2; x += offset_x) {
			tmp = &(point_pix_cache_list[i++]);
			tmp->base.x = x;
			tmp->base.y = y;
			tmp->base.e = 1.0f;
			tmp->angle = angle;
			darray_push(point_list, tmp);
		}
		e = 0.0f;
	}
	else if (abs(dx) > abs(dy)) {
		for (x = x1; x != x2; x += ux) {
			tmp = &(point_pix_cache_list[i++]);
			tmp->base.x = x;
			tmp->base.y = y;
			tmp->base.e = tk_abs(e);
			tmp->angle = angle;
			darray_push(point_list, tmp);
			e += uk;
			if ((e >= 1.0f && uk > 0.0f) || (e <= -1.0f && uk < 0.0f)) {
				e = e + (uk > 0 ? -1 : 1);
				y += uy;
			}
		}
	}
	else {
		uk = 1 / uk;
		for (y = y1; y != y2; y += uy) {
			tmp = &(point_pix_cache_list[i++]);
			tmp->base.x = x;
			tmp->base.y = y;
			tmp->base.e = tk_abs(e);
			tmp->angle = angle;
			darray_push(point_list, tmp);
			e += uk;
			if ((e >= 1.0f && uk > 0.0f) || (e <= -1.0f && uk < 0.0f)) {
				e = e + (uk > 0 ? -1 : 1);
				x += ux;
			}
		}
	}
	tmp = &(point_pix_cache_list[i++]);
	tmp->base.x = x;
	tmp->base.y = y;
	tmp->base.e = tk_abs(e);
	tmp->angle = angle;
	darray_push(point_list, tmp);
	return TRUE;
}

static inline int32_t draw_circle_angle_to_0_360(int32_t angle) {
	int32_t tmp = 0;
	int32_t n = angle - (int32_t)angle;

	tmp = (int32_t)angle % 360;

	if (tmp < 0) {
		tmp = 360 + tmp;
	}

	if (n > 0) {
		tmp += n;
	}

	return tmp;
}

/*
270
180         0
90

*	0   -  90  为数组下表为 0	//QUADRANT_FOURTH
*	90  -  180 为数组下表为 1	//QUADRANT_THIRD
*	180 -  270 为数组下表为 2	//QUADRANT_SECOND
*	270 -  360 为数组下表为 3	//QUADRANT_FIRST
*/
static circle_quadrant_t draw_circle_get_quadrant_for_angle(int32_t angle) {
	if (0 <= angle && angle < 90) {
		return QUADRANT_FOURTH;
	}
	else if (90 <= angle && angle < 180) {
		return QUADRANT_THIRD;
	}
	else if (180 <= angle && angle < 270) {
		return QUADRANT_SECOND;
	}
	else {
		return QUADRANT_FIRST;
	}
}

static void draw_circle_set_quadrant_angle_info_start_angle(quadrant_info_t *quadrant_info_list,
	circle_quadrant_t info_number, int32_t angle, bool_t is_ccw) {
	quadrant_info_list[(uint32_t)info_number].is_one = TRUE;
	if (is_ccw) {
		quadrant_info_list[(uint32_t)info_number].end_angle = angle - info_number * 90;
	}
	else {
		quadrant_info_list[(uint32_t)info_number].start_angle = angle - info_number * 90;
	}
}

static void draw_circle_set_quadrant_angle_info_end_angle(quadrant_info_t *quadrant_info_list,
	circle_quadrant_t info_number, int32_t angle, bool_t is_ccw) {
	quadrant_info_list[(uint32_t)info_number].is_one = TRUE;
	if (is_ccw) {
		quadrant_info_list[(uint32_t)info_number].start_angle = angle - info_number * 90;
	}
	else {
		quadrant_info_list[(uint32_t)info_number].end_angle = angle - info_number * 90;
	}
}

static void draw_circle_set_quadrant_normalize(quadrant_info_t *quadrant_info_list) {
	int32_t i = 0;
	int32_t end_angle = 0;
	int32_t start_angle = 0;
	for (i = 0; i < QUADRANT_COUNT; i++) {
		if (quadrant_info_list[i].start_angle != 0 || quadrant_info_list[i].end_angle != 0)
		{
			if (i == QUADRANT_FOURTH || i == QUADRANT_SECOND)
			{
				end_angle = 90 - quadrant_info_list[i].start_angle;
				start_angle = 90 - quadrant_info_list[i].end_angle;

				quadrant_info_list[i].end_angle = end_angle;
				quadrant_info_list[i].start_angle = start_angle;
			}

			switch ((circle_quadrant_t)i)
			{
			case QUADRANT_FOURTH:
				quadrant_info_list[i].top_type = FRR_QUADRANT_FOURTH_S;
				quadrant_info_list[i].bottom_type = FRR_QUADRANT_FOURTH_F;
				break;
			case QUADRANT_THIRD:
				quadrant_info_list[i].top_type = FRR_QUADRANT_THIRD_S;
				quadrant_info_list[i].bottom_type = FRR_QUADRANT_THIRD_F;
				break;
			case QUADRANT_SECOND:
				quadrant_info_list[i].top_type = FRR_QUADRANT_SECOND_F;
				quadrant_info_list[i].bottom_type = FRR_QUADRANT_SECOND_S;
				break;
			case QUADRANT_FIRST:
				quadrant_info_list[i].top_type = FRR_QUADRANT_FIRST_F;
				quadrant_info_list[i].bottom_type = FRR_QUADRANT_FIRST_S;
				break;
			default:
				break;
			}
		}
	}
}

static void draw_circle_get_quadrant_link_model(quadrant_info_t *quadrant_info_list) {
	int32_t i = 0;
	for (i = 0; i < QUADRANT_COUNT; i++)
	{
		if (quadrant_info_list[i].start_angle != 0)
		{
			switch ((circle_quadrant_t)i)
			{
			case QUADRANT_FOURTH:
				quadrant_info_list[i].is_link_v = quadrant_info_list[QUADRANT_THIRD].start_angle != 0;
				break;
			case QUADRANT_THIRD:
				quadrant_info_list[i].is_link_v = quadrant_info_list[QUADRANT_FOURTH].start_angle != 0;
				break;
			case QUADRANT_SECOND:
				quadrant_info_list[i].is_link_v = quadrant_info_list[QUADRANT_FIRST].start_angle != 0;
				break;
			case QUADRANT_FIRST:
				quadrant_info_list[i].is_link_v = quadrant_info_list[QUADRANT_SECOND].start_angle != 0;
				break;
			default:
				break;
			}
		}

		if (quadrant_info_list[i].end_angle != 0)
		{
			switch ((circle_quadrant_t)i)
			{
			case QUADRANT_FOURTH:
				quadrant_info_list[i].is_link_h = quadrant_info_list[QUADRANT_FIRST].start_angle != 0;
				break;
			case QUADRANT_THIRD:
				quadrant_info_list[i].is_link_h = quadrant_info_list[QUADRANT_SECOND].start_angle != 0;
				break;
			case QUADRANT_SECOND:
				quadrant_info_list[i].is_link_h = quadrant_info_list[QUADRANT_THIRD].start_angle != 0;
				break;
			case QUADRANT_FIRST:
				quadrant_info_list[i].is_link_h = quadrant_info_list[QUADRANT_FOURTH].start_angle != 0;
				break;
			default:
				break;
			}
		}
	}
}

static bool_t draw_circle_get_quadrant_angle_info_list(
	quadrant_info_t quadrant_info_list[4], int32_t start_angle,
	int32_t end_angle, bool_t is_ccw) {

	int32_t i = 0;
	int32_t value_quadrant = 0;
	int32_t offset_end_angle = 0;
	int32_t offset_start_angle = 0;
	circle_quadrant_t end_quadrant = 0;
	circle_quadrant_t start_quadrant = 0;
	end_angle = draw_circle_angle_to_0_360(end_angle);
	start_angle = draw_circle_angle_to_0_360(start_angle);

	if (end_angle == start_angle) {
		return FALSE;
	}

	end_quadrant = draw_circle_get_quadrant_for_angle(end_angle);
	start_quadrant = draw_circle_get_quadrant_for_angle(start_angle);

	if (start_angle > end_angle) {
		value_quadrant = end_angle + 360 - start_angle;
	}
	else {
		value_quadrant = end_angle - start_angle;
	}

	if (is_ccw) {
		value_quadrant = 360 - value_quadrant;
		for (i; i < value_quadrant; i += 90) {
			uint32_t tmp = draw_circle_angle_to_0_360(end_angle + i);
			uint32_t number = (uint32_t)draw_circle_get_quadrant_for_angle(tmp);
			quadrant_info_list[number].is_one = TRUE;
			quadrant_info_list[number].end_angle = 90;
			quadrant_info_list[number].start_angle = 0;
		}
	}
	else {
		for (i = 0; i < value_quadrant; i += 90) {
			uint32_t tmp = draw_circle_angle_to_0_360(start_angle + i);
			uint32_t number = (uint32_t)draw_circle_get_quadrant_for_angle(tmp);
			quadrant_info_list[number].is_one = TRUE;
			quadrant_info_list[number].end_angle = 90;
			quadrant_info_list[number].start_angle = 0;
		}
	}


	if (end_quadrant == start_quadrant) {
		quadrant_info_list[start_quadrant].is_one = value_quadrant <= 90;
		if (start_angle > end_angle) {
			quadrant_info_list[start_quadrant].end_angle = start_angle - start_quadrant * 90;
			quadrant_info_list[start_quadrant].start_angle = end_angle - start_quadrant * 90;
		}
		else {
			quadrant_info_list[start_quadrant].end_angle = end_angle - start_quadrant * 90;
			quadrant_info_list[start_quadrant].start_angle = start_angle - start_quadrant * 90;
		}
	}
	else {
		draw_circle_set_quadrant_angle_info_end_angle(quadrant_info_list, end_quadrant, end_angle, is_ccw);
		draw_circle_set_quadrant_angle_info_start_angle(quadrant_info_list, start_quadrant, start_angle, is_ccw);
	}

	draw_circle_set_quadrant_normalize(quadrant_info_list);
	draw_circle_get_quadrant_link_model(quadrant_info_list);

	return TRUE;
}





static void draw_circle_draw_circluar_point_to_quadrant_hline(
	frr_draw_image_info_t *draw_image_info, int32_t w, int32_t px, int32_t py, bool_t is_left, frr_quadrant_t type) {
	int32_t p = 0;
	int32_t x = 0;
	int32_t y = 0;
	int32_t x1 = 0;
	bool_t is_v = px == 0;
	int32_t xc = draw_image_info->xc;
	int32_t yc = draw_image_info->yc;
	canvas_t *c = draw_image_info->c;
	frr_image_info_t* image_info = &draw_image_info->image_info;
	if (w > 0) {
		if (ffr_get_circluar_point_to_quadrant(xc, yc, px, py, type, &x, &y)) {
			x1 = x;
			if (is_left) {
				int32_t tmp = x - w;
				if (tmp < 0) {
					w += tmp;
					x = 0;
				}
				else {
					x = x - w;
				}
			}

			canvas_fill_rect(c, x, y, w, 1);
		}
	}
}

static void draw_circle_draw_circluar_point_to_quadrant_point(
	frr_draw_image_info_t *draw_image_info, int32_t px, int32_t py, float_t e, bool_t is_left, frr_quadrant_t type) {
	int32_t p = 0;
	int32_t x = 0;
	int32_t y = 0;
	int32_t x1 = 0;
	rgba_t o_color_1;
	canvas_t *c = draw_image_info->c;
	frr_image_info_t* image_info = &draw_image_info->image_info;

	if (ffr_get_circluar_point_to_quadrant(draw_image_info->xc, draw_image_info->yc, px, py, type, &x, &y)) {
		if (!is_left) {
			x--;
		}
		p = frr_image_get_buff_point(image_info, x + c->ox, y + c->oy);
		if (p > 0) {
			ffr_image_data2color(image_info->dst + p, x + c->ox, y + c->oy, image_info->w, image_info->h, image_info->format, &o_color_1);
			ffr_draw_antialiasing_for_point(image_info, c, x + c->ox, y + c->oy, e, *draw_image_info->color, o_color_1);
		}
	}
}

static void draw_circle_draw_circluar_point_to_quadrant_points(
	frr_draw_image_info_t *draw_image_info, darray_t *point1_list, uint32_t point1_list_45_last_point, bool_t is_left, bool_t is_angle_45, frr_quadrant_t type) {

	frr_point_pix_t *iter = NULL;
	frr_point_pix_t *iter_next = NULL;
	uint32_t i = point1_list_45_last_point;

	for (; i > 0; i--) {
		iter = (frr_point_pix_t *)darray_get(point1_list, i);
		if (i - 1 >= 0) {
			iter_next = (frr_point_pix_t *)darray_get(point1_list, i - 1);

			draw_circle_draw_circluar_point_to_quadrant_point(draw_image_info, iter->x, iter->y, iter->e, !is_left, type);

			if (is_angle_45) {
				if (iter->y != iter_next->y) {
					break;
				}
			}
			else {
				if (iter->x != iter_next->x) {
					break;
				}
			}
		}
	}
}

static void draw_circle_draw_circluar_point_to_quadrant_points_l(
	frr_draw_image_info_t *draw_image_info, darray_t *point1_list, uint32_t point1_list_45_last_point, bool_t is_left, bool_t is_angle_45, frr_quadrant_t type) {

	frr_point_pix_t *iter = NULL;
	frr_point_pix_t *iter_next = NULL;
	uint32_t i = point1_list_45_last_point;

	for (; i < point1_list->size - 1; i++) {
		iter = (frr_point_pix_t *)darray_get(point1_list, i);
		iter_next = (frr_point_pix_t *)darray_get(point1_list, i - 1);

		draw_circle_draw_circluar_point_to_quadrant_point(draw_image_info, iter->x, iter->y, iter->e, !is_left, type);

		if (is_angle_45) {
			if (iter->y != iter_next->y) {
				break;
			}
		}
		else {
			if (iter->x != iter_next->x) {
				break;
			}
		}
	}
}

static void draw_circle_draw_circluar_point_to_quadrant_points_e(
	frr_draw_image_info_t *draw_image_info, darray_t *point1_list, uint32_t point1_list_45_last_point, bool_t is_left, bool_t is_angle_45, bool_t is_e, frr_quadrant_t type) {

	frr_point_pix_t *iter = NULL;
	frr_point_pix_t *iter_next = NULL;
	uint32_t i = point1_list_45_last_point;

	for (; i > 0; i--) {
		iter = (frr_point_pix_t *)darray_get(point1_list, i);
		if (i - 1 >= 0) {
			iter_next = (frr_point_pix_t *)darray_get(point1_list, i - 1);

			if (is_e) {
				draw_circle_draw_circluar_point_to_quadrant_point(draw_image_info, iter->x, iter->y, iter->e, !is_left, type);
			}
			else {
				draw_circle_draw_circluar_point_to_quadrant_point(draw_image_info, iter->x, iter->y, 1 - iter->e, !is_left, type);
			}

			if (is_angle_45) {
				if (iter->y != iter_next->y) {
					break;
				}
			}
			else {
				if (iter->x != iter_next->x) {
					break;
				}
			}
		}
	}

}

static void draw_circle_draw_circluar_point_to_quadrant_points_e_n(
	frr_draw_image_info_t *draw_image_info, darray_t *point1_list, uint32_t point1_list_45_last_point, bool_t is_left, bool_t is_angle_45, frr_quadrant_t type) {

	frr_point_pix_t *iter = NULL;
	frr_point_pix_t *iter_next = NULL;
	uint32_t i = point1_list_45_last_point;

	for (; i > 0; i--) {
		iter = (frr_point_pix_t *)darray_get(point1_list, i);
		if (i - 1 >= 0) {
			iter_next = (frr_point_pix_t *)darray_get(point1_list, i - 1);

			draw_circle_draw_circluar_point_to_quadrant_point(draw_image_info, iter->x - 1, iter->y, iter->e, !is_left, type);

			if (is_angle_45) {
				if (iter->y != iter_next->y) {
					break;
				}
			}
			else {
				if (iter->x != iter_next->x) {
					break;
				}
			}
		}
	}

}



static frr_point_pix_t* draw_circle_arc_width_get_right_angled_point_r_x(darray_t *point1_list, uint32_t* point1_list_last_point, uint32_t py, uint32_t* x) {

	bool_t is_get = FALSE;
	frr_point_pix_t *iter = NULL;
	frr_point_pix_t *iter_next = NULL;
	uint32_t i = *point1_list_last_point;

	for (; i < point1_list->size; i++) {
		iter = (frr_point_pix_t *)darray_get(point1_list, i);
		if (i + 1 < point1_list->size) {
			iter_next = (frr_point_pix_t *)darray_get(point1_list, i + 1);
			if (iter->y != iter_next->y && py == iter->y)
			{
				*x = iter->x;
				is_get = TRUE;
				*point1_list_last_point = i;
				break;
			}
		}
		else if (py == iter->y) {
			*x = iter->x;
			is_get = TRUE;
			*point1_list_last_point = i;
			break;
		}
	}

	if (is_get) {
		return iter;
	}
	return NULL;
}

static frr_point_pix_t* draw_circle_arc_width_get_right_angled_point_r_y(darray_t *point1_list, uint32_t* point1_list_point, uint32_t px, uint32_t* y) {

	bool_t is_get = FALSE;
	frr_point_pix_t *iter = NULL;
	frr_point_pix_t *iter_next = NULL;
	uint32_t i = *point1_list_point;

	for (; i < point1_list->size; i++) {
		iter = (frr_point_pix_t *)darray_get(point1_list, i);
		if (i + 1 < point1_list->size) {
			iter_next = (frr_point_pix_t *)darray_get(point1_list, i + 1);
			if (iter->x != iter_next->x && px == iter->x)
			{
				*y = iter->y;
				is_get = TRUE;
				*point1_list_point = i;
				break;
			}
		}
		else if (px == iter->x) {
			*y = iter->y;
			is_get = TRUE;
			*point1_list_point = i;
			break;
		}
	}

	if (is_get) {
		return iter;
	}
	return NULL;
}

static frr_point_pix_t* draw_circle_arc_width_get_right_angled_point_l_y(darray_t *point1_list, uint32_t* point1_list_last_point, uint32_t px, uint32_t* y) {

	bool_t is_get = FALSE;
	frr_point_pix_t *iter = NULL;
	frr_point_pix_t *iter_next = NULL;
	uint32_t i = *point1_list_last_point;

	for (; i > 0; i--) {
		iter = (frr_point_pix_t *)darray_get(point1_list, i);
		if (i + 1 < point1_list->size) {
			iter_next = (frr_point_pix_t *)darray_get(point1_list, i + 1);
			if (iter->x != iter_next->x && px == iter->x)
			{
				*y = iter->y;
				is_get = TRUE;
				break;
			}
		}
		else if (px == iter->x) {
			*y = iter->y;
			is_get = TRUE;
			break;
		}
	}
	*point1_list_last_point = i;

	if (is_get) {
		return iter;
	}
	return NULL;
}

static float_t draw_circle_get_point_45_x_for_angle(bool_t is_angle_45, int32_t angle, int32_t r) {
	float_t x = 0.0f;
	if (is_angle_45) {
		x = sinf(TK_D2R(angle)) * r;
	}
	else {
		x = sinf(TK_D2R(90 - angle)) * r;
	}
	return x;
}

static frr_line_point_pix_t* draw_circle_get_line_point_list(
	int32_t angle, draw_circle_t *ctx, bool_t is_angle_45, float_t* x2, darray_t *line_point_list, frr_line_point_pix_t * frr_line_point_pix_cache_list) {

#define ITER_NUMBER 3

	int32_t d = 0;
	uint32_t i = 0;
	uint32_t size = 0;
	float_t x_1 = 0.0f;
	float_t x_2 = 0.0f;
	float_t min_d = 90.0f;
	frr_point_pix_t *iter1 = NULL;
	frr_point_pix_t *iter2 = NULL;
	frr_point_pix_t *iter1_tmp[ITER_NUMBER];
	frr_point_pix_t *iter2_tmp[ITER_NUMBER];
	darray_t *point1_list_45 = &ctx->point1_list_45;
	darray_t *point2_list_45 = &ctx->point2_list_45;
	frr_line_point_pix_t *point_pix_cache_list = NULL;

	x_1 = draw_circle_get_point_45_x_for_angle(is_angle_45, angle, ctx->radius);
	*x2 = draw_circle_get_point_45_x_for_angle(is_angle_45, angle, ctx->radius + ctx->line_width);

	x_2 = x_1 + 1;

	for (i = 0; i < point1_list_45->size; i++) {
		iter1 = (frr_point_pix_t *)darray_get(point1_list_45, i);
		if ((x_1 < iter1->x || CMP_FLOAT_QE(x_1, iter1->x)) &&
			(iter1->x < x_2 || CMP_FLOAT_QE(x_2, iter1->x))) {
			break;
		}
	}
	iter1_tmp[1] = iter1;
	if (i + 1 < point1_list_45->size) {
		iter1_tmp[2] = (frr_point_pix_t *)darray_get(point1_list_45, i + 1);
	}
	else {
		iter1_tmp[2] = NULL;
	}
	if (i - 1 >= 0) {
		iter1_tmp[0] = (frr_point_pix_t *)darray_get(point1_list_45, i - 1);
	}
	else {
		iter1_tmp[0] = NULL;
	}


	x_1 = *x2;
	x_2 = x_1 + 1;
	for (i = 0; i < point2_list_45->size; i++) {
		iter2 = (frr_point_pix_t *)darray_get(point2_list_45, i);
		if ((x_1 < iter2->x || CMP_FLOAT_QE(x_1, iter2->x)) &&
			(iter2->x < x_2 || CMP_FLOAT_QE(x_2, iter2->x))) {
			break;
		}
	}
	iter2_tmp[1] = iter2;

	if (i + 1 < point2_list_45->size) {
		iter2_tmp[2] = (frr_point_pix_t *)darray_get(point2_list_45, i + 1);
	}
	else {
		iter2_tmp[2] = NULL;
	}
	if (i - 1 >= 0) {
		iter2_tmp[0] = (frr_point_pix_t *)darray_get(point2_list_45, i - 1);
	}
	else {
		iter2_tmp[0] = NULL;
	}

	if (is_angle_45) {
		angle = 90 - angle;
	}

	for (i = 0; i < ITER_NUMBER; i++) {
		if (iter1_tmp[i] != NULL) {
			uint32_t j = 0;
			for (; j < ITER_NUMBER; j++) {
				if (iter2_tmp[j] != NULL && iter1_tmp[i]->x != iter2_tmp[j]->x) {
					float_t angle_k = tk_abs(TK_R2D(atanf((float_t)(iter1_tmp[i]->y - iter2_tmp[j]->y) / (iter1_tmp[i]->x - iter2_tmp[j]->x))));
					float_t tmp_d = tk_abs(angle_k - angle);
					if (tmp_d < min_d) {
						d = j;
						min_d = tmp_d;
						iter1 = iter1_tmp[i];
						iter2 = iter2_tmp[j];
					}
					if (min_d == 0) {
						break;
					}
				}
			}
		}
		if (min_d == 0) {
			break;
		}
	}
	*x2 += (d - 1);

	size = (uint32_t)(sqrt(pow(iter1->x - iter2->x, 2) + pow(iter1->y - iter2->y, 2)) *1.5);

	if (size <= FRR_POINT_PIX_CACHE_SIZE) {
		point_pix_cache_list = frr_line_point_pix_cache_list;
	}
	else {
		point_pix_cache_list = TKMEM_ZALLOCN(frr_line_point_pix_t, size);
	}

	if (!Wu_D_Line(iter2->x, iter2->y, iter1->x, iter1->y, angle, line_point_list, point_pix_cache_list)) {

		if (size > FRR_POINT_PIX_CACHE_SIZE) {
			TKMEM_FREE(point_pix_cache_list);
		}

		return NULL;
	}

	return point_pix_cache_list;
}

static void draw_circle_arc_width_angle_top(frr_draw_image_info_t *image_info, quadrant_info_t* quadrant_info, bool_t is_end_angle_45, float_t x1_2, float_t x2_2) {

	int32_t i = 0;
	int32_t w = 0;
	uint32_t x = 0;
	int32_t last_y = 0;
	uint32_t line_list1_last_point = 0;
	uint32_t line_list2_last_point = 0;
	uint32_t point1_list_45_last_point = 0;

	draw_circle_t *ctx = image_info->ctx;
	darray_t *point1_list_45 = &ctx->point1_list_45;
	darray_t *point2_list_45 = &ctx->point2_list_45;
	darray_t *line1_point_list = &ctx->line1_point_list;
	darray_t *line2_point_list = &ctx->line2_point_list;

	bool_t is_draw_line = TRUE;
	bool_t is_swith_line = FALSE;
	frr_quadrant_t type = quadrant_info->top_type;
	bool_t is_left =
		type == FRR_QUADRANT_FIRST_F || type == FRR_QUADRANT_FIRST_S ||
		type == FRR_QUADRANT_FOURTH_F || type == FRR_QUADRANT_FOURTH_S;

	frr_point_pix_t *point1_head = (frr_point_pix_t *)darray_head(point1_list_45);
	for (i = 0; i < point2_list_45->size - 1; i++)
	{
		frr_point_pix_t *iter = (frr_point_pix_t *)darray_get(point2_list_45, i);
		frr_point_pix_t *iter_next = (frr_point_pix_t *)darray_get(point2_list_45, i + 1);

		if (line1_point_list->size > 0 && x1_2 > iter->x) {
			continue;
		}
		if (line2_point_list->size > 0 && is_end_angle_45 && x2_2 < iter->x) {
			break;
		}

		if (is_draw_line) {
			is_draw_line = FALSE;

			if (!is_swith_line && line1_point_list->size > 0 &&
				draw_circle_arc_width_get_right_angled_point_r_x(line1_point_list, &line_list1_last_point, iter->y, &x) == NULL) {
				is_swith_line = TRUE;
			}

			if (is_swith_line || (line1_point_list->size == 0 && iter->y <= point1_head->y)) {
				draw_circle_arc_width_get_right_angled_point_r_x(point1_list_45, &point1_list_45_last_point, iter->y, &x);
			}

			w = iter->x - x;

			draw_circle_draw_circluar_point_to_quadrant_hline(image_info, w, iter->x, iter->y, is_left, type);

			if (point1_list_45_last_point > 0) {
				draw_circle_draw_circluar_point_to_quadrant_points(image_info, point1_list_45, point1_list_45_last_point, is_left, TRUE, type);
			}

			if (line_list1_last_point > 0) {
				draw_circle_draw_circluar_point_to_quadrant_points(image_info, line1_point_list, line_list1_last_point, is_left, TRUE, type);
			}

		}
		last_y = iter->y;
		draw_circle_draw_circluar_point_to_quadrant_point(image_info, iter->x, iter->y, 1 - iter->e, is_left, type);

		if (iter->y != iter_next->y) {
			is_draw_line = TRUE;
		}
	}

	if (is_end_angle_45) {
		is_swith_line = FALSE;
		for (i = 0; i < line2_point_list->size; i++) {
			frr_point_pix_t *iter = (frr_point_pix_t *)darray_get(line2_point_list, i);
			if (last_y <= iter->y) {
				continue;
			}

			if (i - 1 >= 0) {
				frr_point_pix_t *iter_prev = (frr_point_pix_t *)darray_get(line2_point_list, i - 1);
				if (iter->y == iter_prev->y) {
					continue;
				}
			}

			if (point1_list_45_last_point > 0) {
				draw_circle_draw_circluar_point_to_quadrant_points(image_info, point1_list_45, point1_list_45_last_point, is_left, TRUE, type);
			}
			if (line_list1_last_point > 0) {
				draw_circle_draw_circluar_point_to_quadrant_points(image_info, line1_point_list, line_list1_last_point, is_left, TRUE, type);
			}


			if (!is_swith_line && line1_point_list->size > 0 &&
				draw_circle_arc_width_get_right_angled_point_r_x(line1_point_list, &line_list1_last_point, iter->y, &x) == NULL) {
				is_swith_line = TRUE;
				line_list1_last_point = 0;
			}

			if (is_swith_line || (line1_point_list->size == 0 && iter->y <= point1_head->y)) {
				draw_circle_arc_width_get_right_angled_point_r_x(point1_list_45, &point1_list_45_last_point, iter->y, &x);
			}

			w = iter->x - x;
			if (w > 0) {
				draw_circle_draw_circluar_point_to_quadrant_point(image_info, iter->x, iter->y, 1 - iter->e, is_left, type);
			}
			draw_circle_draw_circluar_point_to_quadrant_hline(image_info, w, iter->x, iter->y, is_left, type);
		}
	}
}

static void draw_circle_arc_width_angle_bottom(frr_draw_image_info_t *image_info, quadrant_info_t* quadrant_info, bool_t is_start_angle_45, float_t x1_2, float_t x2_2) {
	int32_t x = 0;
	int32_t y = 0;
	int32_t w = 0;
	int32_t i = 0;
	int32_t last_x = -1;
	uint32_t line_list1_last_point = 0;
	uint32_t line_list2_last_point = 0;
	uint32_t point1_list_45_last_point = 0;

	draw_circle_t *ctx = image_info->ctx;
	darray_t *point1_list_45 = &ctx->point1_list_45;
	darray_t *point2_list_45 = &ctx->point2_list_45;
	darray_t *line1_point_list = &ctx->line1_point_list;
	darray_t *line2_point_list = &ctx->line2_point_list;

	bool_t is_swith_line = FALSE;
	frr_quadrant_t type = quadrant_info->bottom_type;
	bool_t is_left =
		type == FRR_QUADRANT_FIRST_F || type == FRR_QUADRANT_FIRST_S ||
		type == FRR_QUADRANT_FOURTH_F || type == FRR_QUADRANT_FOURTH_S;

	bool_t is_top = type == FRR_QUADRANT_FIRST_S || type == FRR_QUADRANT_SECOND_S;

	if (is_start_angle_45)
	{
		frr_point_pix_t *point1_list_45_iter = NULL;
		point1_list_45_last_point = point1_list_45->size - 1;
		for (i = point2_list_45->size - 1; i >= 0; i--) {
			frr_point_pix_t *iter = (frr_point_pix_t *)darray_get(point2_list_45, i);

			if (line1_point_list->size > 0 && x1_2 < iter->x) {
				continue;
			}
			if (line2_point_list->size > 0 && x2_2 > iter->x) {
				break;
			}

			if (point1_list_45_iter != NULL) {
				draw_circle_draw_circluar_point_to_quadrant_points(image_info, point1_list_45, point1_list_45_last_point, is_left, FALSE, type);
			}
			if (line_list1_last_point > 0) {
				draw_circle_draw_circluar_point_to_quadrant_points_e(image_info, line1_point_list, line_list1_last_point, is_left, TRUE, FALSE, type);
			}

			if (!is_swith_line && line1_point_list->size > 0 &&
				draw_circle_arc_width_get_right_angled_point_r_y(line1_point_list, &line_list1_last_point, iter->x, &y) == NULL) {
				is_swith_line = TRUE;
				line_list1_last_point = 0;
			}

			if (is_swith_line) {
				point1_list_45_iter = draw_circle_arc_width_get_right_angled_point_l_y(point1_list_45, &point1_list_45_last_point, iter->x, &y);
			}

			w = iter->y - y;
			last_x = iter->x;

			draw_circle_draw_circluar_point_to_quadrant_hline(image_info, w, iter->x, iter->y, is_left, type);
			draw_circle_draw_circluar_point_to_quadrant_point(image_info, iter->x, iter->y, 1 - iter->e, is_left, type);

			if (is_top && i == 1) {
				break;
			}
		}

		if (!is_swith_line) {
			point1_list_45_last_point = 0;
		}

		if (line1_point_list->size > 0) {
			for (i = 0; i < line1_point_list->size - 1; i++) {
				frr_point_pix_t *iter = (frr_point_pix_t *)darray_get(line1_point_list, i);
				if (x1_2 <= iter->x) {
					draw_circle_draw_circluar_point_to_quadrant_point(image_info, iter->x, iter->y, 1 - iter->e, is_left, type);
				}
			}
		}
	}
	else {
		point1_list_45_last_point = 0;
		frr_point_pix_t *point1_head = (frr_point_pix_t *)darray_head(point1_list_45);
		frr_point_pix_t *point2_tail = (frr_point_pix_t *)darray_get(point2_list_45, point2_list_45->size - 2);
		for (i = point2_list_45->size - 1; i >= 0; i--) {

			frr_point_pix_t *iter = (frr_point_pix_t *)darray_get(point2_list_45, i);
			if (point2_tail->y <= iter->x) {
				continue;
			}

			if (line2_point_list->size > 0 && x2_2 > iter->x) {
				break;
			}

			if (point1_list_45_last_point > 0) {
				bool_t is_angle_45 = TRUE;
				frr_quadrant_t type1 = quadrant_info->top_type;
				if (is_swith_line) {
					type1 = quadrant_info->bottom_type;
					is_angle_45 = FALSE;
				}
				draw_circle_draw_circluar_point_to_quadrant_points(image_info, point1_list_45, point1_list_45_last_point, is_left, is_angle_45, type1);
			}
			if (line_list1_last_point > 0) {
				draw_circle_draw_circluar_point_to_quadrant_points_l(image_info, line1_point_list, line_list1_last_point, is_left, TRUE, quadrant_info->top_type);
			}

			if (iter->x > point1_head->y) {
				if (line1_point_list->size > 0) {
					draw_circle_arc_width_get_right_angled_point_r_x(line1_point_list, &line_list1_last_point, iter->x, &y);
				}
			}
			else {
				if (line1_point_list->size == 0 || draw_circle_arc_width_get_right_angled_point_r_x(line1_point_list, &line_list1_last_point, iter->x, &y) == NULL) {
					if (!is_swith_line && draw_circle_arc_width_get_right_angled_point_r_x(point1_list_45, &point1_list_45_last_point, iter->x, &y) == NULL) {
						is_swith_line = TRUE;
						point1_list_45_last_point = point1_list_45->size - 1;
					}
					if (is_swith_line) {
						draw_circle_arc_width_get_right_angled_point_l_y(point1_list_45, &point1_list_45_last_point, iter->x, &y);
					}
				}
			}

			if (i == point2_list_45->size - 1) {
				if (line_list1_last_point > 0) {
					draw_circle_draw_circluar_point_to_quadrant_points(image_info, line1_point_list, line_list1_last_point - 1, is_left, FALSE, quadrant_info->top_type);
				}
			}

			w = iter->y - y;
			last_x = iter->x;

			draw_circle_draw_circluar_point_to_quadrant_hline(image_info, w, iter->x, iter->y, is_left, type);
			draw_circle_draw_circluar_point_to_quadrant_point(image_info, iter->x, iter->y, 1 - iter->e, is_left, type);
			if (is_top && i == 1) {
				break;
			}
		}
	}

	if (line2_point_list->size > 0) {
		bool_t is_l = FALSE;
		bool_t is_angle_45 = TRUE;
		frr_point_pix_t *iter = NULL;
		frr_point_pix_t *point1_head = (frr_point_pix_t *)darray_head(point1_list_45);

		if (is_start_angle_45) {
			line_list1_last_point = 0;
		}
		for (i = 0; i < line2_point_list->size - 1; i++) {
			iter = (frr_point_pix_t *)darray_get(line2_point_list, i);

			if (i - 1 >= 0){
				frr_point_pix_t *iter_prev = (frr_point_pix_t *)darray_get(line2_point_list, i - 1);
				if (iter->x == iter_prev->x) {
					continue;
				}
			}

			if (last_x < 0 || (last_x >= 0 && last_x > iter->x)) {

				if (point1_list_45_last_point > 0) {
					frr_quadrant_t type1 = quadrant_info->top_type;
					if (is_l || is_swith_line) {
						type1 = quadrant_info->bottom_type;
					}
					draw_circle_draw_circluar_point_to_quadrant_points(image_info, point1_list_45, point1_list_45_last_point, is_left, is_angle_45, type1);
				}

				if (line_list1_last_point > 0 && !is_start_angle_45) {
					draw_circle_draw_circluar_point_to_quadrant_points_l(image_info, line1_point_list, line_list1_last_point, is_left, TRUE, quadrant_info->top_type);
				}

				if (!is_swith_line && line1_point_list->size > 0) {

					if (is_start_angle_45 && draw_circle_arc_width_get_right_angled_point_r_y(line1_point_list, &line_list1_last_point, iter->x, &y) == NULL) {
						is_swith_line = TRUE;
						point1_list_45_last_point = point1_list_45->size - 1;
					}
					else if (!is_start_angle_45 && draw_circle_arc_width_get_right_angled_point_r_x(line1_point_list, &line_list1_last_point, iter->x, &y) == NULL)
					{
						is_swith_line = TRUE;
					}
				}

				if (is_swith_line || line1_point_list->size == 0) {
					if (!is_l && iter->x <= point1_head->y && draw_circle_arc_width_get_right_angled_point_r_x(point1_list_45, &point1_list_45_last_point, iter->x, &y) == NULL) {
						is_l = TRUE;
						is_angle_45 = FALSE;
						point1_list_45_last_point = point1_list_45->size - 1;
					}
					if (is_l) {
						draw_circle_arc_width_get_right_angled_point_l_y(point1_list_45, &point1_list_45_last_point, iter->x, &y);
					}
				}

				w = iter->y - y;

				draw_circle_draw_circluar_point_to_quadrant_hline(image_info, w, iter->x, iter->y, is_left, type);
				if (w > 0) {
					draw_circle_draw_circluar_point_to_quadrant_point(image_info, iter->x, iter->y, 1, is_left, type);
					if (i - 1 >= 0) {
						draw_circle_draw_circluar_point_to_quadrant_points_e_n(image_info, line2_point_list, i - 1, !is_left, FALSE, type);
					}
				}
				
			}
		}

		if (i < line2_point_list->size) {
			iter = (frr_point_pix_t *)darray_get(line2_point_list, i);
			draw_circle_draw_circluar_point_to_quadrant_point(image_info, iter->x, iter->y, 1 - iter->e, is_left, type);
			if (i - 1 >= 0) {
				draw_circle_draw_circluar_point_to_quadrant_points_e_n(image_info, line2_point_list, i - 1, !is_left, FALSE, type);
			}
		}

	}
}

static void draw_circle_arc_width_angle(frr_draw_image_info_t *image_info, int32_t start_angle, int32_t end_angle, quadrant_info_t* quadrant_info) {

	uint32_t x = 0;
	uint32_t y = 0;
	uint32_t i = 0;
	uint32_t w = 0;
	float_t x1_2 = 0.0f;
	float_t x2_2 = 0.0f;

	draw_circle_t *ctx = image_info->ctx;

	frr_line_point_pix_t *point_pix_cache1_list = NULL;
	frr_line_point_pix_t *point_pix_cache2_list = NULL;

	darray_t *line1_point_list = &ctx->line1_point_list;
	darray_t *line2_point_list = &ctx->line2_point_list;

	uint32_t line_list1_last_point = 0;
	uint32_t line_list2_last_point = 0;
	uint32_t point1_list_45_last_point = 0;

	bool_t is_end_angle_90 = end_angle == 90;
	bool_t is_start_angle_0 = start_angle == 0;

	bool_t is_end_angle_45 = end_angle <= 45;
	bool_t is_start_angle_45 = start_angle >= 45;

	if (end_angle == start_angle) {
		return;
	}

	line1_point_list->size = 0;
	line2_point_list->size = 0;

	if (!is_end_angle_90) {
		point_pix_cache2_list = draw_circle_get_line_point_list(end_angle, ctx, is_end_angle_45, &x2_2, line2_point_list, frr_line_point_pix_cache2_list);
		if (point_pix_cache2_list == NULL) {
			return;
		}
	}

	if (!is_start_angle_0) {
		point_pix_cache1_list = draw_circle_get_line_point_list(start_angle, ctx, !is_start_angle_45, &x1_2, line1_point_list, frr_line_point_pix_cache1_list);
		if (point_pix_cache1_list == NULL) {
			return;
		}
	}

	if (!is_start_angle_45) {
		draw_circle_arc_width_angle_top(image_info, quadrant_info, is_end_angle_45, x1_2, x2_2);
	}

	if (!is_end_angle_45) {
		draw_circle_arc_width_angle_bottom(image_info, quadrant_info, is_start_angle_45, x1_2, x2_2);
	}

	if (point_pix_cache1_list != NULL && 
		point_pix_cache1_list != (frr_line_point_pix_t*)frr_line_point_pix_cache1_list) {
		TKMEM_FREE(point_pix_cache1_list);
	}
	if (point_pix_cache2_list != NULL && 
		point_pix_cache2_list != (frr_line_point_pix_t*)frr_line_point_pix_cache2_list) {
		TKMEM_FREE(point_pix_cache2_list);
	}
}

static void draw_circle_arc_width_right_angled(frr_draw_image_info_t *image_info, quadrant_info_t* quadrant_info) {

	int32_t y = 0;
	int32_t x = 0;
	int32_t w = 0;
	int32_t i = 0;
	int32_t last_y = 0;
	bool_t is_l = FALSE;
	bool_t is_draw_line = TRUE;

	draw_circle_t *ctx = image_info->ctx;
	darray_t *point1_list_45 = &ctx->point1_list_45;
	darray_t *point2_list_45 = &ctx->point2_list_45;

	uint32_t point1_list_45_last_point = 0;
	frr_point_pix_t *point1_list_45_iter = NULL;
	frr_point_pix_t *point1_head = (frr_point_pix_t *)darray_head(point1_list_45);

	frr_quadrant_t top_type = quadrant_info->top_type;
	frr_quadrant_t bottom_type = quadrant_info->bottom_type;

	bool_t is_top_left =
		top_type == FRR_QUADRANT_FIRST_F || top_type == FRR_QUADRANT_FIRST_S ||
		top_type == FRR_QUADRANT_FOURTH_F || top_type == FRR_QUADRANT_FOURTH_S;

	bool_t is_bottom_left =
		bottom_type == FRR_QUADRANT_FIRST_F || bottom_type == FRR_QUADRANT_FIRST_S ||
		bottom_type == FRR_QUADRANT_FOURTH_F || bottom_type == FRR_QUADRANT_FOURTH_S;

	bool_t is_bottom_top = bottom_type == FRR_QUADRANT_FIRST_S || bottom_type == FRR_QUADRANT_SECOND_S;

	for (i = 0; i < point2_list_45->size - 1; i++) {
		frr_point_pix_t *iter = (frr_point_pix_t *)darray_get(point2_list_45, i);
		frr_point_pix_t *iter_next = (frr_point_pix_t *)darray_get(point2_list_45, i + 1);
		if (is_draw_line) {
			is_draw_line = FALSE;
			if (iter->y <= point1_head->y) {
				if (point1_list_45_iter != NULL) {
					draw_circle_draw_circluar_point_to_quadrant_points(image_info, point1_list_45, point1_list_45_last_point, is_top_left, TRUE, top_type);
				}
				point1_list_45_iter = draw_circle_arc_width_get_right_angled_point_r_x(point1_list_45, &point1_list_45_last_point, iter->y, &x);
				w = iter->x - x;
			}
			else {
				w = iter->x;
			}

			last_y = iter->y;
			draw_circle_draw_circluar_point_to_quadrant_hline(image_info, w, iter->x, iter->y, is_top_left, top_type);
		}

		draw_circle_draw_circluar_point_to_quadrant_point(image_info, iter->x, iter->y, 1 - iter->e, is_top_left, top_type);

		if (iter->y != iter_next->y) {
			is_draw_line = TRUE;
		}
	}

	for (i = point2_list_45->size - 1; i >= 0; i--) {
		frr_point_pix_t *iter = (frr_point_pix_t *)darray_get(point2_list_45, i);

		if (last_y <= iter->x) {
			continue;
		}

		if (point1_list_45_iter != NULL) {
			bool_t is_angle_45 = TRUE;
			frr_quadrant_t type = top_type;
			if (is_l) {
				type = bottom_type;
				is_angle_45 = FALSE;
			}
			draw_circle_draw_circluar_point_to_quadrant_points(image_info, point1_list_45, point1_list_45_last_point, is_bottom_left, is_angle_45, type);
		}

		if (iter->x <= point1_head->y) {
			point1_list_45_iter = draw_circle_arc_width_get_right_angled_point_r_x(point1_list_45, &point1_list_45_last_point, iter->x, &y);
			if (!is_l && point1_list_45_iter == NULL) {
				is_l = TRUE;
			}
			if (is_l) {
				point1_list_45_iter = draw_circle_arc_width_get_right_angled_point_l_y(point1_list_45, &point1_list_45_last_point, iter->x, &y);
			}

			w = iter->y - y;
		}
		else {
			w = iter->y;
		}
		draw_circle_draw_circluar_point_to_quadrant_hline(image_info, w, iter->x, iter->y, is_bottom_left, bottom_type);
		draw_circle_draw_circluar_point_to_quadrant_point(image_info, iter->x, iter->y, 1 - iter->e, is_bottom_left, bottom_type);
		if (is_bottom_top && i == 1) {
			break;
		}
	}

}

static void draw_circle_draw_circle_arc_width(frr_draw_image_info_t *image_info, quadrant_info_t* quadrant_info) {

	if (quadrant_info->start_angle == 0 && quadrant_info->end_angle == 90 && quadrant_info->is_one) {
		draw_circle_arc_width_right_angled(image_info, quadrant_info);
	}
	else if (!quadrant_info->is_one) {
		draw_circle_arc_width_angle(image_info, 0, quadrant_info->start_angle, quadrant_info);
		draw_circle_arc_width_angle(image_info, quadrant_info->end_angle, 90, quadrant_info);
	}
	else {
		draw_circle_arc_width_angle(image_info, quadrant_info->start_angle, quadrant_info->end_angle, quadrant_info);
	}
}

void draw_circle_fill_arc_width(draw_circle_t *ctx, canvas_t *c, int32_t xc,
	int32_t yc, int32_t start_angle,
	int32_t end_angle, bool_t is_ccw,
	rgba_t *color) {
	if (ctx != NULL && start_angle != end_angle) {
		int32_t i = 0;
		frr_draw_image_info_t image_info;
		quadrant_info_t quadrant_info_list[4];
		color_t tmp_color = color_init(color->r, color->g, color->b, color->a);

		memset(quadrant_info_list, 0, sizeof(quadrant_info_list));

		canvas_set_fill_color(c, tmp_color);

		ffr_image_info_create(&image_info.image_info, c);

		image_info.c = c;
		image_info.xc = xc;
		image_info.yc = yc;
		image_info.ctx = ctx;
		image_info.color = color;

		if (draw_circle_get_quadrant_angle_info_list(quadrant_info_list, start_angle,
			end_angle, is_ccw)) {
			for (; i < QUADRANT_COUNT; i++) {
				if (quadrant_info_list[i].start_angle != 0 ||
					quadrant_info_list[i].end_angle != 0) {
					draw_circle_draw_circle_arc_width(&image_info, &quadrant_info_list[i]);
				}
			}
		}
	}
}
