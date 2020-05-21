#include "watch_main_01.h"

#define POW_BUTTON "pow_button"
#define TRAINING_BUTTON "training_button"

#define POW_LABEL "pow_label"
#define WEATHER_LABEL "weather_label"
#define TRAINING_LABEL "training_label"

#define UPDATE_TIME 1000

typedef struct _watch_main_01_t {
  int32_t pow;
  int32_t training;

  widget_t *clock;

  widget_t *pow_label;
  widget_t *training_label;

  widget_t *pow_button;
  widget_t *training_button;

  widget_t *watch_clock;

  uint32_t timer_id;
} watch_main_01_t;

static void watch_main_01_update_data(watch_main_01_t *handle) {
  char tmp[10];

  handle->pow += 5;
  if (handle->pow >= 100) {
    handle->pow = 0;
  }
  memset(tmp, 0, sizeof(tmp));
  tk_snprintf(tmp, 10, "%d", handle->pow);
  widget_set_text_utf8(handle->pow_label, tmp);
  music_play_set_value(handle->pow_button, handle->pow);

  handle->training += 20;
  if (handle->training >= 1000) {
    handle->training = 0;
  }
  memset(tmp, 0, sizeof(tmp));
  tk_snprintf(tmp, 10, "%d", handle->training);
  widget_set_text_utf8(handle->training_label, tmp);
  music_play_set_value(handle->training_button, handle->training);
}

static ret_t watch_main_01_on_update_data(const timer_info_t *timer) {
  watch_main_01_t *handle = (watch_main_01_t *)timer->ctx;

  if (handle->timer_id != TK_INVALID_ID) {
    watch_main_01_update_data(handle);
  }

  return RET_REPEAT;
}

static ret_t watch_main_01_widget_remove_timer_on_destroy(void *ctx,
                                                          event_t *e) {
  watch_main_01_t *handle = (watch_main_01_t *)ctx;

  handle->pow_label = NULL;
  handle->pow_button = NULL;
  handle->training_button = NULL;
  handle->training_label = NULL;

  timer_remove(handle->timer_id);
  handle->timer_id = TK_INVALID_ID;

  return RET_REMOVE;
}

void *watch_main_01_create() { return TKMEM_ZALLOC(watch_main_01_t); }

void watch_main_01_anim_end(void *handle) {
  watch_main_01_t *h = (watch_main_01_t *)handle;
  if (h->clock != NULL) {
    watch_clock_start_animator(h->clock);
  }
}

void watch_main_01_init(void *handle, widget_t *load_widget) {
  watch_main_01_t *h = (watch_main_01_t *)handle;

  h->clock = widget_lookup_by_type(load_widget, WIDGET_TYPE_WATCH_CLOCK, TRUE);

  h->pow_button = widget_lookup(load_widget, POW_BUTTON, TRUE);
  h->training_button = widget_lookup(load_widget, TRAINING_BUTTON, TRUE);
  h->pow_label = widget_lookup(load_widget, POW_LABEL, TRUE);
  h->training_label = widget_lookup(load_widget, TRAINING_LABEL, TRUE);
  return_if_fail(h->clock != NULL && h->pow_button != NULL &&
                 h->training_button != NULL && h->pow_label != NULL &&
                 h->training_label != NULL);

  h->timer_id = timer_add(watch_main_01_on_update_data, handle, UPDATE_TIME);

  widget_on(load_widget, EVT_DESTROY,
            watch_main_01_widget_remove_timer_on_destroy, handle);

  watch_main_01_update_data(h);
}

void watch_main_01_close(void *handle) {
  if (handle != NULL) {
    TKMEM_FREE(handle);
  }
}
