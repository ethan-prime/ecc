CC = gcc
CFLAGS = -Wall -Wextra -g -O3
SRC = $(shell find src -name '*.c')
OBJ = $(SRC:.c=.o)

all: compiler

compiler: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
	rm -rf $(OBJ) compiler
