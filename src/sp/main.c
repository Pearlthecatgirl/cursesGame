#include <curses.h> // This may not be as portable
#include <errno.h> // This may not be as portable
#include <math.h>
#include <pthread.h> // This may not be as portable
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // This may not be as portable

#include "../common/data.h"
#include "../common/definitions.h"
#include "../common/map.h"
#include "../common/shgeom.h"

// #! Global
const float g_tickPeriod=1000.0/TARGET_TICK_RATE;
const float g_framePeriod=1000.0/TARGET_FRAME_RATE;
const float g_inputPeriod=1000.0/TARGET_INPUT_RATE;

// #! Structs

struct attr{
	char *name;
	int namec; // length of name
	char *code; // identifier
	int _value_index; // Value given in scientific notation
	double _value_specific; // Value given in scientific notation
	enum type {INT, FLOAT} type_of_value;
};

struct item{
	struct attr **attrs_array; // Array of attributes
	int attrc; // Number of Attributes
};

struct entity {
	int ex, ey; // X and Y position
	int r, t; // Range and Angle (theta) for direction facing and reach
	short level;
	long int health;
	struct item **entity_inv; // Inventory of entities. Drops table
	int inv_itemc; // Inventory item count
	char *name;

	/*static will always spawn. nonStatic is generated completely*/
	// entities are spawned as void *. typecast to these structs
	enum entity_type {
		furniture_static, furniture_nonStatic,
		passive_static, passive_nonStatic, 
		aggresive_static, aggresive_nonStatic,
	} entity_type; // Enemies, decor, etc
};

struct player {
	struct entity *self;
	char saveId[MAX_FILE_NAME_SIZE];
	char Name[20];
	struct item **player_inv;
	struct item **stash_inv;

	// Onscreen location of the character
	int screenx, screeny;

	// Spawn/respawning
	int resx, resy;
	char *last_open_mapId;
};

struct arg {
	struct base *self;
	struct player* p;
	struct map *currentMap;
	WINDOW *window_array[5];
	// TODO: make a new entity class?
	struct entity **entity_array;
	int entityc;
	int entityc_a;

	short isRunning;
	short iSMenu;
	short autopause; // Pause during menu option
	unsigned long long tick, frame, input;
	int cKey; // Keyboard input
	int mX, mY; // Cursor coordinates

	enum State {world, menu, inventory} gameState;
	
	int cornerCoords[2 * 2 * 2];
	int wOffset, hOffset;
	int use_mouse;
	char seed[16];
	
#ifdef NCURSES_MOUSE_VERSION
	struct shape_vertex *mouse_pathfind;
#endif
};

// #!Functions
//short entity_spawn(struct entity *opt, struct base *self, int use_rand); // returns ret code

void generic_delay(const unsigned long int ms, const unsigned long int unit);
void generic_portableSleep(const int ms);

struct arg *main_init(void);
void main_loop(struct arg *args);
void *main_loopCalculation(void *args);
void *main_loopDisplay(void *args);
void *main_loopInput(void *args);

void util_spawnEntity(struct arg *args);
void util_displayShape(WINDOW *window, struct shape_vertex *shape, char material);
int util_cleanShape(struct shape_vertex *shape);

int world_checkCollision(int wx, int wy, struct map *currentMap);
void world_defineCorners(int px, int py, int *output);
void world_display(struct arg *args);
void world_loopEnv(struct arg *args); // Checks environment objects/mobs only
enum State world_loopMain(struct arg *args); //player only

short
entity_spawn(struct entity *opt, struct data *dat, int use_rand) {
	if (!use_rand) {
		USE_RAND:

	} else {
	
	}
}

// This is a bad function. Don't use this...
void 
generic_delay(const unsigned long int time, const unsigned long int unit) {
	clock_t end=clock()+time*(CLOCKS_PER_SEC / unit);
	while (clock()<end) {}
	return;
}

void 
generic_portableSleep(const int ms) {
	#ifdef WIN32
		Sleep(ms);
	#elif _POSIX_C_SOURCE >= 199309L
		struct timespec ts;
		ts.tv_sec = ms / 1000;
		ts.tv_nsec = (ms % 1000) * 1000000;
		nanosleep(&ts, NULL);
	#endif
}

