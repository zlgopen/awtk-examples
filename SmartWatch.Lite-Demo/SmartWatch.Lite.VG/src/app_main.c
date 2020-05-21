#include "awtk.h"

#if LCD_WIDTH == 400
#include "../res/assets_default.inc"
#else
#include "../res/assets_default_320_240.inc"
#endif // LCD_WIDTH == 400

extern ret_t application_init(void);
extern ret_t application_exit(void);

#include "awtk_main.inc"
