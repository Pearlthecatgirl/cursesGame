#include <curses.h> // This may not be as portable
#include <errno.h> // This may not be as portable
#include <math.h>
#include <pthread.h> // This may not be as portable
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // This may not be as portable

#include "../common/map.h"
#include "../common/definitions.h"

// #! Global
const float g_tickPeriod=1000.0/TARGET_TICK_RATE;
const float g_framePeriod=1000.0/TARGET_FRAME_RATE;
const float g_inputPeriod=1000.0/TARGET_INPUT_RATE;

// #! Structs
struct _Vector {
	int *coord; // coorindate
	int degree; // number of coordinates
};

struct shape_vertex {
	struct _Vector **vertex; // each coordinate of each point
	int pointc; // number of points
};

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
} entity;

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
} player;

struct data {
	char *tileset;
} data;

struct base {
	struct data *dat;
	char *data_path;
	int data_path_len;
} base;

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
} args;

// #!Functions
//short entity_spawn(struct entity *opt, struct base *self, int use_rand); // returns ret code

void generic_delay(const unsigned long int ms, const unsigned long int unit);
int generic_line_draw(int x0, int y0, int x1, int y1, struct shape_vertex *shape);
int generic_line_drawPolar(const int xi, const int yi, const int theta, const int range, struct shape_vertex *shape);
int generic_line_scale(struct shape_vertex *line, signed int scalar);
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
entity_spawn(struct entity *opt, struct arg *args, int use_rand) {
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

int
generic_line_draw(int x0, int y0, int xf, int yf, struct shape_vertex *shape) {
	int dx=xf-x0, dy=yf-y0;
	if (abs(dy)<abs(dx)) {
		int yp=1, rev=0;
		if (x0>xf) {int tmp=x0;x0=xf;xf=tmp;tmp=y0;y0=yf;yf=tmp;rev=1;dy=yf-y0;dx=xf-x0;}
		if (dy<0) {yp=-1;dy*=-1;}
		shape->vertex=malloc(sizeof(struct _Vector *)*(dx+1));
		for (int xc=x0,yc=y0,dec=(2*dy)-dx;xc<xf;xc++) {
			mvprintw(yc,xc,"0");
			if (dec>0) {
				dec+=2*(dy-dx);
				yc+=yp;
			} else dec+=2*dy;
		}
	} else {
		int xp=1, rev=0;
		if (y0>yf) {int tmp=x0;x0=xf;xf=tmp;tmp=y0;y0=yf;yf=tmp;rev=1;dy=yf-y0;dx=xf-x0;}
		if (dx<0) {xp=-1;dx*=-1;}
		for (int yc=y0,xc=x0,dec=(2*dx)-dy;yc<yf;yc++) {
			mvprintw(yc,xc,"0");
			if (dec>0) {
				dec+=2*(dx-dy);
				xc+=xp;
			} else dec+=2*dx;
		}
	}
}

// TODO: FIX THIS LINE DRAW ALGO
//int
//generic_line_draw(int x0, int y0, int x1, int y1, struct shape_vertex *shape) {
//	if (x0==x1 && y0==y1) return 0;
//	int dx=x1-x0;
//	int dy=y1-y0;
//	int dir=1;
//	int dir_x=1, dir_y=1;
//	if (dx<0) dx*=-1;
//	if (dy<0) dy*=-1;
//	if (dx>dy) {
//		shape->vertex=malloc(sizeof(struct _Vector *)*(dx+1));
//		if (!shape->vertex) CRASH(ENOMEM);
//		// TODO: fix race condition with this popping up
//#ifdef DEBUG
//			fprintf(stdout, "dx num: %d, (%d)\n", dx, dx+1);
//#endif
//		shape->pointc=dx+1;
//		if (x0>x1) {
//			int tmp=x0;x0=x1;x1=tmp;tmp=y0;y0=y1;y1=tmp;
//			}
//		int dx=x1-x0;int dy=y1-y0;
//		if (dy<1) dir=-1; 
//		dy*=dir;
//		if (dx!=0) {
//			int yc=y0;int p=2*dy-dx;
//			for (int i=0;i<dx+1;i++) {
//				shape->vertex[i]=malloc(sizeof(struct _Vector));
//				if (!shape->vertex[i]) CRASH(ENOMEM);
//				shape->vertex[i]->degree=2;
//				shape->vertex[i]->coord=malloc(sizeof(int)*shape->vertex[i]->degree);
//				if (!shape->vertex[i]->coord) CRASH(ENOMEM);
//				shape->vertex[i]->coord[0]=x0+i;
//				shape->vertex[i]->coord[1]=yc;
//				if (p>=0){
//					yc+=dir;p=p-2*dx;
//				} p=p+2*dy;
//			}
//			return 1;
//		}
//	} else if (dy>dx){
//		shape->vertex=malloc(sizeof(struct _Vector *)*(dy+1));
//		if (!shape->vertex) CRASH(ENOMEM);
//#ifdef DEBUG
//		fprintf(stdout, "dy num: %d, (%d)\n", dy, dy+1);
//#endif
//		shape->pointc=dy+1;
//		if (y0>y1) {
//		int tmp=x0;x0=x1;x1=tmp;tmp=y0;y0=y1;y1=tmp;
//		}
//		int dx=x1-x0;int dy=y1-y0;
//		if (dx<1) dir=-1; 
//		dx*=dir;
//		if (dy!=0) {
//			int xc=x0;int p=2*dx-dy;
//			for (int i=0;i<dy+1;i++) {
//				shape->vertex[i]=malloc(sizeof(struct _Vector));
//				if (!shape->vertex[i]) CRASH(ENOMEM);
//				shape->vertex[i]->degree=2;
//				shape->vertex[i]->coord=malloc(sizeof(int)*shape->vertex[i]->degree);
//				if (!shape->vertex[i]->coord) CRASH(ENOMEM);
//				shape->vertex[i]->coord[0]=xc;
//				shape->vertex[i]->coord[1]=y0+i;
//				if (p>=0){
//					xc+=dir;p=p-2*dy;
//				} p=p+2*dx;
//			}
//			return 1;
//		}
//	} else {
//		shape->vertex=malloc(sizeof(struct _Vector *)*dy);
//		if (!shape->vertex)	CRASH(ENOMEM);
//#ifdef DEBUG
//	   	fprintf(stdout, "dy num: %d, dx num: %d (should be equal)\n", dy, dx);
//#endif
//		shape->pointc=dy;
//
//		for (int i=0;i<dy;i++) {
//			shape->vertex[i]=malloc(sizeof(struct _Vector));
//			if (!shape->vertex[i]) CRASH(ENOMEM);
//			shape->vertex[i]->degree=2;
//			shape->vertex[i]->coord=malloc(sizeof(int)*shape->vertex[i]->degree);
//			if (!shape->vertex[i]->coord) CRASH(ENOMEM);
//			shape->vertex[i]->coord[0]=x0+(i*dir_x);
//			shape->vertex[i]->coord[1]=y0+(i*dir_y);
//
//		}
//	}
//	return 0;
//}

/*This function returns the return code from the inner function. NOT A SHAPE ARRAY*/
int
generic_line_drawPolar(const int xi, const int yi, const int theta, const int range, struct shape_vertex *shape) {
	int endpt[2]={round(xi-range * (cos(theta))), round(yi -range * (sin(theta)))};
	return generic_line_draw(xi, yi, endpt[0], endpt[1], shape);
}

/* Return code: 0 on fail, 1 on success */
int 
generic_line_scale(struct shape_vertex *line, signed int scalar) {
	if (scalar==0 || scalar==line->pointc) return scalar; // If its 0, do nothing. 	
	// Negative Scaling
	if (scalar<0) {
		for (int i=1;i<line->pointc;i++) {
			line->vertex[i]->coord[0]=line->vertex[0]->coord[0]-(line->vertex[i]->coord[0]-line->vertex[0]->coord[0]);
			line->vertex[i]->coord[1]=line->vertex[1]->coord[1]-(line->vertex[i]->coord[1]-line->vertex[1]->coord[1]);
		}
	}

	// Scale down and scaling up
	if (scalar<line->pointc) {
		for (line->pointc;line->pointc>scalar;line->pointc--) {
			free(line->vertex[line->pointc-1]);
		}
	} else {
	//TODO: upscaling

	}

	return 1;
}

void
generic_freeShape(struct shape_vertex *shape) {
	for (int i=0;i<shape->pointc;i++) {
		free(shape->vertex[i]->coord);
		shape->vertex[i]->degree=0;
	}
	shape->pointc=0;
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
		case'r':
			args->p->self->ex=25;
			args->p->self->ey=25;
			break;
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


