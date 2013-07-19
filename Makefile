CC=musl-gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -O4

all:
	$(CC) $(CFLAGS) ν.c -o ν
