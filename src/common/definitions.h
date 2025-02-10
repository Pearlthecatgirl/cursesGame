#pragma once
#include <curses.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define CRASH(errnum) {fprintf(stderr, "FATAL ERROR (line %d). Code: %s\n", __LINE__, strerror(errnum));endwin();printf("\033[?1003l\n");exit(errnum);}
#define WARN(msg) {fprintf(stderr, "Warning: %s Might crash soon... (line: %d)\n", msg, __LINE__);}
#define mLINES 17 // Max Screen size
#define mCOLS 39 // Max screen size
#define MAX_FILE_NAME_SIZE 16 // File name size
#define TARGET_TICK_RATE 240 
#define TARGET_FRAME_RATE 240
#define TARGET_INPUT_RATE 240
#define HEADER_SIZE 50 //Size of header in each read file
#define MAX_NAME_SIZE 32// Never use this size. It is just a temporary buffer

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Might be deprecated maybe remove?
#define NS_WAIT 1000000000
#define US_WAIT 1000000
#define MS_WAIT 1000
#define S_WAIT 1

