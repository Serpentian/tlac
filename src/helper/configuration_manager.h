#ifndef CONFIGURATION_MANAGER_H
#define CONFIGURATION_MANAGER_H

#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Writes exactly one JSON object (unformatted) followed by '\n'. */
int
config_write_one_line(const char* path, const cJSON* json_object);

/* Reads first line and parses as JSON object. Returns NULL on error. */
cJSON*
config_read_one_line_object(const char* path);

#ifdef __cplusplus
}
#endif

#endif
