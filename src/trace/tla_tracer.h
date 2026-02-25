#ifndef TLA_TRACER_H
#define TLA_TRACER_H

#include <stdint.h>
#include "cJSON.h"
#include "clock/clock.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tla_tracer tla_tracer_t;

/* Creates tracer writing to trace_path. Takes ownership of clock (may be NULL).
   If clock is NULL, behaves like LOCAL clock (returns same clock value). */
tla_tracer_t*
tla_tracer_create(const char* trace_path, tlac_clock_t* clock);

/* Flushes and releases all resources. */
void
tla_tracer_destroy(tla_tracer_t* t);

/* Thread-safe notifyChange. path_array and args_array must be JSON arrays (or NULL -> empty arrays). */
void
tla_tracer_notify_change(tla_tracer_t* t, const char* variable,
			 const cJSON* path_array, const char* op,
			 const cJSON* args_array);

/* Convenience: logs an exception event. */
int64_t
tla_tracer_log_exception(tla_tracer_t* t, const char* desc);

/* Log accumulated changes. event_args_array can be NULL or JSON array.
   clock_value is the local clock to synchronize with (Java version uses 0 by default).
   desc may be NULL/empty.
   Returns the clock value used (or -1 on error). */
int64_t
tla_tracer_log(tla_tracer_t* t, const char* event_name,
	       const cJSON* event_args_array, int64_t clock_value,
	       const char* desc);

/* Convenience overloads */
int64_t
tla_tracer_log0(tla_tracer_t* t);
int64_t
tla_tracer_log_event(tla_tracer_t* t, const char* event_name);
int64_t
tla_tracer_log_event_desc(tla_tracer_t* t, const char* event_name, const char* desc);

#ifdef __cplusplus
}
#endif

#endif
