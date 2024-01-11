CC=gcc
LDFLAGS=-lSDL2main -lSDL2 -lm -g
CFLAGS=-I. -Wall -g
DEPS=src/peripherals.h
BUILDDIR=./build
SRCDIR=./src

$(shell mkdir -p $(BUILDDIR))

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chip8: $(BUILDDIR)/cpu.o $(BUILDDIR)/peripherals.o
	$(CC) $(LDFLAGS) -o chip8.out $(BUILDDIR)/cpu.o $(BUILDDIR)/peripherals.o

clean:
	rm -f $(BUILDDIR)/*.o
	rm -f *.out
	rm -rf build