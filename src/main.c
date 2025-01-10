#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <curses.h>

#define CRASH(errnum) {fprintf(stderr, "FATAL ERROR (line %d). Code: %s"), __LINE__, strerror(errnum);exit(errnum);}
#define mLINES LINES
#define mCOLS COLS
#define MALLOC_ERR 1
#define REALLOC_ERR 2
#define MAX_FILE_NAME_SIZE 16 

struct entity {
	int ex, ey;
} entity;

struct map {
	char *mapName;
	char mapId[MAX_FILE_NAME_SIZE];
	short mapArr;
	int cols, lines;
} map;

struct player {
	struct entity *self;
	char saveId[MAX_FILE_NAME_SIZE];
	char *Name[20];

	// Spawn/respawning
	int resx,resy;
	char *last_open_mapId;
} player;

struct data {
	char *tileset;
} data;

struct base {
	struct data *dat;
} base;

struct arg {
	struct base *self;
	struct player* p;
	struct map *currentMap;
	WINDOW *window_array[5];

	short isRunning;
	short iSMenu;

	enum State {world, menu, inventory} gameState;
	
	int cornerCoords[8];
} args;

// Functions
struct arg *main_init(void);
void main_loop(struct arg *args);

void *util_resizePointer(void *pointer, size_t new_size);


struct arg *
main_init(void) {
	initscr();noecho();cbreak();clear();curs_set(0);
	struct arg *args=malloc(sizeof(args));
	if (!args) CRASH(ENOMEM);
	args->p=malloc(sizeof(player));
	if (!args->p) CRASH(ENOMEM);
	args->self=malloc(sizeof(base));
	if (!args->self) CRASH(ENOMEM);
	args->currentMap=malloc(sizeof(map));
	if (!args->currentMap) CRASH(ENOMEM);
	args->isRunning=1;	

	args->window_array[0]=newwin(mLINES, mCOLS, 0, 0); // Root Window (always on)
	args->window_array[1]=subwin(args->window_array[0], 0, 0, 0, 0); // main UI (always on)
	args->window_array[2]=subwin(args->window_array[0], 0, 0, 0, 0); // World Display
	args->window_array[3]=subwin(args->window_array[2], 0, 0, 0, 0); // World UI (world display)
	args->gameState=world;

	// Move loading map to here

	return args;
}

void *
util_resizePointer(void *pointer, size_t new_size) {
	void *realloc_res=realloc(pointer, new_size);
	if (!realloc_res) {
		fprintf(stdout, "realloc: pointer reallocation failed.\n");
		CRASH(ENOMEM)
	} return realloc_res;
}

int
main(int argc, char **argv) {
	struct arg *args=main_init();

	refresh();
	while (args->isRunning) {
		main_loop(args);
	}

	endwin();
return 0;
}

/**
 * Its a game engine. Made by me :)
 *
 *
 * **/
