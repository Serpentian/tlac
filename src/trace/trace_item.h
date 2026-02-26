#ifndef TLAC_TRACE_ITEM_H
#define TLAC_TRACE_ITEM_H

#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct trace_item {
	char* op;
	cJSON* path; /* JSON array */
	cJSON* args; /* JSON array */
} trace_item_t;

trace_item_t*
trace_item_create(const char* op, const cJSON* path_array, const cJSON* args_array);
void
trace_item_destroy(trace_item_t* it);

/* Returns a new JSON object: { "op": "...", "path": [...], "args": [...] } */
cJSON*
trace_item_jsonize(const trace_item_t* it);

#ifdef __cplusplus
}
#endif

#endif
