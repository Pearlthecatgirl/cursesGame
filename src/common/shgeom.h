#pragma once
#define USE_SHIT_GEOMETRY

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
// Use curses.h, or provide a better terminal access layer
#include <curses.h>
#include <string.h>

// Lets not get ahead of ourselves here.....
#ifndef USE_SHIT_MATH
	#include <math.h>
#else
	#include "shmath.h"
#endif

#ifndef CRASH
	#define CRASH(errnum) {fprintf(stderr, "FATAL ERROR (line %d). Code: %s\n", __LINE__, strerror(errnum));endwin();printf("\033[?1003l\n");exit(errnum);}
#endif

#ifndef WARN
	#define WARN(msg) {fprintf(stderr, "Warning: %s Might crash soon... (line: %d)\n", msg, __LINE__);}
#endif

struct vector {
	int *coord; // coorindate
	int degree; // number of coordinates
};

struct shape_vertex {
	struct vector **vertex; // each coordinate of each point
	int space; // What dimension? This to stop people from trying to merge/normalise/compare
	int pointc; // number of points
};

void generic_freeShape(struct shape_vertex *shape);
int generic_line_draw(int x0, int y0, int x1, int y1, struct shape_vertex *shape);
int generic_line_drawPolar(const int xi, const int yi, const int theta, const int range, struct shape_vertex *shape);
int generic_line_scale(struct shape_vertex *line, signed int scalar);

