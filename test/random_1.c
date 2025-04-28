#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

int
main(void) {
	srand(time(0));
	fprintf(stdout, "%d\n", rand());
return 0;
}
