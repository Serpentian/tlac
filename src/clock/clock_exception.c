#include "clock_exception.h"

const char*
clock_error_str(clock_error_t err) {
	switch (err) {
		case CLOCK_OK: return "OK";
		case CLOCK_ERR_INVALID_ARGUMENT: return "Invalid argument";
		case CLOCK_ERR_IO: return "I/O error";
		case CLOCK_ERR_NETWORK: return "Network error";
		case CLOCK_ERR_NOMEM: return "Out of memory";
		case CLOCK_ERR_INTERNAL: return "Internal error";
		default: return "Unknown error";
	}
}
