#include "animator.h"
#include <stdio.h>

ret_t widget_create_animator_with(widget_t* widget, const char* animation, ...) {
  va_list arg;
  va_start(arg, animation);
  char str[1024];
  memset(str, '\0', sizeof(str));
  int ret = vsnprintf(str, sizeof(str), animation, arg);
  va_end(arg);

  if (ret == sizeof(str) || ret < 0) {
    return RET_BAD_PARAMS;
  }

  return widget_create_animator(widget, str);
}
