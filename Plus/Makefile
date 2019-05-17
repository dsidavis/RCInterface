CFLAGS=-O2
CC=clang

plus.o: plus.c
	$(CC) $(CFLAGS) -o $@ $^

plus:  plus.c
	$(CC) $(CFLAGS) -o $@ $^

test: plus
	./plus

machineCode: plus.o
	objdump  -disassemble  plus.o
