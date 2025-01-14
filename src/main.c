#include <curses.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define CRASH(errnum) {fprintf(stderr, "FATAL ERROR (line %d). Code: %s\n", __LINE__, strerror(errnum));endwin();exit(errnum);}
#define mLINES 17 //	Max Screen size
#define mCOLS 39 // Max screen size
#define MAX_FILE_NAME_SIZE 16 // File name size
#define TARGET_TICK_RATE 20
#define HEADER_SIZE 50 //Size of header in each read file
#define MAX_NAME_SIZE 32// Never use this size. It is just a temporary buffer

//char *tileset=".#,<>";

struct entity {
	int ex, ey; // X and Y position
	int r, t; // Range and Angle (theta) for direction facing and reach
} entity;

struct map {
	char *mapName;
	char mapId[MAX_FILE_NAME_SIZE];
	short *mapArr;
	int cols, lines;
	int size;
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
	short autopause; // Pause during menu option
	unsigned long long tick;
	int cKey; // Keyboard input
	int mX, mY; // Mouse coordinates

	enum State {world, menu, inventory} gameState;

	struct timespec *preFrame, *postFrame;
	
	int cornerCoords[2 * 2 * 2];
	int wOffset, hOffset;
} args;

// Functions
struct arg *main_init(void);
void main_loop(struct arg *args);

void util_loadMap(char *path, char *mapId, struct map *currentMap);
void *util_resizePointer(void *pointer, size_t new_size);

int world_checkCollision(int wx, int wy, struct map *currentMap);
void world_defineCorners(int px, int py, int *output);
void world_display(struct arg *args);
enum State world_loop(struct arg *args);

struct arg *
main_init(void) {
	initscr();noecho();cbreak();clear();curs_set(0);keypad(stdscr, TRUE);
	struct arg *opt=malloc(sizeof(struct arg));
	if (!opt) CRASH(ENOMEM);
	opt->p=malloc(sizeof(player));
	if (!opt->p) CRASH(ENOMEM);
	opt->p->self=malloc(sizeof(entity));
	if (!opt->p->self) CRASH(ENOMEM);
	opt->self=malloc(sizeof(base));
	if (!opt->self) CRASH(ENOMEM);
	opt->self->dat=malloc(sizeof(data));
	if (!opt->self->dat) CRASH(ENOMEM);
	// set 5 tilesets for now... move into dat file later
	opt->self->dat->tileset=malloc(sizeof(char)*6);
	if (!opt->self->dat->tileset) CRASH(ENOMEM);
	if (!strncpy(opt->self->dat->tileset, ".#,><", 6)) CRASH(ENOMEM);

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
	opt->p->self->ey=5;
	opt->p->self->ex=5;

	timeout(( (int) (1/((double)TARGET_TICK_RATE))*1000));
	// Move loading map to here

	opt->tick=0;
	return opt;
}

void
main_loop(struct arg *args) {
	args->tick+=1;
	timespec_get(args->preFrame, TIME_UTC);
	args->cKey=getch();
	wclear(args->window_array[2]);
	box(args->window_array[1], 0, 0);
	wrefresh(args->window_array[1]);

	// Check the box to see if the screen width changes at all
	args->wOffset=args->hOffset=1;
	if (mLINES%2) args->wOffset=2;
	if (mCOLS%2) args->hOffset=2;

	// Move the cursor
	switch (args->cKey) {
		case KEY_UP:
			if (!(args->mY-1<0))args->mY-=1;
			break;
		case KEY_DOWN:
			if (!(args->mY+1>mLINES))args->mY+=1;
			break;
		case KEY_LEFT:
			if (!(args->mY-1<0))args->mX-=1;
			break;
		case KEY_RIGHT:
			if (!(args->mY-1>mCOLS))args->mX+=1;
			break;
		default:
			break;
	}
	char prev_curs='x';
	if (args->tick%30) {
		if (prev_curs=='x') {
			prev_curs='+';
		} else prev_curs='x';
	}

	switch (args->gameState) {
		case world:	
			args->gameState=world_loop(args);
			world_display(args);
			mvwprintw(args->window_array[1], 0 ,0,"x:%d,y:%d, tick: %lld", args->p->self->ex, args->p->self->ey, args->tick);
			mvwprintw(args->window_array[1], 0 ,0,"x:%d,y:%d, tick: %lld", args->p->self->ex, args->p->self->ey, args->tick);
			break;
		case inventory:
			break;
		case menu:
			break;
		default:
			break;
		wrefresh(args->window_array[2]);
	
	}
	
	mvwprintw(args->window_array[0],args->mY, args->mX, "%c", prev_curs);
	wrefresh(args->window_array[0]);
	timespec_get(args->postFrame, TIME_UTC);
	
	double tSetup = (args->postFrame->tv_sec - args->preFrame->tv_sec) + (args->postFrame->tv_nsec - args->preFrame->tv_nsec)/1000000000.0;
	timeout((int)(((1/(double)TARGET_TICK_RATE)-tSetup)*1000));
}

