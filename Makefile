BUILD:
	gcc -std=c11 ./src/*.c -lcurses -lm -lc -Wall -Wextra -Wpedantic
DEBUG:
	gcc -std=c11 ./src/*.c -lcurses -lm -lc -g -o debug -Wall -Wextra -Wpedantic
RELEASE:
	gcc -std=c11 ./src/*.c -lcurses -lm -lc -s -o prog -Wall -Wextra -Wpedantic -Werror
CLEAN:
	rm ./prog ./*.o ./*.out ./debug
