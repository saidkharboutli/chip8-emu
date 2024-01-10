CC=gcc
LDFLAGS=-lSDL2main -lSDL2 -g
CFLAGS=-I. -Wall -g
DEPS=peripherals.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chip8: cpu.o peripherals.o
	$(CC) $(LDFLAGS) -o chip8.out cpu.o peripherals.o
	rm -f *.o

clean:
	rm -f *.o
	rm -f *.out