EXEC = runtests

$(info ====== Building SDL app [${EXEC}] ======)

src = $(wildcard ../src/*.c)
src := $(filter-out ../src/main.c, $(src))
src += $(wildcard *.c)

obj := $(src:.c=.o)
obj := $(patsubst %.o, out/%.o, $(obj))
obj := $(patsubst out/../src/%.o, out/%.o, $(obj))

IFLAGS = -I../include
CFLAGS = -Wall -pedantic -Werror -Wshadow -Wstrict-aliasing -Wstrict-overflow -g
LDLIBS += $(shell pkg-config sdl2 --cflags --libs) -lSDL2_ttf -lm

$(info $$src => [${src}])
$(info $$obj => [${obj}])

all: $(EXEC)
.PHONY: clean

$(EXEC): $(obj)
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)

out/%.o: %.c
	$(CC) $(IFLAGS) $(CFLAGS) -c $< -o $@

out/%.o: ../src/%.c
	$(CC) $(IFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(obj) $(EXEC)
	rm -rf out/*
	touch out/.gitkeep
