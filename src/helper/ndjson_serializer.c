#include "ndjson_serializer.h"
#include <stdlib.h>

cJSON* ndjson_string(const char* s) { return cJSON_CreateString(s ? s : ""); }
cJSON* ndjson_int64(long long v)    { return cJSON_CreateNumber((double)v); }
cJSON* ndjson_double(double v)      { return cJSON_CreateNumber(v); }
cJSON* ndjson_bool(int b)           { return cJSON_CreateBool(b ? 1 : 0); }
cJSON* ndjson_null(void)            { return cJSON_CreateNull(); }

cJSON* ndjson_deep_dup(const cJSON* v) {
	if (!v) return NULL;
	return cJSON_Duplicate((cJSON*)v, 1);
}

cJSON* ndjson_array_of(const cJSON* const* values, int n) {
	cJSON* arr = cJSON_CreateArray();
	if (!arr) return NULL;

	for (int i = 0; i < n; i++) {
		cJSON* dup = ndjson_deep_dup(values[i]);
		if (!dup) dup = cJSON_CreateNull();
		cJSON_AddItemToArray(arr, dup);
	}
	return arr;
}
