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

void util_loadMap(char *path, char *mapId, struct map *currentMap);
void util_loadMap_txt(char *path, char *mapId, struct map*currentMap);
void util_loadMap_legacy(char *path, char *mapId, struct map *currentMap);
void util_loadMap_txt_legacy(char *path, char *mapId, struct map *currentMap);



