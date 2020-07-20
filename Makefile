# set compiler to gcc
CC = gcc
CFLAGS=`sdl2-config --cflags`
SDLLIBS=`sdl2-config --libs`

all: hide unhide

hide: readHeader.c display.c other.c readHeader.h display.h other.h
	gcc hide.c readHeader.c other.c display.c $(CFLAGS) $(SDLLIBS) -o hide
unhide: readHeader.c other.c readHeader.h other.h
	gcc -o unhide unhide.c readHeader.c other.c
clean:
	rm unhide hide

