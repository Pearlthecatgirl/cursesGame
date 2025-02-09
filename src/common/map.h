#pragma once
#include <math.h>
#include "../common/definitions.h"



struct map {
	char *mapName;
	char mapId[MAX_FILE_NAME_SIZE];
	short *mapArr;
	int cols, lines;
	int size;
} map;
