all:
	gcc -Wall -pedantic -std=c99 -g main.c shell.c -o CIS3110sh

val:
	valgrind --leak-check=yes --show-reachable=yes --track-origins=yes ./CIS3110sh -v
clean:
	rm CIS3110sh
