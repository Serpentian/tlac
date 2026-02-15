#ifndef CLOCK_FACTORY_H
#define CLOCK_FACTORY_H

#include "clock.h"
#include "clock_exception.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum clock_type {
	CLOCK_LOCAL  = 0,
	CLOCK_MEMORY = 1,
	CLOCK_FILE   = 2,
	CLOCK_SERVER = 3
} clock_type_t;

/* For CLOCK_FILE: name0 = file path (optional, default "default_clock.mmap")
   For CLOCK_SERVER: name0 = host (optional, default "localhost"),
   		     name1 = port string (optional, default "6666") */
instrumentation_clock_t*
clock_factory_create(clock_type_t type, int namec,
		     const char** namev, clock_error_t* err);

#ifdef __cplusplus
}
#endif

#endif
