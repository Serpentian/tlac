#include "configuration_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
config_write_one_line(const char* path, const cJSON* json_object) {
	if (!path || !json_object || !cJSON_IsObject((cJSON*)json_object)) return -1;

	char* s = cJSON_PrintUnformatted((cJSON*)json_object);
	if (!s) return -1;

	FILE* f = fopen(path, "wb");
	if (!f) { free(s); return -1; }

	fwrite(s, 1, strlen(s), f);
	fwrite("\n", 1, 1, f);
	fclose(f);
	free(s);
	return 0;
}

cJSON*
config_read_one_line_object(const char* path) {
	if (!path) return NULL;

	FILE* f = fopen(path, "rb");
	if (!f) return NULL;

	char buf[1024 * 1024];
	if (!fgets(buf, (int)sizeof(buf), f)) {
		fclose(f);
		return NULL;
	}
	fclose(f);

	size_t len = strlen(buf);
	while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
		buf[len - 1] = '\0';
		len--;
	}

	cJSON* root = cJSON_Parse(buf);
	if (!root || !cJSON_IsObject(root)) {
		if (root) cJSON_Delete(root);
		return NULL;
	}
	return root;
}
