#ifndef TLAC_MEMORY_CLOCK_H
#define TLAC_MEMORY_CLOCK_H

#include "clock.h"
#include "clock_exception.h"

#ifdef __cplusplus
extern "C" {
#endif

tlac_clock_t*
memory_clock_create(clock_error_t* err);

#ifdef __cplusplus
}
#endif

#endif
