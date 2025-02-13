#include <curses.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../common/definitions.h"
#include "../common/map.h"
#include "../common/data.h"

// Context struct 
struct arg {
	int isRunning;
	struct map *loadedMap;	
	struct data *loadedDat;	
};

int init(void);
int main_loop(struct arg *args);

int 
init(void) {
	//initscr();cbreak();clear();refresh();
	fprintf(stdout, "Welcome to the cursesGame engine tool shell. Enter 'help' to get started\n");
	return 1;
}

int
main_loop(struct arg *args) {
	char buf[32];
	char *command;
	fgets(buf, 32, stdin);
	command=strtok(buf, " ");
	fprintf(stdout, "input: %s\n", command);

	if (!strcmp(command, "exit\n")) {
		args->isRunning=0;	
	} else if (!strcmp(command, "exit\n")) {
		args->isRunning=0;	
	} else if (!strcmp(command, "help\n")) {
		fprintf(stdout, "load: Usage: load (arg1: text || bin) (arg2: data || map || save) {arg_id: id} {arg_path: path/to/file_root}\n");
	} else if (!strcmp(command, "load")) {
		// Usage: load (arg1: text || bin) (arg2: data || map || save) {arg_id: id} {arg_path: path/to/file_root}

		// Parse the command + args
		char **arg_vector=malloc(sizeof(char *)*4);
		for (int i=0;i<=4;i++) {
			arg_vector[i]=strtok(NULL, " ");
		}

		if (!strcmp(arg_vector[1], "data")) {
		
		} else if (!strcmp(arg_vector[1], "map")) {
			if (!strcmp(arg_vector[0], "bin")) {
				util_loadMap(arg_vector[3], arg_vector[2], args->loadedMap);
			} else if (!strcmp(arg_vector[0], "text")) { 
			
			} else goto cmd_fail;
		} else if (!strcmp(arg_vector[1], "data")) {

		} else goto cmd_fail;
	} else {
cmd_fail:
		command[strlen(command)-1]='\0';
		fprintf(stderr, "Error: \"%s\" is an invalid command\n", command);
	
	}
}

// TODO: add args support in the future, as well as the path
int
main(void) {
	struct arg *args;
	args->isRunning=init();
	while (args->isRunning) {
		main_loop(args);	
	}
	//endwin();
	return 0;
}

