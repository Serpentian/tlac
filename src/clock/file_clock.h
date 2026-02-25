#ifndef TLAC_FILE_CLOCK_H
#define TLAC_FILE_CLOCK_H

#include "clock.h"
#include "clock_exception.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A file clock that can be shared through multiple processes. Clock value is
 * stored as a memory file map and can be accessed by different processes on the
 * same hardware.
 */
tlac_clock_t*
file_clock_create(const char* path, clock_error_t* err);

#ifdef __cplusplus
}
#endif

#endif
