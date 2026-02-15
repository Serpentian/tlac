#include "trace_item.h"
#include "helper/ndjson_serializer.h"
#include <stdlib.h>
#include <string.h>

static char* dupstr(const char* s) {
	if (!s) s = "";
	size_t n = strlen(s);
	char* r = (char*)malloc(n + 1);
	if (!r) return NULL;
	memcpy(r, s, n + 1);
	return r;
}

trace_item_t*
trace_item_create(const char* op, const cJSON* path_array, const cJSON* args_array) {
	trace_item_t* it = (trace_item_t*)calloc(1, sizeof(*it));
	if (!it) return NULL;

	it->op = dupstr(op);
	it->path = ndjson_deep_dup(path_array);
	it->args = ndjson_deep_dup(args_array);

	if (!it->path) it->path = cJSON_CreateArray();
	if (!it->args) it->args = cJSON_CreateArray();

	if (!it->op || !it->path || !it->args) {
		trace_item_destroy(it);
		return NULL;
	}
	return it;
}

void
trace_item_destroy(trace_item_t* it) {
	if (!it) return;
	free(it->op);
	if (it->path) cJSON_Delete(it->path);
	if (it->args) cJSON_Delete(it->args);
	free(it);
}

cJSON*
trace_item_jsonize(const trace_item_t* it) {
	if (!it) return NULL;

	cJSON* obj = cJSON_CreateObject();
	if (!obj) return NULL;

	cJSON_AddStringToObject(obj, "op", it->op ? it->op : "");
	cJSON_AddItemToObject(obj, "path", ndjson_deep_dup(it->path));
	cJSON_AddItemToObject(obj, "args", ndjson_deep_dup(it->args));
	return obj;
}
