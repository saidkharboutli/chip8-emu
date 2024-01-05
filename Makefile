CC=gcc
LDFLAGS=-lm
CFLAGS=-I.
DEPS=peripherals.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chip8: cpu.o peripherals.o
	$(CC) $(LDFLAGS) -o chip8 cpu.o peripherals.o
