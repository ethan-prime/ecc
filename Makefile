CC = gcc
CFLAGS = -Wall -Wextra -g -O3
SRC = $(shell find src -name '*.c')
OBJ = $(SRC:.c=.o)

all: ecc

ecc: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lm
	rm -rf $(OBJ)

clean:
	rm -rf $(OBJ) ecc