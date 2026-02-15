#include "client_clock.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef struct {
	int sockfd;
} client_clock_impl_t;

static int
connect_tcp(const char* host, int port) {
	char port_str[32];
	snprintf(port_str, sizeof(port_str), "%d", port);

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo* res = NULL;
	if (getaddrinfo(host, port_str, &hints, &res) != 0) return -1;

	int fd = -1;
	for (struct addrinfo* p = res; p; p = p->ai_next) {
		fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (fd < 0) continue;
		if (connect(fd, p->ai_addr, p->ai_addrlen) == 0) {
			freeaddrinfo(res);
			return fd;
		}
		close(fd);
		fd = -1;
	}
	freeaddrinfo(res);
	return -1;
}

static int
send_line(int fd, const char* line) {
	size_t n = strlen(line);
	while (n > 0) {
		ssize_t w = send(fd, line, n, 0);
		if (w < 0) return -1;
		line += (size_t)w;
		n -= (size_t)w;
	}
	return 0;
}

static int
recv_line(int fd, char* buf, size_t cap) {
	size_t i = 0;
	while (i + 1 < cap) {
		char c;
		ssize_t r = recv(fd, &c, 1, 0);
		if (r == 0) return -1;
		if (r < 0) return -1;
		if (c == '\n') break;
		buf[i++] = c;
	}
	buf[i] = '\0';
	return 0;
}

static int64_t
client_next_time(instrumentation_clock_t* self, int64_t clock_value) {
	client_clock_impl_t* impl = (client_clock_impl_t*)self->impl;

	char outbuf[64];
	snprintf(outbuf, sizeof(outbuf), "%lld\n", (long long)clock_value);

	if (send_line(impl->sockfd, outbuf) != 0) {
		printf("Error while getting next time: send failed\n");
		return -1;
	}

	char inbuf[64];
	if (recv_line(impl->sockfd, inbuf, sizeof(inbuf)) != 0) {
		printf("Error while getting next time: recv failed\n");
		return -1;
	}

	char* end = NULL;
	long long v = strtoll(inbuf, &end, 10);
	if (!end || *end != '\0') {
		printf("Error while getting next time: invalid number\n");
		return -1;
	}
	return (int64_t)v;
}

static void
client_destroy(instrumentation_clock_t* self) {
	if (!self) return;
	client_clock_impl_t* impl = (client_clock_impl_t*)self->impl;
	if (impl) {
		if (impl->sockfd >= 0) close(impl->sockfd);
		free(impl);
	}
	free(self);
}

instrumentation_clock_t*
client_clock_create(const char* host, int port, clock_error_t* err) {
	if (err) *err = CLOCK_OK;
	if (!host || !host[0] || port <= 0) {
		if (err) *err = CLOCK_ERR_INVALID_ARGUMENT;
		return NULL;
	}

	int fd = connect_tcp(host, port);
	if (fd < 0) { if (err) *err = CLOCK_ERR_NETWORK; return NULL; }

	instrumentation_clock_t* c =
		(instrumentation_clock_t*)calloc(1, sizeof(*c));
	if (!c) { close(fd); if (err) *err = CLOCK_ERR_NOMEM; return NULL; }

	client_clock_impl_t* impl =
		(client_clock_impl_t*)calloc(1, sizeof(*impl));
	if (!impl) {
		free(c);
		close(fd);
		if (err) *err = CLOCK_ERR_NOMEM;
		return NULL;
	}

	impl->sockfd = fd;
	c->get_next_time = client_next_time;
	c->destroy = client_destroy;
	c->impl = impl;
	return c;
}
