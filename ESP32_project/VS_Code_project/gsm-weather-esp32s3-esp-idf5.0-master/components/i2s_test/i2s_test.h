#ifndef __i2s_test_H
#define __i2s_test_H
#include "main.h"

#if 1
  #define i2s_test_DEBUG(format, ...) my_debug("[i2s_test]- ",format,##__VA_ARGS__);
#else
  #define i2s_test_DEBUG(format, ...) ;
#endif

void i2s_test(void);


#endif
















