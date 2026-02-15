#ifndef NDJSON_SERIALIZER_H
#define NDJSON_SERIALIZER_H

#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Creates a deep-copied cJSON array from an array of cJSON* values.
   Each element is duplicated; input elements remain owned by the caller. */
cJSON*
ndjson_array_of(const cJSON* const* values, int n);

/* Convenience creators */
cJSON*
ndjson_string(const char* s);
cJSON*
ndjson_int64(long long v);
cJSON*
ndjson_double(double v);
cJSON*
ndjson_bool(int b);
cJSON*
ndjson_null(void);

/* Deep-duplicate any cJSON value (portable helper). */
cJSON*
ndjson_deep_dup(const cJSON* v);

#ifdef __cplusplus
}
#endif

#endif
