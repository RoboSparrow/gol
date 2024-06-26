EXECUTABLE = bin/gol

$(info ====== Building SDL app [${EXECUTABLE}] ======)

# create obj sub dirs
$(shell mkdir -p obj)

# list source files and object files
SRC = $(wildcard src/*.c)

# map objects
OBJ = $(SRC:src/%.c=obj/%.o)

# Debug info
$(info $$SRC => [${SRC}])
$(info $$OBJ => [${OBJ}])

## Flags

# preprocessor flags
CPPFLAGS += -Iinclude
# compiler flags
CFLAGS += -Wall -pedantic -Werror -Wshadow -Wstrict-aliasing -Wstrict-overflow -g
# linker flags
LDFLAGS +=
# thirdparty libraries to link
LDLIBS += $(shell pkg-config sdl2 --cflags --libs) -lSDL2_ttf -lm

## Recipes

# list rules not  to produce any target output
.PHONY: all assets clean

all: $(EXECUTABLE) assets

$(EXECUTABLE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

obj/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

assets:
	mkdir -p bin/assets && cp -a -f assets/fonts/*.ttf bin/assets/
	mkdir -p bin/patterns && cp -a -f assets/patterns/. bin/patterns/
	mkdir -p bin/save

# maintenance

clean:
	$(RM) $(OBJ)
	$(RM) bin/patterns/*
	$(RM) bin/save/*
	$(RM) bin/assets/*

# Based on the excellent walkthroughs by [Chnossos](https://stackoverflow.com/a/30602701) and [John Tsiombikas](http://nuclear.mutantstargoat.com/articles/make)
