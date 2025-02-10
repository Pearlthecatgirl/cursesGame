#include <curses.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../common/definitions.h"

struct arg {
	int isRunning;
};

int init(void);
int main_loop(struct arg *args);

int 
init(void) {
	initscr();raw();clear();refresh();
	mvprintw(0,0,"Welcome to the cursesGame engine tool shell. Enter 'help' to get started");
	return 1;
}

int
main_loop(struct arg *args) {
	char buf[32];
	char command[16];
	if (!strncpy(command, strtok(buf, " "), sizeof(char)*16)) CRASH(ENOBUFS);
}

// TODO: add args support in the future, as well as the path
int
main(void) {
	struct arg *args;
	args->isRunning=init();
	while (args->isRunning) {
		main_loop(args);	
	}
	endwin();
	return 0;
}

