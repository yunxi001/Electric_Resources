#ifndef __bsp_i2s_H
#define __bsp_i2s_H
#include "main.h"

#if 1
  #define bsp_i2s_DEBUG(format, ...) my_debug("[bsp_i2s]- ",format,##__VA_ARGS__);
#else
  #define bsp_i2s_DEBUG(format, ...) ;
#endif


#define I2S_NUM    I2S_NUM_0

esp_err_t bsp_i2s_init(void);

#endif
















