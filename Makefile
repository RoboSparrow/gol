CC=gcc
CFLAGS=-Wall -g -I.
DEPS = gol.h gol.c
OBJ = main.o gol.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
