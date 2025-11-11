# Compiler
CC = gcc

# Compiler flags
CFLAGS = -std=c99 -Wall -g -Werror -O3 -Iinclude

# Source files
SRC = src/main.c src/parse.c src/commands/command.c src/commands/help.c src/commands/make.c

# Object files (made from source files)
OBJ = $(SRC:.c=.o)

# Ouput
TARGET = build/adess

# Default target
all: $(TARGET)

# Rule to build the target
$(TARGET): $(OBJ)
	@mkdir -p build # Create directory if it doesn't exist
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile source files into object files
build/%.o: src/%.c
	@mkdir -p build # Create directory if it doesn't exist
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean
# Clean rule to remove object files and executable
clean:
	rm -f build/*.o build/adess
