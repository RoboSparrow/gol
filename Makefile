# executable
EXE = gameoflive

## Sources

# source and build output directories
SRC_DIR = src
OBJ_DIR = obj

# list source files and object files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

## Flags

# preprocessor flags
CPPFLAGS += -Iinclude
# compiler flags
CFLAGS += -Wall -pedantic -Werror -Wshadow -Wstrict-aliasing -Wstrict-overflow -g
# linker flags
LDFLAGS +=
# thirdparty libraries to link
LDLIBS +=

## Recipes

# list rules not  to produce any target output
.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

# Based on the excellent walkthrough by Chnossos https://stackoverflow.com/a/30602701
