#ifndef LOCAL_CLOCK_H
#define LOCAL_CLOCK_H

#include "clock.h"
#include "clock_exception.h"

#ifdef __cplusplus
extern "C" {
#endif

instrumentation_clock_t*
local_clock_create(clock_error_t* err);

#ifdef __cplusplus
}
#endif

#endif
