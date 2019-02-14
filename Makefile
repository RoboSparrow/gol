.PHONY: all clean

all: shell sdl

shell:
	make -f Makefile.shell

sdl:
	make -f Makefile.sdl

clean:
	find obj -type f -name '*.o' -delete
