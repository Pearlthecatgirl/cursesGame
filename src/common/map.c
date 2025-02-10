#include "map.h"

void 
util_loadMap(char *path, char *mapId, struct map *currentMap) {
	FILE *fptr;
	char fullpath[256];

	// /path/to/data/dir/LEVEL/LEVEL1...LEVEL2... 
	if (snprintf(fullpath, 256, "%s%s%s", path, "/LEVEL/", mapId)<0) CRASH(ENOBUFS);
	if (access(fullpath, R_OK)!=0) CRASH(EACCES);
	if (!(fptr=fopen(fullpath, "rb"))) CRASH(ENOMEM);
	
	//char *raw=malloc(sizeof(char)*(HEADER_SIZE+1));
	//if (!raw) CRASH(ENOMEM);
	//if (!fread(raw, sizeof(char)*(HEADER_SIZE+1), 1, fptr)) CRASH(0);
	char *header=malloc(sizeof(char)*(HEADER_SIZE+1));
	if (!header) CRASH(ENOMEM);
	if (!fread(header, sizeof(char)*(HEADER_SIZE), 1, fptr)) CRASH(0);

	// TODO: make the obfuscation function
	//strncpy(header, raw, sizeof(char)*(HEADER_SIZE+1));

	char tmp_nameBuffer[MAX_NAME_SIZE];
	if (!strncpy(currentMap->mapId, mapId, sizeof(char)*MAX_FILE_NAME_SIZE)) CRASH(ENOBUFS);
	if (strcmp(currentMap->mapId, strtok(header, "|"))!=0) fprintf(stderr, "Warning: map id doesn't seem correct...\n");
	if (!strncpy(tmp_nameBuffer, strtok(NULL, "|"), MAX_FILE_NAME_SIZE)) CRASH(ENOBUFS);
	currentMap->lines=atoi(strtok(NULL, "|"));
	currentMap->cols=atoi(strtok(NULL, "|"));
	currentMap->size=atoi(strtok(NULL, "|"));

	currentMap->mapName=malloc(sizeof(char)*strlen(tmp_nameBuffer));
	if (!currentMap->mapName) CRASH(ENOMEM);
	if (!strncpy(currentMap->mapName, tmp_nameBuffer, sizeof(char)*strlen(tmp_nameBuffer))) CRASH(ENOMEM);
	
	if (!(currentMap->mapArr=malloc(sizeof(short)*currentMap->lines*currentMap->cols))) CRASH(ENOMEM);
	for (int i=0;i<currentMap->lines;i++) {
		if (!fread(currentMap->mapArr+(i*50), sizeof(short)*currentMap->cols, 1, fptr)) CRASH(ENOMEM);
	}

	fclose(fptr);
	free(header);
	return;
}

void 
util_loadMap_txt(char *path, char *mapId, struct map *currentMap) {
	FILE *fptr;
	char fullpath[256];

	// /path/to/data/dir/LEVEL/LEVEL1...LEVEL2... 
	if (snprintf(fullpath, 256, "%s/LEVEL/%s", path, mapId)<0) CRASH(ENOBUFS);
	if (access(fullpath, R_OK)!=0) CRASH(EACCES);
	if (!(fptr=fopen(fullpath, "r"))) CRASH(ENOMEM);
	
	//char *raw=malloc(sizeof(char)*(HEADER_SIZE+1));
	//if (!raw) CRASH(ENOMEM);
	//if (!fread(raw, sizeof(char)*(HEADER_SIZE+1), 1, fptr)) CRASH(0);
	char *header=malloc(sizeof(char)*(HEADER_SIZE+1));
	if (!header) CRASH(ENOMEM);
	if (!fread(header, sizeof(char)*(HEADER_SIZE), 1, fptr)) CRASH(0);

	// TODO: make the obfuscation function
	//strncpy(header, raw, sizeof(char)*(HEADER_SIZE+1));

	char tmp_nameBuffer[MAX_NAME_SIZE];
	if (!strncpy(currentMap->mapId, mapId, sizeof(char)*MAX_FILE_NAME_SIZE)) CRASH(ENOBUFS);
	if (strcmp(currentMap->mapId, strtok(header, "|"))!=0) fprintf(stderr, "Warning: map id doesn't seem correct...\n");
	if (!strncpy(tmp_nameBuffer, strtok(NULL, "|"), MAX_FILE_NAME_SIZE)) CRASH(ENOBUFS);
	currentMap->lines=atoi(strtok(NULL, "|"));
	currentMap->cols=atoi(strtok(NULL, "|"));
	currentMap->size=atoi(strtok(NULL, "|"));

	currentMap->mapName=malloc(sizeof(char)*strlen(tmp_nameBuffer));
	if (!currentMap->mapName) CRASH(ENOMEM);
	if (!strncpy(currentMap->mapName, tmp_nameBuffer, sizeof(char)*strlen(tmp_nameBuffer))) CRASH(ENOMEM);
	
	if (!(currentMap->mapArr=malloc(sizeof(short)*currentMap->lines*currentMap->cols))) CRASH(ENOMEM);
	for (int i=0;i<currentMap->lines;i++) {
		if (!fread(currentMap->mapArr+(i*50), sizeof(short)*currentMap->cols, 1, fptr)) CRASH(ENOMEM);
	}

	fclose(fptr);
	free(header);
	return;
}


