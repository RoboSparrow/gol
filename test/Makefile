EXECUTABLE = runtests

$(info ====== Building SDL app [${EXECUTABLE}] ======)

src = $(wildcard ../src/*.c)
src := $(filter-out ../src/main.c, $(src))
src += $(wildcard *.c)

obj := $(src:.c=.o)
obj := $(patsubst ../src/%.o, %.o, $(obj))

IFLAGS = -I../include
CFLAGS = -Wall -pedantic -Werror -Wshadow -Wstrict-aliasing -Wstrict-overflow -g
LDLIBS += $(shell pkg-config sdl2 --cflags --libs) -lSDL2_ttf -lm

$(info $$src => [${src}])
$(info $$obj => [${obj}])

$(EXECUTABLE): $(obj)
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)

%.o: %.c
	$(CC) $(IFLAGS) $(CFLAGS) -c $< -o $@

%.o: ../src/%.c
	$(CC) $(IFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(obj) $(EXECUTABLE)
