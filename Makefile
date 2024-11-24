CC = gcc
CFLAGS = -Wall -I. -Wextra -Wswitch-enum -std=c11 -pedantic

all: jvasm jv

jvasm: src/jvasm.c src/jvvm.c 
	$(CC) $(CFLAGS) -o jvasm src/jvasm.c src/utils.c

jv: src/jv.c src/jvvm.c
	$(CC) $(CFLAGS) -o jv src/jv.c 	src/utils.c