#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef struct {
	int listen_fd;
	pthread_mutex_t mu;
	int64_t clock_value;
} server_state_t;

static int64_t
get_next_time(server_state_t* st, int64_t clock) {
	pthread_mutex_lock(&st->mu);
	int64_t base = st->clock_value;
	int64_t new_value = (base > clock ? base : clock) + 1;
	st->clock_value = new_value;
	pthread_mutex_unlock(&st->mu);
	return new_value;
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

typedef struct {
	int client_fd;
	server_state_t* st;
} handler_args_t;

static void*
handler_thread(void* arg) {
	handler_args_t* a = (handler_args_t*)arg;
	int fd = a->client_fd;
	server_state_t* st = a->st;
	free(a);

	printf("New client connected\n");

	char line[128];
	while (1) {
		if (recv_line(fd, line, sizeof(line)) != 0) break;

		int64_t new_value = -1;
		char* end = NULL;
		long long v = strtoll(line, &end, 10);
		if (end && *end == '\0') {
			new_value = get_next_time(st, (int64_t)v);
		} else {
			printf("Received an invalid clock value\n");
		}

		char out[128];
		snprintf(out, sizeof(out), "%lld\n", (long long)new_value);
		if (send_line(fd, out) != 0) break;
	}

	close(fd);
	return NULL;
}

static int
listen_tcp(int port) {
	char port_str[32];
	snprintf(port_str, sizeof(port_str), "%d", port);

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo* res = NULL;
	if (getaddrinfo(NULL, port_str, &hints, &res) != 0) return -1;

	int fd = -1;
	for (struct addrinfo* p = res; p; p = p->ai_next) {
		fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (fd < 0) continue;

		int yes = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

		if (bind(fd, p->ai_addr, p->ai_addrlen) == 0 && listen(fd, 128) == 0) {
			freeaddrinfo(res);
			return fd;
		}

		close(fd);
		fd = -1;
	}

	freeaddrinfo(res);
	return -1;
}

int
main(int argc, char** argv) {
	if (argc < 2) return 0;
	int port = atoi(argv[1]);
	if (port <= 0) return 0;

	server_state_t st;
	memset(&st, 0, sizeof(st));
	pthread_mutex_init(&st.mu, NULL);
	st.clock_value = 0;

	st.listen_fd = listen_tcp(port);
	if (st.listen_fd < 0) {
		printf("Can't listen on port\n");
		return 1;
	}

	while (1) {
		struct sockaddr_storage ss;
		socklen_t slen = sizeof(ss);
		int cfd = accept(st.listen_fd, (struct sockaddr*)&ss, &slen);
		if (cfd < 0) {
			printf("Can't accept client\n");
			continue;
		}

		handler_args_t* a = (handler_args_t*)calloc(1, sizeof(*a));
		if (!a) { close(cfd); continue; }
		a->client_fd = cfd;
		a->st = &st;

		pthread_t th;
		if (pthread_create(&th, NULL, handler_thread, a) == 0) {
			pthread_detach(th);
		} else {
			free(a);
			close(cfd);
		}
	}
}
