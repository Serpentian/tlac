#include "file_clock.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <errno.h>

typedef struct {
	int fd;
	int64_t* mapped;
	pthread_mutex_t mu;
} file_clock_impl_t;

static int lock_fd(int fd) {
	return flock(fd, LOCK_EX);
}

static int unlock_fd(int fd) {
	return flock(fd, LOCK_UN);
}

static int64_t
file_next_time(instrumentation_clock_t* self, int64_t clock_value) {
	file_clock_impl_t* impl = (file_clock_impl_t*)self->impl;

	pthread_mutex_lock(&impl->mu);
	if (lock_fd(impl->fd) != 0) {
		pthread_mutex_unlock(&impl->mu);
		return -1;
	}

	int64_t current = impl->mapped[0];
	int64_t new_value = (current > clock_value ? current : clock_value) + 1;
	impl->mapped[0] = new_value;

	msync((void*)impl->mapped, sizeof(int64_t), MS_SYNC);
	unlock_fd(impl->fd);
	pthread_mutex_unlock(&impl->mu);
	return new_value;
}

static void
file_destroy(instrumentation_clock_t* self) {
	if (!self) return;
	file_clock_impl_t* impl = (file_clock_impl_t*)self->impl;
	if (impl) {
		if (impl->mapped) munmap((void*)impl->mapped, sizeof(int64_t));
		if (impl->fd >= 0) close(impl->fd);
		pthread_mutex_destroy(&impl->mu);
		free(impl);
	}
	free(self);
}

instrumentation_clock_t*
file_clock_create(const char* path, clock_error_t* err) {
	if (err) *err = CLOCK_OK;
	if (!path || !path[0]) { if (err) *err = CLOCK_ERR_INVALID_ARGUMENT; return NULL; }

	instrumentation_clock_t* c = (instrumentation_clock_t*)calloc(1, sizeof(*c));
	if (!c) { if (err) *err = CLOCK_ERR_NOMEM; return NULL; }

	file_clock_impl_t* impl = (file_clock_impl_t*)calloc(1, sizeof(*impl));
	if (!impl) { free(c); if (err) *err = CLOCK_ERR_NOMEM; return NULL; }

	impl->fd = open(path, O_RDWR | O_CREAT, 0644);
	if (impl->fd < 0) {
		free(impl); free(c);
		if (err) *err = CLOCK_ERR_IO;
		return NULL;
	}

	if (ftruncate(impl->fd, (off_t)sizeof(int64_t)) != 0) {
		close(impl->fd);
		free(impl); free(c);
		if (err) *err = CLOCK_ERR_IO;
		return NULL;
	}

	void* mem = mmap(NULL, sizeof(int64_t),
		  	 PROT_READ | PROT_WRITE, MAP_SHARED, impl->fd, 0);
	if (mem == MAP_FAILED) {
		close(impl->fd);
		free(impl); free(c);
		if (err) *err = CLOCK_ERR_IO;
		return NULL;
	}
	impl->mapped = (int64_t*)mem;

	pthread_mutex_init(&impl->mu, NULL);

	c->get_next_time = file_next_time;
	c->destroy = file_destroy;
	c->impl = impl;
	return c;
}
