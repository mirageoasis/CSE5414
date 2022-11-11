CC = mpicc
LDFLAGS=-lm -lpthread -lrt

all: prefix

prefix:
	$(CC) -g -o prefix main.c

clean:
	rm -rf *~ prefix *.o *.out