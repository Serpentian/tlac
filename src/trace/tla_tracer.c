#include "tla_tracer.h"
#include "trace_item.h"
#include "helper/ndjson_serializer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

typedef struct trace_item_vec {
	trace_item_t** items;
	size_t len;
	size_t cap;
} trace_item_vec_t;

typedef struct update_entry {
	char* var;
	trace_item_vec_t vec;
	struct update_entry* next;
} update_entry_t;

struct tla_tracer {
	char guid[64];
	tlac_clock_t* clock;
	FILE* out;
	update_entry_t* updates;
	pthread_mutex_t mu;
};

static char*
dupstr(const char* s) {
	if (!s) s = "";
	size_t n = strlen(s);
	char* r = (char*)malloc(n + 1);
	if (!r)
		return NULL;
	memcpy(r, s, n + 1);
	return r;
}

static void
vec_push(trace_item_vec_t* v, trace_item_t* it) {
	if (!v || !it) return;
	if (v->len == v->cap) {
		size_t ncap = v->cap ? v->cap * 2 : 4;
		trace_item_t** ni =
			(trace_item_t**)realloc(v->items, ncap * sizeof(*ni));
		if (!ni) return;
		v->items = ni;
		v->cap = ncap;
	}
	v->items[v->len++] = it;
}

static void
vec_clear(trace_item_vec_t* v) {
	if (!v) return;
	for (size_t i = 0; i < v->len; i++)
		trace_item_destroy(v->items[i]);
	free(v->items);
	v->items = NULL;
	v->len = 0;
	v->cap = 0;
}

static update_entry_t*
find_or_create(update_entry_t** head, const char* var) {
	for (update_entry_t* e = *head; e; e = e->next) {
		if (strcmp(e->var, var) == 0) return e;
	}
	update_entry_t* e = (update_entry_t*)calloc(1, sizeof(*e));
	if (!e) return NULL;
	e->var = dupstr(var);
	e->next = *head;
	*head = e;
	return e;
}

static void
updates_clear(update_entry_t** head) {
	update_entry_t* e = *head;
	while (e) {
		update_entry_t* n = e->next;
		free(e->var);
		vec_clear(&e->vec);
		free(e);
		e = n;
	}
	*head = NULL;
}

static void
uuid_like(char out[64]) {
	/* Not cryptographically strong; good enough for a logger id. */
	unsigned long long a = (unsigned long long)time(NULL);
	unsigned long long b = (unsigned long long)getpid();
	unsigned long long c = (unsigned long long)rand();
	snprintf(out, 64, "%08llx-%04llx-%04llx-%04llx-%012llx",
	  (a ^ (c << 16)) & 0xffffffffULL,
	  (b ^ (c >> 8)) & 0xffffULL,
	  (a >> 16) & 0xffffULL,
	  (b >> 16) & 0xffffULL,
	  (a ^ b ^ c) & 0xffffffffffffULL);
}

tla_tracer_t*
tla_tracer_create(const char* trace_path, tlac_clock_t* clock) {
	if (!trace_path || !trace_path[0] || !clock) return NULL;

	FILE* f = fopen(trace_path, "wb");
	if (!f) return NULL;

	tla_tracer_t* t = (tla_tracer_t*)calloc(1, sizeof(*t));
	if (!t) { fclose(f); return NULL; }

	srand((unsigned int)time(NULL) ^ (unsigned int)getpid());
	uuid_like(t->guid);

	t->out = f;
	t->updates = NULL;
	pthread_mutex_init(&t->mu, NULL);

	t->clock = clock; /* takes ownership */
	return t;
}

void
tla_tracer_destroy(tla_tracer_t* t) {
	if (!t) return;
	pthread_mutex_lock(&t->mu);
	updates_clear(&t->updates);
	pthread_mutex_unlock(&t->mu);

	if (t->out) fclose(t->out);
	if (t->clock) t->clock->destroy(t->clock);

	pthread_mutex_destroy(&t->mu);
	free(t);
}

void
tla_tracer_notify_change(tla_tracer_t* t, const char* variable,
			 const cJSON* path_array, const char* op,
			 const cJSON* args_array) {
	if (!t || !variable || !variable[0] || !op || !op[0]) return;

	cJSON* path = NULL;
	cJSON* args = NULL;

	if (path_array && cJSON_IsArray((cJSON*)path_array))
		path = ndjson_deep_dup(path_array);
	else
		path = cJSON_CreateArray();

	if (args_array && cJSON_IsArray((cJSON*)args_array))
		args = ndjson_deep_dup(args_array);
	else
		args = cJSON_CreateArray();

	if (!path || !args) {
		if (path) cJSON_Delete(path);
		if (args) cJSON_Delete(args);
		return;
	}

	trace_item_t* it = trace_item_create(op, path, args);
	cJSON_Delete(path);
	cJSON_Delete(args);
	if (!it) return;

	pthread_mutex_lock(&t->mu);
	update_entry_t* e = find_or_create(&t->updates, variable);
	if (e) vec_push(&e->vec, it);
	else trace_item_destroy(it);
	pthread_mutex_unlock(&t->mu);
}

static int
write_line(FILE* f, const char* s) {
	if (!f || !s) return -1;
	if (fputs(s, f) == EOF) return -1;
	if (fputc('\n', f) == EOF) return -1;
	if (fflush(f) != 0) return -1;
	return 0;
}

static int64_t
log_changes_locked(tla_tracer_t* t, const char* event_name,
		   const cJSON* event_args_array, const char* desc,
		   int64_t clock_value_used) {
	cJSON* jsonEvent = cJSON_CreateObject();
	if (!jsonEvent) return -1;

	cJSON_AddNumberToObject(jsonEvent, "clock", (double)clock_value_used);

	for (update_entry_t* e = t->updates; e; e = e->next) {
		cJSON* jsonActions = cJSON_CreateArray();
		if (!jsonActions) continue;

		for (size_t i = 0; i < e->vec.len; i++) {
			cJSON* ji = trace_item_jsonize(e->vec.items[i]);
			if (!ji) ji = cJSON_CreateNull();
			cJSON_AddItemToArray(jsonActions, ji);
		}
		cJSON_AddItemToObject(jsonEvent, e->var, jsonActions);
	}

	if (event_name && event_name[0])
		cJSON_AddStringToObject(jsonEvent, "event", event_name);

	if (event_args_array && cJSON_IsArray((cJSON*)event_args_array)
	    && cJSON_GetArraySize((cJSON*)event_args_array) > 0) {
		cJSON_AddItemToObject(jsonEvent, "event_args",
				      ndjson_deep_dup(event_args_array));
	}

	if (desc && desc[0])
		cJSON_AddStringToObject(jsonEvent, "desc", desc);

	cJSON_AddStringToObject(jsonEvent, "logger", t->guid);
	char* line = cJSON_PrintUnformatted(jsonEvent);
	cJSON_Delete(jsonEvent);

	if (!line) return -1;

	int rc = write_line(t->out, line);
	free(line);

	updates_clear(&t->updates);
	return rc == 0 ? clock_value_used : -1;
}

int64_t
tla_tracer_log(tla_tracer_t* t, const char* event_name,
	       const cJSON* event_args_array, int64_t clock_value,
	       const char* desc) {
	if (!t) return -1;

	int64_t new_clock = clock_next_time(t->clock, clock_value);
	if (new_clock < 0) return -1;

	pthread_mutex_lock(&t->mu);
	int64_t used = log_changes_locked(t, event_name ? event_name : "",
					  event_args_array,
					  desc ? desc : "", new_clock);
	pthread_mutex_unlock(&t->mu);
	return used;
}