struct arg *
main_init(void) {
	initscr();noecho();raw();clear();curs_set(0);keypad(stdscr, TRUE);
	struct arg *opt=malloc(sizeof(struct arg));
	if (!opt) CRASH(ENOMEM);
	opt->p=malloc(sizeof(struct player));
	if (!opt->p) CRASH(ENOMEM);
	opt->p->self=malloc(sizeof(struct entity));
	if (!opt->p->self) CRASH(ENOMEM);
	opt->self=malloc(sizeof(struct base));
	if (!opt->self) CRASH(ENOMEM);
	opt->self->dat=malloc(sizeof(struct data));
	if (!opt->self->dat) CRASH(ENOMEM);
	// set 5 tilesets for now... move into dat file later
	opt->self->dat->tileset=malloc(sizeof(char)*6);
	if (!opt->self->dat->tileset) CRASH(ENOMEM);
	if (!strncpy(opt->self->dat->tileset, ".#><", 7)) CRASH(ENOMEM);

	opt->currentMap=malloc(sizeof(struct map));
	if (!opt->currentMap) CRASH(ENOMEM);
	opt->isRunning=1;	

	opt->window_array[0]=newwin(mLINES, mCOLS, 0, 0); // Root Window (always on)
	opt->window_array[1]=subwin(opt->window_array[0], 0, 0, 0, 0); // main UI (always on)
	opt->window_array[2]=subwin(opt->window_array[0], 0, 0, 0, 0); // World Display
	opt->window_array[3]=subwin(opt->window_array[2], 0, 0, 0, 0); // World UI (world display)
	
	// TODO: placeholder entity count. Effected by difficulty and player count?
	opt->entityc=5;
	opt->entityc_a=0;
	opt->entity_array=malloc(sizeof (struct entity*)*opt->entityc);

	opt->tick=0;
	opt->frame=0;
	opt->input=0;
	opt->gameState=world;
	opt->p->self->ey=5;
	opt->p->self->ex=5;

	// Move loading map to here
	// TODO: load data/map/player here later
	
	// Initialize mouse input
	opt->use_mouse=0;
#ifdef NCURSES_MOUSE_VERSION
	keypad(stdscr, TRUE);
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION,NULL);
	printf("\033[?1003h\n"); // Mouse caputre escape sequence
	opt->use_mouse=1;
#endif

	timeout(((int)(1/((double)TARGET_TICK_RATE))*1000));
	return opt;
}

void
main_loop(struct arg *args) {
	pthread_t th_display, th_input, th_calc;
	pthread_create(&th_calc, NULL, main_loopCalculation, (void *)args);
	pthread_create(&th_display, NULL, main_loopDisplay, (void *)args);
	pthread_create(&th_input, NULL, main_loopInput, (void *)args);

	pthread_join(th_calc, NULL);
	pthread_join(th_display, NULL);
	pthread_join(th_input, NULL);

}

void *
main_loopCalculation(void *args) {
	struct arg *cArgs=(struct arg *)args;
	clock_t preTick, postTick;
	while (cArgs->isRunning) {
		preTick=clock();
		cArgs->tick++;
		if (!cArgs->autopause) {
			// Handle world events here if autopause is on, handle it in the world (paused while menu or inventory)
		}
		switch (cArgs->gameState) {
			case world:	
				world_loopMain(cArgs);
				break;
			case inventory:
				break;
			case menu:
				break;
			default:
				break;
			wrefresh(cArgs->window_array[2]);
		}
		postTick=clock();
		int wait=g_tickPeriod-(double)(postTick-preTick)*1000.0/CLOCKS_PER_SEC;
		if (wait<0) wait=0;
		generic_portableSleep(wait);
	}	
	return NULL;
}

