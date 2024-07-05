CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g

all: pre

pre: main.o util.o pre_assembler.o macro.o
	$(CC) $(CFLAGS) -o pre main.o util.o pre_assembler.o macro.o

main.o: main.c util.h pre_assembler.h macro.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

pre_assembler.o: pre_assembler.c pre_assembler.h util.h
	$(CC) $(CFLAGS) -c pre_assembler.c

macro.o: macro.c macro.h util.h
	$(CC) $(CFLAGS) -c macro.c

clean:
	rm -f *.o pre
