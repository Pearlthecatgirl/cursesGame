#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <math.h>

struct _vector {
	int *coord; // coorindate
	int degree; // number of coordinates
};

struct shape_vertex {
	struct _vector **vertex; // each coordinate of each point
	int pointc; // number of points
};

struct arg {
	int isRunning;
	int key;
	int mx, my, cx, cy;
};

void
util_displayShape(WINDOW *window, struct shape_vertex *shape, char material) {
	for (int i=0;i<shape->pointc;i++) {
		mvwprintw(window, shape->vertex[i]->coord[1], shape->vertex[i]->coord[0], "%c", material);
	}
}

// TODO: fix segfault
int
generic_line_draw(int x0, int y0, int xf, int yf, struct shape_vertex *shape) {
	int dx=xf-x0, dy=yf-y0;
	if (abs(dy)<abs(dx)) {
		int yp=1, rev=0;
		struct shape_vertex *tmp_shape;
		if (x0>xf) {
			// Is the line reversed? if so, flip everything and create a temporary line to be flipped later
			int tmp=x0;x0=xf;xf=tmp;tmp=y0;y0=yf;yf=tmp;rev=1;dy=yf-y0;dx=xf-x0;
			tmp_shape=malloc(sizeof(struct shape_vertex));
			tmp_shape->pointc=(xf-x0);
			tmp_shape->vertex=malloc(sizeof(struct _vector *)*tmp_shape->pointc);
		}
		if (dy<0) {yp=-1;dy*=-1;}
		shape->pointc=(xf-x0);
		shape->vertex=malloc(sizeof(struct _vector *)*shape->pointc);
		for (int xc=x0,yc=y0,dec=(2*dy)-dx,i=0;xc<xf;xc++,i++) {
			// Allocation for line shape
			shape->vertex[i]=malloc(sizeof(struct _vector));
			shape->vertex[i]->degree=2;
			shape->vertex[i]->coord=malloc(sizeof(int)*shape->vertex[i]->degree);
			shape->vertex[i]->coord[0]=xc;
			shape->vertex[i]->coord[1]=yc;
			// Line drawing decision 
			if (dec>0) {
				dec+=2*(dy-dx);
				yc+=yp;
			} else dec+=2*dy;
		}
		return 1;
	} else {
		int xp=1, rev=0;
		struct shape_vertex *tmp_shape;
		if (y0>yf) {
			// Same deal as above. Flip everything and store the reverse
			int tmp=x0;x0=xf;xf=tmp;tmp=y0;y0=yf;yf=tmp;rev=1;dy=yf-y0;dx=xf-x0;
			tmp_shape=malloc(sizeof(struct shape_vertex));
			tmp_shape->pointc=(xf-x0);
			tmp_shape->vertex=malloc(sizeof(struct _vector *)*tmp_shape->pointc);
		}
		if (dx<0) {xp=-1;dx*=-1;}

		shape->pointc=(yf-y0);
		shape->vertex=malloc(sizeof(struct _vector *)*shape->pointc);

		for (int yc=y0,xc=x0,dec=(2*dx)-dy,i=0;yc<yf;yc++, i++) {
				// Allocation for line shape
				shape->vertex[i]=malloc(sizeof(struct _vector));
				shape->vertex[i]->degree=2;
				shape->vertex[i]->coord=malloc(sizeof(int)*shape->vertex[i]->degree);
				shape->vertex[i]->coord[0]=xc;
				shape->vertex[i]->coord[1]=yc;

				// Line drawing decision 
				if (dec>0) {
					dec+=2*(dx-dy);
					xc+=xp;
				} else dec+=2*dx;
			}
		return 1;
	}
}

int 
main(void) {
	initscr();noecho();cbreak();keypad(stdscr, TRUE);
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION,NULL);
	printf("\033[?1003h\n"); // Mouse caputre escape sequence
	timeout(5);
	struct arg *args=malloc(sizeof(struct arg));
	args->isRunning=1;
	args->mx=0;
	args->my=0;
	args->cx=0;
	args->cy=0;
	while (args->isRunning) {
		clear();
		generic_line_draw(args->cx, args->cy, args->mx, args->my);
		mvprintw(args->my, args->mx, "X");
		mvprintw(args->cy, args->cx, "@");
		args->key=getch();
		switch (args->key) {
			case 'q':
				args->isRunning=0;
				break;
			case 'w':
				args->cy--;
				break;
			case 's':
				args->cy++;
				break;
			case 'a':
				args->cx--;
				break;
			case 'd':
				args->cx++;
				break;
			default:
			break;	
		}
		if (args->key==KEY_MOUSE) {
			MEVENT mevent;
			if (getmouse(&mevent)==OK) {
				args->my=mevent.y;	
				args->mx=mevent.x;	
			}
		}
	}

	printf("\033[?1003l\n"); // Mouse caputre escape sequence
	endwin();
	return 0;
}
