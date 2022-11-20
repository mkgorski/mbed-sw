#ifndef _MBED_COMMON_H_
#define _MBED_COMMON_H_

#include <stddef.h>

#ifndef NULL
  #define NULL 0
#endif

#ifndef bool
  #define bool uint8_t
  #define true (1)
  #define false (0)
#endif

#define CONTAINER_OF(ptr, type, field) \
	((type *)(((char *)(ptr)) - offsetof(type, field)))

#endif /* _MBED_COMMON_H_ */