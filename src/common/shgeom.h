#pragma once

struct vector {
	int *coord; // coorindate
	int degree; // number of coordinates
};

struct shape_vertex {
	struct vector **vertex; // each coordinate of each point
	int space; // What dimension?
	int pointc; // number of points
};

void generic_freeShape(struct shape_vertex *shape) {
int generic_line_draw(int x0, int y0, int x1, int y1, struct shape_vertex *shape);
int generic_line_drawPolar(const int xi, const int yi, const int theta, const int range, struct shape_vertex *shape);
int generic_line_scale(struct shape_vertex *line, signed int scalar);
