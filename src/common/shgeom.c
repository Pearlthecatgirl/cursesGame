#include "shgeom.h"

void
generic_freeShape(struct shape_vertex *shape) {
	for (int i=0;i<shape->pointc;i++) {
		free(shape->vertex[i]->coord);
		shape->vertex[i]->degree=0;
	}
	shape->pointc=0;
}

int
generic_line_draw(int x0, int y0, int xf, int yf, struct shape_vertex *shape) {
	int dx=xf-x0, dy=yf-y0;
	if (abs(dy)<abs(dx)) {
		int yp=1, rev=0;
		struct shape_vertex *tmp_shape;
		if (x0>xf) {
			// Is the line reversed? if so reverse everything
			int tmp=x0;x0=xf;xf=tmp;tmp=y0;y0=yf;yf=tmp;rev=1;dy=yf-y0;dx=xf-x0;
		}
		if (dy<0) {yp=-1;dy*=-1;}
		shape->pointc=(xf-x0);
		shape->vertex=malloc(sizeof(struct vector *)*shape->pointc);
		if (!shape->vertex) CRASH(ENOMEM);
		
		for (int xc=x0,yc=y0,dec=(2*dy)-dx,i=0;xc<xf;xc++,i++) {
			// Allocation for line shape
			shape->vertex[i]=malloc(sizeof(struct vector));
			if (!shape->vertex[i]) CRASH(ENOMEM);
			shape->vertex[i]->degree=2;
			shape->vertex[i]->coord=malloc(sizeof(int)*shape->vertex[i]->degree);
			shape->vertex[i]->coord[0]=xc;
			shape->vertex[i]->coord[1]=yc;
			// Line drawing decision 
			if (dec>0) {
				dec+=2*(dy-dx);
				yc+=yp;
			} else dec+=2*dy;
		} if (!rev) {
			// if the line is reversed, reverse it back
			struct vector *tmp_vector;
			int l=0, r=shape->pointc-1;
			while (l<r) {
				tmp_vector=shape->vertex[l];
				shape->vertex[l]=shape->vertex[r];
				shape->vertex[r]=tmp_vector;
				l++;r--;
			}
		} return 1;
	} else {
		int xp=1, rev=0;
		struct shape_vertex *tmp_shape;
		if (y0>yf) {
			// Same deal as above. Flip everything and store the reverse
			int tmp=x0;x0=xf;xf=tmp;tmp=y0;y0=yf;yf=tmp;rev=1;dy=yf-y0;dx=xf-x0;
		}
		if (dx<0) {xp=-1;dx*=-1;}
		shape->pointc=(yf-y0);
		shape->vertex=malloc(sizeof(struct vector *)*shape->pointc);
		if (!shape->vertex) CRASH(ENOMEM);
		for (int yc=y0,xc=x0,dec=(2*dx)-dy,i=0;yc<yf;yc++, i++) {
			// Allocation for line shape
			shape->vertex[i]=malloc(sizeof(struct vector));
			if (!shape->vertex[i]) CRASH(ENOMEM);
			shape->vertex[i]->degree=2;
			shape->vertex[i]->coord=malloc(sizeof(int)*shape->vertex[i]->degree);
			shape->vertex[i]->coord[0]=xc;
			shape->vertex[i]->coord[1]=yc;

			// Line drawing decision 
			if (dec>0) {
				dec+=2*(dx-dy);
				xc+=xp;
			} else dec+=2*dx;
		} if (!rev) {
			// if the line is reversed, reverse it back
			struct vector *tmp_vector;
			int l=0, r=shape->pointc-1;
			while (l<r) {
				tmp_vector=shape->vertex[l];
				shape->vertex[l]=shape->vertex[r];
				shape->vertex[r]=tmp_vector;
				l++;r--;
			}
		} return 1;
	}
}

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


