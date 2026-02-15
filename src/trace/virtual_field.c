#include "virtual_field.h"
#include "helper/ndjson_serializer.h"
#include <stdlib.h>
#include <string.h>

struct virtual_field {
	tla_tracer_t* tracer;
	path_item_kind_t kind;
	char* name;
	int index;
	int bindex;
	virtual_field_t* parent;
};

static char* dupstr(const char* s) {
	if (!s) s = "";
	size_t n = strlen(s);
	char* r = (char*)malloc(n + 1);
	if (!r) return NULL;
	memcpy(r, s, n + 1);
	return r;
}

virtual_field_t*
virtual_field_variable(tla_tracer_t* tracer, const char* variable_name) {
	if (!tracer || !variable_name || !variable_name[0]) return NULL;
	virtual_field_t* f = (virtual_field_t*)calloc(1, sizeof(*f));
	if (!f) return NULL;
	f->tracer = tracer;
	f->kind = PATH_ITEM_NAME;
	f->name = dupstr(variable_name);
	f->parent = NULL;
	return f;
}

static virtual_field_t*
make_child(virtual_field_t* parent, path_item_kind_t kind, const char* name, int index, int bindex) {
	if (!parent) return NULL;
	virtual_field_t* f = (virtual_field_t*)calloc(1, sizeof(*f));
	if (!f) return NULL;
	f->tracer = parent->tracer;
	f->parent = parent;
	f->kind = kind;
	if (kind == PATH_ITEM_NAME) f->name = dupstr(name);
	if (kind == PATH_ITEM_INDEX) f->index = index;
	if (kind == PATH_ITEM_BINDEX) f->bindex = bindex;
	return f;
}

virtual_field_t*
virtual_field_get_field_name(virtual_field_t* parent, const char* name) {
	return make_child(parent, PATH_ITEM_NAME, name, 0, 0);
}

virtual_field_t*
virtual_field_get_field_index(virtual_field_t* parent, int index) {
	return make_child(parent, PATH_ITEM_INDEX, NULL, index, 0);
}

virtual_field_t*
virtual_field_get_field_bindex(virtual_field_t* parent, int bindex) {
	return make_child(parent, PATH_ITEM_BINDEX, NULL, 0, bindex ? 1 : 0);
}

void
virtual_field_destroy(virtual_field_t* f) {
	if (!f) return;
	free(f->name);
	free(f);
}

static const char*
get_var_name(const virtual_field_t* f) {
	const virtual_field_t* cur = f;
	while (cur && cur->parent) cur = cur->parent;
	return cur && cur->kind == PATH_ITEM_NAME ? cur->name : "";
}

static void
collect_path_items(const virtual_field_t* f, cJSON* arr) {
	if (!f || !arr) return;
	if (f->parent) collect_path_items(f->parent, arr);

	/* Add this item */
	if (f->kind == PATH_ITEM_NAME) {
		cJSON_AddItemToArray(arr, cJSON_CreateString(f->name ? f->name : ""));
	} else if (f->kind == PATH_ITEM_INDEX) {
		cJSON_AddItemToArray(arr, cJSON_CreateNumber((double)f->index));
	} else if (f->kind == PATH_ITEM_BINDEX) {
		cJSON_AddItemToArray(arr, cJSON_CreateBool(f->bindex ? 1 : 0));
	} else {
		cJSON_AddItemToArray(arr, cJSON_CreateNull());
	}
}

static cJSON*
get_path_excluding_var(const virtual_field_t* f) {
	cJSON* full = cJSON_CreateArray();
	if (!full) return NULL;

	collect_path_items(f, full);

	int n = cJSON_GetArraySize(full);
	cJSON* path = cJSON_CreateArray();
	if (!path) { cJSON_Delete(full); return NULL; }

	for (int i = 1; i < n; i++) {
		cJSON* item = cJSON_GetArrayItem(full, i);
		cJSON_AddItemToArray(path, ndjson_deep_dup(item));
	}

	cJSON_Delete(full);
	return path;
}

void
virtual_field_apply(virtual_field_t* f, const char* op, const cJSON* args_array) {
	if (!f || !op || !op[0]) return;

	cJSON* path = get_path_excluding_var(f);
	if (!path) return;

	const char* var = get_var_name(f);

	if (args_array && cJSON_IsArray((cJSON*)args_array)) {
		tla_tracer_notify_change(f->tracer, var, path, op, args_array);
	} else {
		cJSON* empty = cJSON_CreateArray();
		tla_tracer_notify_change(f->tracer, var, path, op, empty);
		cJSON_Delete(empty);
	}

	cJSON_Delete(path);
}

static void
apply0(virtual_field_t* f, const char* op) {
	cJSON* empty = cJSON_CreateArray();
	virtual_field_apply(f, op, empty);
	cJSON_Delete(empty);
}

static void
apply1(virtual_field_t* f, const char* op, const cJSON* v) {
	cJSON* arr = cJSON_CreateArray();
	cJSON_AddItemToArray(arr, ndjson_deep_dup(v ? v : cJSON_CreateNull()));
	virtual_field_apply(f, op, arr);
	cJSON_Delete(arr);
}

static void
apply2(virtual_field_t* f, const char* op, const cJSON* a, const cJSON* b) {
	cJSON* arr = cJSON_CreateArray();
	cJSON_AddItemToArray(arr, ndjson_deep_dup(a ? a : cJSON_CreateNull()));
	cJSON_AddItemToArray(arr, ndjson_deep_dup(b ? b : cJSON_CreateNull()));
	virtual_field_apply(f, op, arr);
	cJSON_Delete(arr);
}

void virtual_field_init(virtual_field_t* f) { apply0(f, "Init"); }
void virtual_field_update(virtual_field_t* f, const cJSON* value) { apply1(f, "Update", value); }
void virtual_field_add_element(virtual_field_t* f, const cJSON* value) { apply1(f, "AddElement", value); }
void virtual_field_remove_element(virtual_field_t* f, const cJSON* value) { apply1(f, "RemoveElement", value); }
void virtual_field_clear(virtual_field_t* f) { apply0(f, "Clear"); }

void virtual_field_add_elements(virtual_field_t* f, const cJSON* values_array) { apply1(f, "AddElements", values_array); }
void virtual_field_add_element_to_bag(virtual_field_t* f, const cJSON* value) { apply1(f, "AddElementToBag", value); }
void virtual_field_remove_element_from_bag(virtual_field_t* f, const cJSON* value) { apply1(f, "RemoveElementFromBag", value); }
void virtual_field_add_to_bag(virtual_field_t* f, const cJSON* value) { apply1(f, "AddToBag", value); }
void virtual_field_remove_from_bag(virtual_field_t* f, const cJSON* value) { apply1(f, "RemoveFromBag", value); }
void virtual_field_clear_bag(virtual_field_t* f) { apply0(f, "ClearBag"); }
void virtual_field_append_element(virtual_field_t* f, const cJSON* value) { apply1(f, "AppendElement", value); }
void virtual_field_reset_key(virtual_field_t* f, const cJSON* key) { apply1(f, "ResetKey", key); }
void virtual_field_set_key(virtual_field_t* f, const cJSON* key, const cJSON* value) { apply2(f, "SetKey", key, value); }
void virtual_field_update_rec(virtual_field_t* f, const cJSON* value) { apply1(f, "UpdateRec", value); }
void virtual_field_init_rec(virtual_field_t* f) { apply0(f, "InitRec"); }
