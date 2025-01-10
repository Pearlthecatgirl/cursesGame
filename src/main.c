
#include <curses.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CRASH(errnum) {fprintf(stderr, "FATAL ERROR (line %d). Code: %s", __LINE__, strerror(errnum));exit(errnum);}
#define mLINES LINES
#define mCOLS COLS
#define MALLOC_ERR 1
#define REALLOC_ERR 2
#define MAX_FILE_NAME_SIZE 16 
#define TARGETTICKRATE 20

struct entity {
	int ex, ey;
} entity;

struct map {
	char *mapName;
	char mapId[MAX_FILE_NAME_SIZE];
	short *mapArr;
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

	struct timespec *preFrame, *postFrame;
	
	int cornerCoords[8];
	int wOffset, hOffset;
} args;

// Functions
struct arg *main_init(void);
void main_loop(struct arg *args);

enum State world_loop(struct arg *args);
int world_checkCollision(int wx, int wy, struct map *currentMap);

void util_loadMap(char *path, char *mapId, struct map *currentMap);
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
	opt->preFrame=malloc(sizeof(struct timespec));
	opt->postFrame=malloc(sizeof(struct timespec));
	opt->isRunning=1;	

	opt->window_array[0]=newwin(mLINES, mCOLS, 0, 0); // Root Window (always on)
	opt->window_array[1]=subwin(opt->window_array[0], 0, 0, 0, 0); // main UI (always on)
	opt->window_array[2]=subwin(opt->window_array[0], 0, 0, 0, 0); // World Display
	opt->window_array[3]=subwin(opt->window_array[2], 0, 0, 0, 0); // World UI (world display)

	opt->gameState=world;

	timeout(( (int) (1/((double)TARGETTICKRATE))*1000));
	// Move loading map to here

	opt->tick=0;
	return opt;
}

void
main_loop(struct arg *args) {
	args->tick+=1;
	timespec_get(args->preFrame, TIME_UTC);
	args->cKey=getch();
	box(args->window_array[1], 0, 0);
	wrefresh(args->window_array[1]);

	// Check the box to see if the screen width changes at all
	args->wOffset=args->hOffset=1;
	if (mLINES%2) args->wOffset=2;
	if (mCOLS%2) args->hOffset=2;

	switch (args->gameState) {
		case world:	
			args->gameState=world_loop(args);
			break;
	
	}
	
	timespec_get(args->postFrame, TIME_UTC);
	
	double tSetup = (args->postFrame->tv_sec - args->preFrame->tv_sec) + (args->postFrame->tv_nsec - args->preFrame->tv_nsec)/1000000000.0;
	timeout((int)(((1/(double)TARGETTICKRATE)-tSetup)*1000));
}

enum State 
world_loop(struct arg *args) {
	switch (args->cKey) {
		case'w':
			if (!world_checkCollision(args->p->self->ex,args->p->self->ey-1,args->currentMap)) {
				args->p->self->ey-=1;
			}
			break;
		case's':
			if (!world_checkCollision(args->p->self->ex,args->p->self->ey+1,args->currentMap)) {
				args->p->self->ey+=1;
			}
			break;
		case'a':
			if (!world_checkCollision(args->p->self->ex-1, args->p->self->ey,args->currentMap)) {
				args->p->self->ex-=1;
			}
			break;
		case'd':
			if (!world_checkCollision(args->p->self->ex+1, args->p->self->ey,args->currentMap)) {
				args->p->self->ex+=1;
			}
			break;
		case'e':
				return inventory;	
			break;
		case'r':
			args->p->self->ex=25;
			args->p->self->ey=25;
			break;
		case'q':args->isRunning=0;
	}
	return world;
}

int
world_checkCollision(int wx, int wy, struct map *currentMap) {
	if (!&currentMap->mapArr) {
	int tile=currentMap->mapArr[wy * currentMap->cols + wx];
		if (tile<2) return tile;
		switch (tile) {
			default:
				return 0;
				break;
		}
		return 0;
	}
	CRASH(ENOMEM)
}

void 
util_loadMap(char *path, char *mapId, struct map *currentMap) {
	FILE *fptr;
	char *fullpath=malloc(sizeof(char)*(strlen(path)+strlen(currentMap->mapId)));
	if (!fullpath) CRASH(ENOMEM);
	if (snprintf(fullpath, sizeof(fullpath), "%s%s", path, mapId)<0) {}

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
