#pragma once
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../common/definitions.h"

struct data {
	// meta data
	char *dataName; // in game content name
	char dataId[MAX_FILE_NAME_SIZE];
	
	// Actual data
	char *tileset;
	int tilesetc; // tileset count
	struct entity *prototype_entity_list;
	int prototype_entityc;
};

struct base {
	struct data *dat;
};

// Use this function in the game. Loads the binary data
// TODO: add obfuscation
void util_loadData(char *path, char *dataId, struct data *contentBlob);
void _loadData_txt(char *path, char *dataId, struct data *contentBlob);

