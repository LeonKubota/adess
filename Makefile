CC := gcc

# I might want to use -O3 (or something similar) to optimize the compiled code
CFLAGS := -std=c99 -Wall -g -Werror -Iinclude
LDLIBS :=

# macOS
ifeq ($(shell uname), Darwin)
	# might need to add "-framework CoreFoundation
	LDLIBS += 
endif

# Linux
ifeq ($(shell uname), Linux)
	# might need to add "-lm"
	LDLIBS += 
endif

# SRC := src/main.c src/parse.c src/commands/command.c src/commands/help.c src/commands/make.c
SRC := $(wildcard src/*.c) $(wildcard src/commands/*.c)
OBJ := $(SRC:.c=.o)

# Ouput
TARGET := build/adess

# Default target
all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p build
	$(CC) -o $@ $^ $(LDLIBS)

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f build/*.o src/*.o src/commands/*.o build/adess

.PHONY: all clean
