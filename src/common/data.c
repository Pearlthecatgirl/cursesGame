#include "data.h"

void
util_loadData(char *path, char *dataId, struct data *contentBlob) {
	FILE *fptr;
	char fullpath[256];

	if (snprintf(fullpath, 256, "%s%s%s", path, "/DATA/", dataId)<0) CRASH(ENOBUFS);
	if (access(fullpath, R_OK)!=0) CRASH(EACCES);
	if (!(fptr=fopen(fullpath, "rb"))) CRASH(ENOMEM);

	char *header=malloc(sizeof(char)*(HEADER_SIZE+1));
	if (!header) CRASH(ENOMEM);
	if (!fread(header, sizeof(char)*(HEADER_SIZE), 1, fptr)) CRASH(0);
	
	// TODO: make the obfuscation function
	//strncpy(header, raw, sizeof(char)*(HEADER_SIZE+1));

	char tmp_nameBuffer[MAX_NAME_SIZE];
	if (!strncpy(contentBlob->dataId, dataId, sizeof(char)*MAX_FILE_NAME_SIZE)) CRASH(ENOBUFS);
	if (strcmp(contentBlob->dataId, strtok(header, "|"))!=0) fprintf(stderr, "Warning: map id doesn't seem correct...\n");
	if (!strncpy(tmp_nameBuffer, strtok(NULL, "|"), MAX_FILE_NAME_SIZE)) CRASH(ENOBUFS);
}

void 
_loadData_txt(char *path, char *dataId, struct data *contentBlob) {
	FILE *fptr;
	char fullpath[256];
}


