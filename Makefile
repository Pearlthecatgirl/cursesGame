build:
	gcc -std=c11 ./src/*.c -lcurses -lm -lc -Wall -Wextra -Wpedantic
debug:
	gcc -std=c11 ./src/*.c -lcurses -lm -lc -g -o debug -Wall -Wextra -Wpedantic
release:
	gcc -std=c11 ./src/*.c -lcurses -lm -lc -s -o prog -Wall -Wextra -Wpedantic -Werror
clean:
	rm ./*.o
