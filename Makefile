BUILD:
	gcc -std=gnu11 ./src/*.c -lcurses -lm -lc -Wall -Wextra -Wpedantic 
DEBUG:
	gcc -std=gnu11 ./src/*.c -lcurses -lm -lc -g -o debug -Wall -Wextra -Wpedantic
RELEASE:
	gcc -std=gnu11 ./src/*.c -lcurses -lm -lc -s -o prog -Wall -Wextra -Wpedantic -Werror
RELEASE_MACOS:
	gcc -std=gnu11 ./src/*.c -lcurses -lm -lc -s -o prog -Wall -Wextra -Wpedantic -Werror -D_DARWIN_C_SOURCE
CLEAN:
	rm ./prog ./*.o ./*.out ./debug
