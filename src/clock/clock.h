#ifndef INSTRUMENTATION_CLOCK_H
#define INSTRUMENTATION_CLOCK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct instrumentation_clock
instrumentation_clock_t;

typedef int64_t
(*clock_next_time_fn)(instrumentation_clock_t* self, int64_t clock_value);

typedef void
(*clock_destroy_fn)(instrumentation_clock_t* self);

struct instrumentation_clock {
	clock_next_time_fn get_next_time;
	clock_destroy_fn destroy;
	void* impl;
};

static inline int64_t
clock_get_next_time(instrumentation_clock_t* c, int64_t clock_value) {
	return c && c->get_next_time ? c->get_next_time(c, clock_value) : -1;
}

static inline int64_t
clock_get_next_time_default(instrumentation_clock_t* c) {
	return clock_get_next_time(c, 0);
}

#ifdef __cplusplus
}
#endif

#endif
