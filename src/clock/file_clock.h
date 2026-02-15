#ifndef FILE_CLOCK_H
#define FILE_CLOCK_H

#include "clock.h"
#include "clock_exception.h"

#ifdef __cplusplus
extern "C" {
#endif

instrumentation_clock_t*
file_clock_create(const char* path, clock_error_t* err);

#ifdef __cplusplus
}
#endif

#endif
