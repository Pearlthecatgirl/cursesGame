build:
	gcc -std=c11 ./src/*.c -lcurses -lm -lc
clean:
	rm ./*.o
