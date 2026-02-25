#ifndef TLAC_CLOCK_H
#define TLAC_CLOCK_H

#include <assert.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum clock_type {
       CLOCK_MEMORY = 0,
       CLOCK_FILE   = 1,
} tlac_clock_type_t;

typedef struct clock tlac_clock_t;

typedef int64_t
(*clock_next_time_fn)(tlac_clock_t* self, int64_t clock_value);

typedef void
(*clock_destroy_fn)(tlac_clock_t* self);

struct clock {
	clock_next_time_fn next_time;
	clock_destroy_fn destroy;
	tlac_clock_type_t type;
	void* impl;
};

static inline int64_t
clock_next_time(tlac_clock_t* c, int64_t clock_value) {
	assert(c && c->next_time);
	return c->next_time(c, clock_value);
}

static inline void
clock_destroy(tlac_clock_t* c) {
	assert(c && c->destroy);
	c->destroy(c);
}

#ifdef __cplusplus
}
#endif

#endif
