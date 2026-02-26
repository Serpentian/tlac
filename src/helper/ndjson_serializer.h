#ifndef NDJSON_SERIALIZER_H
#define NDJSON_SERIALIZER_H

#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

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
