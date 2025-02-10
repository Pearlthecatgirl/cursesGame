#pragma once
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../common/definitions.h"

struct data {
	char *tileset;
	struct entity *prototype_entity_list;
	int prototype_entityc;
} data;

struct base {
	struct data *dat;
	char *data_path;
	int data_path_len;
} base;

void util_loadData(char *path, char *dataId, struct data *contentBlob);
