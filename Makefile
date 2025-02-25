CC=gcc
CFLAGS=-D NDEBUG -std=c90 -Wall -Wextra -pedantic

compile:
	$(CC) $(CFLAGS) -o P1-code P1-code.c

clean:
	rm P1-code
