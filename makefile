VALGRIND_FLAGS=--leak-check=full --track-origins=yes --show-reachable=yes --error-exitcode=2
CFLAGS =-g -std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O0
CC = gcc

all: clean valgrind

main: main.c src/*
	$(CC) $(CFLAGS) src/*.c main.c -o main 2>&1

valgrind: main
	valgrind $(VALGRIND_FLAGS) ./main datos.csv 2>&1

valgrind-pruebas: pruebas
	valgrind $(VALGRIND_FLAGS) ./pruebas 2>&1

pruebas: src/*.c pruebas.c
	$(CC) $(CFLAGS) src/*.c pruebas.c -o pruebas 2>&1

clean:
	rm -f main *.o pruebas 2>&1
