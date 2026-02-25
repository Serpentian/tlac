#include "memory_clock.h"
#include <stdlib.h>
#include <pthread.h>

typedef struct {
	int64_t value;
	pthread_mutex_t mu;
} memory_clock_impl_t;

static int64_t
memory_next_time(tlac_clock_t* self, int64_t clock_value) {
	memory_clock_impl_t* impl = (memory_clock_impl_t*)self->impl;
	pthread_mutex_lock(&impl->mu);
	int64_t base = impl->value;
	int64_t new_value = (base > clock_value ? base : clock_value) + 1;
	impl->value = new_value;
	pthread_mutex_unlock(&impl->mu);
	return new_value;
}

static void
memory_destroy(tlac_clock_t* self) {
	if (!self) return;
	memory_clock_impl_t* impl = (memory_clock_impl_t*)self->impl;
	if (impl) {
		pthread_mutex_destroy(&impl->mu);
		free(impl);
	}
	free(self);
}

tlac_clock_t*
memory_clock_create(clock_error_t* err) {
	if (err) *err = CLOCK_OK;

	tlac_clock_t* c = (tlac_clock_t*)calloc(1, sizeof(*c));
	if (!c) {
		if (err) *err = CLOCK_ERR_NOMEM;
		return NULL;
	}

	memory_clock_impl_t* impl =
		(memory_clock_impl_t*)calloc(1, sizeof(*impl));
	if (!impl) {
		free(c);
		if (err) *err = CLOCK_ERR_NOMEM;
		return NULL;
	}

	impl->value = 0;
	pthread_mutex_init(&impl->mu, NULL);

	c->next_time = memory_next_time;
	c->destroy = memory_destroy;
	c->type = CLOCK_MEMORY;
	c->impl = impl;
	return c;
}
