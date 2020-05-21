#include "watch_main_02.h"

#define WEEK_LABEL "week_label"
#define WEEK_LABEL1 "week_label1"
#define MUSIC_BUTTON "music_button"
#define WEATHCER_LABEL "weather_label"

#define UPDATE_TIME 1000

typedef struct _watch_main_02_t {
  widget_t *clock;

  widget_t *week_label;
  widget_t *week_label1;
  widget_t *weather_label;
  widget_t *music_button;

  int32_t n_music;

  uint32_t timer_id;
} watch_main_02_t;

const char *STR_WEEK[] = {"Sun.",  "Mon.", "Tues.", "Wed.",
                          "Thur.", "Fri.", "Sat."};

static void watch_main_02_update_data(watch_main_02_t *handle) {
  char tmp[10];
  date_time_t dt;

  handle->n_music += 20;
  if (handle->n_music >= 1000) {
    handle->n_music = 0;
  }
  music_play_set_value(handle->music_button, handle->n_music);

  if (date_time_init(&dt) != NULL) {
    memset(tmp, 0, sizeof(tmp));
    tk_snprintf(tmp, 10, "%d", dt.wday);
    widget_set_text_utf8(handle->week_label, tmp);
    if (0 <= dt.wday && dt.wday <= 6) {
      widget_set_text_utf8(handle->week_label1, STR_WEEK[dt.wday]);
    }
  }
}

static ret_t watch_main_02_on_update_data(const timer_info_t *timer) {
  watch_main_02_t *handle = (watch_main_02_t *)timer->ctx;

  if (handle->timer_id != timer->id) {
    return RET_REMOVE;
  }

  if (handle->timer_id != TK_INVALID_ID) {
    watch_main_02_update_data(handle);
  }

  return RET_REPEAT;
}

static ret_t watch_main_02_widget_remove_timer_on_destroy(void *ctx,
                                                          event_t *e) {
  watch_main_02_t *handle = (watch_main_02_t *)ctx;

  handle->week_label = NULL;
  handle->week_label1 = NULL;
  handle->music_button = NULL;
  handle->weather_label = NULL;

  timer_remove(handle->timer_id);
  handle->timer_id = TK_INVALID_ID;

  return RET_REMOVE;
}

void *watch_main_02_create() { return TKMEM_ZALLOC(watch_main_02_t); }

void watch_main_02_anim_end(void *handle) {
  watch_main_02_t *h = (watch_main_02_t *)handle;
  if (h->clock != NULL) {
    watch_clock_start_animator(h->clock);
  }
}

void watch_main_02_init(void *handle, widget_t *load_widget) {
  watch_main_02_t *h = (watch_main_02_t *)handle;

  h->clock = widget_lookup_by_type(load_widget, WIDGET_TYPE_WATCH_CLOCK, TRUE);

  h->week_label = widget_lookup(load_widget, WEEK_LABEL, TRUE);
  h->week_label1 = widget_lookup(load_widget, WEEK_LABEL1, TRUE);
  h->music_button = widget_lookup(load_widget, MUSIC_BUTTON, TRUE);
  h->weather_label = widget_lookup(load_widget, WEATHCER_LABEL, TRUE);
  return_if_fail(h->week_label != NULL && h->weather_label != NULL &&
                 h->music_button != NULL);

  h->timer_id = timer_add(watch_main_02_on_update_data, handle, UPDATE_TIME);

  widget_on(load_widget, EVT_DESTROY,
            watch_main_02_widget_remove_timer_on_destroy, handle);

  watch_main_02_update_data(h);
}

void watch_main_02_close(void *handle) {
  if (handle != NULL) {
    TKMEM_FREE(handle);
  }
}
