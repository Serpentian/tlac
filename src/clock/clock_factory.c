#include "clock_factory.h"
#include "local_clock.h"
#include "memory_clock.h"
#include "file_clock.h"
#include "client_clock.h"

#include <stdlib.h>
#include <string.h>

instrumentation_clock_t*
clock_factory_create(clock_type_t type, int namec,
		     const char** namev, clock_error_t* err) {
    switch (type) {
        case CLOCK_LOCAL:
            return local_clock_create(err);
        case CLOCK_MEMORY:
            return memory_clock_create(err);
        case CLOCK_FILE: {
            const char* path =
		(namec >= 1 && namev && namev[0] && namev[0][0])
			? namev[0]
			: "default_clock.mmap";
            return file_clock_create(path, err);
        }
        case CLOCK_SERVER: {
            const char* host =
		(namec >= 1 && namev && namev[0] && namev[0][0])
			? namev[0]
			: "localhost";
            int port = 6666;
            if (namec >= 2 && namev && namev[1]) port = atoi(namev[1]);
            return client_clock_create(host, port, err);
        }
        default:
            return memory_clock_create(err);
    }
}