int
world_checkCollision(int wx, int wy, struct map *currentMap) {
	int tile=currentMap->mapArr[wy * currentMap->cols + wx];
		if (tile<2) return tile;
		switch (tile) {
			default:
				return 0;
				break;
		}
		return 0;
}

void
world_defineCorners(int px, int py, int * output) {
	/* (0,1)	(2,3)
	 * (4,5)	(6,7)
	 * */
	if (mLINES%2) {
		output[5]=py+(int)(mLINES/2)-1; // Y coord
	} else { output[5]=py+(int)(mLINES/2)-2; } // Y coord
	if (mCOLS%2) {
	output[2]=px+(int)(mCOLS/2)-1;  // X coord
	} else { output[2]=px+(int)(mCOLS/2)-2; } // X coord
										  //
	output[0]=px-(int)(mCOLS/2)+1;  // X coord
	output[1]=py-(int)(mLINES/2)+1; // Y coord
	output[3]=output[1];
	output[4]=output[0];
	output[6]=output[2];
	output[7]=output[5];
}

void
world_display(struct arg *args) {
	int edgeX=args->p->self->ex, edgeY=args->p->self->ey;
	int midX=(int)mLINES/2, midY=(int)mCOLS/2;

	// Calculate whether to scroll, or move the player on screen. 
	if (args->p->self->ex-mCOLS/2+args->wOffset<=0) {
		edgeX=mCOLS/2-args->wOffset+1;
	} else if (args->p->self->ex+mCOLS/2-1>=args->currentMap->cols) edgeX=args->currentMap->cols-mCOLS/2;
	if (args->p->self->ey-mLINES/2+args->hOffset<=0) {
		edgeY=mLINES/2-args->hOffset+1;
	} else if (args->p->self->ey+mLINES/2-1>=args->currentMap->lines) edgeY=args->currentMap->lines-mLINES/2;

	// define edges to draw from
	world_defineCorners(edgeX, edgeY, args->cornerCoords);

	for (int iwx=args->cornerCoords[0], isx=1; /*iwx<args->cornerCoords[2],*/ isx < mCOLS-1;iwx++,isx++) {
		for (int iwy=args->cornerCoords[1], isy=1; /*iwy<args->cornerCoords[5],*/ isy < mLINES-1;iwy++, isy++) {
			mvwprintw(args->window_array[2], isy, isx, "%c", args->self->dat->tileset[args->currentMap->mapArr[iwy * args->currentMap->cols + iwx]]);

			/* Query the screen coordinates for when the map coordinates match up with player's map coordinates*/
			if (iwy==args->p->self->ey) midY=isy;
			if (iwx==args->p->self->ex) midX=isx;
		}
	}
	//mvwprintw(args->window_array[2], midY, midX, "@");
	mvwaddch(args->window_array[2], midY, midX, '@');

	wrefresh(args->window_array[2]);
	wrefresh(args->window_array[1]);
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

void 
util_loadMap(char *path, char *mapId, struct map *currentMap) {
	FILE *fptr;
	char fullpath[256];
	if (snprintf(fullpath, 256, "%s/%s", path, mapId)<0) CRASH(ENOBUFS);
	if (!(access(fullpath, R_OK)==0)) CRASH(EACCES);
	if (!(fptr=fopen(fullpath, "rb"))) CRASH(ENOMEM);
	
	//char *raw=malloc(sizeof(char)*(HEADER_SIZE+1));
	//if (!raw) CRASH(ENOMEM);
	//if (!fread(raw, sizeof(char)*(HEADER_SIZE+1), 1, fptr)) CRASH(0);
	char *header=malloc(sizeof(char)*(HEADER_SIZE+1));
	if (!header) CRASH(ENOMEM);
	if (!fread(header, sizeof(char)*(HEADER_SIZE), 1, fptr)) CRASH(0);

	//strncpy(header, raw, sizeof(char)*(HEADER_SIZE+1));

	char tmp_nameBuffer[MAX_NAME_SIZE];
	if (!strncpy(currentMap->mapId, mapId, sizeof(char)*MAX_FILE_NAME_SIZE)) CRASH(ENOBUFS);
	if (!strcmp(currentMap->mapId, strtok(header, "|"))==0) fprintf(stderr, "Warning: map id doesn't seem correct...\n");
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

	//free(raw);
	free(header);
	return;
}

void *
util_resizePointer(void *pointer, size_t new_size) {
	void *realloc_res=realloc(pointer, new_size);
	if (!realloc_res) {
		CRASH(ENOMEM)
	} return realloc_res;
}

int
main(int argc, char **argv) {
	fprintf(stdout, "args: %d, running: %s", argc, argv[0]);
	struct arg *args=malloc(sizeof(struct arg));
	args=main_init();

	util_loadMap("./data", "MAP000.TST", args->currentMap);

	refresh();
	while (args->isRunning) main_loop(args);

	endwin();
return 0;
}

/**
 * Its a game engine. Made by me :)
 *
 *
 * **/
