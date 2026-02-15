#include "local_clock.h"
#include <stdlib.h>

static int64_t
local_next_time(instrumentation_clock_t* self, int64_t clock_value) {
	(void)self;
	return clock_value;
}

static void
local_destroy(instrumentation_clock_t* self) {
	if (!self)
		return;
	free(self);
}

instrumentation_clock_t*
local_clock_create(clock_error_t* err) {
	if (err) *err = CLOCK_OK;
	instrumentation_clock_t* c =
		(instrumentation_clock_t*)calloc(1, sizeof(*c));
	if (!c) { if (err) *err = CLOCK_ERR_NOMEM; return NULL; }
	c->get_next_time = local_next_time;
	c->destroy = local_destroy;
	c->impl = NULL;
	return c;
}
