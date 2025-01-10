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
	char Name[20];

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
	unsigned long long tick;
	char cKey;

	enum State {world, menu, inventory} gameState;
	
	int cornerCoords[8];
	int xOffset, yOffset;
} args;

// Functions
struct arg *main_init(void);
void main_loop(struct arg *args);

void *util_resizePointer(void *pointer, size_t new_size);


struct arg *
main_init(void) {
	initscr();noecho();cbreak();clear();curs_set(0);
	struct arg *opt=malloc(sizeof(opt));
	if (!opt) CRASH(ENOMEM);
	opt->p=malloc(sizeof(player));
	if (!opt->p) CRASH(ENOMEM);
	opt->p->self=malloc(sizeof(entity));
	if (!opt->p->self) CRASH(ENOMEM);
	opt->self=malloc(sizeof(base));
	if (!opt->self) CRASH(ENOMEM);
	opt->currentMap=malloc(sizeof(map));
	if (!opt->currentMap) CRASH(ENOMEM);
	opt->isRunning=1;	

	opt->window_array[0]=newwin(mLINES, mCOLS, 0, 0); // Root Window (always on)
	opt->window_array[1]=subwin(opt->window_array[0], 0, 0, 0, 0); // main UI (always on)
	opt->window_array[2]=subwin(opt->window_array[0], 0, 0, 0, 0); // World Display
	opt->window_array[3]=subwin(opt->window_array[2], 0, 0, 0, 0); // World UI (world display)

	opt->gameState=world;

	// Move loading map to here

	opt->tick=0;
	return opt;
}

void
main_loop(struct arg *args) {
	args->tick+=1;
	args->cKey=getch();
	//box(args->window_array[1], 0, 0);
	
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
	initscr();
	struct arg *args=malloc(sizeof(args));
	args=main_init();

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
