#ifndef LS_UTILS_H_
#define LS_UTILS_H_

#include <stddef.h>
#define container_of(ptr, type, member) ((type *)((char *) (ptr) - offsetof(type, member)))

#include "ls_logger.h"

#endif
