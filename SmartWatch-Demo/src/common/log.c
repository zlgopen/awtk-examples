#include "log.h"
#include <awtk.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// 复制src最后最多count个字符到dst，调用者确保dst有足够空间
static void strtail(char* dst, const char* src, size_t count) {
  if (dst == NULL && src == NULL) {
    return;
  }

  size_t size = strlen(src);
  if (count >= size) {
    strncpy(dst, src, size);
  } else {
    strncpy(dst, src + size - count, count);
  }
}

void log_print(const char* _file, const char* _func, int _line, const char* _fmt, ...) {
  char str[64];
  int size = 20;
  static uint32_t max_mem = 0;

  // 时间日期
  date_time_t date_time;
  date_time_init(&date_time);
  printf("[%02d-%02d-%02d] [%02d:%02d:%02d] ", (int)date_time.year % 100, (int)date_time.month,
         (int)date_time.day, (int)date_time.hour, (int)date_time.minute, (int)date_time.second);

  // 文件名
  memset(str, '\0', sizeof(str));
  strtail(str, _file, size);
  printf("[%*.*s] ", size, size, str);

  // 其他参数
  va_list arg;
  va_start(arg, _fmt);

  vprintf(_fmt, arg);
  printf("\r\n");

  va_end(arg);
}
