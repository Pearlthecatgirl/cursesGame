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

	// TODO: Load stuff here
}

void 
_loadData_txt(char *path, char *dataId, struct data *contentBlob) {
	FILE *fptr;
	char fullpath[256];
	int section_count=0;

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
	// Count how many unique sections
	if (!(section_count=atoi(strtok(NULL, "|")))) CRASH(ENOBUFS);

	// Loop through as many data points as given in the header
	for (int i=0;i<section_count-1;i++) {
		char entry_header_buf[64];
		if (!fread(entry_header_buf, sizeof(char)*256, 1, fptr)) CRASH(ENOBUFS);
		// TODO: This could segfault.... check later
		char *header=strtok(entry_header_buf, "|");
		int len=atoi(strtok(NULL, "|"));

		// Save the actual content 
		char *content=malloc(sizeof(char)*len);
		if (!content) CRASH(ENOMEM);
		if (!strcmp(header, "tileset")) {
			if (!fread(contentBlob->tileset, sizeof(char)*len, 1, fptr)) CRASH(ENOBUFS);
			contentBlob->tilesetc=len;
		} else if (!strcmp(header, "entity")) {
			// This will be effected by more entity attributes	

		} else if (!strcmp(header, "item")) {

		}
	}
}


