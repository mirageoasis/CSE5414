CC = gcc
CFLAGS=-Wall -O2 -fopenmp
LDFLAGS=-lm -lpthread -lrt

all: hw1

hw1:
	$(CC) -g -Wall -fopenmp -o hw1 main.c

clean:
	rm -rf *~ hw1 *.o *.out