void *
main_loopDisplay(void *args) {
	struct arg *cArgs=(struct arg *)args;
	clock_t preFrame, postFrame;
	while (cArgs->isRunning) {
		preFrame=clock();
		cArgs->frame++;

		cArgs->wOffset=cArgs->hOffset=1;
		if (mLINES%2) cArgs->wOffset=2;
		if (mCOLS%2) cArgs->hOffset=2;
		switch (cArgs->gameState) {
			case world:	
				wclear(cArgs->window_array[2]);
				world_display(cArgs);
				break;
			case inventory:
				break;
			case menu:
				break;
			default:
				break;
			wrefresh(cArgs->window_array[2]);
		}
		mvwprintw(cArgs->window_array[0],cArgs->mY, cArgs->mX, "X");
		mvwprintw(cArgs->window_array[0],mLINES, 0, "Frame: %lld, Input: %lld, Tick: %lld", cArgs->frame, cArgs->input, cArgs->tick);
		box(cArgs->window_array[0], 0, 0);
		wrefresh(cArgs->window_array[0]);
		wrefresh(cArgs->window_array[1]);

		postFrame=clock();
		int wait=g_framePeriod-(double)(postFrame-preFrame)*1000.0/CLOCKS_PER_SEC;
		if (wait<0) wait=0;
		generic_portableSleep(wait);
	}
	return NULL;
}

void *
main_loopInput(void *args) {
	struct arg *cArgs=(struct arg *)args;
	clock_t preInput, postInput;
	while (cArgs->isRunning) {
		preInput=clock();
		cArgs->input++;
		cArgs->cKey=getch();
#ifdef NCURSES_MOUSE_VERSION
		if (cArgs->cKey==KEY_MOUSE) {
			MEVENT mouse_event;
			if (getmouse(&mouse_event)==OK) {
				cArgs->mY=mouse_event.y;
				cArgs->mX=mouse_event.x;
				// TODO: Create a line for player movement
				// Because this will always be 2d, don't have to free. 			
			}
		}
#endif
		postInput=clock();
		int wait=g_inputPeriod-(double)(postInput-preInput)*1000.0/CLOCKS_PER_SEC;
		if (wait<0) wait=0;
		generic_portableSleep(wait);
	}
	return NULL;	
}

int
world_checkCollision(int wx, int wy, struct map *currentMap) {
	int tile=currentMap->mapArr[wy * currentMap->cols + wx];
		if (tile<2) return tile;
		// Run associated functions here
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
	struct shape_vertex *cursorLine=malloc(sizeof(struct shape_vertex));
	if (!generic_line_draw(args->mX, args->mY, midX, midY, cursorLine)) {
#ifdef DEBUG
		WARN("Some issue occured and shape was not drawn. ");
#endif
	}
	generic_line_scale(cursorLine, 5); // Scaling test
	util_displayShape(args->window_array[2], cursorLine,'0');
	generic_freeShape(cursorLine);
	mvwaddch(args->window_array[2], midY, midX, '@');

	wrefresh(args->window_array[2]);
	wrefresh(args->window_array[1]);
}

void 
world_loopEnv(struct arg *args) {
	for (int i=0;i<args->entityc;i++) {
		// spawn if any in the array is NULL
		if (args->entity_array[i]==NULL) {
			// TODO: create entity n spawn	
		}
		// Direction

	}
}

//enum State 
enum State
world_loopMain(struct arg *args) {
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
			args->gameState=inventory;
			break;
#ifdef DEBUG
		case'r':
			args->p->self->ex=25;
			args->p->self->ey=25;
			break;
#endif
		case'q':args->isRunning=0;
	}
	args->gameState=world;
	return world;
}

void
util_displayShape(WINDOW *window, struct shape_vertex *shape, char material) {
	for (int i=0;i<shape->pointc;i++) {
		mvwprintw(window, shape->vertex[i]->coord[1], shape->vertex[i]->coord[0], "%c", material);
	}
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

	// replace ./data with the root filepath from a cfg or something
	util_loadMap("./data", "MAP000.TST", args->currentMap);

	refresh();

	main_loop(args);

	printf("\033[?1003l\n"); // Mouse caputre escape sequence
	endwin();
return 0;
}

/**
 * Its a game engine. Made by me :)
 *
 *
 * **/


