#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <curses.h>

struct arg {
	int key;
	int isRunning;
	int frame;

};

void 
sleep_portable(int ms) {
	#ifdef WIN32
		Sleep(ms);
	#elif _POSIX_C_SOURCE >= 199309L
		struct timespec ts;
		ts.tv_sec = ms / 1000;
		ts.tv_nsec = (ms % 1000) * 1000000;
		nanosleep(&ts, NULL);
	#endif
		//usleep(ms * 1000);
}

void
main_loop(struct arg *args) {

}

int
main(void) {

}
