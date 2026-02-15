#ifndef VIRTUAL_FIELD_H
#define VIRTUAL_FIELD_H

#include "tla_tracer.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum path_item_kind {
	PATH_ITEM_NAME = 0,
	PATH_ITEM_INDEX = 1,
	PATH_ITEM_BINDEX = 2
} path_item_kind_t;

typedef struct virtual_field virtual_field_t;

/* Root field for a variable name. */
virtual_field_t*
virtual_field_variable(tla_tracer_t* tracer, const char* variable_name);

/* Child fields */
virtual_field_t*
virtual_field_get_field_name(virtual_field_t* parent, const char* name);
virtual_field_t*
virtual_field_get_field_index(virtual_field_t* parent, int index);
virtual_field_t*
virtual_field_get_field_bindex(virtual_field_t* parent, int bindex);

/* Free a virtual field chain node (does not free parent). */
void
virtual_field_destroy(virtual_field_t* f);

/* Generic apply: args_array can be NULL or JSON array. */
void
virtual_field_apply(virtual_field_t* f, const char* op, const cJSON* args_array);

/* Convenience ops */
void
virtual_field_init(virtual_field_t* f);
void
virtual_field_update(virtual_field_t* f, const cJSON* value);
void
virtual_field_add_element(virtual_field_t* f, const cJSON* value);
void
virtual_field_remove_element(virtual_field_t* f, const cJSON* value);
void
virtual_field_clear(virtual_field_t* f);

/* Bag/Seq/Map/Rec ops (names match Java strings) */
void
virtual_field_add_elements(virtual_field_t* f, const cJSON* values_array);
void
virtual_field_add_element_to_bag(virtual_field_t* f, const cJSON* value);
void
virtual_field_remove_element_from_bag(virtual_field_t* f, const cJSON* value);
void
virtual_field_add_to_bag(virtual_field_t* f, const cJSON* value);
void
virtual_field_remove_from_bag(virtual_field_t* f, const cJSON* value);
void
virtual_field_clear_bag(virtual_field_t* f);
void
virtual_field_append_element(virtual_field_t* f, const cJSON* value);
void
virtual_field_reset_key(virtual_field_t* f, const cJSON* key);
void
virtual_field_set_key(virtual_field_t* f, const cJSON* key, const cJSON* value);
void
virtual_field_update_rec(virtual_field_t* f, const cJSON* value);
void
virtual_field_init_rec(virtual_field_t* f);

#ifdef __cplusplus
}
#endif

#endif
