#ifndef CLIENT_CLOCK_H
#define CLIENT_CLOCK_H

#include "clock.h"
#include "clock_exception.h"

#ifdef __cplusplus
extern "C" {
#endif

instrumentation_clock_t*
client_clock_create(const char* host, int port, clock_error_t* err);

#ifdef __cplusplus
}
#endif

#endif
