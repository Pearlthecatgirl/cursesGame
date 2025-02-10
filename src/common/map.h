#pragma once
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../common/definitions.h"

struct map {
	char *mapName;
	char mapId[MAX_FILE_NAME_SIZE];
	short *mapArr;
	int cols, lines;
	int size;
};

// Use this function in the game. Loads the binary data
// TODO: add obfuscation
void util_loadMap(char *path, char *mapId, struct map *currentMap);


