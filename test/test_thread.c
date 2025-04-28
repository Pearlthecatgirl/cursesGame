#include <pthread.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

#define TARGET_TICK_RATE 120
#define TARGET_FRAME_RATE 60
#define TARGET_INPUT_RATE 120

const float g_tickPeriod=1000.0/TARGET_TICK_RATE;
const float g_framePeriod=1000.0/TARGET_FRAME_RATE;
const float g_inputPeriod=1000.0/TARGET_INPUT_RATE;

struct arg {
	int key;
	int isRunning;
	
	// Packed into seperate structs
	int ey, ex;
	unsigned long long int tick, frame;
};

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

void 
*_loop_io(void *args) {
	struct arg *cArgs=(struct arg *)args;
	clock_t pre, post;
	while (cArgs->isRunning) {
		pre=clock();
		cArgs->tick++;
		// Routine here
		cArgs->key=getch();

		post=clock();
		int wait=g_inputPeriod-(double)(post-pre)*1000.0/CLOCKS_PER_SEC;
		generic_portableSleep(wait);
	}
}

void 
*_loop_display(void *args) {
	struct arg *cArgs=(struct arg *)args;
	clock_t pre, post;
	while (cArgs->isRunning) {
	pre=clock();
	cArgs->frame++;
	// Routine here
	clear();
	mvprintw(0,0, "frame: %lld, tick: %lld, x: %d, y: %d", cArgs->frame, cArgs->tick, cArgs->ex, cArgs->ey);
	mvprintw(cArgs->ey, cArgs->ex, "@");
	post=clock();
	int wait=g_framePeriod-(double)(post-pre)*1000.0/CLOCKS_PER_SEC;
	generic_portableSleep(wait);
	}
}
void 
main_loop(struct arg *args) {
	pthread_t th_io, th_disp;
	pthread_create(&th_io, NULL, &_loop_io, args);
	pthread_create(&th_disp, NULL, &_loop_display, args);
	clock_t pre, post;
	while (args->isRunning) {
		pre=clock();
		switch(args->key) {
			case 'w': args->ey--;	
				break;
			case 's': args->ey++;	
				break;
			case 'a': args->ex--;	
				break;
			case 'd': args->ex++;	
				break;
		}
		post=clock();
		int wait=g_inputPeriod-(double)(post-pre)*1000.0/CLOCKS_PER_SEC;
		generic_portableSleep(wait);
	}
	pthread_join(th_io, NULL);
	pthread_join(th_disp, NULL);
}

int
main(void) {
	// Main entrypoint
	initscr();noecho();cbreak();clear();curs_set(0);keypad(stdscr, TRUE);
	struct arg *args=malloc(sizeof(struct arg));
	timeout(( (int) (1/((double)TARGET_TICK_RATE))*1000));
	args->isRunning=1;
	args->frame=0;
	args->tick=0;
	main_loop(args);

	endwin();
	return 0;
}
