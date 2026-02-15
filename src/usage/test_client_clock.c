#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "clock/client_clock.h"
#include "clock/clock.h"

int main(int argc, char** argv) {
	if (argc < 4) {
		printf("Usage: TestClientClock <id> <host> <port>\n");
		return 0;
	}

	const char* id = argv[1];
	const char* host = argv[2];
	int port = atoi(argv[3]);

	printf("Starting client clock for %s on %s:%d\n", id, host, port);

	clock_error_t err = CLOCK_OK;
	instrumentation_clock_t* clk = client_clock_create(host, port, &err);
	if (!clk) {
		printf("Error while creating client clock: %s\n", clock_error_str(err));
		return 1;
	}

	srand((unsigned int)time(NULL));
	int64_t clock = 0;

	while (1) {
		clock = clock_get_next_time(clk, clock);
		printf("Next time for %s : %lld\n", id, (long long)clock);
		struct timespec ts;
		ts.tv_sec = 0;
		ts.tv_nsec = (long)((rand() % 1000) * 1000000L);
		nanosleep(&ts, NULL);
	}

	clk->destroy(clk);
	return 0;
}
