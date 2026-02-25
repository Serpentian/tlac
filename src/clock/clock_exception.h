#ifndef TLAC_CLOCK_EXCEPTION_H
#define TLAC_CLOCK_EXCEPTION_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum clock_error {
	CLOCK_OK = 0,
	CLOCK_ERR_INVALID_ARGUMENT = 1,
	CLOCK_ERR_IO = 2,
	CLOCK_ERR_NOMEM = 3,
	CLOCK_ERR_INTERNAL = 4
} clock_error_t;

const char*
clock_error_str(clock_error_t err);

#ifdef __cplusplus
}
#endif

#endif
