#pragma once
//#define LOG_ENABLE

void log_print(const char* _file, const char* _func, int _line, const char* _fmt, ...);

#ifdef LOG_ENABLE
#define LOG(_fmt, ...) log_print(__FILE__, __FUNCTION__, __LINE__, _fmt, ##__VA_ARGS__);
#else
#define LOG(_fmt, ...)
#endif  // LOG_ENABLE
